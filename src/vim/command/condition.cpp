#include "command.h"
#include "env.h"
#include "../res/gdscript_parser.hpp"
#include "../editor_adaptor.h"
#include "../main.h"

bool VimCommand::is_recording(Ref<EditorAdaptor> editor_adaptor, Ref<VimCore> core) {
    return core->macro_manager->is_recording();
}