#include "tetris_godot.h"
#include "core/os/memory.h"
#include "core/error/error_macros.h"
#include "core/input/input_map.h"

Tetris::Tetris() {
    tetris_sim_host host;
    host.alloc = Tetris::_godot_alloc;
    host.free = Tetris::_godot_free;
    host.panic = Tetris::_godot_panic;
    host.input_pressed = Tetris::_godot_input_pressed;
    host.context = this;

    sim = tetris_sim_init(host, 0x8556);
}

Tetris::~Tetris() {
    tetris_sim_deinit(sim);
}

void Tetris::_process(float delta) {
    _poll_input();
    
    tetris_sim_update(sim);
}

void Tetris::_poll_input() {
    InputMap* input_map = InputMap::get_singleton();

    const char* input_to_action[TETRIS_INPUT_COUNT] = {
        "move_left",        // TETRIS_INPUT_MOVE_LEFT
        "move_right",       // TETRIS_INPUT_MOVE_RIGHT
        "fast_drop",        // TETRIS_INPUT_FAST_DROP
        "rotate_left",      // TETRIS_INPUT_ROTATE_LEFT
        "rotate_right",     // TETRIS_INPUT_ROTATE_RIGHT
    };
    
    for (int input = 0; input < TETRIS_INPUT_COUNT; ++input) {
        input_pressed[input] = false;

        const List<Ref<InputEvent>>* events = input_map->action_get_events(input_to_action[input]);
        if (!events) {
            continue;
        }
        
        for (auto it = events->begin(); it != events->end(); ++it) {
            const Ref<InputEvent>& event = *it;
            input_pressed[input] |= event->is_pressed();
        }
    }
}

void Tetris::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_READY: {
            set_process(true);
            break;
        }
        case NOTIFICATION_PROCESS: {
            _process(get_process_delta_time());
            break;
        }
    }
}

void* Tetris::_godot_alloc(void* context, size_t size) {
    return memalloc(size);
}

void Tetris::_godot_free(void* context, void* ptr) {
    memfree(ptr);
}

void Tetris::_godot_panic(void* context, const char* err_msg) {
    CRASH_NOW_MSG(err_msg);
}

bool Tetris::_godot_input_pressed(void* context, const tetris_input input) {
    Tetris* tetris = static_cast<Tetris*>(context);
    return tetris->input_pressed[input];
}