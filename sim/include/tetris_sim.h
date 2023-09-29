#ifndef TETRIS_SIM_H
#define TETRIS_SIM_H

#include "tetris_host.h"
#include <stdbool.h>

typedef struct tetris_sim tetris_sim;

// initializes the simulation. internally allocates memory that can only be freed by calling deinit
tetris_sim* tetris_sim_init(tetris_host host);

// deinitialize the simulation. must be called on a sim pointer that was created with init
void tetris_sim_deinit(tetris_sim* sim);

// updates the simulation
void tetris_sim_update(tetris_sim* sim);

// returns true when the simulation is finished and the player has lost
bool tetris_sim_is_game_over(const tetris_sim* sim);

// get the width of the play field
int tetris_sim_get_matrix_width(const tetris_sim* sim);

// get the height of the play field
int tetris_sim_get_matrix_height(const tetris_sim* sim);

// returns true if a cell in the play field is occupied by a piece of tetronimo
bool tetris_sim_get_matrix_value(const tetris_sim* sim, int x, int y);

// get the max width of a tetronimo piece. pieces can be smaller than the max width
int tetris_sim_get_tetronimo_max_width(const tetris_sim* sim);

// get the max height of a tetronimo piece. pieces can be smaller than the max height
int tetris_sim_get_tetronimo_max_height(const tetris_sim* sim);

// returns true if (in tetronimo-space) the x and y value is a cell on the tetronimo -
// this takes into account the tetronimo's current rotation
bool tetris_sim_get_tetronimo_value(const tetris_sim* sim, int x, int y);

// returns the x position of the tetronimo in the play space. the origin is in the top left
int tetris_sim_get_tetronimo_pos_x(const tetris_sim* sim);

// returns the y position of the tetronimo in the play space. the origin is in the top left
int tetris_sim_get_tetronimo_pos_y(const tetris_sim* sim);

#endif // TETRIS_SIM_H