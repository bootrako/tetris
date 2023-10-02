#include <stdio.h>
#include <windows.h>
#include <tetris_sim.h>

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

typedef struct console_host_context_t {
    LARGE_INTEGER counter_start;
    LARGE_INTEGER counter_frequency;
    bool input_pressed[TETRIS_INPUT_COUNT];
} console_host_context;

static void* console_host_alloc(void* context, size_t size) {
    return malloc(size);
}

static void console_host_free(void* context, void* ptr) {
    free(ptr);
}

static bool console_host_input_pressed(void* context, tetris_input input) {
    console_host_context* host_context = (console_host_context*)context;
    return host_context->input_pressed[input];
}

static console_host_context* console_host_context_init() {
    console_host_context* host_context = (console_host_context*)malloc(sizeof(console_host_context));
    for (int i = 0; i < TETRIS_INPUT_COUNT; ++i) {
        host_context->input_pressed[i] = false;
    }
    return host_context;
}

static void console_host_context_deinit(console_host_context* host_context) {
    free(host_context);
}

static bool console_input_vk_pressed(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static void console_host_context_update(console_host_context* host_context) {
    host_context->input_pressed[TETRIS_INPUT_MOVE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_A) | console_input_vk_pressed(CONSOLE_INPUT_VK_LEFT);
    host_context->input_pressed[TETRIS_INPUT_MOVE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_D) | console_input_vk_pressed(CONSOLE_INPUT_VK_RIGHT);
    host_context->input_pressed[TETRIS_INPUT_FAST_DROP] = console_input_vk_pressed(CONSOLE_INPUT_VK_S) | console_input_vk_pressed(CONSOLE_INPUT_VK_DOWN);
    host_context->input_pressed[TETRIS_INPUT_ROTATE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_O) | console_input_vk_pressed(CONSOLE_INPUT_VK_Z);
    host_context->input_pressed[TETRIS_INPUT_ROTATE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_P) | console_input_vk_pressed(CONSOLE_INPUT_VK_X);
}

static uint64_t console_get_random_seed() {
    FILETIME system_time;
    GetSystemTimePreciseAsFileTime(&system_time);
    return (ULARGE_INTEGER){ .LowPart = system_time.dwLowDateTime, .HighPart = system_time.dwHighDateTime }.QuadPart;
}

#define CONSOLE_SIM_DRAW_X_PADDING (4)
#define CONSOLE_SIM_DRAW_Y_PADDING (3)
#define CONSOLE_MATRIX_BORDER_CHAR ('.')
#define CONSOLE_MATRIX_SET_CHAR ('x')
#define CONSOLE_TETRONIMO_SET_CHAR ('x')

typedef struct console_render_t {
    HANDLE console_handle;
    int screen_width;
    int screen_height;
    char* screen;
} console_render;

static console_render* console_render_init() {
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

static void console_render_deinit(console_render* render) {
    free(render->screen);
    CloseHandle(render->console_handle);
    free(render);
}

static void console_render_draw_matrix_border(console_render* render, const tetris_sim* sim) {
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

static void console_render_draw_matrix(console_render* render, const tetris_sim* sim) {
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
}

static void console_render_draw_tetronimo(console_render* render, const tetris_sim* sim) {
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

static void console_render_draw_ui(console_render* render, const tetris_sim* sim) {
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

static void console_render_present(console_render* render) {
    DWORD bytesWritten;
    WriteConsoleOutputCharacter(render->console_handle, render->screen, render->screen_width * render->screen_height, (COORD){ 0, 0 }, &bytesWritten);
}

int main(int argc, const char** argv) {
    console_host_context* host_context = console_host_context_init();
    const tetris_sim_host sim_host = {
        .alloc = console_host_alloc,
        .free = console_host_free,
        .input_pressed = console_host_input_pressed,
        .context = host_context,
    };
    tetris_sim* sim = tetris_sim_init(sim_host, console_get_random_seed());
    console_render* render = console_render_init();

    console_render_draw_matrix_border(render, sim);

    LARGE_INTEGER counter_frequency;
    QueryPerformanceFrequency(&counter_frequency);

    const float time_per_frame = tetris_sim_time_per_frame(sim);
    float frame_time_accumulator = 0.0f;
    while (!tetris_sim_is_game_over(sim)) {
        LARGE_INTEGER counter_start;
        QueryPerformanceCounter(&counter_start);

        console_host_context_update(host_context);

        while (frame_time_accumulator > time_per_frame) {
            tetris_sim_update(sim);
            frame_time_accumulator -= time_per_frame;
        }

        console_render_draw_matrix(render, sim);
        console_render_draw_tetronimo(render, sim);
        console_render_draw_ui(render, sim);
        console_render_present(render);

        Sleep((DWORD)(time_per_frame * 1000.0f));

        LARGE_INTEGER counter_end;
        QueryPerformanceCounter(&counter_end);

        frame_time_accumulator += (float)(counter_end.QuadPart - counter_start.QuadPart) / (float)(counter_frequency.QuadPart);
    }

    console_render_deinit(render);
    tetris_sim_deinit(sim);
    console_host_context_deinit(host_context);

    return 0;
}