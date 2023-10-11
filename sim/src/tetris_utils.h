#ifndef TETRIS_UTILS_H
#define TETRIS_UTILS_H

#include <stdint.h>

#define TETRIS_XSTR(str) #str
#define TETRIS_STR(str) TETRIS_XSTR(str)
#define TETRIS_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

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

#define TETRIS_BITSIZE(type) (sizeof(type) << 3)

#ifdef __GNUC__
    #define TETRIS_CLZ(x) __builtin_clz(x)
    #define TETRIS_CTZ(x) __builtin_ctz(x)
#else
#error unsupported compiler
#endif

#endif // TETRIS_UTILS_H