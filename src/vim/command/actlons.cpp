#include "command.h"
#include "env.h"
#include "../res/gdscript_parser.hpp"
#include "../editor_adaptor.h"
#include "../main.h"
#include<godot_cpp/classes/os.hpp>

void VimCommand::paste(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("after") || !args.has("repeat")) {
        ERR_PRINT("paste 오류: after 및 repeat 인자가 필요합니다.");
        return;
    }
    bool after = args["after"];
    int repeat = args["repeat"];
    bool line_wise = core->registers->line_wise;
    String clipboard_text = core->registers->text;

    String text = "";
    for(int i=0; i < repeat; i++) {
        text += clipboard_text;
    }

    int line = editor_adaptor->curr_line();
    int column = editor_adaptor->curr_column();

    if(line_wise) {
        if(after) {
            text = "\n" + text.substr(0, text.length() - 1);
            column = editor_adaptor->line_text(line).length();
        }else column = 0;
    }else {
        column += after ? 1 : 0;
    }

    editor_adaptor->set_curr_column(column);
    editor_adaptor->insert_text(text);
}

void VimCommand::undo(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("undo 오류: repeat 인자가 필요합니다.");
        return;
    }
    int repeat = args["repeat"];
    for(int i=0; i < repeat; i++) {
        editor_adaptor->undo();
    }
    editor_adaptor->deselect();
}

void VimCommand::redo(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("redo 오류: repeat 인자가 필요합니다.");
        return;
    }
    int repeat = args["repeat"];
    for(int i=0; i < repeat; i++) {
        editor_adaptor->redo();
    }
    editor_adaptor->deselect();
}

void VimCommand::replace(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("selected_character")) {
        ERR_PRINT("replace 오류: selected_character 인자가 필요합니다.");
        return;
    }
    String to_replace = args["selected_character"];
    int line = editor_adaptor->curr_line();
    int column = editor_adaptor->curr_column();
    editor_adaptor->select(line, column, line, column+1);
    editor_adaptor->replace_selection(to_replace);
}

void VimCommand::enter_insert_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("insert_at")) {
        ERR_PRINT("enter_insert_mode 오류: insert_at 인자가 필요합니다.");
        return;
    }
    int line = editor_adaptor->curr_line();
    int column = editor_adaptor->curr_column();
    String insert_at = args["insert_at"];

    core->current->enter_insert_mode();

    if(insert_at == "inplace") {
    }else if(insert_at == "after") {
        editor_adaptor->set_curr_column(column + 1);
    }else if(insert_at == "bol") {
        editor_adaptor->set_curr_column(editor_adaptor->find_first_non_white_space_character(line));
    }else if(insert_at == "eol") {
        editor_adaptor->set_curr_column(INF_COLUMN);
    }else if(insert_at == "new_line_below") {
        editor_adaptor->set_curr_column(INF_COLUMN);
        editor_adaptor->simulate_press(Key::KEY_ENTER);
    }else if(insert_at == "new_line_above") {
        editor_adaptor->set_curr_column(0);
        if(line == 0) {
            editor_adaptor->insert_text("\n");
            editor_adaptor->jump_to(0, 0);
        }else {
            editor_adaptor->jump_to(line - 1, INF_COLUMN);
            editor_adaptor->simulate_press(Key::KEY_ENTER);
        }
    }
}

void VimCommand::enter_visual_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    bool line_wise = args.get("line_wise", false);
    core->current->enter_visual_mode(line_wise);
}

void VimCommand::search(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(OS::get_singleton()->get_name() == "macOS") {
        editor_adaptor->simulate_press(Key::KEY_F, 0, false, false, false, true);
    }else editor_adaptor->simulate_press(Key::KEY_F, 0, true, false, false, false);
    core->last_search_command = "/";
}

void VimCommand::jump_list_walk(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat") || !args.has("forward")) {
        ERR_PRINT("jump_list_walk 오류: repeat 및 forward 인자가 필요합니다.");
        return;
    }
    int repeat = args["repeat"];
    bool forward = args["forward"];

    int offset = repeat * (forward ? 1 : -1);
    Ref<VimPosition> pos = core->current->jump_list->move(offset);
    if(pos != nullptr) {
        if(!forward) core->current->jump_list->set_next(editor_adaptor->curr_position());
        editor_adaptor->jump_to(pos->line, pos->column);
    }
}

