#ifndef TETRIS_TETRONIMO_H
#define TETRIS_TETRONIMO_H

#include "tetris_utils.h"
#include <stdbool.h>
#include <tetris_sim.h>

typedef struct tetris_ctx_t tetris_ctx;
typedef struct tetris_matrix_t tetris_matrix;

// config
#define TETRIS_TETRONIMO_MAX_WIDTH (4)
#define TETRIS_TETRONIMO_MAX_HEIGHT (4)
typedef uint8_t tetris_tetronimo_row;               // needs to have enough bits to hold tetronimo max width

typedef struct tetris_tetronimo_rotation_t {
    tetris_tetronimo_row rows[TETRIS_TETRONIMO_MAX_HEIGHT];
} tetris_tetronimo_rotation;

typedef struct tetris_tetronimo_t {
    const tetris_tetronimo_rotation* rotations;     // points to different configurations of the tetronimo representing its rotations
    int num_rotations;                              // how many rotations this tetronimo has
    int current_rotation;                           // the index of the current rotation
    int x;                                          // the x position (matrix space) of the tetronimo
    int y;                                          // the y position (matrix space) of the tetronimo
    tetris_tetronimo_shape shape;                   // the shape of this tetronimo
    bool is_active;                                 // true when the tetronimo is active in the matrix
} tetris_tetronimo;

typedef struct {
    tetris_rand rand;                               // random number generator
    const tetris_tetronimo* next;                   // the next tetronimo that will be spawned
} tetris_tetronimo_spawner;

// attempts to create a new tetronimo using the spawner and returns whether the creation was successful
bool tetris_tetronimo_init(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, tetris_tetronimo_spawner* spawner);

// attempts to move the tetronimo. if a collision happens, the move might not do anything. returns true if there were collisions
bool tetris_tetronimo_move(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_x, const int dir_y);

// rotates the tetronimo, then tries to find an available space if there is a collision
void tetris_tetronimo_rotate(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_rot);

// returns the bits of a given tetronimo row
tetris_tetronimo_row tetris_tetronimo_get_row(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int row);

// given an x and y in tetronimo coordinates, returns true if the bit is set in the tetronimo, based on its current rotation
bool tetris_tetronimo_get_cell(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int x, const int y);

// initializes a tetronimo spawner with a seed for the RNG.
void tetris_tetronimo_spawner_init(tetris_ctx* ctx, tetris_tetronimo_spawner* spawner, const uint64_t seed);

// returns the next tetronimo. also generates the following tetronimo.
tetris_tetronimo tetris_tetronimo_spawner_next(tetris_ctx* ctx, tetris_tetronimo_spawner* spawner);

#endif // TETRIS_TETRONIMO_H