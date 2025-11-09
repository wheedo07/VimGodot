#ifndef __JUMP_LIST_HPP__
#define __JUMP_LIST_HPP__

#include "position.hpp"
class JumpList {
    private:
        Array buffer;
        int pointer = 0;

    public:
        JumpList(int capy = 20) {
            buffer.resize(capy);
        }

        void add(Ref<VimPosition> old_pos, Ref<VimPosition> new_pos) {
            Ref<VimPosition> current = Object::cast_to<VimPosition>(buffer[pointer]);
            if(current == nullptr || !current->equals(*old_pos.ptr())) {
                pointer = (pointer + 1) % buffer.size();
                buffer[pointer] = old_pos;
            }
            pointer = (pointer + 1 ) % buffer.size();
            buffer[pointer] = new_pos;
        }

        void set_next(Ref<VimPosition> pos) {
            pointer = (pointer + 1) % buffer.size();
            buffer[pointer] = pos;
        }

        Ref<VimPosition> move(int offset) {
            int t = (pointer + offset) % buffer.size();
            Ref<VimPosition> pos = Object::cast_to<VimPosition>(buffer[t]);
            if(pos != nullptr)
                pointer = t;
            return pos;
        }

        void on_lines_edited(int from, int to) {
            for(int i=0; i < buffer.size(); i++) {
                Ref<VimPosition> pos = Object::cast_to<VimPosition>(buffer[i]);
                if(pos != nullptr && pos->line > from) {
                    pos->line += (to - from);
                }
            }
        }
};

#endif