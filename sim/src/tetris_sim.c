#include "tetris_sim.h"
#include "tetris_grid.h"
#include "tetris_tetronimo.h"

const float tetris_move_timer_init = 0.25f;

struct tetris_sim {
    tetris_host host;
    tetris_grid grid;
    tetris_tetronimo_spawner spawner;
    tetris_tetronimo tetronimo;
    uint64_t time;
    float move_timer;
    bool game_over;
};

tetris_sim* tetris_sim_init(tetris_host host) {
    tetris_sim* sim = (tetris_sim*)host.alloc(sizeof(tetris_sim));
    sim->host = host;
    tetris_grid_init(&sim->grid);
    tetris_tetronimo_spawner_init(&sim->spawner, sim->time);
    tetris_tetronimo_init(&sim->tetronimo, &sim->grid, &sim->spawner);
    sim->time = sim->host.time();
    sim->move_timer = tetris_move_timer_init;
    sim->game_over = false;
    return sim;
}

void tetris_sim_deinit(tetris_sim* sim) {
    sim->host.free(sim);
}

static float tetris_sim_delta_time(tetris_sim* sim) {
    const uint64_t last_time = sim->time;
    sim->time = sim->host.time();

    const float ns_to_s = 1.0f / 1000000000.0f;
    return(float)(sim->time - last_time) * ns_to_s;
}

void tetris_sim_update(tetris_sim* sim) {
    if (tetris_sim_is_game_over(sim)) {
        return;
    }

    float delta_time = tetris_sim_delta_time(sim);
    delta_time = 0.033f;

    sim->move_timer -= delta_time;
    if (sim->move_timer <= 0.0f) {
        tetris_tetronimo_move(&sim->tetronimo, &sim->grid, 0, 1);
        sim->move_timer = tetris_move_timer_init;
    }

    if (sim->tetronimo.is_grounded) {
        tetris_grid_merge(&sim->grid, &sim->tetronimo);
        tetris_tetronimo_init(&sim->tetronimo, &sim->grid, &sim->spawner);
        if (sim->tetronimo.is_grounded) {
            sim->game_over = true;
            return;
        }
    }
}

bool tetris_sim_is_game_over(const tetris_sim* sim) {
    return sim->game_over;
}

int tetris_sim_get_num_grid_rows(const tetris_sim* sim) {
    return TETRIS_GRID_HEIGHT;
}

const uint16_t* tetris_sim_get_grid_rows(const tetris_sim* sim) {
    return sim->grid.rows;
}

int tetris_sim_get_num_tetronimo_rows(const tetris_sim* sim) {
    return TETRIS_TETRONIMO_MAX_HEIGHT;
}

const uint8_t* tetris_sim_get_tetronimo_rows(const tetris_sim* sim) {
    return sim->tetronimo.rotations[sim->tetronimo.current_rotation].rows;
}

int tetris_sim_get_tetronimo_x(const tetris_sim* sim) {
    return sim->tetronimo.x + 3;
}

int tetris_sim_get_tetronimo_y(const tetris_sim* sim) {
    return sim->tetronimo.y;
}