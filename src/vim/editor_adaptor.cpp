#include "editor_adaptor.h"
#include "res/position.hpp"
#include "session.h"
#include "env.h"
#include<godot_cpp/classes/input.hpp>
using namespace godot;

EditorAdaptor::EditorAdaptor() {
    code_edit = nullptr;
    complex_ops = 0;
}

void EditorAdaptor::_bind_methods() {}

void EditorAdaptor::set_vim_core(Ref<VimCore> p_vim) {
    vim = p_vim;
}

void EditorAdaptor::set_code_editor(CodeEdit* editor) {
    code_edit = editor;
}

Ref<VimPosition> EditorAdaptor::curr_position() {
    return memnew(VimPosition(code_edit->get_caret_line(), code_edit->get_caret_column()));
}

int EditorAdaptor::curr_line() {
    return code_edit->get_caret_line();
}

int EditorAdaptor::first_visible_line() {
    return code_edit->get_first_visible_line();
}

int EditorAdaptor::last_visible_line() {
    return code_edit->get_last_full_visible_line();
}

int EditorAdaptor::last_line() {
    return code_edit->get_line_count() - 1;
}

int EditorAdaptor::curr_column() {
    int col = code_edit->get_caret_column();
    if(vim->current->visual_mode) col-=1;
    return col;
}

int EditorAdaptor::get_visible_line_count(int from_line, int to_line) {
    return code_edit->get_visible_line_count_in_range(from_line, to_line);
}

int EditorAdaptor::last_column(int line) {
    if(line == -1) line = curr_line();
    return code_edit->get_line(line).length();
}

String EditorAdaptor::line_text(int line) {
    return code_edit->get_line(line);
}

Ref<VimPosition> EditorAdaptor::last_pos() {
    int line = last_line();
    return memnew(VimPosition(line, last_column(line)));
}

CharIterator* EditorAdaptor::chars(int line, int column, bool forward, bool one_line) {
    return new CharIterator(this, line, column, forward, one_line);
}

void EditorAdaptor::set_curr_column(int col) {
    code_edit->set_caret_column(col);
}

void EditorAdaptor::jump_to(int line, int column) {
    int max_line = last_line();
    if(line < 0) line = 0;
    if(line > max_line) line = max_line;
    
    int max_column = last_column(line);
    if(column < 0) column = 0;
    if(column > max_column) column = max_column;
    
    if(line >= 0 && line <= max_line) {
        code_edit->unfold_line(line);
    }
    update_margin(line);
    code_edit->set_caret_line(line);
    code_edit->set_caret_column(column);
}

void EditorAdaptor::update_margin(int line) {
    int max_line = last_line();
    if(line < 0 || line > max_line || !code_edit) return;
    
    int folded_lines_above = 0;
    int folded_lines_below = 0;
    
    int up_check_line = line - MARGIN_LINES_UP - 1;
    int down_check_line = line + MARGIN_LINES_DOWN + 1;
    
    if(up_check_line >= 0 && up_check_line <= max_line) {
        folded_lines_above = code_edit->get_next_visible_line_offset_from(up_check_line, -(MARGIN_LINES_UP + 1)) - (MARGIN_LINES_UP + 1);
    }
    
    if(down_check_line >= 0 && down_check_line <= max_line) {
        folded_lines_below = code_edit->get_next_visible_line_offset_from(down_check_line, MARGIN_LINES_DOWN + 1) - (MARGIN_LINES_DOWN + 1);
    }
    
    if(line < first_visible_line() + MARGIN_LINES_UP + folded_lines_above) {
        int lines_to_move = first_visible_line() - (first_visible_line() + MARGIN_LINES_UP) + line - folded_lines_above;
        code_edit->set_line_as_first_visible(MAX(0, lines_to_move));
    }else if(line > last_visible_line() - MARGIN_LINES_DOWN - folded_lines_below) {
        int lines_to_move = last_visible_line() - (last_visible_line() - MARGIN_LINES_DOWN) + line + folded_lines_below;
        code_edit->set_line_as_last_visible(MIN(max_line, lines_to_move));
    }
}

int EditorAdaptor::next_unfolded_line(int line, int offset, bool forward) {
    int step = forward ? 1 : -1;
    if(line + step > last_line() || line + step < 0) {
        return line;
    }
    int count = code_edit->get_next_visible_line_offset_from(line + step, offset * step);
    return line + count * (forward ? 1 : -1);
}

