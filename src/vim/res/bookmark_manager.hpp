#ifndef __BOOKMARK_MANAGER_HPP__
#define __BOOKMARK_MANAGER_HPP__

#include<godot_cpp/classes/node.hpp>
using namespace godot;
class BookmarkManager {
    private:
        Dictionary bookmarks;

    public:
        void on_lines_edited(int from, int to) {
            Array keys = bookmarks.keys();
            for(int i=0; i < keys.size(); i++) {
                int line = bookmarks[keys[i]];
                if(line >= from) {
                    line += (to - from);
                    bookmarks[keys[i]] = line;
                }
            }
        }

        void set_bookmark(String name, int line) {
            bookmarks[name] = line;
        }

        int get_bookmark(String name) {
            return bookmarks.get(name, -1);
        }
};

#endif