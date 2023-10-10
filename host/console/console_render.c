#include "console_render.h"
#include <stdio.h>
#include <tetris_sim.h>

#define CONSOLE_SIM_DRAW_X_PADDING (4)
#define CONSOLE_SIM_DRAW_Y_PADDING (3)
#define CONSOLE_MATRIX_BORDER_CHAR ('.')
#define CONSOLE_MATRIX_SET_CHAR ('x')
#define CONSOLE_MATRIX_CLEAR_CHAR ('o')
#define CONSOLE_TETRONIMO_SET_CHAR ('x')

typedef enum console_render_color_t {
    CONSOLE_RENDER_COLOR_BACKGROUND,
    CONSOLE_RENDER_COLOR_FOREGROUND,
    CONSOLE_RENDER_COLOR_TETRONIMO_START, 
    CONSOLE_RENDER_COLOR_TETRONIMO_T = CONSOLE_RENDER_COLOR_TETRONIMO_START,
    CONSOLE_RENDER_COLOR_TETRONIMO_J,
    CONSOLE_RENDER_COLOR_TETRONIMO_Z,
    CONSOLE_RENDER_COLOR_TETRONIMO_O,
    CONSOLE_RENDER_COLOR_TETRONIMO_S,
    CONSOLE_RENDER_COLOR_TETRONIMO_L,
    CONSOLE_RENDER_COLOR_TETRONIMO_I,
    CONSOLE_RENDER_COLOR_ROW_CLEARED,
} console_render_color;

console_render* console_render_init() {
    console_render* render = (console_render*)malloc(sizeof(console_render));

    render->cur_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    render->nxt_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    
    SetConsoleActiveScreenBuffer(render->cur_handle);

    CONSOLE_CURSOR_INFO cursor_info;
    cursor_info.dwSize = 100;
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(render->cur_handle, &cursor_info);
    SetConsoleCursorInfo(render->nxt_handle, &cursor_info);

    CONSOLE_SCREEN_BUFFER_INFOEX screen_buffer_info;
    screen_buffer_info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(render->cur_handle, &screen_buffer_info);

    const COLORREF BLACK = RGB(0, 0, 0);
    const COLORREF WHITE = RGB(200, 200, 200);
    const COLORREF SHAPE_COLOR_0 = RGB(215, 150, 215);
    const COLORREF SHAPE_COLOR_1 = RGB(215, 100, 215);
    const COLORREF SHAPE_COLOR_2 = RGB(215, 50, 215);
    const COLORREF ROW_CLEARED_COLOR = RGB(255, 185, 255);

    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_BACKGROUND] = BLACK;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_FOREGROUND] = WHITE;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_T] = SHAPE_COLOR_0;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_J] = SHAPE_COLOR_1;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_Z] = SHAPE_COLOR_2;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_O] = SHAPE_COLOR_0;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_S] = SHAPE_COLOR_1;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_L] = SHAPE_COLOR_2;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_TETRONIMO_I] = SHAPE_COLOR_0;
    screen_buffer_info.ColorTable[CONSOLE_RENDER_COLOR_ROW_CLEARED] = ROW_CLEARED_COLOR;
    SetConsoleScreenBufferInfoEx(render->cur_handle, &screen_buffer_info);
    SetConsoleScreenBufferInfoEx(render->nxt_handle, &screen_buffer_info);

    render->screen_width = screen_buffer_info.dwSize.X;
    render->screen_height = screen_buffer_info.dwSize.Y;

    render->screen = (char*)malloc(sizeof(char) * render->screen_width * render->screen_height);
    render->screen_attributes = (WORD*)malloc(sizeof(WORD) * render->screen_width * render->screen_height);
    for (int screen_index = 0; screen_index < render->screen_width * render->screen_height; ++screen_index) {
        render->screen[screen_index] = ' ';
        render->screen_attributes[screen_index] = CONSOLE_RENDER_COLOR_FOREGROUND;
    }

    return render;
}

