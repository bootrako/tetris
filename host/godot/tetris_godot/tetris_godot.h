#ifndef TETRIS_GODOT_H
#define TETRIS_GODOT_H

#include "core/object/ref_counted.h"
#include <tetris_sim.h>

class Tetris : public RefCounted {
    GDCLASS(Tetris, RefCounted);
public:
    Tetris();
    virtual ~Tetris();

    void update_sim();
    void poll_input();

    float get_sim_time_per_frame() const;
    bool is_sim_game_over() const;

    int get_matrix_width() const;
    int get_matrix_height() const;
    bool get_matrix_value(int x, int y) const;

    bool is_tetronimo_active() const;
    int get_tetronimo_max_width() const;
    int get_tetronimo_max_height() const;
    bool get_tetronimo_value(int x, int y) const;
    bool get_next_tetronimo_value(int x, int y) const;
    int get_tetronimo_pos_x() const;
    int get_tetronimo_pos_y() const;

    int get_score() const;
    int get_lines() const;
    int get_level() const;

    int get_statistic_count() const;
    String get_statistic_name(int index) const;
    int get_statistic_value(int index) const;

    bool event_tetronimo_spawned() const;
    bool event_tetronimo_moved() const;
    bool event_tetronimo_locked() const;
    Vector<int> event_matrix_rows_cleared() const;

protected:
    static void _bind_methods();

private:
    static void* _godot_alloc(void* context, size_t size);
    static void _godot_free(void* context, void* ptr);
    static void _godot_panic(void* context, const char* err_msg);
    static bool _godot_input_pressed(void* context, const tetris_input input);

    tetris_sim* sim;
    bool input_pressed[TETRIS_INPUT_COUNT];
};

#endif // TETRIS_GODOT_H