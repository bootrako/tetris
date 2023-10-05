#ifndef TETRIS_EVENTS_H
#define TETRIS_EVENTS_H

#include "tetris_tetronimo.h"

typedef struct tetris_events_t {
    int matrix_rows_cleared[TETRIS_TETRONIMO_MAX_HEIGHT];   // the list of rows that were cleared this frame
    int num_matrix_rows_cleared;                            // the number of rows that were cleared this frame
    bool tetronimo_spawned;                                 // true if the tetronimo was spawned during this frame
    bool tetronimo_moved;                                   // true if the tetronimo moved during this frame
    bool tetronimo_locked;                                  // true if the tetronimo was locked in place during this frame
} tetris_events;

void tetris_events_init(tetris_events* events);

#endif // TETRIS_EVENTS_H