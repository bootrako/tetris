#ifndef TETRIS_GODOT_H
#define TETRIS_GODOT_H

#include "scene/main/node.h"
#include <tetris_sim.h>

struct tetris_sim;

class Tetris : public Node {
    GDCLASS(Tetris, Node);
public:
    Tetris();
    virtual ~Tetris();
private:
    void _process(float delta);
    void _notification(int p_notification);
    void _poll_input();

    static void* _godot_alloc(void* context, size_t size);
    static void _godot_free(void* context, void* ptr);
    static void _godot_panic(void* context, const char* err_msg);
    static bool _godot_input_pressed(void* context, const tetris_input input);

    bool input_pressed[TETRIS_INPUT_COUNT];
    tetris_sim* sim;
};

#endif // TETRIS_GODOT_H