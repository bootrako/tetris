#include "tetris_sim.h"
#include "tetris_matrix.h"
#include "tetris_tetronimo.h"
#include "tetris_utils.h"

#define TETRIS_FAST_DROP_TIMER (3)
#define TETRIS_RESET_TIMER (12)
#define TETRIS_HORIZONTAL_HOLD_DELAY (16)
#define TETRIS_HORIZONTAL_HOLD_TIMER (6)

struct tetris_sim {
    tetris_sim_host host;                           // the host interface
    tetris_matrix matrix;                           // the play field
    tetris_tetronimo_spawner spawner;               // spawner for new tetronimos
    tetris_tetronimo tetronimo;                     // the current tetronimo on the play field
    bool input_buffer[TETRIS_INPUT_COUNT * 2];      // buffer storing inputs for last and current frame
    bool* input_pressed_prv;                        // points to last poll's inputs
    bool* input_pressed_cur;                        // points to current poll's inputs
    int score;                                      // total score
    int lines;                                      // total number of lines cleared
    int level;                                      // current level of the player
    int fast_drop_count;                            // number of cells fast dropped for the current tetronimo
    int drop_timer;                                 // timer counting down till the next drop
    int reset_timer;                                // timer counting down till the next tetronimo reset
    int horizontal_timer;                           // timer related to holding left or right
    int horizontal_dir;                             // the direction of left or right holding
    int event_tetronimo_locked_num_rows_cleared;    // if tetronimo was locked this frame, the number of frames that were cleared
    int event_tetronimo_locked_rows_cleared[TETRIS_TETRONIMO_MAX_HEIGHT]; // if tetronimo was locekd this frame, the list of rows that were cleared
    bool horizontal_hold;                           // true when player is holding left or right
    bool fast_drop;                                 // true when player is dropping quickly by pressing the fast drop input
    bool game_over;                                 // true when the player has lost
    bool event_tetronimo_spawned;                   // true if the tetronimo was spawned during this frame
    bool event_tetronimo_moved;                     // true if the tetronimo moved during this frame
    bool event_tetronimo_locked;                    // true if the tetronimo was locked in place during this frame
};

static void tetris_sim_poll_input(tetris_sim* sim) {
    bool* temp = sim->input_pressed_prv;
    sim->input_pressed_prv = sim->input_pressed_cur;
    sim->input_pressed_cur = temp;
    for (int input = 0; input < TETRIS_INPUT_COUNT; ++input) {
        sim->input_pressed_cur[input] = sim->host.input_pressed(sim->host.context, input);
    }
}

static bool tetris_sim_input_just_pressed(tetris_sim* sim, tetris_input input) {
    return sim->input_pressed_cur[input] && !sim->input_pressed_prv[input];
}

static bool tetris_sim_input_just_released(tetris_sim* sim, tetris_input input) {
    return !sim->input_pressed_cur[input] && sim->input_pressed_prv[input];
}

static int tetris_sim_calc_drop_timer(const tetris_sim* sim) {
    const int drop_frames_per_level[] = { 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 };
    const int level_index = sim->level >= TETRIS_ARRAY_LEN(drop_frames_per_level) ? TETRIS_ARRAY_LEN(drop_frames_per_level) - 1 : sim->level;
    return drop_frames_per_level[level_index];
}

static int tetris_sim_calc_score(const tetris_sim* sim, const int completed_lines) {
    const int completed_lines_base_score[] = { 0, 40, 100, 300, 1200 };
    return (sim->fast_drop_count) + (completed_lines_base_score[completed_lines] * (sim->level + 1));
}

