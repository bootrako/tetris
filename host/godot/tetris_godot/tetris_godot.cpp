#include "tetris_godot.h"
#include "core/error/error_macros.h"
#include "core/input/input.h"
#include "core/input/input_map.h"
#include "core/math/math_funcs.h"
#include "core/os/memory.h"

TetrisSim::TetrisSim() {
    tetris_sim_host host;
    host.alloc = TetrisSim::_godot_alloc;
    host.free = TetrisSim::_godot_free;
    host.panic = TetrisSim::_godot_panic;
    host.input_pressed = TetrisSim::_godot_input_pressed;
    host.context = this;

    Math::randomize();
    sim = tetris_sim_init(host, Math::rand());
}

TetrisSim::~TetrisSim() {
    tetris_sim_deinit(sim);
}

void TetrisSim::update() {
    tetris_sim_update(sim);
}

void TetrisSim::poll_input() {
    const char* const input_to_action[TETRIS_INPUT_COUNT] = {
        "move_left",        // TETRIS_INPUT_MOVE_LEFT
        "move_right",       // TETRIS_INPUT_MOVE_RIGHT
        "fast_drop",        // TETRIS_INPUT_FAST_DROP
        "rotate_left",      // TETRIS_INPUT_ROTATE_LEFT
        "rotate_right",     // TETRIS_INPUT_ROTATE_RIGHT
    };

    // https://github.com/godotengine/godot/issues/36396
    // manually poll input events since input actions can miss a input release event
    // this only seems to happen on web builds 
    Input* input_singleton = Input::get_singleton();
    InputMap* input_map = InputMap::get_singleton();
    for (int input = 0 ; input < TETRIS_INPUT_COUNT; ++input) {
        const List<Ref<InputEvent>>* events = input_map->action_get_events(input_to_action[input]);
        if (!events) {
            continue;
        }

        for (const Ref<InputEvent>& element : *events) {
            if (!element.is_valid()) {
                continue;
            }

            Ref<InputEventKey> key = element;
            if (!key.is_valid()) {
                continue;
            }

            input_pressed[input] = input_singleton->is_key_pressed(key->get_keycode());
        }
    }
}

float TetrisSim::get_time_per_frame() const {
    return tetris_sim_time_per_frame(sim);
}

bool TetrisSim::is_game_over() const {
    return tetris_sim_is_game_over(sim);
}

int TetrisSim::get_matrix_width() const {
    return tetris_sim_get_matrix_width(sim);
}

int TetrisSim::get_matrix_height() const {
    return tetris_sim_get_matrix_height(sim);
}

bool TetrisSim::get_matrix_cell(int x, int y) const {
    return tetris_sim_get_matrix_cell(sim, x, y);
}

int TetrisSim::get_matrix_cell_shape(int x, int y) const {
    return tetris_sim_get_matrix_cell_shape(sim, x, y);
}

bool TetrisSim::is_tetronimo_active() const {
    return tetris_sim_is_tetronimo_active(sim);
}

int TetrisSim::get_tetronimo_max_width() const {
    return tetris_sim_get_tetronimo_max_width(sim);
}

int TetrisSim::get_tetronimo_max_height() const {
    return tetris_sim_get_tetronimo_max_height(sim);
}

bool TetrisSim::get_tetronimo_cell(int x, int y) const {
    return tetris_sim_get_tetronimo_cell(sim, x, y);
}

int TetrisSim::get_tetronimo_shape() const {
    return tetris_sim_get_tetronimo_shape(sim);
}

bool TetrisSim::get_next_tetronimo_cell(int x, int y) const {
    return tetris_sim_get_next_tetronimo_cell(sim, x, y);
}

int TetrisSim::get_next_tetronimo_shape() const {
    return tetris_sim_get_next_tetronimo_shape(sim);
}

int TetrisSim::get_tetronimo_pos_x() const {
    return tetris_sim_get_tetronimo_pos_x(sim);
}

int TetrisSim::get_tetronimo_pos_y() const {
    return tetris_sim_get_tetronimo_pos_y(sim);
}

int TetrisSim::get_score() const {
    return tetris_sim_get_score(sim);
}

int TetrisSim::get_lines() const {
    return tetris_sim_get_lines(sim);
}

int TetrisSim::get_level() const {
    return tetris_sim_get_level(sim);
}

