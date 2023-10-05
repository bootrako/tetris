#ifndef CONSOLE_HOST_H
#define CONSOLE_HOST_H

#include <windows.h>
#include <tetris_sim.h>

typedef struct console_host_context_t {
    LARGE_INTEGER counter_start;
    LARGE_INTEGER counter_frequency;
    bool input_pressed[TETRIS_INPUT_COUNT];
} console_host_context;

void* console_host_alloc(void* context, size_t size);
void console_host_free(void* context, void* ptr);
void console_host_panic(void* context, const char* err_msg);
bool console_host_input_pressed(void* context, tetris_input input);

console_host_context* console_host_context_init();
void console_host_context_deinit(console_host_context* host_context);
void console_host_context_update(console_host_context* host_context);

#endif // CONSOLE_HOST_H