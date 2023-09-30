#include "tetris_sim.h"
#include "tetris_matrix.h"
#include "tetris_tetronimo.h"

#define TETRIS_MOVE_TIMER_INIT (0.5f)
#define TETRIS_MOVE_TIMER_FAST_FALL (0.1f)

struct tetris_sim {
    tetris_sim_host host;                       // the host interface
    tetris_matrix matrix;                       // the play field
    tetris_tetronimo_spawner spawner;           // spawner for new tetronimos
    tetris_tetronimo tetronimo;                 // the current tetronimo on the play field
    bool input_buffer[TETRIS_INPUT_COUNT * 2];  // buffer storing inputs for last and current frame
    bool* input_pressed_prv;                    // points to last poll's inputs
    bool* input_pressed_cur;                    // points to current poll's inputs
    float time;                                 // the current timestamp
    float fall_timer;                           // timer counting down till the next down-move
    bool fast_fall;                             // true when player is falling quickly by pressing the fast fall input
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
    sim->time = sim->host.time(sim->host.context);
    sim->fall_timer = TETRIS_MOVE_TIMER_INIT;
    sim->fast_fall = false;
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

    if (tetris_sim_input_just_pressed(sim, TETRIS_INPUT_FAST_FALL)) {
        sim->fast_fall = true;
        sim->fall_timer = 0.0f;
    }

    if (tetris_sim_input_just_released(sim, TETRIS_INPUT_FAST_FALL)) {
        sim->fast_fall = false;
        sim->fall_timer = TETRIS_MOVE_TIMER_INIT;
    }

    const float last_time = sim->time;
    sim->time = sim->host.time(sim->host.context);
    const float delta_time = sim->time - last_time;

    sim->fall_timer -= delta_time;
    if (sim->fall_timer <= 0.0f) {
        tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 0, 1);
        sim->fall_timer += sim->fast_fall ? TETRIS_MOVE_TIMER_FAST_FALL : TETRIS_MOVE_TIMER_INIT;
    }

    if (sim->tetronimo.is_grounded) {
        tetris_matrix_merge(&sim->matrix, &sim->tetronimo);
        tetris_tetronimo_init(&sim->tetronimo, &sim->matrix, &sim->spawner);
        if (sim->tetronimo.is_grounded) {
            sim->game_over = true;
            return;
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

int tetris_sim_get_tetronimo_max_width(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_MAX_WIDTH;
}

int tetris_sim_get_tetronimo_max_height(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_MAX_HEIGHT;
}

bool tetris_sim_get_tetronimo_value(const tetris_sim* sim, int x, int y) {
    return tetris_tetronimo_get_value(&sim->tetronimo, x, y);
}

int tetris_sim_get_tetronimo_pos_x(const tetris_sim* sim) {
    return sim->tetronimo.x;
}

int tetris_sim_get_tetronimo_pos_y(const tetris_sim* sim) {
    return sim->tetronimo.y;
}