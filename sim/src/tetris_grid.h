#ifndef TETRIS_GRID_H
#define TETRIS_GRID_H

#include <stdint.h>
#include <stdbool.h>

typedef struct tetris_tetronimo_t tetris_tetronimo;

typedef uint16_t tetris_grid_row;

#define TETRIS_GRID_HEIGHT 24

typedef struct tetris_grid_t {
    tetris_grid_row rows[TETRIS_GRID_HEIGHT];
} tetris_grid;

void tetris_grid_init(tetris_grid* grid);
void tetris_grid_merge(tetris_grid* grid, const tetris_tetronimo* tetronimo);
bool tetris_grid_collide(const tetris_grid* grid, const tetris_tetronimo* tetronimo);

#endif // TETRIS_GRID_H