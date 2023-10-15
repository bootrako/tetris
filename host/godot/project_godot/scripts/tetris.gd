class_name Tetris
extends Node2D

var sim: TetrisSim = TetrisSim.new()

@onready var tetronimo_spawned: bool = false
@onready var tetronimo_moved: bool = false
@onready var tetronimo_moved_horiz: bool = false
@onready var tetronimo_rotated: bool = false
@onready var tetronimo_locked: bool = false
@onready var matrix_rows_cleared: PackedInt32Array = []
@onready var num_frames_simmed: int = 0

@onready var _sim_time_per_frame: float = sim.get_time_per_frame()
@onready var _frame_accumulator: float = 0

func _process(delta: float) -> void:
    sim.poll_input()
    
    tetronimo_spawned = false
    tetronimo_moved = false
    tetronimo_moved_horiz = false
    tetronimo_rotated = false
    tetronimo_locked = false
    matrix_rows_cleared = []
    num_frames_simmed = 0
    
    _frame_accumulator += delta
    while _frame_accumulator > _sim_time_per_frame:
        var prev_tetronimo_pos_x := sim.get_tetronimo_pos_x()
        sim.update()
        num_frames_simmed += 1
        _frame_accumulator -= _sim_time_per_frame
        if sim.event_tetronimo_spawned():
            tetronimo_spawned = true
        if sim.event_tetronimo_moved():
            tetronimo_moved = true
            if sim.get_tetronimo_pos_x() != prev_tetronimo_pos_x:
                tetronimo_moved_horiz = true
        if sim.event_tetronimo_rotated():
            tetronimo_rotated = true
        if sim.event_tetronimo_locked():
            tetronimo_locked = true
        var matrix_rows_cleared_this_frame := sim.event_matrix_rows_cleared()
        if !matrix_rows_cleared_this_frame.is_empty():
            matrix_rows_cleared = matrix_rows_cleared_this_frame

func shape_to_color_index(shape: int) -> Cell.ColorIndex:
    match shape:
        0, 3, 6:
            return Cell.ColorIndex.SHAPE_0
        1, 4:
            return Cell.ColorIndex.SHAPE_2
        2, 5:
            return Cell.ColorIndex.SHAPE_1
        _:
            return Cell.ColorIndex.BACKGROUND
