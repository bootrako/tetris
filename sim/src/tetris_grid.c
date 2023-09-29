#include "tetris_grid.h"
#include "tetris_tetronimo.h"

const int tetronimo_shift_base = 13;

// playable field is 10 cells wide
// 1 = blocked cell, 0 = empty cell
// 0b1110 0000 0000 0111
const tetris_grid_row tetris_grid_row_init_state = 0xE007;

void tetris_grid_init(tetris_grid* grid) {
    for (int i = 0; i < TETRIS_GRID_HEIGHT; ++i) {
        grid->rows[i] = tetris_grid_row_init_state;
    }
}

static tetris_grid_row tetris_grid_get_tetronimo_grid_row(const tetris_tetronimo* tetronimo, int index) {
    const tetris_tetronimo_row* tetronimo_rows = tetronimo->rotations[tetronimo->current_rotation].rows;
    return ((tetris_grid_row)tetronimo_rows[index]) << (tetronimo_shift_base - tetronimo->x);
}

void tetris_grid_merge(tetris_grid* grid, const tetris_tetronimo* tetronimo) {
    for (int i = 0; i < TETRIS_TETRONIMO_MAX_HEIGHT; ++i) {
        grid->rows[i + tetronimo->y] |= tetris_grid_get_tetronimo_grid_row(tetronimo, i);
    }
}

bool tetris_grid_collide(const tetris_grid* grid, const tetris_tetronimo* tetronimo) {
    if (tetronimo->y + TETRIS_TETRONIMO_MAX_HEIGHT > TETRIS_GRID_HEIGHT) {
        return true;
    }

    for (int i = 0; i < TETRIS_TETRONIMO_MAX_HEIGHT; ++i) {
        if ((grid->rows[i + tetronimo->y] & tetris_grid_get_tetronimo_grid_row(tetronimo, i)) != 0) {
            return true;
        }
    }
    return false;
}