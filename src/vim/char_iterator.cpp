#include "char_iterator.h"
#include "editor_adaptor.h"
using namespace godot;

CharIterator::CharIterator(Ref<EditorAdaptor> p_editor_adaptor, int p_line, int p_column, bool p_forward, bool p_one_line) {
    editor_adaptor = p_editor_adaptor;
    line = p_line;
    column = p_column;
    forward = p_forward;
    one_line = p_one_line;
}

bool CharIterator::_ensure_column_valid() {
    if(column < 0 || column > line_text.length()) {
        line += forward ? 1 : -1;
        if(one_line || line < 0 || line > editor_adaptor->last_line()) {
            return false;
        }
        line_text = editor_adaptor->line_text(line);
        column = forward ? 0 : line_text.length();
    }
    return true;
}

bool CharIterator::init() {
    if(line < 0 || line > editor_adaptor->last_line()) return false;
    line_text = editor_adaptor->line_text(line);
    return _ensure_column_valid();
}

bool CharIterator::next() {
    column += forward ? 1 : -1;
    return _ensure_column_valid();
}

Ref<VimCharPos> CharIterator::get() {
    Ref<VimCharPos> charpos = memnew(VimCharPos);
    return charpos->set_data(line_text, line, column);
}