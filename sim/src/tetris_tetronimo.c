#include "tetris_tetronimo.h"
#include "tetris_matrix.h"
#include "tetris_utils.h"

const char* const k_tetris_tetronimo_shape_names[TETRIS_TETRONIMO_SHAPE_COUNT] = {
    "T", // TETRIS_TETRONIMO_SHAPE_T
    "J", // TETRIS_TETRONIMO_SHAPE_J
    "Z", // TETRIS_TETRONIMO_SHAPE_Z
    "O", // TETRIS_TETRONIMO_SHAPE_I
    "S", // TETRIS_TETRONIMO_SHAPE_S
    "L", // TETRIS_TETRONIMO_SHAPE_L
    "I"  // TETRIS_TETRONIMO_SHAPE_I
};

// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000 
// . . . . 0000 // . . x . 0010 // . . x . 0010 // . . x . 0010
// . x x x 0111 // . x x . 0110 // . x x x 0111 // . . x x 0011
// . . x . 0010 // . . x . 0010 // . . . . 0000 // . . x . 0010
const tetris_tetronimo_rotation k_tetronimo_t[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x2 } },
    { .rows = { 0x0, 0x2, 0x6, 0x2 } },
    { .rows = { 0x0, 0x2, 0x7, 0x0 } },
    { .rows = { 0x0, 0x2, 0x3, 0x2 } }
};

// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000
// . . . . 0000 // . . x . 0010 // . x . . 0100 // . . x x 0011
// . x x x 0111 // . . x . 0010 // . x x x 0111 // . . x . 0010
// . . . x 0001 // . x x . 0110 // . . . . 0000 // . . x . 0010
const tetris_tetronimo_rotation k_tetronimo_j[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x1 } },
    { .rows = { 0x0, 0x2, 0x2, 0x6 } },
    { .rows = { 0x0, 0x4, 0x7, 0x0 } },
    { .rows = { 0x0, 0x3, 0x2, 0x2 } }
};

// . . . . 0000 // . . . . 0000
// . . . . 0000 // . . . x 0001
// . x x . 0110 // . . x x 0011
// . . x x 0011 // . . x . 0010
const tetris_tetronimo_rotation k_tetronimo_z[] = {
    { .rows = { 0x0, 0x0, 0x6, 0x3 } },
    { .rows = { 0x0, 0x1, 0x3, 0x2 } }
};

// . . . .  0000
// . . . .  0000
// . x x .  0110
// . x x .  0110
const tetris_tetronimo_rotation k_tetronimo_o[] = { 
    { .rows = { 0x0, 0x0, 0x6, 0x6 } }
};

// . . . . 0000 // . . . . 0000
// . . . . 0000 // . . x . 0010
// . . x x 0011 // . . x x 0011
// . x x . 0110 // . . . x 0001
const tetris_tetronimo_rotation k_tetronimo_s[] = {
    { .rows = { 0x0, 0x0, 0x3, 0x6 } },
    { .rows = { 0x0, 0x2, 0x3, 0x1 } }
};

// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000 
// . . . . 0000 // . x x . 0110 // . . . x 0001 // . . x . 0010
// . x x x 0111 // . . x . 0010 // . x x x 0111 // . . x . 0010
// . x . . 0100 // . . x . 0010 // . . . . 0000 // . . x x 0011
const tetris_tetronimo_rotation k_tetronimo_l[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x4 } },
    { .rows = { 0x0, 0x6, 0x2, 0x2 } },
    { .rows = { 0x0, 0x1, 0x7, 0x0 } },
    { .rows = { 0x0, 0x2, 0x2, 0x3 } }
};

// . . . . 0000 // . . x . 0010
// . . . . 0000 // . . x . 0010
// x x x x 1111 // . . x . 0010
// . . . . 0000 // . . x . 0010
const tetris_tetronimo_rotation k_tetronimo_i[] = { 
    { .rows = { 0x0, 0x0, 0xF, 0x0 } },
    { .rows = { 0x2, 0x2, 0x2, 0x2 } }
};

