#ifndef __VIM_H__
#define __VIM_H__

#include<godot_cpp/classes/ref_counted.hpp>
#include<godot_cpp/classes/editor_plugin.hpp>
#include<godot_cpp/classes/script.hpp>
#include "res/register.hpp"
#include "macro_manager.h"
#include "editor_adaptor.h"
#include "session.h"
namespace godot {
    class VimCore : public RefCounted {
        GDCLASS(VimCore, RefCounted);

        protected:
            static void _bind_methods();

        public:
            Ref<VIMSession> current;
            MacroManager* macro_manager;
            Dictionary last_char_search;
            String last_search_command;
            String search_buffer;
            Register* registers;

        private:
            Dictionary sessions;

            void _off_press_key(Ref<InputEventKey> key_event);

        public:
            VimCore();

            void press_key(Ref<InputEventKey> key_event);
            void set_current_session(Ref<Script> script, Ref<EditorAdaptor> editor_adaptor);
            void remove_session(Ref<Script> script);
    };
}

#endif