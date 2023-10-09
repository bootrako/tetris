#include "tetris_tetronimo.h"
#include "tetris_ctx.h"
#include "tetris_matrix.h"

// A            // B            // C            // D
// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000 
// . . . . 0000 // . . x . 0010 // . . x . 0010 // . . x . 0010
// . x x x 0111 // . x x . 0110 // . x x x 0111 // . . x x 0011
// . . x . 0010 // . . x . 0010 // . . . . 0000 // . . x . 0010
static const tetris_tetronimo_rotation k_tetronimo_t[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x2 } }, // A
    { .rows = { 0x0, 0x2, 0x6, 0x2 } }, // B
    { .rows = { 0x0, 0x2, 0x7, 0x0 } }, // C
    { .rows = { 0x0, 0x2, 0x3, 0x2 } }  // D
};

// A            // B            // C            // D
// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000
// . . . . 0000 // . . x . 0010 // . x . . 0100 // . . x x 0011
// . x x x 0111 // . . x . 0010 // . x x x 0111 // . . x . 0010
// . . . x 0001 // . x x . 0110 // . . . . 0000 // . . x . 0010
static const tetris_tetronimo_rotation k_tetronimo_j[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x1 } }, // A
    { .rows = { 0x0, 0x2, 0x2, 0x6 } }, // B
    { .rows = { 0x0, 0x4, 0x7, 0x0 } }, // C
    { .rows = { 0x0, 0x3, 0x2, 0x2 } }  // D
};

// A            // B
// . . . . 0000 // . . . . 0000
// . . . . 0000 // . . . x 0001
// . x x . 0110 // . . x x 0011
// . . x x 0011 // . . x . 0010
static const tetris_tetronimo_rotation k_tetronimo_z[] = {
    { .rows = { 0x0, 0x0, 0x6, 0x3 } }, // A
    { .rows = { 0x0, 0x1, 0x3, 0x2 } }  // B
};

// A
// . . . .  0000
// . . . .  0000
// . x x .  0110
// . x x .  0110
static const tetris_tetronimo_rotation k_tetronimo_o[] = { 
    { .rows = { 0x0, 0x0, 0x6, 0x6 } }  // A
};

// A            // B
// . . . . 0000 // . . . . 0000
// . . . . 0000 // . . x . 0010
// . . x x 0011 // . . x x 0011
// . x x . 0110 // . . . x 0001
static const tetris_tetronimo_rotation k_tetronimo_s[] = {
    { .rows = { 0x0, 0x0, 0x3, 0x6 } }, // A
    { .rows = { 0x0, 0x2, 0x3, 0x1 } }  // B
};

// A            // B            // C            // D
// . . . . 0000 // . . . . 0000 // . . . . 0000 // . . . . 0000 
// . . . . 0000 // . x x . 0110 // . . . x 0001 // . . x . 0010
// . x x x 0111 // . . x . 0010 // . x x x 0111 // . . x . 0010
// . x . . 0100 // . . x . 0010 // . . . . 0000 // . . x x 0011
static const tetris_tetronimo_rotation k_tetronimo_l[] = {
    { .rows = { 0x0, 0x0, 0x7, 0x4 } }, // A
    { .rows = { 0x0, 0x6, 0x2, 0x2 } }, // B
    { .rows = { 0x0, 0x1, 0x7, 0x0 } }, // C
    { .rows = { 0x0, 0x2, 0x2, 0x3 } }  // D
};

// A            // B
// . . . . 0000 // . . x . 0010
// . . . . 0000 // . . x . 0010
// x x x x 1111 // . . x . 0010
// . . . . 0000 // . . x . 0010
static const tetris_tetronimo_rotation k_tetronimo_i[] = { 
    { .rows = { 0x0, 0x0, 0xF, 0x0 } }, // A
    { .rows = { 0x2, 0x2, 0x2, 0x2 } }  // B
};

