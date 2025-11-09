#include "command_dispatcher.h"
#include "command/command.h"
#include "editor_adaptor.h"
#include "main.h"
#include "env.h"
using namespace godot;

CommandDispatcher::CommandDispatcher(Array p_key_map) {
    key_map = p_key_map;
    vim_command.instantiate();
}

bool CommandDispatcher::dispatch(Ref<InputEventKey> key, Ref<VimCore> core, Ref<EditorAdaptor> editor_adaptor) {
    String key_str = key->as_text_keycode();
    InputStats* stats = core->current->input_stats;
    core->macro_manager->push_key(key);

    if(key_str == "Escape") {
        stats->clear();
        core->macro_manager->on_command_processed(Dictionary(), core->current->insert_mode);
        core->current->enter_normal_mode();
        return false;
    }

    if(core->current->insert_mode) return false;

    if(!Array::make("Shift", "Ctrl", "Alt", "Escape").has(key_str)) {
        if(key_str.is_valid_int() && stats->buffer.is_empty()) {
            stats->push_repeat_digit(key_str);
            if(stats->get_repeat() > 0) return true;
        }
        stats->push_key(key);
        String context = core->current->visual_mode ? "VISUAL" : "NORMAL";
        CommandMatchResult* match_result = match_commands(context, stats, editor_adaptor, core);

        if(!match_result->full.is_empty()) {
            Dictionary command = match_result->full[0];
            int change_num = core->current->text_change_number;
            if(process_command(command, editor_adaptor, core)) {
                stats->clear();
                if(core->current->is_normal_mode()) {
                    core->macro_manager->on_command_processed(command, core->current->text_change_number > change_num);
                }
            }
        }else if(match_result->partial.is_empty()) {
            stats->clear();
        }
        delete match_result;
    }
    return true;
}

CommandMatchResult* CommandDispatcher::match_commands(String context, InputStats* input_state, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    CommandMatchResult* result = new CommandMatchResult();
    Array pressed = input_state->key_codes();

    for(int i=0; i < key_map.size(); i++) {
        if(!is_command_available(key_map[i], context, editor_adaptor, core)) continue;
        Array mapped = Dictionary(key_map[i])["keys"];
        if(mapped.size() > 0 && String(mapped[mapped.size() - 1]) == "{char}") {
            if(pressed.slice(0, -1) == mapped.slice(0, -1) && pressed.size() == mapped.size()) {
                result->full.append(key_map[i]);
            }else if(mapped.slice(0, pressed.size()-1) == pressed.slice(0, -1)) {
                result->partial.append(key_map[i]);
            }else continue;
        }else {
            if(pressed == mapped) {
                result->full.append(key_map[i]);
            }else if(mapped.slice(0, pressed.size()) == pressed) {
                result->partial.append(key_map[i]);
            }else continue;
        }
    }
    return result;
}

bool CommandDispatcher::is_command_available(Dictionary command, String context, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(command.has("context")) {
        String command_context = command["context"];
        if(command_context != context) {
            return false;
        }
    }

    String when = command.get("when", "");
    if(!when.is_empty()) {
        Callable when_callable = Callable(vim_command.ptr(), when);
        if(!when_callable.call(editor_adaptor, core)) return false;
    }

    String when_not = command.get("when_not", "");
    if(!when_not.is_empty()) {
        Callable when_not_callable = Callable(vim_command.ptr(), when_not);
        if(when_not_callable.call(editor_adaptor, core)) return false;
    }

    return true;
}

