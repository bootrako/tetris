#include "tetris_utils.h"
#include "tetris_ctx.h"
#ifdef _MSC_VER
#include <intrin.h>
#endif

static uint32_t rotl(const uint32_t x, const int k) {
    return (x << k) | (x >> (32 - k));
}

void tetris_rand_init(tetris_ctx* ctx, tetris_rand* rand, const uint64_t seed) {
    TETRIS_CTX_CHECK(ctx, seed != 0);
    rand->state[0] = (uint32_t)seed;
    rand->state[1] = (uint32_t)(seed >> 32);
}

uint32_t tetris_rand_next(tetris_ctx* ctx, tetris_rand* rand) {
    const uint32_t s0 = rand->state[0];
    const uint32_t s1 = rand->state[1] ^ s0;

    rand->state[0] = rotl(s0, 26) ^ s1 ^ (s1 << 9);
    rand->state[1] = rotl(s1, 13);

    return rotl(s0 * 0x9E3779BB, 5) * 5;
}

int tetris_rand_range(tetris_ctx* ctx, tetris_rand* rand, const int min, const int max) {
    TETRIS_CTX_CHECK(ctx, max >= min);
    const int ret = (int)(tetris_rand_next(ctx, rand) % (max - min)) + min;
    TETRIS_CTX_CHECK(ctx, ret >= min && ret < max);
    return ret;
}

int tetris_clz(unsigned int x) {
#if defined(__GNUC__)
    if (x == 0) {
        return 32;
    } else {
        return __builtin_clz(x);
    }
#elif defined(_MSC_VER)
    unsigned long index = 0;
    if (_BitScanReverse(&index, x)) {
        return 31 - index;
    } else {
        return 32;
    }
#else
#error unsupported compiler
#endif
}

int tetris_ctz(unsigned int x) {
#if defined(__GNUC__)
    if (x == 0) {
        return 32;
    } else {
        return __builtin_ctz(x);
    }
#elif defined(_MSC_VER)
    unsigned long index = 0;
    if (_BitScanForward(&index, x)) {
        return index;
    } else {
        return 32;
    }
#else
#error unsupported compiler
#endif
}