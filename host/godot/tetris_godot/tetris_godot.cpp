#include "tetris_godot.h"

void Tetris::add(int p_value) {
    count += p_value;
}

void Tetris::reset() {
    count = 0;
}

int Tetris::get_total() const {
    return count;
}

void Tetris::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Tetris::add);
    ClassDB::bind_method(D_METHOD("reset"), &Tetris::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Tetris::get_total);
}

Tetris::Tetris() {
    count = 0;
}