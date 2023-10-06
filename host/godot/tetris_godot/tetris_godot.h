#ifndef TETRIS_GODOT_H
#define TETRIS_GODOT_H

#include "core/object/ref_counted.h"

class Tetris : public RefCounted {
    GDCLASS(Tetris, RefCounted);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int p_value);
    void reset();
    int get_total() const;

    Tetris();
};

#endif // TETRIS_GODOT_H