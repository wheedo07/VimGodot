#ifndef __TEXT_RANGE_HPP__
#define __TEXT_RANGE_HPP__

#include<godot_cpp/classes/ref_counted.hpp>
#include "position.hpp"
class VimTextRange : public RefCounted {
    GDCLASS(VimTextRange, RefCounted);

    protected:
        static void _bind_methods() {};

    public:
        Ref<VimPosition> from;
        Ref<VimPosition> to;

        VimTextRange() {
            from.instantiate();
            to.instantiate();
        }

        VimTextRange(Ref<VimPosition> p_from, Ref<VimPosition> p_to) {
            from = p_from;
            to = p_to;
        }

        static Ref<VimTextRange> from_num4(int from_line, int from_column, int to_line, int to_column) {
            return memnew(VimTextRange(memnew(VimPosition(from_line, from_column)), memnew(VimPosition(to_line, to_column))));
        }

        static Ref<VimTextRange> from_num3(int line, int from_column, int to_column) {
            return from_num4(line, from_column, line, to_column);
        }

        String to_string() {
            return "[" + from->to_string() + " - " + to->to_string() + "]";
        }

        bool is_single_line() {
            return from->line == to->line;
        }

        bool is_empty() {
            return from->line == to->line && from->column == to->column;
        }
};

#endif