#ifndef __MACRO_H__
#define __MACRO_H__

#include<godot_cpp/classes/ref_counted.hpp>
#include<godot_cpp/classes/input_event_key.hpp>
#include "../editor_adaptor.h"
using namespace godot;
class VimMacro : public RefCounted {
    GDCLASS(VimMacro, RefCounted);

    protected:
        static void _bind_methods() {};

    public:
        Array keys;
        bool enabled = false;

        String _to_string() {
            PackedStringArray s;
            for(int i=0; i < keys.size(); i++) {
                Ref<InputEventKey> key = keys[i];
                s.append(key->as_text_keycode());
            }
            return String(",").join(s);
        }

        void play(Ref<EditorAdaptor> editor_adaptor) {
            for(int i=0; i < keys.size(); i++) {
                Ref<InputEventKey> key = keys[i];
                editor_adaptor->simulate_press_key(key);
            }
            editor_adaptor->simulate_press(Key::KEY_ESCAPE);
        }
};

#endif