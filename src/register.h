#ifndef __REGISTER_H__
#define __REGISTER_H__
#include<godot_cpp/core/class_db.hpp>
using namespace godot;

#include "vim_plugins.h"
#include "vim/main.h"
#include "vim/session.h"
#include "vim/editor_adaptor.h"
#include "vim/command/command.h"
#include "vim/res/position.hpp"
#include "vim/res/char_pos.hpp"
#include "vim/res/macro.hpp"
#include "vim/res/text_range.hpp"

void godot_vim_init(ModuleInitializationLevel p_level);
void godot_vim_uninit(ModuleInitializationLevel p_level);

#endif