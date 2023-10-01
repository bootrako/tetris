#include "tetris_tetronimo.h"
#include "tetris_matrix.h"
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

#define TETRIS_TETRONIMO_INIT_X (4)
#define TETRIS_TETRONIMO_INIT_Y (0)

static bool tetris_tetronimo_resolve_collisions(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, int dir_x, int dir_y) {
    bool has_collisions = false;
    while (tetris_matrix_collide(matrix, tetronimo, false)) {
        tetronimo->x -= dir_x;
        tetronimo->y -= dir_y;
        has_collisions = true;
    }
    return has_collisions;
}

static void tetris_tetronimo_resolve_out_of_bounds(tetris_tetronimo* tetronimo, const tetris_matrix* matrix) {
    const int dir = (tetronimo->x < TETRIS_MATRIX_WIDTH - tetronimo->x) ? 1 : -1;
    while (tetris_matrix_collide(matrix, tetronimo, true)) {
        tetronimo->x += dir;
    }
}

void tetris_tetronimo_init(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, tetris_tetronimo_spawner* spawner) {
    *tetronimo = tetris_tetronimo_spawner_spawn(spawner);
    tetronimo->x = TETRIS_TETRONIMO_INIT_X;
    tetronimo->y = TETRIS_TETRONIMO_INIT_Y;
    tetronimo->is_grounded = tetris_tetronimo_resolve_collisions(tetronimo, matrix, 0, 1);
    tetronimo->is_active = true;
}

bool tetris_tetronimo_move(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_x, const int dir_y) {
    tetronimo->x += dir_x;
    tetronimo->y += dir_y;
    const bool has_collisions = tetris_tetronimo_resolve_collisions(tetronimo, matrix, dir_x, dir_y);
    if (dir_y == 1) {
        tetronimo->is_grounded = has_collisions;
    }
    return has_collisions;
}

void tetris_tetronimo_rotate(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_rot) {
    tetronimo->current_rotation = (tetronimo->current_rotation + dir_rot + tetronimo->num_rotations) % tetronimo->num_rotations;
    tetris_tetronimo_resolve_out_of_bounds(tetronimo, matrix);
    tetronimo->is_grounded = tetris_tetronimo_resolve_collisions(tetronimo, matrix, 0, 1);
}

bool tetris_tetronimo_get_value(const tetris_tetronimo* tetronimo, const int x, const int y) {
    return (tetronimo->rotations[tetronimo->current_rotation].rows[y] >> (TETRIS_TETRONIMO_MAX_WIDTH - 1 - x)) & 1;
}

static void tetris_tetronimo_spawner_gen_next(tetris_tetronimo_spawner* spawner) {
    if (spawner->bag_remaining == 0) {
        for (int i = 0; i < TETRIS_TETRONIMO_COUNT; ++i) {
            spawner->bag[i] = &k_tetronimoes[i];
        }
        spawner->bag_remaining = TETRIS_TETRONIMO_COUNT;
    }

    const uint32_t rand_index = tetris_rand_range(&spawner->rand, 0, spawner->bag_remaining);
    spawner->next = *spawner->bag[rand_index];

    spawner->bag_remaining--;
    spawner->bag[rand_index] = spawner->bag[spawner->bag_remaining]; 
}

void tetris_tetronimo_spawner_init(tetris_tetronimo_spawner* spawner, const uint64_t seed) {
    tetris_rand_init(&spawner->rand, seed);
    spawner->bag_remaining = 0;
    tetris_tetronimo_spawner_gen_next(spawner);
}

tetris_tetronimo tetris_tetronimo_spawner_spawn(tetris_tetronimo_spawner* spawner) {
    tetris_tetronimo spawned = spawner->next;
    tetris_tetronimo_spawner_gen_next(spawner);
    return spawned;
}

const tetris_tetronimo* tetris_tetronimo_spawner_next(const tetris_tetronimo_spawner* spawner) {
    return &spawner->next;
}