String EditorAdaptor::range_text(Ref<VimTextRange> range) {
    PackedStringArray s;
    CharIterator* it = chars(range->from->line, range->from->column);
    if(it->init()) {
        do {
            Ref<VimCharPos> p = it->get();
            if (p->equals(*range->to.ptr())) {
                break;
            }
            s.append(p->get_character());
        } while(it->next());
    }
    
    return String("").join(s);
}

Ref<VimPosition> EditorAdaptor::offset_pos(Ref<VimPosition> pos, int offset) {
    int count = abs(offset) + 1;
    bool forward = offset > 0;
    CharIterator* it = chars(pos->line, pos->column, forward);
    
    if(it->init()) {
        do {
            count -= 1;
            if (count == 0) {
                Ref<VimCharPos> p = it->get();
                return memnew(VimPosition(p->line, p->column));
            }
        } while(it->next());
    }
    return nullptr;
}

String EditorAdaptor::char_at(int line, int column) {
    String s = line_text(line);
    return (column >= 0 && column < s.length()) ? s.substr(column, 1) : "";
}

String EditorAdaptor::selected_text() {
    return code_edit->get_selected_text();
}

void EditorAdaptor::set_block_caret(bool block) {
    if(block) {
        if(curr_column() == last_column() + 1) {
            code_edit->set_caret_type(TextEdit::CARET_TYPE_LINE);
            code_edit->add_theme_constant_override("caret_width", 8);
        }else {
            code_edit->set_caret_type(TextEdit::CARET_TYPE_BLOCK);
            code_edit->add_theme_constant_override("caret_width", 1);
        }
    }else {
        code_edit->set_caret_type(TextEdit::CARET_TYPE_LINE);
        code_edit->add_theme_constant_override("caret_width", 1);
    }
}

void EditorAdaptor::set_caret_blink(bool blink) {
    code_edit->set_caret_blink_enabled(blink);
}

void EditorAdaptor::deselect() {
    code_edit->deselect();
}

void EditorAdaptor::select(int from_line, int from_column, int to_line, int to_column) {
    if(to_line > last_line()) {
        to_line = last_line();
        to_column = INF_COLUMN;
    }
    code_edit->select(from_line, from_column, to_line, to_column);
}

void EditorAdaptor::select_range(Ref<VimTextRange> range) {
    select(range->from->line, range->from->column, range->to->line, range->to->column);
}

void EditorAdaptor::select_by_pos2(Ref<VimPosition> from, Ref<VimPosition> to) {
    select(from->line, from->column, to->line, to->column);
}

void EditorAdaptor::delete_selection() {
    code_edit->delete_selection();
}

Ref<VimTextRange> EditorAdaptor::selection() {
    Ref<VimPosition> from;
    Ref<VimPosition> to;
    if(code_edit->has_selection()) {
        from = Ref<VimPosition>(memnew(VimPosition(code_edit->get_selection_from_line(), code_edit->get_selection_from_column())));
        to = Ref<VimPosition>(memnew(VimPosition(code_edit->get_selection_to_line(), code_edit->get_selection_to_column())));
    }else {
        from = Ref<VimPosition>(memnew(VimPosition(code_edit->get_caret_line(), code_edit->get_caret_column())));
        to = Ref<VimPosition>(memnew(VimPosition(code_edit->get_caret_line(), code_edit->get_caret_column())));
    }
    return memnew(VimTextRange(from, to));
}

void EditorAdaptor::begin_complex_operation() {
    complex_ops += 1;
    if(complex_ops == 1) {
        code_edit->begin_complex_operation();
    }
}

void EditorAdaptor::end_complex_operation() {
    complex_ops -= 1;
    if(complex_ops == 0) {
        code_edit->end_complex_operation();
    }
}

void EditorAdaptor::insert_text(String text) {
    code_edit->insert_text_at_caret(text);
}

void EditorAdaptor::replace_selection(String text) {
    int col = curr_column();
    begin_complex_operation();
    delete_selection();
    insert_text(text);
    end_complex_operation();
    set_curr_column(col);
}

