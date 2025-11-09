#ifndef __GDSCRIPT_PARSER_HPP__
#define __GDSCRIPT_PARSER_HPP__

#include "char_pos.hpp"
#include "../editor_adaptor.h"
#include "../char_iterator.h"
class GDScriptParser {
    private:
        Dictionary open_symbol;
        Dictionary close_symbol;

        bool in_comment = false;
        int escape_count = 0;
        bool valid = true;
        bool eof = false;
        Ref<EditorAdaptor> editor_adaptor;
        CharIterator* char_iterator;
        String target_open_symbol;
        String target_close_symbol;

    public:
        Ref<VimPosition> pos;
        Array stack;

        GDScriptParser(Ref<EditorAdaptor> editor_adaptor, Ref<VimPosition> from, String target) {
            this->editor_adaptor = editor_adaptor;
            char_iterator = editor_adaptor->chars(from->line, from->column);
            if(!char_iterator->init()) eof = true;

            open_symbol["("] = ")";
            open_symbol["["] = "]";
            open_symbol["{"] = "}";
            open_symbol["'"] = "'";
            open_symbol["\""] = "\"";

            close_symbol[")"] = "(";
            close_symbol["]"] = "[";
            close_symbol["}"] = "{";

            if(open_symbol.has(target)) {
                target_open_symbol = target;
                target_close_symbol = open_symbol[target];
            }else if(close_symbol.has(target)) {
                target_close_symbol = target;
                target_open_symbol = close_symbol[target];
            }
        }

        String get_stack_top_char() {
            return stack.is_empty() ? "" : Object::cast_to<VimCharPos>(stack.back())->get_character();
        }


        String parse_one_char() {
            if(eof || !valid) return "";
        
            Ref<VimCharPos> p = char_iterator->get();
            pos = p;
        
            if(!char_iterator->next()) eof = true;
            String c = p->get_character();
            String top = get_stack_top_char();
        
            if (Array::make("'", "\"").has(top)) {
                if (c == top && escape_count % 2 == 0) {
                    stack.pop_back();
                    escape_count = 0;
                    return c;
                }
                escape_count = (c == "\\") ? escape_count + 1 : 0;
            }else if(in_comment) {
                if (c == "\n") {
                    in_comment = false;
                }
            }else if(c == "#") {
                in_comment = true;
            }else if(c == target_open_symbol) {
                stack.append(p);
                return c;
            }else if(c == target_close_symbol) {
                if (top == target_open_symbol) {
                    stack.pop_back();
                    return c;
                } else {
                    valid = false;
                }
            }
        
            return "";
        }

        bool parse_until(Ref<VimPosition> to) {
            while(valid && !eof) {
                parse_one_char();
                if(pos.is_valid() && pos->line == to->line && pos->column == to->column)
                    break;
            }
            return valid && !eof;
        }

        Ref<VimPosition> find_matching() {
            int depth = stack.size();
            while(valid && !eof) {
                parse_one_char();
                if(stack.size() < depth) return pos;
            }
            return nullptr;
        }
};

#endif