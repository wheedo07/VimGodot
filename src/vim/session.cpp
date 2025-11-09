#include "session.h"
#include "editor_adaptor.h"
using namespace godot;

VIMSession::VIMSession() {
    insert_mode = false;
    visual_mode = false;
    visual_line = false;
    input_stats = new InputStats();
    last_motion = "";
    last_h_pos = 0;
    jump_list = new JumpList();
    bookmark_manager = new BookmarkManager();
    visual_start_pos.instantiate();
    text_change_number = 0;
}

void VIMSession::_bind_methods() {}

bool VIMSession::is_normal_mode() const {
    return !insert_mode && !visual_mode;
}

void VIMSession::enter_normal_mode() {
    if(insert_mode) editor_adaptor->end_complex_operation();
    insert_mode = false;
    visual_mode = false;
    visual_line = false;
    editor_adaptor->set_block_caret(true);
}

void VIMSession::enter_insert_mode() {
    insert_mode = true;
    visual_mode = false;
    visual_line = false;
    editor_adaptor->set_block_caret(false);
    editor_adaptor->begin_complex_operation();
}

void VIMSession::enter_visual_mode(bool line_mode) {
    insert_mode = false;
    visual_mode = true;
    visual_line = line_mode;

    visual_start_pos = editor_adaptor->curr_position()->right();
    if(line_mode) {
        editor_adaptor->select(visual_start_pos->line + 1, 0, visual_start_pos->line, 0);
    }else editor_adaptor->select_by_pos2(visual_start_pos->left(), visual_start_pos);
}