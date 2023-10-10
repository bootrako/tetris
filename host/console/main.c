#include "console_host.h"
#include "console_render.h"
#include <stdio.h>

static uint64_t console_get_random_seed() {
    FILETIME system_time;
    GetSystemTimePreciseAsFileTime(&system_time);
    return (ULARGE_INTEGER){ .LowPart = system_time.dwLowDateTime, .HighPart = system_time.dwHighDateTime }.QuadPart;
}

int main(int argc, const char** argv) {
    console_host_context* host_context = console_host_context_init();
    const tetris_sim_host sim_host = {
        .alloc = console_host_alloc,
        .free = console_host_free,
        .panic = console_host_panic,
        .input_pressed = console_host_input_pressed,
        .context = host_context,
    };
    tetris_sim* sim = tetris_sim_init(sim_host, console_get_random_seed());
    console_render* render = console_render_init();

    // caching these values to display cleared lines until tetronimo respawn
    int* rows_cleared = (int*)malloc(sizeof(int) * tetris_sim_get_tetronimo_max_height(sim));
    int num_rows_cleared = 0;

    // keep track of how many times each shape was spawned
    int shape_stats[TETRIS_TETRONIMO_SHAPE_COUNT];
    for (int shape = 0; shape < TETRIS_TETRONIMO_SHAPE_COUNT; ++shape) {
        shape_stats[shape] = 0;
    }
    shape_stats[tetris_sim_get_tetronimo_shape(sim)]++;

    console_render_draw_matrix_border(render, sim);
    console_render_draw_matrix(render, sim, NULL, 0);
    console_render_draw_tetronimo(render, sim);
    console_render_draw_ui(render, sim, shape_stats);

    // present to each buffer so we don't have to draw matrix border again
    console_render_present(render);
    console_render_present(render);

    LARGE_INTEGER counter_frequency;
    QueryPerformanceFrequency(&counter_frequency);

    const float time_per_frame = tetris_sim_time_per_frame(sim);
    float frame_time_accumulator = 0.0f;
    while (!tetris_sim_is_game_over(sim)) {
        LARGE_INTEGER counter_start;
        QueryPerformanceCounter(&counter_start);

        console_host_context_update(host_context);

        bool tetronimo_spawned = false;
        bool tetronimo_moved = false;
        while (frame_time_accumulator > time_per_frame) {
            tetris_sim_update(sim);
            tetronimo_spawned |= tetris_sim_event_tetronimo_spawned(sim);
            tetronimo_moved |= tetris_sim_event_tetronimo_moved(sim);
            int num_rows_cleared_this_frame = tetris_sim_event_num_matrix_rows_cleared(sim);
            if (num_rows_cleared_this_frame != 0) {
                num_rows_cleared = num_rows_cleared_this_frame;
                const int* rows_cleared_this_frame = tetris_sim_event_matrix_rows_cleared(sim);
                for (int i = 0; i < num_rows_cleared; ++i) {
                    rows_cleared[i] = rows_cleared_this_frame[i];
                }
            }
            frame_time_accumulator -= time_per_frame;
        }

        if (tetronimo_spawned) {
            num_rows_cleared = 0;
            shape_stats[tetris_sim_get_tetronimo_shape(sim)]++;
        }

        if (tetronimo_spawned || tetronimo_moved || num_rows_cleared != 0) {
            console_render_draw_matrix(render, sim, rows_cleared, num_rows_cleared);
            console_render_draw_tetronimo(render, sim);
            console_render_draw_ui(render, sim, shape_stats);
            console_render_present(render);
        }

        Sleep((DWORD)(time_per_frame * 1000.0f));

        LARGE_INTEGER counter_end;
        QueryPerformanceCounter(&counter_end);

        const float max_frame_time = time_per_frame * 10.0f;
        float frame_time = (float)(counter_end.QuadPart - counter_start.QuadPart) / (float)(counter_frequency.QuadPart);
        if (frame_time > max_frame_time) {
            frame_time = max_frame_time;
        }
        frame_time_accumulator += frame_time;
    }

    console_render_deinit(render);
    tetris_sim_deinit(sim);
    console_host_context_deinit(host_context);

    return 0;
}