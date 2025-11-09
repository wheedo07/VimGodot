#ifndef __COMMAND_DISPATCHER_H__
#define __COMMAND_DISPATCHER_H__

#include<godot_cpp/variant/dictionary.hpp>
#include<godot_cpp/variant/array.hpp>
#include<godot_cpp/classes/input_event_key.hpp>
#include "res/input_stats.hpp"
namespace godot {
    class EditorAdaptor;
    class VimCore;
    class VimCommand;

    class CommandMatchResult {
        public:
            Array full;
            Array partial;
    };

    class CommandDispatcher {
        private:
            Ref<VimCommand> vim_command;
            Array key_map;

        public:
            CommandDispatcher(Array p_key_map);

            bool dispatch(Ref<InputEventKey> key, Ref<VimCore> core, Ref<EditorAdaptor> editor_adaptor);
            CommandMatchResult* match_commands(String context, InputStats* input_state, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);

            bool process_command(Dictionary command, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void process_action(String action, Dictionary action_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void process_operator(String oper, Dictionary oper_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant process_motion(String motion, Dictionary motion_args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            bool is_command_available(Dictionary command, String context, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
    };
}

#endif