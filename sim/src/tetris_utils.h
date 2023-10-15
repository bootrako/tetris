#ifndef TETRIS_UTILS_H
#define TETRIS_UTILS_H

#include <stdint.h>

#define TETRIS_XSTR(str) #str
#define TETRIS_STR(str) TETRIS_XSTR(str)
#define TETRIS_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define TETRIS_BITSIZE(type) (sizeof(type) << 3)

typedef struct tetris_ctx_t tetris_ctx;

typedef struct {
    uint32_t state[2];
} tetris_rand;

// initialize a random number generator with a given seed
void tetris_rand_init(tetris_ctx* ctx, tetris_rand* rand, const uint64_t seed);

// returns the next number in the RNG sequence
uint32_t tetris_rand_next(tetris_ctx* ctx, tetris_rand* rand);

// returns a random number between min and max
int tetris_rand_range(tetris_ctx* ctx, tetris_rand* rand, const int min, const int max);

// returns the count of leading zeroes before a the first set bit, starting at most significant
int tetris_clz(unsigned int x);

// returns the count of trailing zeroes after the last set bit, to the least significant
int tetris_ctz(unsigned int x);

#endif // TETRIS_UTILS_H