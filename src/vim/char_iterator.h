#ifndef __CHAR_ITERATOR_H__
#define __CHAR_ITERATOR_H__

#include<godot_cpp/variant/string.hpp>
#include "res/char_pos.hpp"
namespace godot {
    class EditorAdaptor;

    class CharIterator {
        private:
            Ref<EditorAdaptor> editor_adaptor;
            bool forward;
            bool one_line;
            String line_text;
        
        public:
            int line;
            int column;

            CharIterator(Ref<EditorAdaptor> p_editor_adaptor, int p_line, int p_column, bool p_forward, bool p_one_line);

            bool _ensure_column_valid();
            bool init();
            bool next();
            Ref<VimCharPos> get();
    };
}

#endif