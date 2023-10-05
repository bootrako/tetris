#ifndef TETRIS_SIM_H
#define TETRIS_SIM_H

#include <stdbool.h>
#include <stdint.h>

typedef enum tetris_input_t {
    TETRIS_INPUT_MOVE_LEFT,
    TETRIS_INPUT_MOVE_RIGHT,
    TETRIS_INPUT_FAST_DROP,
    TETRIS_INPUT_ROTATE_LEFT,
    TETRIS_INPUT_ROTATE_RIGHT,
    TETRIS_INPUT_COUNT
} tetris_input;

typedef struct {
    void*(*alloc)(void* context, size_t size);                  // allocate memory with the given size
    void(*free)(void* context, void* ptr);                      // free allocated memory
    void(*panic)(void* context, const char* err_msg);           // called when an unrecoverable error has occurred
    bool(*input_pressed)(void* context, tetris_input input);    // returns true if an input is currently pressed
    void* context;                                              // context object for storing host data
} tetris_sim_host;

typedef struct tetris_sim tetris_sim;

// initializes the simulation. internally allocates memory that can only be freed by calling deinit
tetris_sim* tetris_sim_init(tetris_sim_host host, const uint64_t random_seed);

// deinitialize the simulation. must be called on a sim pointer that was created with init
void tetris_sim_deinit(tetris_sim* sim);

// updates the simulation
void tetris_sim_update(tetris_sim* sim);

// gets the amount of time in seconds for each frame
float tetris_sim_time_per_frame(const tetris_sim* sim);

// returns true when the simulation is finished and the player has lost
bool tetris_sim_is_game_over(const tetris_sim* sim);

// get the width of the play field
int tetris_sim_get_matrix_width(const tetris_sim* sim);

// get the height of the play field
int tetris_sim_get_matrix_height(const tetris_sim* sim);

// returns true if a cell in the play field is occupied by a piece of tetronimo
bool tetris_sim_get_matrix_value(const tetris_sim* sim, int x, int y);

// returns true if the tetronimo is active in the matrix
bool tetris_sim_is_tetronimo_active(const tetris_sim* sim);

// get the max width of a tetronimo piece. pieces can be smaller than the max width
int tetris_sim_get_tetronimo_max_width(const tetris_sim* sim);

// get the max height of a tetronimo piece. pieces can be smaller than the max height
int tetris_sim_get_tetronimo_max_height(const tetris_sim* sim);

// returns true if (in tetronimo-space) the x and y value is a cell on the tetronimo - this takes into account the tetronimo's current rotation
bool tetris_sim_get_tetronimo_value(const tetris_sim* sim, int x, int y);

// returns true if (in tetronimo-space) the x and y value is a cell on the next tetronimo
bool tetris_sim_get_next_tetronimo_value(const tetris_sim* sim, int x, int y);

// returns the x position of the tetronimo in the play space. the origin is in the top left
int tetris_sim_get_tetronimo_pos_x(const tetris_sim* sim);

// returns the y position of the tetronimo in the play space. the origin is in the top left
int tetris_sim_get_tetronimo_pos_y(const tetris_sim* sim);

// return the total score
int tetris_sim_get_score(const tetris_sim* sim);

// return the number of lines cleared
int tetris_sim_get_lines(const tetris_sim* sim);

// return the current level
int tetris_sim_get_level(const tetris_sim* sim);

// returns the number of statistics
int tetris_sim_get_statistic_count(const tetris_sim* sim);

// returns the name of the statistic at a given index
const char* tetris_sim_get_statistic_name(const tetris_sim* sim, int index);

// returns the value of the statistic at a given index
int tetris_sim_get_statistic_value(const tetris_sim* sim, int index);

// returns true if a tetronimo was spawned this frame
bool tetris_sim_event_tetronimo_spawned(const tetris_sim* sim);

// returns true if a tetronimo was moved this frame
bool tetris_sim_event_tetronimo_moved(const tetris_sim* sim);

// returns true if a tetronimo was locked in place this frame
bool tetris_sim_event_tetronimo_locked(const tetris_sim* sim);

// returns the number of matrix rows cleared this frame
int tetris_sim_event_num_matrix_rows_cleared(const tetris_sim* sim);

// returns an array of the which matrix row indices were cleared this frame
const int* tetris_sim_event_matrix_rows_cleared(const tetris_sim* sim); 

#endif // TETRIS_SIM_H