static const tetris_tetronimo k_tetronimoes[TETRIS_TETRONIMO_SHAPE_COUNT] = {
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

bool tetris_tetronimo_init(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, tetris_tetronimo_spawner* spawner) {
    *tetronimo = tetris_tetronimo_spawner_next(ctx, spawner);
    tetronimo->x = TETRIS_TETRONIMO_INIT_X;
    tetronimo->y = TETRIS_TETRONIMO_INIT_Y;

    const bool spawn_is_valid = tetris_matrix_is_tetronimo_valid(ctx, matrix, tetronimo);
    tetronimo->is_active = spawn_is_valid;
    ctx->events.tetronimo_spawned = spawn_is_valid;

    return spawn_is_valid;
}

bool tetris_tetronimo_move(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_x, const int dir_y) {
    TETRIS_CTX_CHECK(ctx, dir_x >= -1 && dir_x <= 1);
    TETRIS_CTX_CHECK(ctx, dir_y >= -1 && dir_y <= 1);

    tetronimo->x += dir_x;
    tetronimo->y += dir_y;

    const bool move_is_valid = tetris_matrix_is_tetronimo_valid(ctx, matrix, tetronimo);
    if (move_is_valid) {
        ctx->events.tetronimo_moved |= true;
    } else {
        tetronimo->x -= dir_x;
        tetronimo->y -= dir_y;
        if (dir_y == 1) {
            tetronimo->is_active = false;
            ctx->events.tetronimo_locked |= true;
        }
    }
    return move_is_valid;
}

void tetris_tetronimo_rotate(tetris_ctx* ctx, tetris_tetronimo* tetronimo, const tetris_matrix* matrix, const int dir_rot) {
    TETRIS_CTX_CHECK(ctx, dir_rot == -1 || dir_rot == 1);

    const int prev_rotation = tetronimo->current_rotation;
    tetronimo->current_rotation = (tetronimo->current_rotation + dir_rot + tetronimo->num_rotations) % tetronimo->num_rotations;
    const bool rotate_is_valid = tetris_matrix_is_tetronimo_valid(ctx, matrix, tetronimo);
    if (rotate_is_valid) {
        ctx->events.tetronimo_moved |= true;
    } else {
        tetronimo->current_rotation = prev_rotation;
    }
}

tetris_tetronimo_row tetris_tetronimo_get_row(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int row) {
    TETRIS_CTX_CHECK(ctx, tetronimo->current_rotation >= 0 && tetronimo->current_rotation < tetronimo->num_rotations);
    TETRIS_CTX_CHECK(ctx, row >= 0 && row < TETRIS_TETRONIMO_MAX_HEIGHT);

    return tetronimo->rotations[tetronimo->current_rotation].rows[row];
}

bool tetris_tetronimo_get_cell(const tetris_ctx* ctx, const tetris_tetronimo* tetronimo, const int x, const int y) {
    TETRIS_CTX_CHECK(ctx, x >= 0 && x < TETRIS_TETRONIMO_MAX_WIDTH);
    
    return (tetris_tetronimo_get_row(ctx, tetronimo, y) >> (TETRIS_TETRONIMO_MAX_WIDTH - 1 - x)) & 1;
}

void tetris_tetronimo_spawner_init(tetris_ctx* ctx, tetris_tetronimo_spawner* spawner, const uint64_t seed) {
    tetris_rand_init(ctx, &spawner->rand, seed);
    spawner->next = &k_tetronimoes[tetris_rand_range(ctx, &spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes))];
}

tetris_tetronimo tetris_tetronimo_spawner_next(tetris_ctx* ctx, tetris_tetronimo_spawner* spawner) {
    tetris_tetronimo next = *spawner->next;
    TETRIS_CTX_CHECK(ctx, next.shape >= 0 && next.shape < TETRIS_TETRONIMO_SHAPE_COUNT);

    int next_index = tetris_rand_range(ctx, &spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes));

    // try one reroll if we get the same piece as last time
    if (&k_tetronimoes[next_index] == spawner->next) {
        next_index = tetris_rand_range(ctx, &spawner->rand, 0, TETRIS_ARRAY_LEN(k_tetronimoes));
    }

    spawner->next = &k_tetronimoes[next_index];

    return next;
}