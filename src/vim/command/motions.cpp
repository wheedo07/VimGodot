#include "command.h"
#include "env.h"
#include "../res/gdscript_parser.hpp"
#include "../editor_adaptor.h"
#include "../main.h"

Variant VimCommand::move_by_characters(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    bool one_line = args.get("one_line", false);
    if(!args.has("repeat") && !args.has("forward")) {
        ERR_PRINT("move_by_characters 오류: repeat 또는 forward 인자가 필요합니다.");
        return Variant();
    }
    
    bool forward = args.get("forward", true);
    int repeat = args.get("repeat", 1);
    int columns = cur->column + repeat * (forward ? 1 : -1);
    int line = cur->line;
    
    if(columns > editor_adaptor->last_column(line)) {
        if(one_line || core->current->visual_mode) {
            columns = MAX(0, editor_adaptor->last_column(line));
        } else {
            line += 1;
            if(line > editor_adaptor->last_line()) {
                line = editor_adaptor->last_line();
                columns = editor_adaptor->last_column(line);
            } else {
                columns = 0;
            }
        }
    }else if(columns < 0) {
        if(one_line) {
            columns = 0;
        }else {
            line -= 1;
            if(line < 0) {
                line = 0;
                columns = 0;
            }else columns = editor_adaptor->last_column(line);
        }
    }

    return memnew(VimPosition(line, columns));
}

Variant VimCommand::move_by_scroll(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward")) {
        ERR_PRINT("move_by_scroll 오류: forward 인자가 필요합니다.");
        return Variant();
    }
    int count = editor_adaptor->get_visible_line_count(editor_adaptor->first_visible_line(), editor_adaptor->last_visible_line());
    return memnew(VimPosition(editor_adaptor->next_unfolded_line(cur->line, count / 2, args["forward"]), cur->column));
}

Variant VimCommand::move_by_page(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward")) {
        ERR_PRINT("move_by_page 오류: forward 인자가 필요합니다.");
        return Variant();
    }
    int count = editor_adaptor->get_visible_line_count(editor_adaptor->first_visible_line(), editor_adaptor->last_visible_line());
    return memnew(VimPosition(editor_adaptor->next_unfolded_line(cur->line, count, args["forward"]), cur->column));
}

Variant VimCommand::move_to_column(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("move_to_column 오류: repeat 인자가 필요합니다.");
        return Variant();
    }
    return memnew(VimPosition(cur->line, int(args["repeat"]) - 1));
}

Variant VimCommand::move_by_lines(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat") || !args.has("forward")) {
        ERR_PRINT("move_by_lines 오류: repeat 및 forward 인자가 필요합니다.");
        return Variant();
    }
    int column = cur->column;
    String last_motion = core->current->last_motion;

    if(Array::make("move_by_lines", "move_by_scroll", "move_by_page", 
        "move_to_end_of_line", "move_to_column").has(last_motion)) {
        column = core->current->last_h_pos;
    } else {
        core->current->last_h_pos = column;
    }
    int line = editor_adaptor->next_unfolded_line(cur->line, args["repeat"], args["forward"]);

    if(args.get("to_first_char", false)) {
        column = editor_adaptor->find_first_non_white_space_character(line);
    }

    return memnew(VimPosition(line, column));
}

Variant VimCommand::move_to_first_non_white_space_character(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("change_line") || !args.has("repeat")) {
        ERR_PRINT("move_to_first_non_white_space_character 오류: change_line 및 repeat 인자가 필요합니다.");
        return Variant();
    }
    int new_line = cur->line;
    if(args["change_line"]) {
        new_line += int(args["repeat"]) - 1;
    }
    int i = editor_adaptor->find_first_non_white_space_character(new_line);
    return memnew(VimPosition(new_line, i));
}

Variant VimCommand::move_to_start_of_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    return memnew(VimPosition(cur->line, 0));
}

Variant VimCommand::move_to_end_of_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("move_to_end_of_line 오류: repeat 인자가 필요합니다.");
        return Variant();
    }
    int line = cur->line;
    int repeat = args["repeat"];
    if(repeat > 1) {
        line = editor_adaptor->next_unfolded_line(line, repeat - 1);
    }
    core->current->last_h_pos = INF_COLUMN;

    return memnew(VimPosition(line, editor_adaptor->last_column(line)));
}

Variant VimCommand::move_to_top_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    return memnew(VimPosition(editor_adaptor->first_visible_line(), cur->column));
}

Variant VimCommand::move_to_bottom_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    return memnew(VimPosition(editor_adaptor->last_visible_line(), cur->column));
}

Variant VimCommand::move_to_middle_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    int first = editor_adaptor->first_visible_line();
    int count = editor_adaptor->get_visible_line_count(first, editor_adaptor->last_visible_line());
    return memnew(VimPosition(editor_adaptor->next_unfolded_line(first, count / 2), cur->column));
}

