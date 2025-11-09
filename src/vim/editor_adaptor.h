#ifndef __EDITOR_ADAPTOR_H__
#define __EDITOR_ADAPTOR_H__

#include<godot_cpp/classes/code_edit.hpp>
#include<godot_cpp/classes/editor_settings.hpp>
#include<godot_cpp/classes/input_event_key.hpp>
#include<functional>
#include "main.h"
#include "res/text_range.hpp"
#include "char_iterator.h"
using namespace std;
namespace godot {
    class EditorAdaptor : public RefCounted {
        GDCLASS(EditorAdaptor, RefCounted);

        protected:
            static void _bind_methods();

        private:
            Ref<VimCore> vim;
            int complex_ops;

        public:
            CodeEdit* code_edit;

            EditorAdaptor();
            void set_vim_core(Ref<VimCore> p_vim);

            void set_code_editor(CodeEdit* editor);
            Ref<VimPosition> curr_position();
            void set_curr_column(int col);
            void jump_to(int line, int column);
            void update_margin(int line);
            int next_unfolded_line(int line, int offset, bool forward=true);
            Ref<VimPosition> offset_pos(Ref<VimPosition> pos, int offset);
            String range_text(Ref<VimTextRange> range);
            String char_at(int line, int column);
            void set_block_caret(bool block);
            void set_caret_blink(bool blink);
            void deselect();
            void select(int from_line, int from_column, int to_line, int to_column);
            void select_range(Ref<VimTextRange> range);
            void select_by_pos2(Ref<VimPosition> from, Ref<VimPosition> to);
            void delete_selection();
            Ref<VimTextRange> selection();
            void begin_complex_operation();
            void end_complex_operation();
            void insert_text(String text);
            void replace_selection(String text);
            void toggle_folding(int line);
            void fold_all();
            void unfold_all();
            void undo();
            void redo();
            void indent();
            void unindent();
            void simulate_press_key(Ref<InputEventKey> key_event);
            void simulate_press(Key key, int unicode=0, bool ctrl=false, bool alt=false, bool shift=false, bool meta=false);
            int find_first_non_white_space_character(int line);
            Ref<VimCharPos> find_forward(int line, int col, function<bool(Ref<VimCharPos>)> condition, bool one_line = false);
            Ref<VimCharPos> find_backward(int line, int col, function<bool(Ref<VimCharPos>)> condition, bool one_line = false);
            Ref<VimTextRange> get_word_at_pos(int line, int column);
            Ref<VimPosition> search(String text, int line, int column, bool match_case, bool whole_word, bool forward);
            bool has_focus();
            bool is_alphanumeric(String text);
            bool is_breaker(String text);
            bool is_lower_alpha(String text);

            int get_visible_line_count(int from_line, int to_line);
            int curr_line();
            int curr_column();
            int first_visible_line();
            int last_visible_line();
            int last_line();
            int last_column(int line=-1);
            String line_text(int line);
            Ref<VimPosition> last_pos();
            CharIterator* chars(int line, int column, bool forward=true, bool one_line=false);
            String selected_text();
    };
}

#endif