const tetris_tetronimo k_tetronimoes[] = {
    { .shape = TETRIS_TETRONIMO_SHAPE_T, .rotations = k_tetronimo_t, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_t) },
    { .shape = TETRIS_TETRONIMO_SHAPE_J, .rotations = k_tetronimo_j, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_j) },
    { .shape = TETRIS_TETRONIMO_SHAPE_Z, .rotations = k_tetronimo_z, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_z) },
    { .shape = TETRIS_TETRONIMO_SHAPE_O, .rotations = k_tetronimo_o, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_o) },
    { .shape = TETRIS_TETRONIMO_SHAPE_S, .rotations = k_tetronimo_s, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_s) },
    { .shape = TETRIS_TETRONIMO_SHAPE_L, .rotations = k_tetronimo_l, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_l) },
    { .shape = TETRIS_TETRONIMO_SHAPE_I, .rotations = k_tetronimo_i, .num_rotations = TETRIS_ARRAY_LEN(k_tetronimo_i) },
};

#define TETRIS_TETRONIMO_INIT_X (3)
#define TETRIS_TETRONIMO_INIT_Y (-2)

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
    // get the direction away from the nearest wall
    const int dir = (tetronimo->x < TETRIS_MATRIX_WIDTH - tetronimo->x) ? 1 : -1;
    while (tetris_matrix_collide(matrix, tetronimo, true)) {
        tetronimo->x += dir;
    }
}

void tetris_tetronimo_init(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, tetris_tetronimo_spawner* spawner) {
    *tetronimo = tetris_tetronimo_spawner_spawn(spawner);
    tetronimo->x = TETRIS_TETRONIMO_INIT_X;
    tetronimo->y = TETRIS_TETRONIMO_INIT_Y;
    tetronimo->is_locked = tetris_tetronimo_resolve_collisions(tetronimo, matrix, 0, 1);
    tetronimo->is_active = true;
}

bool tetris_tetronimo_move(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_x, const int dir_y) {
    tetronimo->x += dir_x;
    tetronimo->y += dir_y;
    const bool has_collisions = tetris_tetronimo_resolve_collisions(tetronimo, matrix, dir_x, dir_y);
    if (dir_y == 1) {
        tetronimo->is_locked = has_collisions;
    }
    return has_collisions;
}

void tetris_tetronimo_rotate(tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_rot) {
    tetronimo->current_rotation = (tetronimo->current_rotation + dir_rot + tetronimo->num_rotations) % tetronimo->num_rotations;
    tetris_tetronimo_resolve_out_of_bounds(tetronimo, matrix);
    tetronimo->is_locked = tetris_tetronimo_resolve_collisions(tetronimo, matrix, 0, 1);
}

tetris_tetronimo_row tetris_tetronimo_get_row(const tetris_tetronimo* tetronimo, const int row) {
    return tetronimo->rotations[tetronimo->current_rotation].rows[row];
}

bool tetris_tetronimo_get_value(const tetris_tetronimo* tetronimo, const int x, const int y) {
    return (tetronimo->rotations[tetronimo->current_rotation].rows[y] >> (TETRIS_TETRONIMO_MAX_WIDTH - 1 - x)) & 1;
}

void tetris_tetronimo_spawner_init(tetris_tetronimo_spawner* spawner, const uint64_t seed) {
    tetris_rand_init(&spawner->rand, seed);
    spawner->next = &k_tetronimoes[tetris_rand_range(&spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes))];
    for (int i = 0; i < TETRIS_ARRAY_LEN(spawner->num_spawned); ++i) {
        spawner->num_spawned[i] = 0;
    }
}

tetris_tetronimo tetris_tetronimo_spawner_spawn(tetris_tetronimo_spawner* spawner) {
    tetris_tetronimo spawned = *spawner->next;
    spawner->num_spawned[spawned.shape]++;

    uint32_t next_index = tetris_rand_range(&spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes));

    // try one reroll if we get the same piece as last time
    if (&k_tetronimoes[next_index] == spawner->next) {
        next_index = tetris_rand_range(&spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes));
    }

    spawner->next = &k_tetronimoes[next_index];

    return spawned;
}