void EditorAdaptor::toggle_folding(int line) {
    if(code_edit->is_line_folded(line)) {
        code_edit->unfold_line(line);
    }else {
        while(line >= 0) {
            if(code_edit->can_fold_line(line)) {
                code_edit->fold_line(line);
                break;
            }
            line -= 1;
        }
    }
}

void EditorAdaptor::fold_all() {
    code_edit->fold_all_lines();
}

void EditorAdaptor::unfold_all() {
    code_edit->unfold_all_lines();
}

void EditorAdaptor::undo() {
    code_edit->undo();
}

void EditorAdaptor::redo() {
    code_edit->redo();
}

void EditorAdaptor::indent() {
    code_edit->indent_lines();
}

void EditorAdaptor::unindent() {
    code_edit->unindent_lines();
}

void EditorAdaptor::simulate_press_key(Ref<InputEventKey> key_event) {
    vim->press_key(key_event);
}

void EditorAdaptor::simulate_press(Key key, int unicode, bool ctrl, bool alt, bool shift, bool meta) {
    Ref<InputEventKey> key_event = memnew(InputEventKey);
    if(ctrl) key_event->set_ctrl_pressed(true);
    if(alt) key_event->set_alt_pressed(true);
    if(shift) key_event->set_shift_pressed(true);
    if(meta) key_event->set_meta_pressed(true);
    key_event->set_keycode(key);
    key_event->set_key_label(key);
    key_event->set_unicode(unicode);
    vim->press_key(key_event);
}

int EditorAdaptor::find_first_non_white_space_character(int line) {
    String s = line_text(line);
    return s.length() - s.lstrip(" \t\r\n").length();
}

Ref<VimCharPos> EditorAdaptor::find_forward(int line, int col, std::function<bool(Ref<VimCharPos>)> condition, bool one_line) {
    CharIterator* it = chars(line, col, true, one_line);
    if (it->init()) {
        do {
            Ref<VimCharPos> p = it->get();
            if (condition(p)) {
                return p;
            }
        } while (it->next());
    }
    return nullptr;
}

Ref<VimCharPos> EditorAdaptor::find_backward(int line, int col, std::function<bool(Ref<VimCharPos>)> condition, bool one_line) {
    CharIterator* it = chars(line, col, false, one_line);
    if (it->init()) {
        do {
            Ref<VimCharPos> p = it->get();
            if (condition(p)) {
                return p;
            }
        } while (it->next());
    }
    return nullptr;
}

bool EditorAdaptor::is_alphanumeric(String text) {
    if(text.length() == 0) return false;
    char32_t c = text[0];
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_');
}

bool EditorAdaptor::is_breaker(String text) {
    if(text.length() == 0) return false;
    
    String breakers = "!\"#$%&()*+,-./:;<=>?@[\\]^`'{|}~";
    char32_t c = text[0];
    
    for(int i = 0; i < breakers.length(); i++) {
        if(breakers[i] == c) {
            return true;
        }
    }
    return false;
}

bool EditorAdaptor::is_lower_alpha(String text) {
    if(text.length() == 0) return false;
    char32_t c = text[0];
    return (c >= 'a' && c <= 'z');
}

Ref<VimTextRange> EditorAdaptor::get_word_at_pos(int line, int column) {
    Ref<VimCharPos> end = find_forward(line, column, [this](Ref<VimCharPos> p) {
        return !is_alphanumeric(p->get_character());
    }, true);
    Ref<VimCharPos> start = find_backward(line, column - 1, [this](Ref<VimCharPos> p) {
        return !is_alphanumeric(p->get_character());
    }, true);
    return memnew(VimTextRange(start->right(), end));
}

Ref<VimPosition> EditorAdaptor::search(String text, int line, int col, bool match_case, bool whole_word, bool forward) {
    int flags = 0;
    if(match_case) flags |= TextEdit::SEARCH_MATCH_CASE;
    if(whole_word) flags |= TextEdit::SEARCH_WHOLE_WORDS;
    if(!forward) flags |= TextEdit::SEARCH_BACKWARDS;
    Vector2 result = code_edit->search(text, flags, line, col);

    if(result.x < 0 || result.y < 0) return nullptr;

    code_edit->set_search_text(text);
    return memnew(VimPosition(result.y, result.x));
}

bool EditorAdaptor::has_focus() {
    return code_edit && code_edit->has_focus();
}