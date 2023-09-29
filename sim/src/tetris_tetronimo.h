#ifndef TETRIS_TETRONIMO_H
#define TETRIS_TETRONIMO_H

#include "tetris_utils.h"
#include <stdbool.h>

typedef struct tetris_grid_t tetris_grid;

typedef uint8_t tetris_tetronimo_row;

#define TETRIS_TETRONIMO_MAX_HEIGHT 4

typedef struct tetris_tetronimo_rotation_t {
    tetris_tetronimo_row rows[TETRIS_TETRONIMO_MAX_HEIGHT];
} tetris_tetronimo_rotation;

typedef struct tetris_tetronimo_t {
    const tetris_tetronimo_rotation* rotations;
    int num_rotations;
    int current_rotation;
    int x;
    int y;
    bool is_grounded;
} tetris_tetronimo;

typedef struct {
    tetris_rand rand;
} tetris_tetronimo_spawner;

void tetris_tetronimo_init(tetris_tetronimo* tetronimo, const tetris_grid* grid, tetris_tetronimo_spawner* spawner);
void tetris_tetronimo_move(tetris_tetronimo* tetronimo, const tetris_grid* grid, const int dir_x, const int dir_y);

void tetris_tetronimo_spawner_init(tetris_tetronimo_spawner* spawner, const uint64_t seed);
tetris_tetronimo tetris_tetronimo_spawner_next(tetris_tetronimo_spawner* spawner);

#endif // TETRIS_TETRONIMO_H