#include "tetris_godot.h"
#include "core/error/error_macros.h"
#include "core/input/input.h"
#include "core/os/memory.h"

Tetris::Tetris() {
    tetris_sim_host host;
    host.alloc = Tetris::_godot_alloc;
    host.free = Tetris::_godot_free;
    host.panic = Tetris::_godot_panic;
    host.input_pressed = Tetris::_godot_input_pressed;
    host.context = this;

    // todo: actually get a random seed
    const uint64_t random_seed = 0x8765;
    sim = tetris_sim_init(host, random_seed);
}

Tetris::~Tetris() {
    tetris_sim_deinit(sim);
}

void Tetris::update_sim() {
    tetris_sim_update(sim);
}

void Tetris::poll_input() {
    Input* input_singleton = Input::get_singleton();

    const char* const input_to_action[TETRIS_INPUT_COUNT] = {
        "move_left",        // TETRIS_INPUT_MOVE_LEFT
        "move_right",       // TETRIS_INPUT_MOVE_RIGHT
        "fast_drop",        // TETRIS_INPUT_FAST_DROP
        "rotate_left",      // TETRIS_INPUT_ROTATE_LEFT
        "rotate_right",     // TETRIS_INPUT_ROTATE_RIGHT
    };
    for (int input = 0; input < TETRIS_INPUT_COUNT; ++input) {
        input_pressed[input] = input_singleton->is_action_pressed(input_to_action[input]);
    }
}

float Tetris::get_sim_time_per_frame() const {
    return tetris_sim_time_per_frame(sim);
}

bool Tetris::is_sim_game_over() const {
    return tetris_sim_is_game_over(sim);
}

int Tetris::get_matrix_width() const {
    return tetris_sim_get_matrix_width(sim);
}

int Tetris::get_matrix_height() const {
    return tetris_sim_get_matrix_height(sim);
}

bool Tetris::get_matrix_cell(int x, int y) const {
    return tetris_sim_get_matrix_cell(sim, x, y);
}

int Tetris::get_matrix_cell_shape(int x, int y) const {
    return tetris_sim_get_matrix_cell_shape(sim, x, y);
}

bool Tetris::is_tetronimo_active() const {
    return tetris_sim_is_tetronimo_active(sim);
}

int Tetris::get_tetronimo_max_width() const {
    return tetris_sim_get_tetronimo_max_width(sim);
}

int Tetris::get_tetronimo_max_height() const {
    return tetris_sim_get_tetronimo_max_height(sim);
}

bool Tetris::get_tetronimo_cell(int x, int y) const {
    return tetris_sim_get_tetronimo_cell(sim, x, y);
}

int Tetris::get_tetronimo_shape() const {
    return tetris_sim_get_tetronimo_shape(sim);
}

bool Tetris::get_next_tetronimo_cell(int x, int y) const {
    return tetris_sim_get_next_tetronimo_cell(sim, x, y);
}

int Tetris::get_next_tetronimo_shape() const {
    return tetris_sim_get_next_tetronimo_shape(sim);
}

int Tetris::get_tetronimo_pos_x() const {
    return tetris_sim_get_tetronimo_pos_x(sim);
}

int Tetris::get_tetronimo_pos_y() const {
    return tetris_sim_get_tetronimo_pos_y(sim);
}

int Tetris::get_score() const {
    return tetris_sim_get_score(sim);
}

int Tetris::get_lines() const {
    return tetris_sim_get_lines(sim);
}

int Tetris::get_level() const {
    return tetris_sim_get_level(sim);
}

bool Tetris::event_tetronimo_spawned() const {
    return tetris_sim_event_tetronimo_spawned(sim);
}

bool Tetris::event_tetronimo_moved() const {
    return tetris_sim_event_tetronimo_moved(sim);
}

bool Tetris::event_tetronimo_locked() const {
    return tetris_sim_event_tetronimo_locked(sim);
}

Vector<int> Tetris::event_matrix_rows_cleared() const {
    Vector<int> ret;
    const int num_rows_cleared = tetris_sim_event_num_matrix_rows_cleared(sim);
    const int* rows_cleared = tetris_sim_event_matrix_rows_cleared(sim);
    for (int i = 0; i < num_rows_cleared; ++i) {
        ret.push_back(rows_cleared[i]);
    }
    return ret;
}

void Tetris::_bind_methods() {
    ClassDB::bind_method(D_METHOD("poll_input"), &Tetris::poll_input);
    ClassDB::bind_method(D_METHOD("update_sim"), &Tetris::update_sim);

    ClassDB::bind_method(D_METHOD("get_sim_time_per_frame"), &Tetris::get_sim_time_per_frame);
    ClassDB::bind_method(D_METHOD("is_sim_game_over"), &Tetris::is_sim_game_over);

    ClassDB::bind_method(D_METHOD("get_matrix_width"), &Tetris::get_matrix_width);
    ClassDB::bind_method(D_METHOD("get_matrix_height"), &Tetris::get_matrix_height);
    ClassDB::bind_method(D_METHOD("get_matrix_cell", "x", "y"), &Tetris::get_matrix_cell);
    ClassDB::bind_method(D_METHOD("get_matrix_cell_shape", "x", "y"), &Tetris::get_matrix_cell_shape);

    ClassDB::bind_method(D_METHOD("is_tetronimo_active"), &Tetris::is_tetronimo_active);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_width"), &Tetris::get_tetronimo_max_width);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_height"), &Tetris::get_tetronimo_max_height);
    ClassDB::bind_method(D_METHOD("get_tetronimo_cell", "x", "y"), &Tetris::get_tetronimo_cell);
    ClassDB::bind_method(D_METHOD("get_tetronimo_shape"), &Tetris::get_tetronimo_shape);
    ClassDB::bind_method(D_METHOD("get_next_tetronimo_cell", "x", "y"), &Tetris::get_next_tetronimo_cell);
    ClassDB::bind_method(D_METHOD("get_next_tetronimo_shape"), &Tetris::get_next_tetronimo_shape);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_x"), &Tetris::get_tetronimo_pos_x);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_y"), &Tetris::get_tetronimo_pos_y);

    ClassDB::bind_method(D_METHOD("get_score"), &Tetris::get_score);
    ClassDB::bind_method(D_METHOD("get_lines"), &Tetris::get_lines);
    ClassDB::bind_method(D_METHOD("get_level"), &Tetris::get_level);

    ClassDB::bind_method(D_METHOD("event_tetronimo_spawned"), &Tetris::event_tetronimo_spawned);
    ClassDB::bind_method(D_METHOD("event_tetronimo_moved"), &Tetris::event_tetronimo_moved);
    ClassDB::bind_method(D_METHOD("event_tetronimo_locked"), &Tetris::event_tetronimo_locked);
    ClassDB::bind_method(D_METHOD("event_matrix_rows_cleared"), &Tetris::event_matrix_rows_cleared);
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