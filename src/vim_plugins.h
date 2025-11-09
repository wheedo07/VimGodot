#ifndef __VIM_PLUGINS_H__
#define __VIM_PLUGINS_H__

#include<godot_cpp/classes/editor_plugin.hpp>
#include<godot_cpp/classes/editor_interface.hpp>
#include "vim/main.h"
#include "vim/editor_adaptor.h"
#include "vim/command_dispatcher.h"
#include "vim/session.h"
namespace godot {
    class VimEditorPlugin : public EditorPlugin {
        GDCLASS(VimEditorPlugin, EditorPlugin);

        protected:
            static void _bind_methods();

        private:
            Array command_whitelist;
            EditorInterface* editor_interface;
            CommandDispatcher* command_dispatcher;
            Ref<VimCore> core;
            Ref<EditorAdaptor> editor_adaptor;

            void _on_script_changed(Ref<Script> script);
            void _on_script_closed(Ref<Script> script);
            void _on_search_text_changed(String text);
            void _on_caret_changed();
            void on_lines_edited_from(int form, int to);
            Node* find_first_node_of_type(Node* q, String type);

        public:
            VimEditorPlugin();

            void _enter_tree() override;
            void _exit_tree() override;
            void _input(const Ref<InputEvent> &p_event) override; 

            void init();
    };
};

#endif