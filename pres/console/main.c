#include <windows.h>
#include <tetris_sim.h>

LARGE_INTEGER g_start_counter;
LARGE_INTEGER g_counter_frequency;

void* host_alloc(size_t size) {
    return malloc(size);
}

void host_free(void* ptr) {
    free(ptr);
}

float host_time() {
    LARGE_INTEGER end_counter;
    QueryPerformanceCounter(&end_counter);
    return (float)(end_counter.QuadPart - g_start_counter.QuadPart) / g_counter_frequency.QuadPart;
}

uint64_t host_seed() {
    FILETIME system_time;
    GetSystemTimePreciseAsFileTime(&system_time);
    return (ULARGE_INTEGER){ .LowPart = system_time.dwLowDateTime, .HighPart = system_time.dwHighDateTime }.QuadPart;
}

const tetris_host k_host = {
    .alloc = host_alloc,
    .free = host_free,
    .time = host_time,
    .seed = host_seed,
};

int main(int argc, const char** argv) {
    QueryPerformanceCounter(&g_start_counter);
    QueryPerformanceFrequency(&g_counter_frequency);

    HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(consoleHandle);

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);

    const int screen_width = screenBufferInfo.dwSize.X;
    const int screen_height = screenBufferInfo.dwSize.Y;

    char* screen = (char*)malloc(screen_width * screen_height);

    tetris_sim* sim = tetris_sim_init(k_host);

    const int matrix_width = tetris_sim_get_matrix_width(sim);
    const int matrix_height = tetris_sim_get_matrix_height(sim);
    const int tetronimo_max_width = tetris_sim_get_tetronimo_max_width(sim);
    const int tetronimo_max_height = tetris_sim_get_tetronimo_max_height(sim);

    const int sim_draw_x_offset = 2;
    const int sim_draw_y_offset = 2;

    // draw border
    const char border_char = '.';

    // top and bottom border
    for (int x = 0; x < matrix_width + sim_draw_x_offset * 2; ++x) {
        for (int y = 0; y < sim_draw_y_offset; ++y) {
            screen[y * screen_width + x] = border_char;
            screen[(matrix_height + sim_draw_y_offset + y) * screen_width + x] = border_char;
        }
    }
    // left and right border
    for (int y = 0; y < matrix_height + sim_draw_y_offset * 2; ++y) {
        for (int x = 0; x < sim_draw_x_offset; ++x) {
            screen[y * screen_width + x] = border_char;
            screen[y * screen_width + matrix_width + sim_draw_x_offset + x] = border_char;
        }
    }

    while (!tetris_sim_is_game_over(sim)) {
        Sleep(16);

        tetris_sim_update(sim);

        // draw matrix
        for (int y = 0; y < matrix_height; ++y) {
            // clear the existing line
            memset(screen + ((y + sim_draw_y_offset) * screen_width + sim_draw_x_offset), 0, matrix_width);

            // draw the updated line
            for (int x = 0; x < matrix_width; ++x) {
                if (tetris_sim_get_matrix_value(sim, x, y)) {
                    screen[(y + sim_draw_y_offset) * screen_width + x + sim_draw_x_offset] = 'x';
                }
            }
        }

        // draw tetronimo
        const int tetronimo_x = tetris_sim_get_tetronimo_pos_x(sim);
        const int tetronimo_y = tetris_sim_get_tetronimo_pos_y(sim); 
        for (int y = 0; y < tetronimo_max_height; ++y) {
            for (int x = 0; x < tetronimo_max_width; ++x) {
                if (tetris_sim_get_tetronimo_value(sim, x, y)) {
                    screen[(tetronimo_y + y + sim_draw_y_offset) * screen_width + (tetronimo_x + x + sim_draw_x_offset)] = 'x';
                }
            }
        }

        DWORD bytesWritten;
        WriteConsoleOutputCharacter(consoleHandle, screen, screen_width * screen_height, (COORD){ 0, 0 }, &bytesWritten);
    }

    tetris_sim_deinit(sim);

    free(screen);

    return 0;
}