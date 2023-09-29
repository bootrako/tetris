#ifndef TETRIS_SIM_H
#define TETRIS_SIM_H

#include "tetris_host.h"
#include <stdbool.h>

typedef struct tetris_sim tetris_sim;

tetris_sim* tetris_sim_init(tetris_host host);
void tetris_sim_deinit(tetris_sim* sim);
void tetris_sim_update(tetris_sim* sim);

bool tetris_sim_is_game_over(const tetris_sim* sim);

int tetris_sim_get_num_grid_rows(const tetris_sim* sim);
const uint16_t* tetris_sim_get_grid_rows(const tetris_sim* sim);

int tetris_sim_get_num_tetronimo_rows(const tetris_sim* sim);
const uint8_t* tetris_sim_get_tetronimo_rows(const tetris_sim* sim);
int tetris_sim_get_tetronimo_x(const tetris_sim* sim);
int tetris_sim_get_tetronimo_y(const tetris_sim* sim);

#endif // TETRIS_SIM_H