Variant VimCommand::move_to_line_or_edge_of_document(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward") || !args.has("repeat") || !args.has("repeat_is_explicit")) {
        ERR_PRINT("move_to_line_or_edge_of_document 오류: forward, repeat 및 repeat_is_explicit 인자가 필요합니다.");
        return Variant();
    }
    int line = args["forward"] ? editor_adaptor->last_line() : 0;
    if(args["repeat_is_explicit"]) line = int(args["repeat"]) - 1;
    return memnew(VimPosition(line, editor_adaptor->find_first_non_white_space_character(line)));
}

Variant VimCommand::move_by_words(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward") || !args.has("word_end") || !args.has("big_word") || !args.has("repeat")) {
        ERR_PRINT("move_by_words 오류: forward, word_end, big_word 및 repeat 인자가 필요합니다.");
        return Variant();
    }
    int start_line = cur->line;
    int start_column = cur->column;
    Ref<VimPosition> start_pos = cur->duplicate();

    if(start_column > 0 && start_column == editor_adaptor->last_column(start_line) + 1) {
        cur = Ref<VimPosition>(memnew(VimPosition(start_line, start_column - 1)));
    }

    bool forward = args["forward"];
    bool word_end = args["word_end"];
    bool big_word = args["big_word"];
    int repeat = args["repeat"];
    bool empty_line_is_word = !(forward && word_end);
    bool one_line = !core->current->input_stats->vim_operator.is_empty();

    if((forward && !word_end) || (!forward && word_end)) repeat++;

    Array words;
    for(int i=0; i < repeat; i++) {
        Ref<VimTextRange> word = _find_word(cur, editor_adaptor, forward, big_word, empty_line_is_word, one_line);
        if(word != nullptr) {
            words.append(word);
            cur = Ref<VimPosition>(memnew(VimPosition(word->from->line, forward ? word->to->column-1 : word->from->column)));
        }else {
            Ref<VimTextRange> w = forward ? memnew(VimTextRange(editor_adaptor->last_pos(), editor_adaptor->last_pos())) : memnew(VimTextRange());
            words.append(w);
            break;
        }
    }

    bool short_circuit = words.size() != repeat;
    Ref<VimTextRange> first_word = Object::cast_to<VimTextRange>(words[0]);
    Ref<VimTextRange> last_word = Object::cast_to<VimTextRange>(words.pop_back());
    if(forward && !word_end) {
        if(core->current->input_stats->vim_operator == "change") {
            if(!short_circuit) last_word = words.pop_back();
            return last_word->to;
        }

        if(!short_circuit && !start_pos->equals(*first_word->from.ptr())) last_word = words.pop_back();
        return last_word->from;
    }else if(forward && word_end) {
        return last_word->to->left();
    }else if(!forward && word_end) {
        if(!short_circuit && !start_pos->equals(*first_word->to->left().ptr())) last_word = words.pop_back();
        return last_word->to->left();
    }else {
        return last_word->from;
    }
}

Variant VimCommand::move_to_matched_symbol(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    Ref<VimCharPos> symbol = editor_adaptor->find_forward(cur->line, cur->column, [](Ref<VimCharPos> p) {
        String c = p->get_character();
        return Array::make("(",")","{","}","[","]").has(c);
    }, true);
    if(!symbol.is_valid()) return Variant();
    String counter_part = SYMBOLS[symbol->get_character()];

    Array from_positions = Array::make(memnew(VimPosition(symbol->line, 0)), memnew(VimPosition));
    for(int i=0; i < from_positions.size(); i++) {
        Ref<VimPosition> from = Object::cast_to<VimPosition>(from_positions[i]);
        GDScriptParser parser = GDScriptParser(editor_adaptor, from, symbol->get_character());
        if(!parser.parse_until(symbol)) continue;

        String symbol_char = symbol->get_character();
        if(Array::make(")", "]", "}").has(symbol_char)) {
            parser.stack.reverse();
            for(int j=0; j < parser.stack.size(); j++) {
                Object* stack_obj = parser.stack[j];
                if(stack_obj) {
                    Ref<VimCharPos> p = Object::cast_to<VimCharPos>(stack_obj);
                    if(p.is_valid() && p->get_character() == counter_part) {
                        return memnew(VimPosition(p->line, p->column));
                    }
                }
            }
            continue;
        } else {
            parser.parse_one_char();
            Ref<VimPosition> match_pos = parser.find_matching();
            return match_pos;
        }
    } 
    return Variant();
}

