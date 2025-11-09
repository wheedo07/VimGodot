#ifndef __MACRO_MANAGER_H__
#define __MACRO_MANAGER_H__

#include<godot_cpp/variant/dictionary.hpp>
#include<godot_cpp/variant/string.hpp>
#include<godot_cpp/variant/array.hpp>
#include<godot_cpp/classes/input_event_key.hpp>
namespace godot {
    class VimCore;
    class EditorAdaptor;
    class MacroManager {
        private:
            Ref<VimCore> vim;
            Dictionary macros;
            String recording_name;
            Array playing_names;
            Array command_buffer;

        public:
            MacroManager(Ref<VimCore> p_vim);

            void start_record_macro(String name);
            void stop_record_macro();
            bool is_recording();
            void play_macro(int n, String name, Ref<EditorAdaptor> editor_adaptor);
            void on_macro_finished(Ref<EditorAdaptor> editor_adaptor);
            void push_key(Ref<InputEventKey> key_event);
            void on_command_processed(Dictionary command, bool is_edit);
    };
}

#endif