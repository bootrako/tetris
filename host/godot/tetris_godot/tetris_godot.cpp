#include "tetris_godot.h"
#include "core/error/error_macros.h"
#include "core/input/input_map.h"
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
    InputMap* input_map = InputMap::get_singleton();

    const char* const input_to_action[TETRIS_INPUT_COUNT] = {
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

bool Tetris::get_matrix_value(int x, int y) const {
    return tetris_sim_get_matrix_value(sim, x, y);
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

bool Tetris::get_tetronimo_value(int x, int y) const {
    return tetris_sim_get_tetronimo_value(sim, x, y);
}

bool Tetris::get_next_tetronimo_value(int x, int y) const {
    return tetris_sim_get_next_tetronimo_value(sim, x, y);
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

int Tetris::get_statistic_count() const {
    return tetris_sim_get_statistic_count(sim);
}

String Tetris::get_statistic_name(int index) const {
    return tetris_sim_get_statistic_name(sim, index);
}

int Tetris::get_statistic_value(int index) const {
    return tetris_sim_get_statistic_value(sim, index);
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
    ClassDB::bind_method(D_METHOD("get_matrix_value", "x", "y"), &Tetris::get_matrix_value);

    ClassDB::bind_method(D_METHOD("is_tetronimo_active"), &Tetris::is_tetronimo_active);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_width"), &Tetris::get_tetronimo_max_width);
    ClassDB::bind_method(D_METHOD("get_tetronimo_max_height"), &Tetris::get_tetronimo_max_height);
    ClassDB::bind_method(D_METHOD("get_tetronimo_value", "x", "y"), &Tetris::get_tetronimo_value);
    ClassDB::bind_method(D_METHOD("get_next_tetronimo_value", "x", "y"), &Tetris::get_next_tetronimo_value);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_x"), &Tetris::get_tetronimo_pos_x);
    ClassDB::bind_method(D_METHOD("get_tetronimo_pos_y"), &Tetris::get_tetronimo_pos_y);

    ClassDB::bind_method(D_METHOD("get_score"), &Tetris::get_score);
    ClassDB::bind_method(D_METHOD("get_lines"), &Tetris::get_lines);
    ClassDB::bind_method(D_METHOD("get_level"), &Tetris::get_level);

    ClassDB::bind_method(D_METHOD("get_statistic_count"), &Tetris::get_statistic_count);
    ClassDB::bind_method(D_METHOD("get_statistic_name", "index"), &Tetris::get_statistic_name);
    ClassDB::bind_method(D_METHOD("get_statistic_value", "index"), &Tetris::get_statistic_value);

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