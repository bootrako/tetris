#ifndef TETRIS_HOST_H
#define TETRIS_HOST_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void*(*alloc)(size_t size); // allocate memory with the given size
    void(*free)(void* ptr);     // free allocated memory
    float(*time)();             // get a high resolution, monotonically-increasing timestamp
    uint64_t(*seed)();          // get a value that can be used as a seed in a random number generator
} tetris_host;

#endif // TETRIS_HOST_H