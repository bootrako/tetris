#include "tetris_sim.h"
#include "tetris_matrix.h"
#include "tetris_tetronimo.h"
#include "tetris_utils.h"

#define TETRIS_FAST_DROP_TIMER (0.05f)
#define TETRIS_RESET_TIMER (0.2f)

struct tetris_sim {
    tetris_sim_host host;                       // the host interface
    tetris_matrix matrix;                       // the play field
    tetris_tetronimo_spawner spawner;           // spawner for new tetronimos
    tetris_tetronimo tetronimo;                 // the current tetronimo on the play field
    bool input_buffer[TETRIS_INPUT_COUNT * 2];  // buffer storing inputs for last and current frame
    bool* input_pressed_prv;                    // points to last poll's inputs
    bool* input_pressed_cur;                    // points to current poll's inputs
    int score;                                  // total score
    int lines;                                  // total number of lines cleared
    int level;                                  // current level of the player
    int fast_drop_count;                        // number of cells fast dropped for the current tetronimo
    float time;                                 // the current timestamp
    float drop_timer;                           // timer counting down till the next drop
    float reset_timer;                          // timer counting down till the next tetronimo reset
    bool fast_drop;                             // true when player is dropping quickly by pressing the fast drop input
    bool game_over;                             // true when the player has lost
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

static float tetris_sim_calc_drop_timer(const tetris_sim* sim) {
    const int drop_frames_per_level[] = { 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 };
    const int level_index = sim->level >= TETRIS_ARRAY_LEN(drop_frames_per_level) ? TETRIS_ARRAY_LEN(drop_frames_per_level) - 1 : sim->level;
    return (float)drop_frames_per_level[level_index] / 60.0f;
}

static int tetris_sim_calc_score(const tetris_sim* sim, const int completed_lines) {
    const int completed_lines_base_score[] = { 0, 40, 100, 300, 1200 };
    return (sim->fast_drop_count / 2) + (completed_lines_base_score[completed_lines] * (sim->level + 1));
}

tetris_sim* tetris_sim_init(tetris_sim_host host) {
    tetris_sim* sim = (tetris_sim*)host.alloc(host.context, sizeof(tetris_sim));
    sim->host = host;
    tetris_matrix_init(&sim->matrix);
    tetris_tetronimo_spawner_init(&sim->spawner, sim->host.seed(sim->host.context));
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
    sim->time = sim->host.time(sim->host.context);
    sim->drop_timer = tetris_sim_calc_drop_timer(sim);
    sim->reset_timer = TETRIS_RESET_TIMER;
    sim->fast_drop = false;
    sim->game_over = false;
    return sim;
}

void tetris_sim_deinit(tetris_sim* sim) {
    sim->host.free(sim->host.context, sim);
}

void tetris_sim_update(tetris_sim* sim) {
    if (tetris_sim_is_game_over(sim)) {
        return;
    }

    tetris_sim_poll_input(sim);

    const float last_time = sim->time;
    sim->time = sim->host.time(sim->host.context);
    const float delta_time = sim->time - last_time;

    if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_FAST_DROP)) {
        sim->fast_drop = true;
        sim->drop_timer = 0.0f;
    }

    if (tetris_sim_input_just_released(sim, TETRIS_INPUT_FAST_DROP)) {
        sim->fast_drop = false;
        sim->drop_timer = tetris_sim_calc_drop_timer(sim);
    }

    if (sim->tetronimo.is_active) {
        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_MOVE_LEFT)) {
            tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, -1, 0);
        }

        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_MOVE_RIGHT)) {
            tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 1, 0);
        }

        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_ROTATE_LEFT)) {
            tetris_tetronimo_rotate(&sim->tetronimo, &sim->matrix, -1);
        }

        if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_ROTATE_RIGHT)) {
            tetris_tetronimo_rotate(&sim->tetronimo, &sim->matrix, 1);
        }
        
        sim->drop_timer -= delta_time;
        if (sim->drop_timer <= 0.0f) {
            const bool has_collision = tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 0, 1);
            if (!has_collision && sim->fast_drop) {
                sim->fast_drop_count++;
            }
            sim->drop_timer += sim->fast_drop ? TETRIS_FAST_DROP_TIMER : tetris_sim_calc_drop_timer(sim);
        }

        if (sim->tetronimo.is_grounded) {
            tetris_matrix_merge(&sim->matrix, &sim->tetronimo);
            sim->tetronimo.is_active = false;
            sim->reset_timer = TETRIS_RESET_TIMER;
        } 
    } else {
        sim->reset_timer -= delta_time;
        if (sim->reset_timer <= 0.0f) {
            const int completed_lines = tetris_matrix_remove_completed_lines(&sim->matrix);
            sim->score += tetris_sim_calc_score(sim, completed_lines);
            sim->lines += completed_lines;
            sim->level = sim->lines / 10;
            sim->fast_drop_count = 0;

            tetris_tetronimo_init(&sim->tetronimo, &sim->matrix, &sim->spawner);
            if (sim->tetronimo.is_grounded) {
                sim->game_over = true;
                return;
            }
        }
    }
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
    return tetris_tetronimo_get_value(&sim->spawner.next, x, y);
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