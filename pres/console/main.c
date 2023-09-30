#include <windows.h>
#include <tetris_sim.h>

#define CONSOLE_INPUT_VK_A (0x41)
#define CONSOLE_INPUT_VK_D (0x44)
#define CONSOLE_INPUT_VK_E (0x45)
#define CONSOLE_INPUT_VK_Q (0x51)
#define CONSOLE_INPUT_VK_S (0x53)
#define CONSOLE_INPUT_VK_W (0x57)
#define CONSOLE_INPUT_VK_X (0x58)
#define CONSOLE_INPUT_VK_Z (0x5A)
#define CONSOLE_INPUT_VK_LEFT (VK_LEFT)
#define CONSOLE_INPUT_VK_RIGHT (VK_RIGHT)
#define CONSOLE_INPUT_VK_UP (VK_UP)
#define CONSOLE_INPUT_VK_DOWN (VK_DOWN)

typedef struct console_host_context_t {
    LARGE_INTEGER counter_start;
    LARGE_INTEGER counter_frequency;
    bool input_pressed[TETRIS_INPUT_ACTION_COUNT];
} console_host_context;

static void* console_host_alloc(void* context, size_t size) {
    return malloc(size);
}

static void console_host_free(void* context, void* ptr) {
    free(ptr);
}

static float console_host_time(void* context) {
    console_host_context* host_context = (console_host_context*)context;
    LARGE_INTEGER counter_end;
    QueryPerformanceCounter(&counter_end);
    return (float)(counter_end.QuadPart - host_context->counter_start.QuadPart) / host_context->counter_frequency.QuadPart;
}

static uint64_t console_host_seed(void* context) {
    FILETIME system_time;
    GetSystemTimePreciseAsFileTime(&system_time);
    return (ULARGE_INTEGER){ .LowPart = system_time.dwLowDateTime, .HighPart = system_time.dwHighDateTime }.QuadPart;
}

static bool console_host_input_pressed(void* context, tetris_input_action input_action) {
    console_host_context* host_context = (console_host_context*)context;
    return host_context->input_pressed[input_action];
}

