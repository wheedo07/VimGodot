#include "command.h"
#include "env.h"
#include "../res/gdscript_parser.hpp"
#include "../editor_adaptor.h"
#include "../main.h"

VimCommand::VimCommand() {
    SYMBOLS["("] = ")";
    SYMBOLS[")"] = "(";
    SYMBOLS["["] = "]";
    SYMBOLS["]"] = "[";
    SYMBOLS["{"] = "}";
    SYMBOLS["}"] = "{";
    SYMBOLS["\""] = "\"";
    SYMBOLS["'"] = "'";
}

void VimCommand::_bind_methods() {
    // [MOTIONS] //
    ClassDB::bind_method(D_METHOD("move_by_characters", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_by_characters);
    ClassDB::bind_method(D_METHOD("move_by_scroll", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_by_scroll);
    ClassDB::bind_method(D_METHOD("move_by_page", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_by_page);
    ClassDB::bind_method(D_METHOD("move_to_column", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_column);
    ClassDB::bind_method(D_METHOD("move_by_lines", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_by_lines);
    ClassDB::bind_method(D_METHOD("move_to_first_non_white_space_character", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_first_non_white_space_character);
    ClassDB::bind_method(D_METHOD("move_to_start_of_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_start_of_line);
    ClassDB::bind_method(D_METHOD("move_to_end_of_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_end_of_line);
    ClassDB::bind_method(D_METHOD("move_to_top_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_top_line);
    ClassDB::bind_method(D_METHOD("move_to_bottom_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_bottom_line);
    ClassDB::bind_method(D_METHOD("move_to_middle_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_middle_line);
    ClassDB::bind_method(D_METHOD("move_to_line_or_edge_of_document", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_line_or_edge_of_document);
    ClassDB::bind_method(D_METHOD("move_by_words", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_by_words);
    ClassDB::bind_method(D_METHOD("move_to_matched_symbol", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_matched_symbol);
    ClassDB::bind_method(D_METHOD("move_to_next_char", "cur", "args", "editor_adaptor", "core"), &VimCommand::move_to_next_char);
    ClassDB::bind_method(D_METHOD("repeat_last_char_search", "cur", "args", "editor_adaptor", "core"), &VimCommand::repeat_last_char_search);
    ClassDB::bind_method(D_METHOD("expand_to_line", "cur", "args", "editor_adaptor", "core"), &VimCommand::expand_to_line);
    ClassDB::bind_method(D_METHOD("find_word_under_caret", "cur", "args", "editor_adaptor", "core"), &VimCommand::find_word_under_caret);
    ClassDB::bind_method(D_METHOD("find_again", "cur", "args", "editor_adaptor", "core"), &VimCommand::find_again);
    ClassDB::bind_method(D_METHOD("text_object", "cur", "args", "editor_adaptor", "core"), &VimCommand::text_object);
    ClassDB::bind_method(D_METHOD("go_to_bookmark", "cur", "args", "editor_adaptor", "core"), &VimCommand::go_to_bookmark);
    // [end MOTIONS] //

    // [OPERATORS] //
    ClassDB::bind_method(D_METHOD("deletes", "args", "editor_adaptor", "core"), &VimCommand::deletes);
    ClassDB::bind_method(D_METHOD("change", "args", "editor_adaptor", "core"), &VimCommand::change);
    ClassDB::bind_method(D_METHOD("yank", "args", "editor_adaptor", "core"), &VimCommand::yank);
    ClassDB::bind_method(D_METHOD("change_case", "args", "editor_adaptor", "core"), &VimCommand::change_case);
    ClassDB::bind_method(D_METHOD("toggle_case", "args", "editor_adaptor", "core"), &VimCommand::toggle_case);
    ClassDB::bind_method(D_METHOD("delete_and_enter_insert_mode", "args", "editor_adaptor", "core"), &VimCommand::delete_and_enter_insert_mode);
    // [end OPERATORS] //

    // [ACTIONS] //
    ClassDB::bind_method(D_METHOD("paste", "args", "editor_adaptor", "core"), &VimCommand::paste);
    ClassDB::bind_method(D_METHOD("undo", "args", "editor_adaptor", "core"), &VimCommand::undo);
    ClassDB::bind_method(D_METHOD("redo", "args", "editor_adaptor", "core"), &VimCommand::redo);
    ClassDB::bind_method(D_METHOD("replace", "args", "editor_adaptor", "core"), &VimCommand::replace);
    ClassDB::bind_method(D_METHOD("enter_insert_mode", "args", "editor_adaptor", "core"), &VimCommand::enter_insert_mode);
    ClassDB::bind_method(D_METHOD("enter_visual_mode", "args", "editor_adaptor", "core"), &VimCommand::enter_visual_mode);
    ClassDB::bind_method(D_METHOD("search", "args", "editor_adaptor", "core"), &VimCommand::search);
    ClassDB::bind_method(D_METHOD("jump_list_walk", "args", "editor_adaptor", "core"), &VimCommand::jump_list_walk);
    ClassDB::bind_method(D_METHOD("toggle_folding", "args", "editor_adaptor", "core"), &VimCommand::toggle_folding);
    ClassDB::bind_method(D_METHOD("fold_all", "args", "editor_adaptor", "core"), &VimCommand::fold_all);
    ClassDB::bind_method(D_METHOD("unfold_all", "args", "editor_adaptor", "core"), &VimCommand::unfold_all);
    ClassDB::bind_method(D_METHOD("repeat_last_edit", "args", "editor_adaptor", "core"), &VimCommand::repeat_last_edit);
    ClassDB::bind_method(D_METHOD("record_macro", "args", "editor_adaptor", "core"), &VimCommand::record_macro);
    ClassDB::bind_method(D_METHOD("stop_record_macro", "args", "editor_adaptor", "core"), &VimCommand::stop_record_macro);
    ClassDB::bind_method(D_METHOD("play_macro", "args", "editor_adaptor", "core"), &VimCommand::play_macro);
    ClassDB::bind_method(D_METHOD("indent", "args", "editor_adaptor", "core"), &VimCommand::indent);
    ClassDB::bind_method(D_METHOD("join_lines", "args", "editor_adaptor", "core"), &VimCommand::join_lines);
    ClassDB::bind_method(D_METHOD("set_bookmark", "args", "editor_adaptor", "core"), &VimCommand::set_bookmark);
    ClassDB::bind_method(D_METHOD("go_to_doc", "args", "editor_adaptor", "core"), &VimCommand::go_to_doc);
    // [end ACTIONS] //

    // [CONDITION] //
    ClassDB::bind_method(D_METHOD("is_recording", "editor_adaptor", "core"), &VimCommand::is_recording);
    // [end CONDITION] //
}

Ref<VimTextRange> VimCommand::_find_word(Ref<VimPosition> cur, Ref<EditorAdaptor> editor_adaptor, bool forward, bool big_word, bool empty_line_is_word, bool one_line) {
    vector<function<bool(String)>> char_tests;
    
    if (big_word) {
        char_tests.push_back([editor_adaptor](String c) {
            return editor_adaptor->is_alphanumeric(c) || editor_adaptor->is_breaker(c);
        });
    } else {
        char_tests.push_back([editor_adaptor](String c) {
            return editor_adaptor->is_alphanumeric(c);
        });
        char_tests.push_back([editor_adaptor](String c) {
            return editor_adaptor->is_breaker(c);
        });
    }
    CharIterator* it = editor_adaptor->chars(cur->line, cur->column, forward);
    
    if(!it->init()) {
        return nullptr;
    }
    
    do {
        Ref<VimCharPos> p = it->get();
        
        if(one_line && p->get_character() == "\n") {
            return VimTextRange::from_num3(p->line, p->column, INF_COLUMN);
        }
        
        if(p->line != cur->line && empty_line_is_word && 
            editor_adaptor->line_text(p->line).strip_edges().is_empty()) {
            return VimTextRange::from_num3(p->line, 0, 0);
        }
        
        for(auto& char_test : char_tests) {
            if (char_test(p->get_character())) {
                int word_start = p->column;
                int word_end = word_start;
                
                CharIterator* word_it = editor_adaptor->chars(p->line, p->column, forward, true);
                if (word_it->init()) {
                    do {
                        Ref<VimCharPos> q = word_it->get();
                        if (!char_test(q->get_character())) {
                            break;
                        }
                        word_end = q->column;
                    } while (word_it->next());
                }
                
                if(p->line == cur->line && word_start == cur->column && word_end == word_start) {
                    continue;
                } else {
                    int start = MIN(word_start, word_end);
                    int end = MAX(word_start + 1, word_end + 1);
                    return VimTextRange::from_num3(p->line, start, end);
                }
            }
        }
    } while(it->next());
    
    return nullptr;
}