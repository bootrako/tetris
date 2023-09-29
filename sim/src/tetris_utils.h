#ifndef TETRIS_UTILS_H
#define TETRIS_UTILS_H

#define TETRIS_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#include <stdint.h>

typedef struct {
    uint32_t state[2];
} tetris_rand;

// initialize a random number generator with a given seed
void tetris_rand_init(tetris_rand* rand, const uint64_t seed);

// returns the next number in the RNG sequence
uint32_t tetris_rand_next(tetris_rand* rand);

// returns a random number between min and max
uint32_t tetris_rand_range(tetris_rand* rand, const uint32_t min, const uint32_t max);

#endif // TETRIS_UTILS_H