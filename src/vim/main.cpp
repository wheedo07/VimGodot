#include "main.h"
#include "macro_manager.h"
#include<godot_cpp/classes/input.hpp>
using namespace godot;

VimCore::VimCore() {
    registers = new Register();
    macro_manager = new MacroManager(this);
}

void VimCore::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_off_press_key", "key_event"), &VimCore::_off_press_key);
}

void VimCore::set_current_session(Ref<Script> script, Ref<EditorAdaptor> editor_adaptor) {
    Ref<VIMSession> sess = Object::cast_to<VIMSession>(sessions[script]);
    if(!sess.is_valid()) {
        sess.instantiate();
        sess->editor_adaptor = editor_adaptor;
        sessions[script] = sess;
    }
    current = sess;
}

void VimCore::remove_session(Ref<Script> script) {
    sessions.erase(script);
}

void VimCore::press_key(Ref<InputEventKey> key_event) {
    Ref<InputEventKey> event = key_event->duplicate();
    event->set_pressed(true);
    Input::get_singleton()->parse_input_event(event);
    call_deferred("_off_press_key", event);
}

void VimCore::_off_press_key(Ref<InputEventKey> key_event) {
    Ref<InputEventKey> event = key_event->duplicate();
    event->set_pressed(false);
    Input::get_singleton()->parse_input_event(event);
}