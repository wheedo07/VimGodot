#include "vim_plugins.h"
#include "env.h"
#include<godot_cpp/classes/json.hpp>
#include<godot_cpp/classes/file_access.hpp>
#include<godot_cpp/classes/script_editor.hpp>
#include<godot_cpp/classes/script_editor_base.hpp>
#include<godot_cpp/classes/line_edit.hpp>
#include<godot_cpp/classes/viewport.hpp>
#include<godot_cpp/classes/engine.hpp>
using namespace godot;

VimEditorPlugin::VimEditorPlugin() {
    command_dispatcher = nullptr;
    core.instantiate();
    editor_adaptor.instantiate();

    editor_adaptor->set_vim_core(core);
}

void VimEditorPlugin::_bind_methods() {
    ClassDB::bind_method(D_METHOD("init"), &VimEditorPlugin::init);
}

void VimEditorPlugin::_enter_tree() {
    call_deferred("init");
}

void VimEditorPlugin::_exit_tree() {}

void VimEditorPlugin::init() {
    _reload_settings();
    editor_interface = get_editor_interface();
    ScriptEditor* editor = editor_interface->get_script_editor();
    editor->connect("editor_script_changed", callable_mp(this, &VimEditorPlugin::_on_script_changed));
    editor->connect("script_close", callable_mp(this, &VimEditorPlugin::_on_script_closed));
    _on_script_changed(editor->get_current_script());

    Node* find_bar = find_first_node_of_type(editor, "FindReplaceBar");
    LineEdit* find_bar_line_edit = Object::cast_to<LineEdit>(find_first_node_of_type(find_bar, "LineEdit"));
    find_bar_line_edit->connect("text_changed", callable_mp(this, &VimEditorPlugin::_on_search_text_changed));

    EditorCommandPalette* command_palette = editor_interface->get_command_palette();
    command_palette->add_command("VimGodot: Reload Settings", "VimGodot", callable_mp(this, &VimEditorPlugin::_reload_settings), String::utf8("VimGodot의 설정 파일을 다시 불러옵니다"));
}

void VimEditorPlugin::_reload_settings() {
    Ref<FileAccess> key_file = FileAccess::open(KEY_PATH, FileAccess::READ);
    Ref<FileAccess> white_list_file = FileAccess::open(WHITE_LIST_PATH, FileAccess::READ);
    if(!key_file.is_valid() || !white_list_file.is_valid()) {
        ERR_PRINT("VimGodot: vim을 위한 설정 파일을 열 수 없습니다");
        return;
    }
    command_dispatcher = new CommandDispatcher(JSON::parse_string(key_file->get_as_text()));
    command_whitelist = JSON::parse_string(white_list_file->get_as_text());
    print_line(String::utf8("VimGodot: 설정 파일을 불러왔습니다"));
}

void VimEditorPlugin::_input(const Ref<InputEvent> &event) {
    if(!command_dispatcher || !event->is_class("InputEventKey")) return;
    Ref<InputEventKey> key = event;
    if(!key.is_valid() || !key->is_pressed() || !editor_adaptor->has_focus()) return;

    if(key->get_keycode_with_modifiers() == Key::KEY_NONE && key->get_unicode() == CODE_MACRO_PLAY_END) {
        core->macro_manager->on_macro_finished(editor_adaptor);
		get_viewport()->set_input_as_handled();
        return;
    }

    String key_code = key->as_text_keycode();
    if(command_whitelist.has(key_code)) return;

    if(command_dispatcher->dispatch(key, core, editor_adaptor)) {
        get_viewport()->set_input_as_handled();
    }
}

void VimEditorPlugin::_on_script_changed(Ref<Script> script) {
    core->set_current_session(script, editor_adaptor);

    ScriptEditor* editor = editor_interface->get_script_editor();
    ScriptEditorBase* scrpit_editor_base = editor->get_current_editor();
    if(scrpit_editor_base && ObjectDB::get_instance(scrpit_editor_base->get_instance_id())) {
        CodeEdit* code_editor = Object::cast_to<CodeEdit>(scrpit_editor_base->get_base_editor());
        editor_adaptor->set_code_editor(code_editor);
        editor_adaptor->set_block_caret(true);
        editor_adaptor->set_caret_blink(false);

        Callable caret_changed = callable_mp(this, &VimEditorPlugin::_on_caret_changed);
        if(!code_editor->is_connected("caret_changed", caret_changed)) {
            code_editor->connect("caret_changed", caret_changed);
        }

        Callable lines_edited_from = callable_mp(this, &VimEditorPlugin::on_lines_edited_from);
        if(!code_editor->is_connected("lines_edited_from", lines_edited_from)) {
            code_editor->connect("lines_edited_from", lines_edited_from);
        }
    }
}

void VimEditorPlugin::_on_script_closed(Ref<Script> script) {
    core->remove_session(script);
}

void VimEditorPlugin::on_lines_edited_from(int from, int to) {
    core->current->jump_list->on_lines_edited(from, to);
    core->current->text_change_number++;
    core->current->bookmark_manager->on_lines_edited(from, to);
}

void VimEditorPlugin::_on_search_text_changed(String text) {
    core->search_buffer = text;
}

void VimEditorPlugin::_on_caret_changed() {
    editor_adaptor->set_block_caret(!core->current->insert_mode);
}

Node* VimEditorPlugin::find_first_node_of_type(Node* q, String type) {
    if(q->is_class(type)) {
        return q;
    }
    for(int i=0; i < q->get_child_count(); i++) {
        Node* child = Object::cast_to<Node>(q->get_child(i));
        if(child) {
            Node* res = find_first_node_of_type(child, type);
            if(res) {
                return res;
            }
        }
    }
    return nullptr;
}