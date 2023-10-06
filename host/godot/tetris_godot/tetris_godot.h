#ifndef TETRIS_GODOT_H
#define TETRIS_GODOT_H

#include "core/object/ref_counted.h"

struct tetris_sim;

class Tetris : public RefCounted {
    GDCLASS(Tetris, RefCounted);
public:
    Tetris();
    virtual ~Tetris();
protected:
    static void _bind_methods();
private:
    tetris_sim* sim;
};

#endif // TETRIS_GODOT_H