#include "tetris_sim.h"
#include "tetris_matrix.h"
#include "tetris_tetronimo.h"

const float tetris_move_timer_init = 0.25f;

struct tetris_sim {
    tetris_host host;                   // the host interface
    tetris_matrix matrix;               // the play field
    tetris_tetronimo_spawner spawner;   // spawner for new tetronimos
    tetris_tetronimo tetronimo;         // the current tetronimo on the play field
    float time;                         // the current timestamp
    float move_timer;                   // timer counting down till the next down-move
    bool game_over;                     // true when the player has lost
};

tetris_sim* tetris_sim_init(tetris_host host) {
    tetris_sim* sim = (tetris_sim*)host.alloc(sizeof(tetris_sim));
    sim->host = host;
    tetris_matrix_init(&sim->matrix);
    tetris_tetronimo_spawner_init(&sim->spawner, sim->host.seed());
    tetris_tetronimo_init(&sim->tetronimo, &sim->matrix, &sim->spawner);
    sim->time = sim->host.time();
    sim->move_timer = tetris_move_timer_init;
    sim->game_over = false;
    return sim;
}

void tetris_sim_deinit(tetris_sim* sim) {
    sim->host.free(sim);
}

void tetris_sim_update(tetris_sim* sim) {
    if (tetris_sim_is_game_over(sim)) {
        return;
    }

    const float last_time = sim->time;
    sim->time = sim->host.time();
    const float delta_time = sim->time - last_time;

    sim->move_timer -= delta_time;
    if (sim->move_timer <= 0.0f) {
        tetris_tetronimo_move(&sim->tetronimo, &sim->matrix, 0, 1);
        sim->move_timer += tetris_move_timer_init;
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