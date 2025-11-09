#ifndef __COMMAND_H__
#define __COMMAND_H__

#include<godot_cpp/classes/ref_counted.hpp>
#include "../res/text_range.hpp"
namespace godot {
    class VimCore;
    class EditorAdaptor;

    class VimCommand : public RefCounted {
        GDCLASS(VimCommand, RefCounted);

        protected:
            static void _bind_methods();

        private:
            Dictionary SYMBOLS;
            Ref<VimTextRange> _find_word(Ref<VimPosition> cur, Ref<EditorAdaptor> editor_adaptor, bool forward, bool big_word, bool empty_line_is_word, bool one_line);

        public:
            VimCommand();

            // [MOTIONS] //
            Variant move_by_characters(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_by_scroll(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_by_page(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_column(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_by_lines(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_first_non_white_space_character(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_start_of_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_end_of_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_top_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_bottom_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_middle_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_line_or_edge_of_document(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_by_words(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_matched_symbol(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant move_to_next_char(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant repeat_last_char_search(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant expand_to_line(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant find_word_under_caret(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant find_again(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant text_object(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            Variant go_to_bookmark(Ref<VimPosition> cur, Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            // [end MOTIONS] //

            // [OPERATORS] //
            void deletes(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void yank(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void change(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void change_case(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void toggle_case(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void delete_and_enter_insert_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            // [end OPERATORS] //

            // [ACTIONS] //
            void paste(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void undo(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void redo(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void replace(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void enter_insert_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void enter_visual_mode(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void search(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void jump_list_walk(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void toggle_folding(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void fold_all(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void unfold_all(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void repeat_last_edit(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void record_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void stop_record_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void play_macro(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void indent(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void join_lines(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void set_bookmark(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            void go_to_doc(Dictionary args, Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            // [end ACTIONS] //

            // [CONDITION] //
            bool is_recording(Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core);
            // [end CONDITION] //
    };
}

#endif