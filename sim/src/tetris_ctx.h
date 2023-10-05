#ifndef TETRIS_CTX_H
#define TETRIS_CTX_H

#include <tetris_sim.h>
#include "tetris_events.h"
#include "tetris_utils.h"

typedef struct tetris_ctx_t {
    tetris_sim_host host;       // the interface passed in by the host
    tetris_events events;       // event state that is modified during sim update
} tetris_ctx;

void tetris_ctx_init(tetris_ctx* ctx, const tetris_sim_host host);

#ifndef NDEBUG
#define TETRIS_CTX_CHECK(ctx, expr) if (!(expr)) (ctx)->host.panic((ctx)->host.context, __FILE__ "." TETRIS_STR(__LINE__) ": assert failed: " #expr)
#else
#define TETRIS_CTX_CHECK(ctx, expr)
#endif

#endif // TETRIS_CTX_H