#include "register_types.h"
#include "tetris_godot.h"
#include "core/object/class_db.h"

void initialize_tetris_godot_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
            return;
    }
    ClassDB::register_class<Tetris>();
}

void uninitialize_tetris_godot_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
            return;
    }
}