bool TetrisSim::event_tetronimo_spawned() const {
    return tetris_sim_event_tetronimo_spawned(sim);
}

bool TetrisSim::event_tetronimo_moved() const {
    return tetris_sim_event_tetronimo_moved(sim);
}

bool TetrisSim::event_tetronimo_rotated() const {
    return tetris_sim_event_tetronimo_rotated(sim);
}

bool TetrisSim::event_tetronimo_locked() const {
    return tetris_sim_event_tetronimo_locked(sim);
}

Vector<int> TetrisSim::event_matrix_rows_cleared() const {
    Vector<int> ret;
    const int num_rows_cleared = tetris_sim_event_num_matrix_rows_cleared(sim);
    const int* rows_cleared = tetris_sim_event_matrix_rows_cleared(sim);
    for (int i = 0; i < num_rows_cleared; ++i) {
        ret.push_back(rows_cleared[i]);
    }
    return ret;
}

void TetrisSim::_bind_methods() {
    ClassDB::bind_method(D_METHOD("poll_input"), &TetrisSim::poll_input);
    ClassDB::bind_method(D_METHOD("update"), &TetrisSim::update);

    ClassDB::bind_method(D_METHOD("get_time_per_frame"), &TetrisSim::get_time_per_frame);
    ClassDB::bind_method(D_METHOD("is_game_over"), &TetrisSim::is_game_over);

    ClassDB::bind_method(D_METHOD("get_matrix_width"), &TetrisSim::get_matrix_width);
    ClassDB::bind_method(D_METHOD("get_matrix_height"), &TetrisSim::get_matrix_height);
    ClassDB::bind_method(D_METHOD("get_matrix_cell", "x", "y"), &TetrisSim::get_matrix_cell);
    ClassDB::bind_method(D_METHOD("get_matrix_cell_shape", "x", "y"), &TetrisSim::get_matrix_cell_shape);

    ClassDB::bind_method(D_METHOD("is_tetronimo_active"), &TetrisSim::is_tetronimo_active);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_width"), &TetrisSim::get_tetronimo_max_width);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_height"), &TetrisSim::get_tetronimo_max_height);
    ClassDB::bind_method(D_METHOD("get_tetronimo_cell", "x", "y"), &TetrisSim::get_tetronimo_cell);
    ClassDB::bind_method(D_METHOD("get_tetronimo_shape"), &TetrisSim::get_tetronimo_shape);
    ClassDB::bind_method(D_METHOD("get_next_tetronimo_cell", "x", "y"), &TetrisSim::get_next_tetronimo_cell);
    ClassDB::bind_method(D_METHOD("get_next_tetronimo_shape"), &TetrisSim::get_next_tetronimo_shape);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_x"), &TetrisSim::get_tetronimo_pos_x);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_y"), &TetrisSim::get_tetronimo_pos_y);

    ClassDB::bind_method(D_METHOD("get_score"), &TetrisSim::get_score);
    ClassDB::bind_method(D_METHOD("get_lines"), &TetrisSim::get_lines);
    ClassDB::bind_method(D_METHOD("get_level"), &TetrisSim::get_level);

    ClassDB::bind_method(D_METHOD("event_tetronimo_spawned"), &TetrisSim::event_tetronimo_spawned);
    ClassDB::bind_method(D_METHOD("event_tetronimo_moved"), &TetrisSim::event_tetronimo_moved);
    ClassDB::bind_method(D_METHOD("event_tetronimo_rotated"), &TetrisSim::event_tetronimo_rotated);
    ClassDB::bind_method(D_METHOD("event_tetronimo_locked"), &TetrisSim::event_tetronimo_locked);
    ClassDB::bind_method(D_METHOD("event_matrix_rows_cleared"), &TetrisSim::event_matrix_rows_cleared);
}

void* TetrisSim::_godot_alloc(void* context, size_t size) {
    return memalloc(size);
}

void TetrisSim::_godot_free(void* context, void* ptr) {
    memfree(ptr);
}

void TetrisSim::_godot_panic(void* context, const char* err_msg) {
    CRASH_NOW_MSG(err_msg);
}

bool TetrisSim::_godot_input_pressed(void* context, const tetris_input input) {
    TetrisSim* tetris = static_cast<TetrisSim*>(context);
    return tetris->input_pressed[input];
}