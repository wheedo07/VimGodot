#ifndef __SESSION_H__
#define __SESSION_H__

#include<godot_cpp/classes/ref_counted.hpp>
#include "res/input_stats.hpp"
#include "res/jump_list.hpp"
#include "res/bookmark_manager.hpp"
namespace godot {
    class EditorAdaptor;

    class VIMSession : public RefCounted {
        GDCLASS(VIMSession, RefCounted);

        protected:
            static void _bind_methods();

        public:
            Ref<EditorAdaptor> editor_adaptor;
            InputStats* input_stats;
            JumpList* jump_list;
            Ref<VimPosition> visual_start_pos;
            BookmarkManager* bookmark_manager;
            String last_motion;
            int last_h_pos;
            int text_change_number;

            bool visual_mode;
            bool insert_mode;
            bool visual_line;

        public:
            VIMSession();
            bool is_normal_mode() const;

            void enter_normal_mode();
            void enter_insert_mode();
            void enter_visual_mode(bool line_mode=false);
    };
}

#endif