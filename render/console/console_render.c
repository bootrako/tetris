#include "console_render.h"
#include <stdio.h>
#include <tetris_sim.h>

#define CONSOLE_SIM_DRAW_X_PADDING (4)
#define CONSOLE_SIM_DRAW_Y_PADDING (3)
#define CONSOLE_MATRIX_BORDER_CHAR ('.')
#define CONSOLE_MATRIX_SET_CHAR ('x')
#define CONSOLE_MATRIX_CLEAR_CHAR ('o')
#define CONSOLE_TETRONIMO_SET_CHAR ('x')

console_render* console_render_init() {
    console_render* render = (console_render*)malloc(sizeof(console_render));

    render->console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(render->console_handle);

    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    GetConsoleScreenBufferInfo(render->console_handle, &screen_buffer_info);

    render->screen_width = screen_buffer_info.dwSize.X;
    render->screen_height = screen_buffer_info.dwSize.Y;

    render->screen = (char*)malloc(render->screen_width * render->screen_height);
    memset(render->screen, 0, render->screen_width * render->screen_height);

    return render;
}

void console_render_deinit(console_render* render) {
    free(render->screen);
    CloseHandle(render->console_handle);
    free(render);
}

void console_render_draw_matrix_border(console_render* render, const tetris_sim* sim) {
    const int matrix_width = tetris_sim_get_matrix_width(sim);
    const int matrix_height = tetris_sim_get_matrix_height(sim);

    // top and bottom border
    for (int x = 0; x < matrix_width + CONSOLE_SIM_DRAW_X_PADDING * 2; ++x) {
        for (int y = 0; y < CONSOLE_SIM_DRAW_Y_PADDING; ++y) {
            render->screen[y * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
            render->screen[(matrix_height + CONSOLE_SIM_DRAW_Y_PADDING + y) * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
        }
    }
    // left and right border
    for (int y = 0; y < matrix_height + CONSOLE_SIM_DRAW_Y_PADDING * 2; ++y) {
        for (int x = 0; x < CONSOLE_SIM_DRAW_X_PADDING; ++x) {
            render->screen[y * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
            render->screen[y * render->screen_width + matrix_width + CONSOLE_SIM_DRAW_X_PADDING + x] = CONSOLE_MATRIX_BORDER_CHAR;
        }
    }
}

void console_render_draw_matrix(console_render* render, const tetris_sim* sim, const int* rows_cleared, const int num_rows_cleared) {
    const int matrix_width = tetris_sim_get_matrix_width(sim);
    const int matrix_height = tetris_sim_get_matrix_height(sim);

    for (int y = 0; y < matrix_height; ++y) {
        // clear the existing line
        memset(render->screen + ((y + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + CONSOLE_SIM_DRAW_X_PADDING), 0, matrix_width);

        // draw the updated line
        for (int x = 0; x < matrix_width; ++x) {
            if (tetris_sim_get_matrix_value(sim, x, y)) {
                render->screen[(y + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + x + CONSOLE_SIM_DRAW_X_PADDING] = CONSOLE_MATRIX_SET_CHAR;
            }
        }
    }

    // draw over the cleared lines with a special char
    for (int row = 0; row < num_rows_cleared; ++row) {
        memset(render->screen + ((rows_cleared[row] + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + CONSOLE_SIM_DRAW_X_PADDING), CONSOLE_MATRIX_CLEAR_CHAR, matrix_width);
    }
}

void console_render_draw_tetronimo(console_render* render, const tetris_sim* sim) {
    if (!tetris_sim_is_tetronimo_active(sim)) {
        return;
    }
    const int tetronimo_max_width = tetris_sim_get_tetronimo_max_width(sim);
    const int tetronimo_max_height = tetris_sim_get_tetronimo_max_height(sim);
    const int pos_x = tetris_sim_get_tetronimo_pos_x(sim);
    const int pos_y = tetris_sim_get_tetronimo_pos_y(sim); 
    for (int tetronimo_y = 0; tetronimo_y < tetronimo_max_height; ++tetronimo_y) {
        if (pos_y + tetronimo_y < 0) {
            continue;
        }
        for (int tetronimo_x = 0; tetronimo_x < tetronimo_max_width; ++tetronimo_x) {
            if (tetris_sim_get_tetronimo_value(sim, tetronimo_x, tetronimo_y)) {
                render->screen[(pos_y + tetronimo_y + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + (pos_x + tetronimo_x + CONSOLE_SIM_DRAW_X_PADDING)] = CONSOLE_TETRONIMO_SET_CHAR;
            }
        }
    }
}

void console_render_draw_ui(console_render* render, const tetris_sim* sim) {
    const int matrix_width = tetris_sim_get_matrix_width(sim);

    // draw text scores
    int draw_x = (matrix_width + CONSOLE_SIM_DRAW_X_PADDING * 2) + 3;
    int draw_y = 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "SCORE");
    draw_y += 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%d", tetris_sim_get_score(sim));

    draw_y += 2;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "LINES");
    draw_y += 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%d", tetris_sim_get_lines(sim));

    draw_y += 2;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "LEVEL");
    draw_y += 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%d", tetris_sim_get_level(sim));

    // draw next tetronimo
    const int tetronimo_max_width = tetris_sim_get_tetronimo_max_width(sim);
    const int tetronimo_max_height = tetris_sim_get_tetronimo_max_height(sim);

    draw_y += 2;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "NEXT");
    draw_y += 1;
    const int tetronimo_y_offset = 1;
    for (int tetronimo_y = tetronimo_y_offset; tetronimo_y < tetronimo_max_height; ++tetronimo_y) {
        for (int tetronimo_x = 0; tetronimo_x < tetronimo_max_width; ++tetronimo_x) {
            render->screen[(draw_y + tetronimo_y - tetronimo_y_offset) * render->screen_width + (draw_x + tetronimo_x)] = tetris_sim_get_next_tetronimo_value(sim, tetronimo_x, tetronimo_y) ? CONSOLE_TETRONIMO_SET_CHAR : ' ';
        }
    }

    // draw statistics
    const int statistic_count = tetris_sim_get_statistic_count(sim);

    draw_y += tetronimo_max_height + 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "STATISTICS");
    draw_y += 1;

    memset(render->screen + (draw_y * render->screen_width + draw_x), 0, render->screen_width - draw_x);
    memset(render->screen + ((draw_y + 1) * render->screen_width + draw_x), 0, render->screen_width - draw_x);
    for (int stat = 0; stat < statistic_count; ++stat) {
        sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%s", tetris_sim_get_statistic_name(sim, stat));
        draw_y += 1;
        draw_x += sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%d", tetris_sim_get_statistic_value(sim, stat));
        draw_x += 1;
        draw_y -= 1;
    }
}

void console_render_present(console_render* render) {
    DWORD bytesWritten;
    WriteConsoleOutputCharacter(render->console_handle, render->screen, render->screen_width * render->screen_height, (COORD){ 0, 0 }, &bytesWritten);
}