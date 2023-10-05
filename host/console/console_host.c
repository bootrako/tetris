#include "console_host.h"
#include <stdio.h>

#define CONSOLE_INPUT_VK_A (0x41)
#define CONSOLE_INPUT_VK_D (0x44)
#define CONSOLE_INPUT_VK_O (0x4F)
#define CONSOLE_INPUT_VK_P (0x50)
#define CONSOLE_INPUT_VK_S (0x53)
#define CONSOLE_INPUT_VK_X (0x58)
#define CONSOLE_INPUT_VK_Z (0x5A)
#define CONSOLE_INPUT_VK_LEFT (VK_LEFT)
#define CONSOLE_INPUT_VK_RIGHT (VK_RIGHT)
#define CONSOLE_INPUT_VK_DOWN (VK_DOWN)

void* console_host_alloc(void* context, size_t size) {
    return malloc(size);
}

void console_host_free(void* context, void* ptr) {
    free(ptr);
}

void console_host_panic(void* context, const char* err_msg) {
    printf("%s\n", err_msg);
    abort();
}

bool console_host_input_pressed(void* context, tetris_input input) {
    console_host_context* host_context = (console_host_context*)context;
    return host_context->input_pressed[input];
}

console_host_context* console_host_context_init() {
    console_host_context* host_context = (console_host_context*)malloc(sizeof(console_host_context));
    for (int i = 0; i < TETRIS_INPUT_COUNT; ++i) {
        host_context->input_pressed[i] = false;
    }
    return host_context;
}

void console_host_context_deinit(console_host_context* host_context) {
    free(host_context);
}

static bool console_input_vk_pressed(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

void console_host_context_update(console_host_context* host_context) {
    host_context->input_pressed[TETRIS_INPUT_MOVE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_A) | console_input_vk_pressed(CONSOLE_INPUT_VK_LEFT);
    host_context->input_pressed[TETRIS_INPUT_MOVE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_D) | console_input_vk_pressed(CONSOLE_INPUT_VK_RIGHT);
    host_context->input_pressed[TETRIS_INPUT_FAST_DROP] = console_input_vk_pressed(CONSOLE_INPUT_VK_S) | console_input_vk_pressed(CONSOLE_INPUT_VK_DOWN);
    host_context->input_pressed[TETRIS_INPUT_ROTATE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_O) | console_input_vk_pressed(CONSOLE_INPUT_VK_Z);
    host_context->input_pressed[TETRIS_INPUT_ROTATE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_P) | console_input_vk_pressed(CONSOLE_INPUT_VK_X);
}