void console_render_deinit(console_render* render) {
    free(render->screen);
    CloseHandle(render->cur_handle);
    CloseHandle(render->nxt_handle);
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
        for (int x = 0; x < matrix_width; ++x) {
            tetris_matrix_cell cell = tetris_sim_get_matrix_cell(sim, x, y);
            const int screen_index = (y + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + x + CONSOLE_SIM_DRAW_X_PADDING;
            if (cell.is_set) {
                render->screen[screen_index] = CONSOLE_MATRIX_SET_CHAR;
                render->screen_attributes[screen_index] = CONSOLE_RENDER_COLOR_TETRONIMO_START + cell.shape;
            } else {
                render->screen[screen_index] = ' ';
                render->screen_attributes[screen_index] = CONSOLE_RENDER_COLOR_FOREGROUND;
            }
        }
    }

    // draw over the cleared lines with a special char
    for (int row = 0; row < num_rows_cleared; ++row) {
        for (int x = 0; x < matrix_width; ++x) {
            const int screen_index = (rows_cleared[row] + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + x + CONSOLE_SIM_DRAW_X_PADDING;
            render->screen[screen_index] = CONSOLE_MATRIX_CLEAR_CHAR;
            render->screen_attributes[screen_index] = CONSOLE_RENDER_COLOR_ROW_CLEARED;
        }
    }
}

void console_render_draw_tetronimo(console_render* render, const tetris_sim* sim) {
    if (!tetris_sim_is_tetronimo_active(sim)) {
        return;
    }
    const tetris_tetronimo_shape shape = tetris_sim_get_tetronimo_shape(sim);
    const int tetronimo_max_width = tetris_sim_get_tetronimo_max_width(sim);
    const int tetronimo_max_height = tetris_sim_get_tetronimo_max_height(sim);
    const int pos_x = tetris_sim_get_tetronimo_pos_x(sim);
    const int pos_y = tetris_sim_get_tetronimo_pos_y(sim); 
    for (int tetronimo_y = 0; tetronimo_y < tetronimo_max_height; ++tetronimo_y) {
        if (pos_y + tetronimo_y < 0) {
            continue;
        }
        for (int tetronimo_x = 0; tetronimo_x < tetronimo_max_width; ++tetronimo_x) {
            if (tetris_sim_get_tetronimo_cell(sim, tetronimo_x, tetronimo_y)) {
                const int screen_index = (pos_y + tetronimo_y + CONSOLE_SIM_DRAW_Y_PADDING) * render->screen_width + (pos_x + tetronimo_x + CONSOLE_SIM_DRAW_X_PADDING);
                render->screen[screen_index] = CONSOLE_TETRONIMO_SET_CHAR;
                render->screen_attributes[screen_index] = CONSOLE_RENDER_COLOR_TETRONIMO_START + shape;
            }
        }
    }
}

void console_render_draw_ui(console_render* render, const tetris_sim* sim, const int* shape_stats) {
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
            render->screen[(draw_y + tetronimo_y - tetronimo_y_offset) * render->screen_width + (draw_x + tetronimo_x)] = tetris_sim_get_next_tetronimo_cell(sim, tetronimo_x, tetronimo_y) ? CONSOLE_TETRONIMO_SET_CHAR : ' ';
        }
    }

    // draw statistics
    const char* const shape_names[TETRIS_TETRONIMO_SHAPE_COUNT] = {
        "T", // TETRIS_TETRONIMO_SHAPE_T
        "J", // TETRIS_TETRONIMO_SHAPE_J
        "Z", // TETRIS_TETRONIMO_SHAPE_Z
        "O", // TETRIS_TETRONIMO_SHAPE_I
        "S", // TETRIS_TETRONIMO_SHAPE_S
        "L", // TETRIS_TETRONIMO_SHAPE_L
        "I"  // TETRIS_TETRONIMO_SHAPE_I
    };
    draw_y += tetronimo_max_height + 1;
    sprintf(render->screen + (draw_y * render->screen_width + draw_x), "STATISTICS");
    draw_y += 1;

    memset(render->screen + (draw_y * render->screen_width + draw_x), ' ', render->screen_width - draw_x);
    memset(render->screen + ((draw_y + 1) * render->screen_width + draw_x), ' ', render->screen_width - draw_x);
    for (int shape = 0; shape < TETRIS_TETRONIMO_SHAPE_COUNT; ++shape) {
        sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%s ", shape_names[shape]);
        draw_y += 1;
        draw_x += sprintf(render->screen + (draw_y * render->screen_width + draw_x), "%d ", shape_stats[shape]);
        draw_x += 1;
        draw_y -= 1;
    }
}

void console_render_present(console_render* render) {
    // render to back buffer
    DWORD chars_written;
    WriteConsoleOutputCharacter(render->nxt_handle, render->screen, render->screen_width * render->screen_height, (COORD){ 0, 0 }, &chars_written);
    DWORD attrs_written;
    WriteConsoleOutputAttribute(render->nxt_handle, render->screen_attributes, render->screen_width * render->screen_height, (COORD){ .X = 0, .Y = 0 }, &attrs_written);

    // swap back buffer
    SetConsoleActiveScreenBuffer(render->nxt_handle);
    HANDLE tmp = render->cur_handle;
    render->cur_handle = render->nxt_handle;
    render->nxt_handle = tmp;
}