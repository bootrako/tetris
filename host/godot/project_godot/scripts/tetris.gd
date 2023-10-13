class_name Tetris
extends Node2D

@onready var sim: TetrisSim = TetrisSim.new()
@onready var _sim_time_per_frame: float = sim.get_time_per_frame()
@onready var _frame_accumulator: float = 0

func _process(delta: float) -> void:
    sim.poll_input()
    
    _frame_accumulator += delta
    while (_frame_accumulator > _sim_time_per_frame):
        sim.update()
        _frame_accumulator -= _sim_time_per_frame

func shape_to_color_index(shape: int) -> Cell.ColorIndex:
    match shape:
        0, 3, 6:
            return Cell.ColorIndex.SHAPE_0
        1, 4:
            return Cell.ColorIndex.SHAPE_1
        2, 5:
            return Cell.ColorIndex.SHAPE_2
        _:
            return Cell.ColorIndex.BACKGROUND
