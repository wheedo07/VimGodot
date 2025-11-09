#include "command.h"
#include "env.h"
#include "../res/gdscript_parser.hpp"
#include "../editor_adaptor.h"
#include "../main.h"

void VimCommand::deletes(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->selected_text();
    core->registers->set_text(text, args.get("line_wise", false));
    editor_adaptor->delete_selection();

    int line = editor_adaptor->curr_line();
    int column = editor_adaptor->curr_column();
    if(column > editor_adaptor->last_column(line)) {
        editor_adaptor->set_curr_column(editor_adaptor->last_column(line));
    }
}

void VimCommand::yank(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->selected_text();
    editor_adaptor->deselect();
    core->registers->set_text(text, args.get("line_wise", false));
}

void VimCommand::change(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->selected_text();
    core->registers->set_text(text, args.get("line_wise", false));

    editor_adaptor->delete_selection();
    core->current->enter_insert_mode();
}

void VimCommand::change_case(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    bool lower_case = args.get("lower", false);
    String text = editor_adaptor->selected_text();
    editor_adaptor->replace_selection(lower_case ? text.to_lower() : text.to_upper());
}

void VimCommand::toggle_case(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->selected_text();
    PackedStringArray s;
    for (int i = 0; i < text.length(); i++) {
        String c = text.substr(i, 1);
        if(c == c.to_upper()) s.append(c.to_lower());
        else s.append(c.to_upper());
    }

    editor_adaptor->replace_selection(String("").join(s));
}

void VimCommand::delete_and_enter_insert_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    String text = editor_adaptor->selected_text();
    core->registers->set_text(text, args.get("line_wise", false));

    editor_adaptor->delete_selection();
    int line = editor_adaptor->curr_line();
    int column = editor_adaptor->curr_column();
    if(column > editor_adaptor->last_column(line)) {
        editor_adaptor->set_curr_column(editor_adaptor->last_column(line));
    }
    core->current->enter_insert_mode();
}