tetris_sim* tetris_sim_init(tetris_sim_host host, const uint64_t random_seed) {
    tetris_sim* sim = (tetris_sim*)host.alloc(host.context, sizeof(tetris_sim));
    sim->host = host;
    tetris_matrix_init(&sim->matrix);
    tetris_tetronimo_spawner_init(&sim->spawner, random_seed);
    tetris_tetronimo_init(&sim->tetronimo, &sim->matrix, &sim->spawner);
    for (int input = 0; input < TETRIS_INPUT_COUNT * 2; ++input) {
        sim->input_buffer[input] = false;
    }
    sim->input_pressed_prv = &sim->input_buffer[0];
    sim->input_pressed_cur = &sim->input_buffer[TETRIS_INPUT_COUNT];
    sim->score = 0;
    sim->lines = 0;
    sim->level = 0;
    sim->fast_drop_count = 0;
    sim->drop_timer = tetris_sim_calc_drop_timer(sim);
    sim->reset_timer = TETRIS_RESET_TIMER;
    sim->horizontal_timer = 0;
    sim->horizontal_hold = false;
    sim->fast_drop = false;
    sim->game_over = false;
    sim->event_tetronimo_spawned = false;
    sim->event_tetronimo_moved = false;
    sim->event_tetronimo_locked = false;
    return sim;
}

void tetris_sim_deinit(tetris_sim* sim) {
    sim->host.free(sim->host.context, sim);
}

void tetris_sim_update(tetris_sim* sim) {
    if (tetris_sim_is_game_over(sim)) {
        return;
    }

    sim->event_tetronimo_spawned = false;
    sim->event_tetronimo_moved = false;
    sim->event_tetronimo_locked = false;

    tetris_sim_poll_input(sim);

    if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_FAST_DROP)) {
        sim->fast_drop = true;
        sim->drop_timer = TETRIS_FAST_DROP_TIMER;
    }

    if (tetris_sim_input_just_released(sim, TETRIS_INPUT_FAST_DROP)) {
        sim->fast_drop = false;
        sim->fast_drop_count = 0;
        sim->drop_timer = tetris_sim_calc_drop_timer(sim);
    }

    if (sim->tetronimo.is_active) {
        if (!sim->fast_drop && tetris_sim_input_just_pressed(sim, TETRIS_INPUT_MOVE_LEFT)) {
            if (!tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, -1, 0)) {
                sim->event_tetronimo_moved = true;
            }
            sim->horizontal_hold = true;
            sim->horizontal_timer = TETRIS_HORIZONTAL_HOLD_DELAY;
            sim->horizontal_dir = -1;
        }

        if (tetris_sim_input_just_released(sim, TETRIS_INPUT_MOVE_LEFT)) {
            sim->horizontal_hold = false;
        }

        if (!sim->fast_drop && tetris_sim_input_just_pressed(sim, TETRIS_INPUT_MOVE_RIGHT)) {
            if (!tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 1, 0)) {
                sim->event_tetronimo_moved = true;
            }
            sim->horizontal_hold = true;
            sim->horizontal_timer = TETRIS_HORIZONTAL_HOLD_DELAY;
            sim->horizontal_dir = 1;
        }

        if (tetris_sim_input_just_released(sim, TETRIS_INPUT_MOVE_RIGHT)) {
            sim->horizontal_hold = false;
        }

        if (sim->horizontal_hold) {
            sim->horizontal_timer--;
            if (sim->horizontal_timer <= 0) {
                if (!tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, sim->horizontal_dir, 0)) {
                    sim->event_tetronimo_moved = true;
                }
                sim->horizontal_timer = TETRIS_HORIZONTAL_HOLD_TIMER;
            }
        }

        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_ROTATE_LEFT)) {
            tetris_tetronimo_rotate(&sim->tetronimo, &sim->matrix, -1);
            sim->event_tetronimo_moved = true;
        }

        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_ROTATE_RIGHT)) {
            tetris_tetronimo_rotate(&sim->tetronimo, &sim->matrix, 1);
            sim->event_tetronimo_moved = true;
        }
        
        sim->drop_timer--;
        if (sim->drop_timer <= 0) {
            if (!tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 0, 1)) {
                sim->event_tetronimo_moved = true;
                if (sim->fast_drop) {
                    sim->fast_drop_count++;
                }
            }
            sim->drop_timer += sim->fast_drop ? TETRIS_FAST_DROP_TIMER : tetris_sim_calc_drop_timer(sim);
        }

        if (sim->tetronimo.is_locked) {
            tetris_matrix_merge(&sim->matrix, &sim->tetronimo);
            sim->event_tetronimo_locked = true;
            sim->event_tetronimo_locked_num_rows_cleared = tetris_matrix_get_completed_lines(&sim->matrix, sim->event_tetronimo_locked_rows_cleared);
            sim->tetronimo.is_active = false;
            sim->reset_timer = TETRIS_RESET_TIMER;
        } 
    } else {
        sim->reset_timer--;
        if (sim->reset_timer <= 0.0f) {
            const int completed_lines = tetris_matrix_remove_completed_lines(&sim->matrix);
            sim->score += tetris_sim_calc_score(sim, completed_lines);
            sim->lines += completed_lines;
            sim->level = sim->lines / 10;
            sim->horizontal_hold = false;
            sim->fast_drop_count = 0;
            sim->fast_drop = false;
            sim->drop_timer = tetris_sim_calc_drop_timer(sim);

            tetris_tetronimo_init(&sim->tetronimo, &sim->matrix, &sim->spawner);
            if (sim->tetronimo.is_locked) {
                sim->game_over = true;
                return;
            }
            sim->event_tetronimo_spawned = true;
        }
    }
}

