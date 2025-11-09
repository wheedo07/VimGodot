#ifndef __INPUT_STATS_HPP__
#define __INPUT_STATS_HPP__

#include<godot_cpp/classes/input_event_key.hpp>
using namespace godot;
class InputStats {
    private:
        String prefix_repeat;
        String motion_repeat;

    public:
        Dictionary operator_args;
        String vim_operator;
        Array buffer;

        void push_key(Ref<InputEventKey> key_event) {
            buffer.append(key_event);
        }

        void push_repeat_digit(String d) {
            if(vim_operator.is_empty()) {
                prefix_repeat += d;
            }else motion_repeat += d;
        }

        int get_repeat() {
            int repeat = 1;
            if(!prefix_repeat.is_empty()) 
                repeat = MAX(repeat, 1) * prefix_repeat.to_int();

            if(!motion_repeat.is_empty())
                repeat = MAX(repeat, 1) * motion_repeat.to_int();

            return repeat;
        }

        Array key_codes() {
            Array codes;
            for(int i=0; i < buffer.size(); i++) {
                Ref<InputEventKey> key_event = buffer[i];
                codes.append(key_event->as_text_keycode());
            }
            return codes;
        }

        void clear() {
            prefix_repeat = "";
            motion_repeat = "";
            vim_operator = "";
            buffer.clear();
        }
};

#endif