static console_host_context* console_host_context_init() {
    console_host_context* host_context = (console_host_context*)malloc(sizeof(console_host_context));
    QueryPerformanceCounter(&host_context->counter_start);
    QueryPerformanceFrequency(&host_context->counter_frequency);
    for (int i = 0; i < TETRIS_INPUT_ACTION_COUNT; ++i) {
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
    host_context->input_pressed[TETRIS_INPUT_ACTION_MOVE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_A) | console_input_vk_pressed(CONSOLE_INPUT_VK_LEFT);
    host_context->input_pressed[TETRIS_INPUT_ACTION_MOVE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_D) | console_input_vk_pressed(CONSOLE_INPUT_VK_RIGHT);
    host_context->input_pressed[TETRIS_INPUT_ACTION_MOVE_DOWN] = console_input_vk_pressed(CONSOLE_INPUT_VK_S) | console_input_vk_pressed(CONSOLE_INPUT_VK_DOWN);
    host_context->input_pressed[TETRIS_INPUT_ACTION_MOVE_DROP] = console_input_vk_pressed(CONSOLE_INPUT_VK_W) | console_input_vk_pressed(CONSOLE_INPUT_VK_UP);
    host_context->input_pressed[TETRIS_INPUT_ACTION_ROTATE_LEFT] = console_input_vk_pressed(CONSOLE_INPUT_VK_Q) | console_input_vk_pressed(CONSOLE_INPUT_VK_Z);
    host_context->input_pressed[TETRIS_INPUT_ACTION_ROTATE_RIGHT] = console_input_vk_pressed(CONSOLE_INPUT_VK_E) | console_input_vk_pressed(CONSOLE_INPUT_VK_X);
}

#define CONSOLE_SIM_DRAW_X_OFFSET (2)
#define CONSOLE_SIM_DRAW_Y_OFFSET (2)
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

static void console_render_draw_matrix_border(console_render* render, const int matrix_width, const int matrix_height) {
    // top and bottom border
    for (int x = 0; x < matrix_width + CONSOLE_SIM_DRAW_X_OFFSET * 2; ++x) {
        for (int y = 0; y < CONSOLE_SIM_DRAW_Y_OFFSET; ++y) {
            render->screen[y * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
            render->screen[(matrix_height + CONSOLE_SIM_DRAW_Y_OFFSET + y) * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
        }
    }
    // left and right border
    for (int y = 0; y < matrix_height + CONSOLE_SIM_DRAW_Y_OFFSET * 2; ++y) {
        for (int x = 0; x < CONSOLE_SIM_DRAW_X_OFFSET; ++x) {
            render->screen[y * render->screen_width + x] = CONSOLE_MATRIX_BORDER_CHAR;
            render->screen[y * render->screen_width + matrix_width + CONSOLE_SIM_DRAW_X_OFFSET + x] = CONSOLE_MATRIX_BORDER_CHAR;
        }
    }
}

static void console_render_draw_matrix(console_render* render, const tetris_sim* sim, const int matrix_width, const int matrix_height) {
    for (int y = 0; y < matrix_height; ++y) {
        // clear the existing line
        memset(render->screen + ((y + CONSOLE_SIM_DRAW_Y_OFFSET) * render->screen_width + CONSOLE_SIM_DRAW_X_OFFSET), 0, matrix_width);

        // draw the updated line
        for (int x = 0; x < matrix_width; ++x) {
            if (tetris_sim_get_matrix_value(sim, x, y)) {
                render->screen[(y + CONSOLE_SIM_DRAW_Y_OFFSET) * render->screen_width + x + CONSOLE_SIM_DRAW_X_OFFSET] = CONSOLE_MATRIX_SET_CHAR;
            }
        }
    }
}

static void console_render_draw_tetronimo(console_render* render, const tetris_sim* sim, const int tetronimo_max_width, const int tetronimo_max_height) {
    const int tetronimo_x = tetris_sim_get_tetronimo_pos_x(sim);
    const int tetronimo_y = tetris_sim_get_tetronimo_pos_y(sim); 
    for (int y = 0; y < tetronimo_max_height; ++y) {
        for (int x = 0; x < tetronimo_max_width; ++x) {
            if (tetris_sim_get_tetronimo_value(sim, x, y)) {
                render->screen[(tetronimo_y + y + CONSOLE_SIM_DRAW_Y_OFFSET) * render->screen_width + (tetronimo_x + x + CONSOLE_SIM_DRAW_X_OFFSET)] = CONSOLE_TETRONIMO_SET_CHAR;
            }
        }
    }
}

static void console_render_present(console_render* render) {
    DWORD bytesWritten;
    WriteConsoleOutputCharacter(render->console_handle, render->screen, render->screen_width * render->screen_height, (COORD){ 0, 0 }, &bytesWritten);
}

static void console_render_deinit(console_render* render) {
    free(render->screen);
    CloseHandle(render->console_handle);
    free(render);
}

int main(int argc, const char** argv) {
    console_host_context* host_context = console_host_context_init();
    const tetris_sim_host sim_host = {
        .alloc = console_host_alloc,
        .free = console_host_free,
        .time = console_host_time,
        .seed = console_host_seed,
        .input_pressed = console_host_input_pressed,
        .context = host_context,
    };
    tetris_sim* sim = tetris_sim_init(sim_host);
    console_render* render = console_render_init();

    const int matrix_width = tetris_sim_get_matrix_width(sim);
    const int matrix_height = tetris_sim_get_matrix_height(sim);
    const int tetronimo_max_width = tetris_sim_get_tetronimo_max_width(sim);
    const int tetronimo_max_height = tetris_sim_get_tetronimo_max_height(sim);

    console_render_draw_matrix_border(render, matrix_width, matrix_height);

    while (!tetris_sim_is_game_over(sim)) {
        Sleep(16);

        console_host_context_update(host_context);
        tetris_sim_update(sim);

        console_render_draw_matrix(render, sim, matrix_width, matrix_height);
        console_render_draw_tetronimo(render, sim, tetronimo_max_width, tetronimo_max_height);
        console_render_present(render);
    }

    console_render_deinit(render);
    tetris_sim_deinit(sim);
    console_host_context_deinit(host_context);

    return 0;
}