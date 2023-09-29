#ifndef TETRIS_MATRIX_H
#define TETRIS_MATRIX_H

#include <stdint.h>
#include <stdbool.h>

typedef struct tetris_tetronimo_t tetris_tetronimo;

// config
#define TETRIS_MATRIX_WIDTH 10
#define TETRIS_MATRIX_HEIGHT 24
typedef uint16_t tetris_matrix_row;     // needs to have enough bits to hold matrix width

typedef struct tetris_matrix_t {
    tetris_matrix_row rows[TETRIS_MATRIX_HEIGHT];
} tetris_matrix;

// initializes the matrix bits
void tetris_matrix_init(tetris_matrix* matrix);

// merges a tetronimo into a matrix, so that the tetronimo's rows become part of the matrix's
void tetris_matrix_merge(tetris_matrix* matrix, const tetris_tetronimo* tetronimo);

// returns true if the tetronimo is overlapping a set bit on the matrix, or if the tetronimo is out of bounds.
bool tetris_matrix_collide(const tetris_matrix* matrix, const tetris_tetronimo* tetronimo);

// given an x and y in matrix coordinates, returns true if the bit is set in the matrix
bool tetris_matrix_get_value(const tetris_matrix* matrix, const int x, const int y);

#endif // TETRIS_MATRIX_H