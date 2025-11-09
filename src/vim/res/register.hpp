#ifndef __REGISTER_HPP__
#define __REGISTER_HPP__

#include<godot_cpp/variant/string.hpp>
using namespace godot;
class Register {
    public:
        String text = "";
        bool line_wise = false;

        void set_text(const String& p_text, bool p_line_wise) {
            text = p_text;
            line_wise = p_line_wise;
        }
};

#endif