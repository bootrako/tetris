#include "tetris_matrix.h"
#include "tetris_tetronimo.h"

#define TETRIS_MATRIX_ROW_BIT_LENGTH (sizeof(tetris_matrix_row) * 8)
#define TETRIS_MATRIX_ROW_BIT_PADDING ((TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_WIDTH) / 2)
#define TETRIS_MATRIX_ROW_ALL_BITS_SET ((tetris_matrix_row)(-1))

// the init state has 1s for the non-playable area, and 0s for the (centered) playable area.
// for example, if the matrix width is 10, and the bit length is 16:
// 0b1110 0000 0000 0111
#define TETRIS_MATRIX_ROW_INIT (~((TETRIS_MATRIX_ROW_ALL_BITS_SET >> TETRIS_MATRIX_ROW_BIT_PADDING) & ~(TETRIS_MATRIX_ROW_ALL_BITS_SET >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING))))

void tetris_matrix_init(tetris_matrix* matrix) {
    for (int row = 0; row < TETRIS_MATRIX_HEIGHT; ++row) {
        matrix->rows[row] = TETRIS_MATRIX_ROW_INIT;
    }
}

// converts a tetronimo row to a matrix row, factoring in the tetronimo's x position
static tetris_matrix_row tetris_matrix_tetronimo_row_to_matrix_row(const tetris_tetronimo* tetronimo, const int row) {
    const tetris_tetronimo_row tetronimo_row = tetronimo->rotations[tetronimo->current_rotation].rows[row];
    const int shift = TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - TETRIS_TETRONIMO_MAX_WIDTH - tetronimo->x;
    if (shift > 0) {
        return ((tetris_matrix_row)tetronimo_row) << shift;
    } else {
        return ((tetris_matrix_row)tetronimo_row) >> -shift;
    }
}

void tetris_matrix_merge(tetris_matrix* matrix, const tetris_tetronimo* tetronimo) {
    for (int row = 0; row < TETRIS_TETRONIMO_MAX_HEIGHT; ++row) {
        matrix->rows[row + tetronimo->y] |= tetris_matrix_tetronimo_row_to_matrix_row(tetronimo, row);
    }
}

bool tetris_matrix_collide(const tetris_matrix* matrix, const tetris_tetronimo* tetronimo, const bool bounds_only) {
    for (int tetronimo_row = 0; tetronimo_row < TETRIS_TETRONIMO_MAX_HEIGHT; ++tetronimo_row) {
        // check for overlap with matrix bits
        const tetris_matrix_row matrix_row = bounds_only ? TETRIS_MATRIX_ROW_INIT : matrix->rows[tetronimo_row + tetronimo->y];
        if ((matrix_row & tetris_matrix_tetronimo_row_to_matrix_row(tetronimo, tetronimo_row)) != 0) {
            return true;
        }

        // check floor collision
        if (tetronimo_row + tetronimo->y >= TETRIS_MATRIX_HEIGHT) {
            return tetris_tetronimo_get_row(tetronimo, tetronimo_row) != 0;
        }
    }

    // if we get here, no collision
    return false;
}

int tetris_matrix_remove_completed_lines(tetris_matrix* matrix) {
    int num_completed_lines = 0;

    const tetris_matrix_row* read = &matrix->rows[TETRIS_MATRIX_HEIGHT - 1];
    tetris_matrix_row* write = &matrix->rows[TETRIS_MATRIX_HEIGHT - 1];
    while (read != &matrix->rows[0]) {
        if (*read == TETRIS_MATRIX_ROW_ALL_BITS_SET) {
            num_completed_lines++;
        } else {
            *write = *read;
            write--;
        }
        read--;
    }
    while (write != &matrix->rows[0]) {
        *write = TETRIS_MATRIX_ROW_INIT;
        write--;
    }

    return num_completed_lines;
}

bool tetris_matrix_get_value(const tetris_matrix* matrix, const int x, const int y) {
    return (matrix->rows[y] >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - 1 - x)) & 1;
}