bool CommandDispatcher::process_command(Dictionary command, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!command.has("type") || !command.has("keys")) {
        ERR_PRINT("에러: 잘못된 명령어 형식입니다.");
        return false;
    }
    Array keys = command["keys"];
    String type = command["type"];
    if(type == "ACTION" && !command.has("action")) {
        ERR_PRINT("에러: ACTION 타입은 action 필드가 필요합니다.");
        return false;
    }
    if((type == "MOTION" || type == "OPERATOR_MOTION") && !command.has("motion")) {
        ERR_PRINT("에러: MOTION 타입은 motion 필드가 필요합니다.");
        return false;
    }
    if((type == "OPERATOR" || type == "OPERATOR_MOTION") && !command.has("operator")) {
        ERR_PRINT("에러: OPERATOR 타입은 operator 필드가 필요합니다.");
        return false;
    }

    Ref<VIMSession> vim_session = core->current;
    InputStats* input_stats = vim_session->input_stats;
    Ref<VimPosition> start = memnew(VimPosition(editor_adaptor->curr_line(), editor_adaptor->curr_column()));

    if(!input_stats->vim_operator.is_empty() && (type != "MOTION" && type != "OPERATOR")) return false;

    if(type == "ACTION") {
        Dictionary action_args = command.get("action_args", Dictionary());
        if(String(keys[keys.size() - 1]) == "{char}") {
            Ref<InputEventKey> key = Object::cast_to<InputEventKey>(input_stats->buffer.back());
            String selected_char = String::chr(key->get_unicode());
            action_args["selected_character"] = selected_char;
        }
        process_action(command["action"], action_args, editor_adaptor, core);
        return true;
    }else if(type == "MOTION" || type == "OPERATOR_MOTION") {
        Dictionary motion_args = command.get("motion_args", Dictionary());
        if(type == "OPERATOR_MOTION") {
            Dictionary operator_args = command.get("operator_args", Dictionary());
            input_stats->vim_operator = command["operator"];
            input_stats->operator_args = operator_args;
        }

        if(String(keys[keys.size() - 1]) == "{char}") {
            Ref<InputEventKey> key = Object::cast_to<InputEventKey>(input_stats->buffer.back());
            String selected_char = String::chr(key->get_unicode());
            motion_args["selected_character"] = selected_char;
        }
        Variant motion_result = process_motion(command["motion"], motion_args, editor_adaptor, core);
        if(motion_result.get_type() == Variant::NIL) return true;

        if(vim_session->visual_mode) {
            Ref<VimPosition> visual_start = vim_session->visual_start_pos->left();
            if(motion_result.get_type() == Variant::OBJECT) {
                Object* obj = motion_result;
                if(obj->is_class("VimTextRange")) {
                    Ref<VimTextRange> text_range = Object::cast_to<VimTextRange>(obj);
                    visual_start = text_range->from;
                    motion_result = text_range->to;
                }
            }
            
            Ref<VimPosition> new_pos = Object::cast_to<VimPosition>(motion_result);
            if(new_pos.is_valid()) {
                if(vim_session->visual_line) {
                    int start_line = visual_start->line;
                    int new_line = new_pos->line;
                    int new_col = 0;
                    
                    if(new_line == editor_adaptor->last_line()) {
                        new_col = editor_adaptor->last_column(new_pos->line) + 1;
                    } else if(visual_start->line >= new_pos->line) {
                        start_line += 1;
                    }
                    
                    editor_adaptor->select(start_line, 0, new_line, new_col);
                } else {
                    editor_adaptor->select_by_pos2(visual_start, new_pos->right());
                }
            }
        }else if(input_stats->vim_operator.is_empty()) {
            if(motion_result.get_type() == Variant::OBJECT) {
                Object* obj = motion_result;
                if(obj->is_class("VimPosition")) {
                    Ref<VimPosition> new_pos = Object::cast_to<VimPosition>(obj);
                    editor_adaptor->jump_to(new_pos->line, new_pos->column);
                }
            }
        }else {
            Ref<VimPosition> motion_end = start;
            if(motion_result.get_type() == Variant::OBJECT) {
                Object* obj = motion_result;
                if(obj->is_class("VimTextRange")) {
                    Ref<VimTextRange> text_range = Object::cast_to<VimTextRange>(obj);
                    start = text_range->from;
                    motion_end = text_range->to;
                } else if(obj->is_class("VimPosition")) {
                    motion_end = Object::cast_to<Ref<VimPosition>>(obj);
                }
            }
            
            if(motion_end.is_valid()) {
                bool inclusive = motion_args.get("inclusive", false);
                if(inclusive) {
                    editor_adaptor->select_by_pos2(start, motion_end->right());
                }else {
                    editor_adaptor->select_by_pos2(start, motion_end);
                }
                
                process_operator(input_stats->vim_operator, input_stats->operator_args, editor_adaptor, core);
            }
        }
        
        return true;
    }else if(type == "OPERATOR") {
        Dictionary operator_args = command.get("operator_args", Dictionary());
        if(core->current->visual_mode) {
            operator_args["line_wise"] = core->current->visual_line;
            process_operator(command["operator"], operator_args, editor_adaptor, core);
            if(operator_args.get("normal_mode_after_visual", true)) core->current->enter_normal_mode();
            return true;
        }else if(input_stats->vim_operator.is_empty()) {
            input_stats->vim_operator = command["operator"];
            input_stats->operator_args = operator_args;
            input_stats->buffer.clear();
            return false;
        }else {
            if(input_stats->vim_operator == String(command["operator"])) {
                operator_args["line_wise"] = true;
                Ref<VimPosition> new_pos = Object::cast_to<VimPosition>(process_motion("expand_to_line", Dictionary(), editor_adaptor, core));
                if(new_pos->compare_to(*start.ptr()) > 0) {
                    editor_adaptor->select(start->line, 0, new_pos->line + 1, 0);
                }else editor_adaptor->select(new_pos->line, 0, start->line + 1, 0);

                process_operator(command["operator"], operator_args, editor_adaptor, core);
                return true;
            }
        }
    }
    return false;
}

void CommandDispatcher::process_action(String action, Dictionary action_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    action_args["repeat"] = MAX(1, core->current->input_stats->get_repeat());
    Callable(vim_command.ptr(), action).call(action_args, editor_adaptor, core);

    if(core->current->visual_mode && action != "enter_visual_mode") {
        core->current->enter_normal_mode();
    }
}

Variant CommandDispatcher::process_motion(String motion, Dictionary motion_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    Ref<VimPosition> cur = memnew(VimPosition(editor_adaptor->curr_line(), editor_adaptor->curr_column()));
    int user_repeat = core->current->input_stats->get_repeat();
    if(user_repeat > 0) {
        motion_args["repeat"] = user_repeat;
        motion_args["repeat_is_explicit"] = true;
    }else {
        motion_args["repeat"] = 1;
        motion_args["repeat_is_explicit"] = false;
    }

    Variant ret = Callable(vim_command.ptr(), motion).call(cur, motion_args, editor_adaptor, core);
    if(ret.get_type() != Variant::OBJECT) return Variant();
    Object* obj = ret;
    if(obj->is_class("VimPosition")) {
        Ref<VimPosition> new_pos = Object::cast_to<VimPosition>(obj);
        if(new_pos->column == INF_COLUMN) {
            new_pos->column = editor_adaptor->last_column(new_pos->line);
        }
        
        if(motion_args.get("to_jump_list", false)) {
            core->current->jump_list->add(cur, new_pos);
        }
    }

    core->current->last_motion = motion;
    return ret;
}

void CommandDispatcher::process_operator(String oper, Dictionary oper_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    Callable(vim_command.ptr(), oper).call(oper_args, editor_adaptor, core);
}