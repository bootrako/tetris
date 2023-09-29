#include "tetris_matrix.h"
#include "tetris_tetronimo.h"

#define TETRIS_MATRIX_ROW_BIT_LENGTH (sizeof(tetris_matrix_row) * 8)
#define TETRIS_MATRIX_ROW_BIT_PADDING ((TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_WIDTH) / 2)
#define TETRIX_MATRIX_ROW_ALL_BITS_SET ((tetris_matrix_row)(-1))

// the init state has 1s for the non-playable area, and 0s for the (centered) playable area.
// for example, if the matrix width is 10, and the bit length is 16:
// 0b1110 0000 0000 0111
const tetris_matrix_row k_tetris_matrix_row_init_state = ~((TETRIX_MATRIX_ROW_ALL_BITS_SET >> TETRIS_MATRIX_ROW_BIT_PADDING) & ~(TETRIX_MATRIX_ROW_ALL_BITS_SET >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING)));

void tetris_matrix_init(tetris_matrix* matrix) {
    for (int row = 0; row < TETRIS_MATRIX_HEIGHT; ++row) {
        matrix->rows[row] = k_tetris_matrix_row_init_state;
    }
}

// converts a tetronimo row to a matrix row, factoring in the tetronimo's x position
static tetris_matrix_row tetris_matrix_tetronimo_row_to_matrix_row(const tetris_tetronimo* tetronimo, const int row) {
    const tetris_tetronimo_row tetronimo_row = tetronimo->rotations[tetronimo->current_rotation].rows[row];
    return ((tetris_matrix_row)tetronimo_row) << (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - TETRIS_TETRONIMO_MAX_WIDTH - tetronimo->x);
}

void tetris_matrix_merge(tetris_matrix* matrix, const tetris_tetronimo* tetronimo) {
    for (int row = 0; row < TETRIS_TETRONIMO_MAX_HEIGHT; ++row) {
        matrix->rows[row + tetronimo->y] |= tetris_matrix_tetronimo_row_to_matrix_row(tetronimo, row);
    }
}

bool tetris_matrix_collide(const tetris_matrix* matrix, const tetris_tetronimo* tetronimo) {
    // check for out of bounds
    if (tetronimo->x < 0 || tetronimo->x >= TETRIS_MATRIX_WIDTH || tetronimo->y + TETRIS_TETRONIMO_MAX_HEIGHT > TETRIS_MATRIX_HEIGHT) {
        return true;
    }

    // check for overlap
    for (int row = 0; row < TETRIS_TETRONIMO_MAX_HEIGHT; ++row) {
        if ((matrix->rows[row + tetronimo->y] & tetris_matrix_tetronimo_row_to_matrix_row(tetronimo, row)) != 0) {
            return true;
        }
    }

    // if we get here, no collision
    return false;
}

bool tetris_matrix_get_value(const tetris_matrix* matrix, const int x, const int y) {
    return (matrix->rows[y] >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - 1 - x)) & 1;
}