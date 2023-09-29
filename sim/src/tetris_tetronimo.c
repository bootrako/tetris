#include "tetris_tetronimo.h"
#include "tetris_grid.h"
#include "tetris_utils.h"

// . . . .  0000
// . . . .  0000
// x x . .  1100
// x x . .  1100
const tetris_tetronimo_rotation k_tetronimo_o[] = { 
    { .rows = { 0x0, 0x0, 0xC, 0xC } }
};

// x . . . 1000 // . . . . 0000
// x . . . 1000 // . . . . 0000
// x . . . 1000 // . . . . 0000
// x . . . 1000 // x x x x 1111
const tetris_tetronimo_rotation k_tetronimo_i[] = { 
    { .rows = { 0x8, 0x8, 0x8, 0x8 } },
    { .rows = { 0x0, 0x0, 0x0, 0xF } }
};

// . . . . 0000 // . . . . 0000
// . . . . 0000 // x . . . 1000
// . x x . 0110 // x x . . 1100
// x x . . 1100 // . x . . 0100
const tetris_tetronimo_rotation k_tetronimo_s[] = {
    { .rows = { 0x0, 0x0, 0x6, 0xC } },
    { .rows = { 0x0, 0x8, 0xC, 0x4 } }
};

// . . . . 0000 // . . . . 0000
// . . . . 0000 // . x . . 0100
// x x . . 1100 // x x . . 1100
// . x x . 0110 // x . . . 1000
const tetris_tetronimo_rotation k_tetronimo_z[] = {
    { .rows = { 0x0, 0x0, 0xC, 0x6 } },
    { .rows = { 0x0, 0x4, 0xC, 0x8 } }
};

// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000 
// . . . . 0000 // . x . . 0100 // . . . . 0000 // x . . . 1000
// x x x . 1110 // x x . . 1100 // . x . . 0100 // x x . . 1100
// . x . . 0100 // . x . . 0100 // x x x . 1110 // x . . . 1000
const tetris_tetronimo_rotation k_tetronimo_t[] = {
    { .rows = { 0x0, 0x0, 0xE, 0x4 } },
    { .rows = { 0x0, 0x4, 0xC, 0x4 } },
    { .rows = { 0x0, 0x0, 0x4, 0xE } },
    { .rows = { 0x0, 0x8, 0xC, 0x8 } }
};

// . x . . 0100 // . . . . 0000 // x x . . 1100 // . . . . 0000
// . x . . 0100 // . . . . 0000 // x . . . 1000 // . . . . 0000
// . x . . 0100 // x . . . 1000 // x . . . 1000 // x x x x 1111
// x x . . 1100 // x x x x 1111 // x . . . 1000 // . . . x 0001
const tetris_tetronimo_rotation k_tetronimo_j[] = {
    { .rows = { 0x4, 0x4, 0x4, 0xC } },
    { .rows = { 0x0, 0x0, 0x8, 0xF } },
    { .rows = { 0xC, 0x8, 0x8, 0x8 } },
    { .rows = { 0x0, 0x0, 0xF, 0x1 } }
};

// x . . . 1000 // . . . . 0000 // x x . . 1100 // . . . . 0000 
// x . . . 1000 // . . . . 0000 // . x . . 0100 // . . . . 0000
// x . . . 1000 // x x x x 1111 // . x . . 0100 // . . . x 0001
// x x . . 1100 // x . . . 1000 // . x . . 0100 // x x x x 1111
const tetris_tetronimo_rotation k_tetronimo_l[] = {
    { .rows = { 0x8, 0x8, 0x8, 0xC } },
    { .rows = { 0x0, 0x0, 0xF, 0x8 } },
    { .rows = { 0xC, 0x4, 0x4, 0x4 } },
    { .rows = { 0x0, 0x0, 0x1, 0xF } }
};

const tetris_tetronimo k_tetronimoes[] = {
    { .rotations = k_tetronimo_o, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_o) },
    { .rotations = k_tetronimo_i, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_i) },
    { .rotations = k_tetronimo_s, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_s) },
    { .rotations = k_tetronimo_z, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_z) },
    { .rotations = k_tetronimo_t, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_t) },
    { .rotations = k_tetronimo_j, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_j) },
    { .rotations = k_tetronimo_l, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_l) },
};

const int tetris_player_start_x = 5;
const int tetris_player_start_y = 4;

static bool tetris_tetronimo_resolve_collisions(tetris_tetronimo* tetronimo, const tetris_grid* grid, int dir_x, int dir_y) {
    bool has_collisions = false;
    while (tetris_grid_collide(grid, tetronimo)) {
        tetronimo->x -= dir_x;
        tetronimo->y -= dir_y;
        has_collisions = true;
        // probably should assert or something here to prevent an infinite loop
    }
    return has_collisions;
}

void tetris_tetronimo_init(tetris_tetronimo* tetronimo, const tetris_grid* grid, tetris_tetronimo_spawner* spawner) {
    *tetronimo = tetris_tetronimo_spawner_next(spawner);
    tetronimo->x = tetris_player_start_x;
    tetronimo->y = tetris_player_start_y;
    tetronimo->is_grounded = tetris_tetronimo_resolve_collisions(tetronimo, grid, 0, 1);
}

void tetris_tetronimo_move(tetris_tetronimo* tetronimo, const tetris_grid* grid, const int dir_x, const int dir_y) {
    tetronimo->x += dir_x;
    tetronimo->y += dir_y;
    const bool has_collisions = tetris_tetronimo_resolve_collisions(tetronimo, grid, dir_x, dir_y);
    if (dir_y == 1) {
        tetronimo->is_grounded = has_collisions;
    }
}

void tetris_tetronimo_spawner_init(tetris_tetronimo_spawner* spawner, const uint64_t seed) {
    tetris_rand_init(&spawner->rand, seed);
}

tetris_tetronimo tetris_tetronimo_spawner_next(tetris_tetronimo_spawner* spawner) {
    return k_tetronimoes[tetris_rand_range(&spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes))];
}