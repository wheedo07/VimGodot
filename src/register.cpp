#include "register.h"

void godot_vim_init(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) return;
    GDREGISTER_CLASS(VimEditorPlugin);
    GDREGISTER_CLASS(VimCore);
    GDREGISTER_CLASS(EditorAdaptor);
    GDREGISTER_CLASS(VIMSession);
    GDREGISTER_CLASS(VimPosition);
    GDREGISTER_CLASS(VimCharPos);
    GDREGISTER_CLASS(VimMacro);
    GDREGISTER_CLASS(VimCommand);
    GDREGISTER_CLASS(VimTextRange);

    EditorPlugins::add_by_type<VimEditorPlugin>();
}

void godot_vim_uninit(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) return;
}

extern "C" {
    GDExtensionBool GDE_EXPORT godot_vim_main_init(const GDExtensionInterfaceGetProcAddress p_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        GDExtensionBinding::InitObject init_obj(p_address, p_library, r_initialization);
        init_obj.register_initializer(godot_vim_init);
        init_obj.register_terminator(godot_vim_uninit);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);
        return init_obj.init();
    }
}