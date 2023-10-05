#include "tetris_events.h"

void tetris_events_init(tetris_events* events) {
    events->num_matrix_rows_cleared = 0;
    events->tetronimo_spawned = false;
    events->tetronimo_moved = false;
    events->tetronimo_locked = false;
}