void VimCommand::toggle_folding(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    editor_adaptor->toggle_folding(editor_adaptor->curr_line());
}

void VimCommand::fold_all(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    editor_adaptor->fold_all();
}

void VimCommand::unfold_all(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    editor_adaptor->unfold_all();
}

void VimCommand::repeat_last_edit(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("repeat_last_edit 오류: repeat 인자가 필요합니다.");
        return;
    }
    core->macro_manager->play_macro(args["repeat"], ".", editor_adaptor);
}

void VimCommand::record_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("selected_character")) {
        ERR_PRINT("record_macro 오류: selected_character 인자가 필요합니다.");
        return;
    }
    String name = args["selected_character"];
    if(editor_adaptor->is_alphanumeric(name)) {
        core->macro_manager->start_record_macro(name);
    }
}

void VimCommand::stop_record_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    core->macro_manager->stop_record_macro();
}

void VimCommand::play_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("selected_character") || !args.has("repeat")) {
        ERR_PRINT("play_macro 오류: selected_character 및 repeat 인자가 필요합니다.");
        return;
    }
    String name = args["selected_character"];
    int repeat = args["repeat"];
    if(editor_adaptor->is_alphanumeric(name)) {
        core->macro_manager->play_macro(repeat, name, editor_adaptor);
    }
}

void VimCommand::indent(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("indent 오류: repeat 인자가 필요합니다.");
        return;
    }
    int repeat = args["repeat"];
    bool forward = args.get("forward", false);
    Ref<VimTextRange> range = editor_adaptor->selection();
    if(!range->is_single_line() && range->to->column == 0) {
        editor_adaptor->select(range->from->line, range->from->column, range->to->line-1, INF_COLUMN);
    }
    editor_adaptor->begin_complex_operation();
    for(int i=0; i < repeat; i++) {
        if(forward) editor_adaptor->indent();
        else editor_adaptor->unindent();
    }
    editor_adaptor->end_complex_operation();
}

void VimCommand::join_lines(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("repeat")) {
        ERR_PRINT("join_lines 오류: repeat 인자가 필요합니다.");
        return;
    }
    int repeat = args["repeat"];

    if(core->current->is_normal_mode()) {
        int line = editor_adaptor->curr_line();
        editor_adaptor->select(line, 0, line + repeat, INF_COLUMN);
    }
    Ref<VimTextRange> range = editor_adaptor->selection();
    editor_adaptor->select(range->from->line, 0, range->to->line, INF_COLUMN);

    PackedStringArray s = { editor_adaptor->line_text(range->from->line) };
    for(int line = range->from->line + 1; line <= range->to->line; line++) {
        String line_text = editor_adaptor->line_text(line);
        line_text = line_text.lstrip(" \t\n");
        s.append(line_text);
    }
    
    String joined_text = String(" ").join(s);
    editor_adaptor->replace_selection(joined_text);
}

void VimCommand::set_bookmark(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    if(!args.has("selected_character")) {
        ERR_PRINT("set_bookmark 오류: selected_character 인자가 필요합니다.");
        return;
    }
    String name = args["selected_character"];
    if(editor_adaptor->is_lower_alpha(name)) {
        core->current->bookmark_manager->set_bookmark(name, editor_adaptor->curr_line());
    }
}

void VimCommand::go_to_doc(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->line_text(editor_adaptor->curr_line());
    int begin = text.rfind("res://", editor_adaptor->curr_column());
    String symbol = text.substr(begin - 1, 1);
    int end = text.find(symbol, begin);
    String search_word;

    if(editor_adaptor->curr_column() <= end) {
        search_word = text.substr(begin, end - begin);
    }else {
        search_word = editor_adaptor->code_edit->get_word_under_caret();
    }
    editor_adaptor->code_edit->emit_signal("symbol_lookup", search_word, editor_adaptor->curr_line(), editor_adaptor->curr_column());
}