#include "macro_manager.h"
#include "main.h"
#include "env.h"
#include "res/macro.hpp"
using namespace godot;

MacroManager::MacroManager(Ref<VimCore> p_vim) {
    vim = p_vim;
    recording_name = "";
}

void MacroManager::start_record_macro(String name) {
    recording_name = name;
    macros[name] = memnew(VimMacro);
}

void MacroManager::stop_record_macro() {
    VimMacro* macro = Object::cast_to<VimMacro>(macros[recording_name]);
    macro->enabled = true;
    recording_name = "";
}

bool MacroManager::is_recording() {
    return recording_name != "";
}

void MacroManager::play_macro(int n, String name, Ref<EditorAdaptor> editor_adaptor) {
    VimMacro* macro = Object::cast_to<VimMacro>(macros[name]);
    if(!macro || !macro->enabled) return;
    if(playing_names.has(name)) return;

    playing_names.append(name);
    if(playing_names.size() == 1) {
        editor_adaptor->begin_complex_operation();
    }

    for(int i=0; i < n; i++) {
        macro->play(editor_adaptor);
    }

    editor_adaptor->simulate_press(Key::KEY_NONE, CODE_MACRO_PLAY_END);
}

void MacroManager::on_macro_finished(Ref<EditorAdaptor> editor_adaptor) {
    playing_names.pop_back();
    if(playing_names.is_empty()) {
        editor_adaptor->end_complex_operation();
    }
}

void MacroManager::push_key(Ref<InputEventKey> key_event) {
    command_buffer.append(key_event);
    if(!recording_name.is_empty()) {
        VimMacro* macro = Object::cast_to<VimMacro>(macros[recording_name]);
        macro->keys.append(key_event);
    }
}

void MacroManager::on_command_processed(Dictionary command, bool is_edit) {
    String action = command.get("action", "");
    if(is_edit && action != "repeat_last_edit") {
        VimMacro* macro = memnew(VimMacro);
        macro->keys = command_buffer.duplicate();
        macro->enabled = true;
        macros["."] = macro;
    }
    command_buffer.clear();
}