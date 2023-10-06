#include "tetris_godot.h"
#include <tetris_sim.h>

Tetris::Tetris() {
    tetris_sim_host host;
    sim = tetris_sim_init(host, 0);
}

Tetris::~Tetris() {
    tetris_sim_deinit(sim);
}

void Tetris::_bind_methods() {
    //ClassDB::bind_method(D_METHOD("add", "value"), &Tetris::add);
}