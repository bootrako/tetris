#ifndef CONSOLE_RENDER_H
#define CONSOLE_RENDER_H

#include <windows.h>
#include <tetris_sim.h>

typedef struct console_render_t {
    HANDLE console_handle;
    int screen_width;
    int screen_height;
    char* screen;
} console_render;

console_render* console_render_init();
void console_render_deinit(console_render* render);
void console_render_draw_matrix_border(console_render* render, const tetris_sim* sim);
void console_render_draw_matrix(console_render* render, const tetris_sim* sim, const int* rows_cleared, const int num_rows_cleared);
void console_render_draw_tetronimo(console_render* render, const tetris_sim* sim);
void console_render_draw_ui(console_render* render, const tetris_sim* sim);
void console_render_present(console_render* render);

#endif // CONSOLE_RENDER_H