float tetris_sim_time_per_frame(const tetris_sim* sim) {
    return 1.0f / 60.0f;
}

bool tetris_sim_is_game_over(const tetris_sim* sim) {
    return sim->game_over;
}

int tetris_sim_get_matrix_width(const tetris_sim* sim) {
    return TETRIS_MATRIX_WIDTH;
}

int tetris_sim_get_matrix_height(const tetris_sim* sim) {
    return TETRIS_MATRIX_HEIGHT;
}

bool tetris_sim_get_matrix_value(const tetris_sim* sim, int x, int y) {
    return tetris_matrix_get_value(&sim->matrix, x, y);
}

bool tetris_sim_is_tetronimo_active(const tetris_sim* sim) {
    return sim->tetronimo.is_active;
}

int tetris_sim_get_tetronimo_max_width(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_MAX_WIDTH;
}

int tetris_sim_get_tetronimo_max_height(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_MAX_HEIGHT;
}

bool tetris_sim_get_tetronimo_value(const tetris_sim* sim, int x, int y) {
    return tetris_tetronimo_get_value(&sim->tetronimo, x, y);
}

bool tetris_sim_get_next_tetronimo_value(const tetris_sim* sim, int x, int y) {
    return tetris_tetronimo_get_value(sim->spawner.next, x, y);
}

int tetris_sim_get_tetronimo_pos_x(const tetris_sim* sim) {
    return sim->tetronimo.x;
}

int tetris_sim_get_tetronimo_pos_y(const tetris_sim* sim) {
    return sim->tetronimo.y;
}

int tetris_sim_get_score(const tetris_sim* sim) {
    return sim->score;
}

int tetris_sim_get_lines(const tetris_sim* sim) {
    return sim->lines;
}

int tetris_sim_get_level(const tetris_sim* sim) {
    return sim->level;
}

int tetris_sim_get_statistic_count(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_SHAPE_COUNT;
}

const char* tetris_sim_get_statistic_name(const tetris_sim* sim, int index) {
    return k_tetris_tetronimo_shape_names[index];
}

int tetris_sim_get_statistic_value(const tetris_sim* sim, int index) {
    return sim->spawner.num_spawned[index];
}

bool tetris_sim_event_tetronimo_spawned(const tetris_sim* sim) {
    return sim->event_tetronimo_spawned;
}

bool tetris_sim_event_tetronimo_moved(const tetris_sim* sim) {
    return sim->event_tetronimo_moved;
}

bool tetris_sim_event_tetronimo_locked(const tetris_sim* sim) {
    return sim->event_tetronimo_locked;
}

int tetris_sim_event_tetronimo_locked_get_num_rows_cleared(const tetris_sim* sim) {
    return sim->event_tetronimo_locked_num_rows_cleared;
}

const int* tetris_sim_event_tetronimo_locked_get_rows_cleared(const tetris_sim* sim) {
    return sim->event_tetronimo_locked_rows_cleared;
}