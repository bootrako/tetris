#include "tetris_ctx.h"

void tetris_ctx_init(tetris_ctx* ctx, const tetris_sim_host host) {
    ctx->host = host;
    tetris_events_init(&ctx->events);
}