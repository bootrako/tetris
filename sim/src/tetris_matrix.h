#ifndef TETRIS_MATRIX_H
#define TETRIS_MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include <tetris_sim.h>

typedef struct tetris_ctx_t tetris_ctx;
typedef struct tetris_tetronimo_t tetris_tetronimo;

// config
#define TETRIS_MATRIX_WIDTH 10
#define TETRIS_MATRIX_HEIGHT 20
typedef uint16_t tetris_matrix_row;                 // needs to have enough bits to hold matrix width

typedef struct tetris_matrix_cell_info_t {
    tetris_tetronimo_shape shape;                   // the tetronimo shape the cell came from
} tetris_matrix_cell_info;

typedef struct tetris_matrix_t {
    tetris_matrix_row rows[TETRIS_MATRIX_HEIGHT];                                   // for each row of the matrix, the central TETRIS_MATRIX_WIDTH bits represent the row's cells
    tetris_matrix_cell_info rows_info[TETRIS_MATRIX_HEIGHT][TETRIS_MATRIX_WIDTH];   // additional info about each cell
} tetris_matrix;

// initializes the matrix bits
void tetris_matrix_init(tetris_ctx* ctx, tetris_matrix* matrix);

// merges a tetronimo into a matrix, so that the tetronimo's rows become part of the matrix's
void tetris_matrix_merge(tetris_ctx* ctx, tetris_matrix* matrix, const tetris_tetronimo* tetronimo);

// returns false if the tetronimo is overlapping a set bit on the matrix, or if the tetronimo is out of bounds.
bool tetris_matrix_is_tetronimo_valid(const tetris_ctx* ctx, const tetris_matrix* matrix, const tetris_tetronimo* tetronimo);

// removes cleared lines from the matrix. returns the number of cleared lines
int tetris_matrix_remove_cleared_lines(tetris_ctx* ctx, tetris_matrix* matrix);

// given an x and y in matrix coordinates, returns true if the bit is set in the matrix
bool tetris_matrix_get_cell_value(const tetris_ctx* ctx, const tetris_matrix* matrix, const int x, const int y);

tetris_tetronimo_shape tetris_matrix_get_cell_shape(const tetris_ctx* ctx, const tetris_matrix* matrix, const int x, const int y);

#endif // TETRIS_MATRIX_H