Variant VimCommand::move_to_next_char(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward") || !args.has("selected_character") || !args.has("repeat")) {
        ERR_PRINT("move_to_next_char 오류: forward, selected_character 및 repeat 인자가 필요합니다.");
        return Variant();
    }
    core->last_char_search = args;
    bool forward = args["forward"];
    String selected_character = args["selected_character"];
    int repeat = args["repeat"];
    bool stop_before = args.get("stop_before", false);

    int start_col = cur->column + (forward ? 1 : -1);
    Ref<VimPosition> old_pos = memnew(VimPosition(cur->line, cur->column));
   
    CharIterator* it = editor_adaptor->chars(cur->line, start_col, forward, true);
    print_line("wow");
    if(!it->init()) {
        return Variant();
    }
    print_line("wow2");
    
    do {
        Ref<VimCharPos> ch = it->get();
        if (ch->get_character() == selected_character) {
            repeat--;
            if (repeat == 0) {
                if (stop_before) {
                    return old_pos;
                } else {
                    return memnew(VimPosition(ch->line, ch->column));
                }
            }
        }
        old_pos = Ref<VimPosition>(memnew(VimPosition(ch->line, ch->column)));
    } while(it->next());
    
    return Variant();
}

Variant VimCommand::repeat_last_char_search(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    Dictionary last_char_search = core->last_char_search;
    if(last_char_search.is_empty()) return Variant();

    args["forward"] = last_char_search["forward"];
    args["selected_character"] = last_char_search["selected_character"];
    args["repeat"] = last_char_search["repeat"];
    args["stop_before"] = last_char_search.get("stop_before", false);
    args["inclusive"] = last_char_search.get("inclusive", false);
    return move_to_next_char(cur, args, editor_adaptor, core);
}

Variant VimCommand::expand_to_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("expand_to_line 오류: repeat 인자가 필요합니다.");
        return Variant();
    }
    int repeat = args["repeat"];
    return memnew(VimPosition(cur->line + repeat - 1, INF_COLUMN));
}

Variant VimCommand::find_word_under_caret(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward")) {
        ERR_PRINT("find_word_under_caret 오류: forward 인자가 필요합니다.");
        return Variant();
    }
    bool forward = args["forward"];
    Ref<VimTextRange> range = editor_adaptor->get_word_at_pos(cur->line, cur->column);
    String text = editor_adaptor->range_text(range);
    Ref<VimPosition> pos = editor_adaptor->search(text, cur->line, cur->column + (forward ? 1 : -1), false, true, forward);
    core->last_search_command = forward ? "*" : "#";
    core->search_buffer = text;
    return pos;
}

Variant VimCommand::find_again(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("forward")) {
        ERR_PRINT("find_again 오류: forward 인자가 필요합니다.");
        return Variant();
    }
    bool forward = args["forward"];
    forward = forward == (core->last_search_command != "#");
    bool option = Array::make("#", "*").has(core->last_search_command);
    cur = forward ? editor_adaptor->offset_pos(cur, 1) : editor_adaptor->offset_pos(cur, -1);
    return editor_adaptor->search(core->search_buffer, cur->line, cur->column, option, option, forward);
}

Variant VimCommand::text_object(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("inner") || !args.has("object")) {
        ERR_PRINT("text_object 오류: inner 및 object 인자가 필요합니다.");
        return Variant();
    }
    bool inner = args["inner"];
    String object = args["object"];
    if(object == "w" && inner) return editor_adaptor->get_word_at_pos(cur->line, cur->column);

    if(Array::make("(", "[", "{", "\"", "'").has(object)) {
        String counter_part = SYMBOLS[object];
        
        Array from_positions = Array::make(memnew(VimPosition(cur->line, 0)), memnew(VimPosition(0,0)));
        for(int i=0; i < from_positions.size(); i++) {
            Ref<VimPosition> from = Object::cast_to<VimPosition>(from_positions[i]);
            GDScriptParser parser = GDScriptParser(editor_adaptor, from, object);
            if(!parser.parse_until(cur)) continue;
            
            Ref<VimTextRange> range = memnew(VimTextRange);
            if(parser.get_stack_top_char() == object) {
                range->from = parser.stack.back();
                range->to = parser.find_matching();
            }else if(editor_adaptor->char_at(cur->line, cur->column) == object) {
                parser.parse_one_char();
                range->from = parser.pos;
                range->to = parser.find_matching();
            }else continue;

            if(inner) {
                range->from = editor_adaptor->offset_pos(range->from, 1);
            }else range->to = editor_adaptor->offset_pos(range->to, 1);

            return range;
        }
    }
    return Variant();
}

Variant VimCommand::go_to_bookmark(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("selected_character")) {
        ERR_PRINT("go_to_bookmark 오류: selected_character 인자가 필요합니다.");
        return Variant();
    }
    int line = core->current->bookmark_manager->get_bookmark(args["selected_character"]);
    if(line < 0) return Variant();
    return memnew(VimPosition(line, 0));
}