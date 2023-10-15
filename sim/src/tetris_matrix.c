#include "tetris_matrix.h"
#include "tetris_ctx.h"
#include "tetris_tetronimo.h"

#define TETRIS_MATRIX_ROW_BIT_LENGTH (TETRIS_BITSIZE(tetris_matrix_row))
#define TETRIS_MATRIX_ROW_BIT_PADDING ((TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_WIDTH) / 2)
#define TETRIS_MATRIX_ROW_ALL_BITS_SET ((tetris_matrix_row)(-1))

// the init state has 1s for the non-playable area, and 0s for the (centered) playable area.
// for example, if the matrix width is 10, and the bit length is 16:
// 0b1110 0000 0000 0111
#define TETRIS_MATRIX_ROW_INIT (~((TETRIS_MATRIX_ROW_ALL_BITS_SET >> TETRIS_MATRIX_ROW_BIT_PADDING) & ~(TETRIS_MATRIX_ROW_ALL_BITS_SET >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING))))

#define TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE (3)
#define TETRIS_MATRIX_SHAPE_ROW_BIT_LENGTH (TETRIS_BITSIZE(tetris_matrix_shape_row))
#define TETRIS_MATRIX_SHAPE_ROW_BIT_PADDING (TETRIS_MATRIX_SHAPE_ROW_BIT_LENGTH - (TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE * TETRIS_MATRIX_WIDTH))
#define TETRIS_MATRIX_SHAPE_ROW_ALL_BITS_SET ((tetris_matrix_shape_row)(-1))
#define TETRIS_MATRIX_SHAPE_ROW_CELL_MASK (TETRIS_MATRIX_SHAPE_ROW_ALL_BITS_SET >> (TETRIS_MATRIX_SHAPE_ROW_BIT_LENGTH - TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE))
#define TETRIS_MATRIX_SHAPE_ROW_INIT (0)

// converts a tetronimo row to a matrix row, factoring in the tetronimo's x position
static tetris_matrix_row tetris_tetronimo_row_to_matrix_row(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int row) {
    const tetris_matrix_row matrix_row = tetris_tetronimo_get_row(ctx, tetronimo, row);
    if (matrix_row == 0) {
        return 0;
    }
    const int shift = TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - TETRIS_TETRONIMO_MAX_WIDTH - tetronimo->x;
    return (shift >= 0) ? matrix_row << shift : matrix_row >> -shift;
}

// converts a tetronimo row to a matrix shape row, factoring in the tetronimo's x position
static tetris_matrix_shape_row tetris_tetronimo_row_to_matrix_shape_row(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int row) {
    const unsigned int matrix_row = (unsigned int)tetris_tetronimo_row_to_matrix_row(ctx, tetronimo, row);
    if (matrix_row == 0) {
        return 0;
    }
    const int bit_start = tetris_ctz(matrix_row);
    const int bit_count = (TETRIS_BITSIZE(unsigned int)) - tetris_clz(matrix_row) - bit_start;
    
    tetris_matrix_shape_row shape_row = 0;
    if (bit_count > 0) {
        for (int i = 0; i < bit_count; ++i) {
            shape_row <<= TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE;
            shape_row |= tetronimo->shape;
        }
        shape_row <<= (bit_start - TETRIS_MATRIX_ROW_BIT_PADDING) * TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE;
    }
    return shape_row;
}

void tetris_matrix_init(tetris_ctx* ctx, tetris_matrix* matrix) {
    for (int row = 0; row < TETRIS_MATRIX_HEIGHT; ++row) {
        matrix->rows[row] = TETRIS_MATRIX_ROW_INIT;
        matrix->shape_rows[row] = TETRIS_MATRIX_SHAPE_ROW_INIT;
    }
}

