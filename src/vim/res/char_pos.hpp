#ifndef __CHAR_POS_HPP__
#define __CHAR_POS_HPP__

#include "position.hpp"
class VimCharPos : public VimPosition {
    GDCLASS(VimCharPos, VimPosition);

    protected:
        static void _bind_methods() {};

    public:
        String line_text = "";
        Ref<VimCharPos> set_data(String line_text, int line, int column) {
            this->line = line;
            this->column = column;
            this->line_text = line_text;
            return this;
        }

        String get_character() {
            if(column < 0 || column >= line_text.length()) {
                return "\n";
            }
            return line_text.substr(column, 1);
        }
};

#endif