#ifndef __POSITION_HPP__
#define __POSITION_HPP__

#include<godot_cpp/classes/ref_counted.hpp>
#include<godot_cpp/variant/string.hpp>
#include<godot_cpp/variant/array.hpp>
using namespace godot;
class VimPosition : public RefCounted {
    GDCLASS(VimPosition, RefCounted);

    protected:
        static void _bind_methods() {};

    public:
        int line = 0;
        int column = 0;

        VimPosition() {
            line = 0;
            column = 0;
        };

        VimPosition(int p_line, int p_column) {
            line = p_line;
            column = p_column;
        };

        String to_string() const {
            return String("(") + String::num_int64(line) + String(", ") + String::num_int64(column) + String(")");
        };

        bool equals(const VimPosition& other) const {
            return line == other.line && column == other.column;
        }

        int compare_to(const VimPosition& other) const {
            if (line < other.line) return -1;
            if (line > other.line) return 1;
            if (column < other.column) return -1;
            if (column > other.column) return 1;
            return 0;
        }

        Ref<VimPosition> duplicate() const { 
            return memnew(VimPosition(line, column));
        }

        Ref<VimPosition> up() {
            return memnew(VimPosition(line - 1, column));
        }

        Ref<VimPosition> down() {
            return memnew(VimPosition(line + 1, column));
        }

        Ref<VimPosition> left() {
            return memnew(VimPosition(line, column - 1));
        }

        Ref<VimPosition> right() {
            return memnew(VimPosition(line, column + 1));
        }
};

#endif