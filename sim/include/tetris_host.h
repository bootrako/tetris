#ifndef TETRIS_HOST_H
#define TETRIS_HOST_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void*(*alloc)(size_t size);
    void(*free)(void* ptr);
    uint64_t(*time)();
} tetris_host;

#endif // TETRIS_HOST_H