#include <windows.h>
#include <tetris_sim.h>

void* host_alloc(size_t size) {
    return malloc(size);
}

void host_free(void* ptr) {
    free(ptr);
}

uint64_t host_time() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    return (uint64_t)(counter.QuadPart / frequency.QuadPart);
}

const tetris_host k_host = {
    .alloc = host_alloc,
    .free = host_free,
    .time = host_time,
};

int main(int argc, const char** argv)
{
    HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(consoleHandle);

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);

    const int screenWidth = screenBufferInfo.dwSize.X;
    const int screenHeight = screenBufferInfo.dwSize.Y;

    char* screen = (char*)malloc(screenWidth * screenHeight);

    tetris_sim* sim = tetris_sim_init(k_host);

    while (!tetris_sim_is_game_over(sim)) {
        Sleep(33);

        tetris_sim_update(sim);

        memset(screen, 0, screenWidth * screenHeight);

        const int num_grid_rows = tetris_sim_get_num_grid_rows(sim);
        const uint16_t* grid_rows = tetris_sim_get_grid_rows(sim);
        for (int i = 0; i < num_grid_rows; ++i) {
            for (int bit = 0; bit < 16; ++bit) {
                if ((grid_rows[i] >> bit) & 1) {
                    screen[i * screenWidth + bit] = 'x';
                }
            }
        }

        const int num_tetronimo_rows = tetris_sim_get_num_tetronimo_rows(sim);
        const uint8_t* tetronimo_rows = tetris_sim_get_tetronimo_rows(sim);
        const int tetronimo_x = tetris_sim_get_tetronimo_x(sim);
        const int tetronimo_y = tetris_sim_get_tetronimo_y(sim);
        for (int i = 0; i < num_tetronimo_rows; ++i) {
            for (int bit = 0; bit < 8; ++bit) {
                if ((tetronimo_rows[i] >> bit) & 1) {
                    screen[(tetronimo_y + i) * screenWidth + tetronimo_x + bit] = 'x';
                }
            }
        }

        DWORD bytesWritten = 0;
        WriteConsoleOutputCharacter(consoleHandle, screen, screenWidth * screenHeight, (COORD){ 0, 0 }, &bytesWritten);
    }

    tetris_sim_deinit(sim);

    free(screen);

    return 0;
}