int tetris_matrix_merge(tetris_ctx* ctx, tetris_matrix* matrix, const tetris_tetronimo* tetronimo) {
    ctx->events.num_matrix_rows_cleared = 0;
    for (int row = 0; row < TETRIS_TETRONIMO_MAX_HEIGHT; ++row) {
        const int matrix_row_index = row + tetronimo->y;
        if (matrix_row_index < 0) {
            continue;
        }
        if (matrix_row_index >= TETRIS_MATRIX_HEIGHT) {
            break;
        }
        matrix->rows[matrix_row_index] |= tetris_tetronimo_row_to_matrix_row(ctx, tetronimo, row);
        matrix->shape_rows[matrix_row_index] |= tetris_tetronimo_row_to_matrix_shape_row(ctx, tetronimo, row);
        if (matrix->rows[matrix_row_index] == TETRIS_MATRIX_ROW_ALL_BITS_SET) {
            ctx->events.matrix_rows_cleared[ctx->events.num_matrix_rows_cleared] = matrix_row_index;
            ctx->events.num_matrix_rows_cleared++;
        }
    }
    return ctx->events.num_matrix_rows_cleared;
}

bool tetris_matrix_is_tetronimo_valid(const tetris_ctx* ctx, const tetris_matrix* matrix, const tetris_tetronimo* tetronimo) {
    for (int tetronimo_row = 0; tetronimo_row < TETRIS_TETRONIMO_MAX_HEIGHT; ++tetronimo_row) {
        const int matrix_row_index = tetronimo_row + tetronimo->y;

        // ignore anything above matrix
        if (matrix_row_index < 0) {
            continue;
        }
        
        // check floor overlap
        if (matrix_row_index >= TETRIS_MATRIX_HEIGHT) {
            return tetris_tetronimo_get_row(ctx, tetronimo, tetronimo_row) == 0;
        }

        // check for overlap with matrix bits
        if ((matrix->rows[matrix_row_index] & tetris_tetronimo_row_to_matrix_row(ctx, tetronimo, tetronimo_row)) != 0) {
            return false;
        }
    }

    // if we get here, no overlap
    return true;
}

int tetris_matrix_remove_cleared_lines(tetris_ctx* ctx, tetris_matrix* matrix) {
    int num_cleared_lines = 0;

    int read_row = TETRIS_MATRIX_HEIGHT - 1;
    int write_row = TETRIS_MATRIX_HEIGHT - 1;
    while (read_row >= 0) {
        if (matrix->rows[read_row] == TETRIS_MATRIX_ROW_ALL_BITS_SET) {
            num_cleared_lines++;
        } else {
            matrix->rows[write_row] = matrix->rows[read_row];
            matrix->shape_rows[write_row] = matrix->shape_rows[read_row];
            write_row--;
        }
        read_row--;
    }
    while (write_row >= 0) {
        matrix->rows[write_row] = TETRIS_MATRIX_ROW_INIT;
        matrix->shape_rows[write_row] = TETRIS_MATRIX_SHAPE_ROW_INIT;
        write_row--;
    }

    return num_cleared_lines;
}

bool tetris_matrix_get_cell_value(const tetris_ctx* ctx, const tetris_matrix* matrix, const int x, const int y) {
    TETRIS_CTX_CHECK(ctx, x >= 0 && x < TETRIS_MATRIX_WIDTH);
    TETRIS_CTX_CHECK(ctx, y >= 0 && y < TETRIS_MATRIX_HEIGHT);
    return (matrix->rows[y] >> (TETRIS_MATRIX_ROW_BIT_LENGTH - TETRIS_MATRIX_ROW_BIT_PADDING - 1 - x)) & 1;
}

tetris_tetronimo_shape tetris_matrix_get_cell_shape(const tetris_ctx* ctx, const tetris_matrix* matrix, const int x, const int y) {
    TETRIS_CTX_CHECK(ctx, x >= 0 && x < TETRIS_MATRIX_WIDTH);
    TETRIS_CTX_CHECK(ctx, y >= 0 && y < TETRIS_MATRIX_HEIGHT);
    return (matrix->shape_rows[y] >> ((TETRIS_MATRIX_WIDTH - 1 - x) * TETRIS_MATRIX_SHAPE_ROW_BITS_PER_SHAPE)) & TETRIS_MATRIX_SHAPE_ROW_CELL_MASK;
}