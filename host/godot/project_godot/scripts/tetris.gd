class_name Tetris
extends Node2D

var sim: TetrisSim = TetrisSim.new()

@onready var tetronimo_spawned: bool = false
@onready var tetronimo_moved: bool = false

@onready var _sim_time_per_frame: float = sim.get_time_per_frame()
@onready var _frame_accumulator: float = 0

func _process(delta: float) -> void:
    sim.poll_input()
    
    tetronimo_spawned = false
    tetronimo_moved = false
    
    _frame_accumulator += delta
    while (_frame_accumulator > _sim_time_per_frame):
        sim.update()
        _frame_accumulator -= _sim_time_per_frame
        if (sim.event_tetronimo_spawned()):
            tetronimo_spawned = true
        if (sim.event_tetronimo_moved()):
            tetronimo_moved = true

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
