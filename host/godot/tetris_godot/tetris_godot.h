#ifndef TETRIS_GODOT_H
#define TETRIS_GODOT_H

#include "core/object/ref_counted.h"
#include <tetris_sim.h>

class TetrisSim : public RefCounted {
    GDCLASS(TetrisSim, RefCounted);
public:
    TetrisSim();
    virtual ~TetrisSim();

    void update();
    void poll_input();

    float get_time_per_frame() const;
    bool is_game_over() const;

    int get_matrix_width() const;
    int get_matrix_height() const;
    bool get_matrix_cell(int x, int y) const;
    int get_matrix_cell_shape(int x, int y) const;

    bool is_tetronimo_active() const;
    int get_tetronimo_max_width() const;
    int get_tetronimo_max_height() const;
    bool get_tetronimo_cell(int x, int y) const;
    int get_tetronimo_shape() const;
    bool get_next_tetronimo_cell(int x, int y) const;
    int get_next_tetronimo_shape() const;
    int get_tetronimo_pos_x() const;
    int get_tetronimo_pos_y() const;

    int get_score() const;
    int get_lines() const;
    int get_level() const;

    bool event_tetronimo_spawned() const;
    bool event_tetronimo_moved() const;
    bool event_tetronimo_locked() const;
    bool event_tetronimo_rotated() const;
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