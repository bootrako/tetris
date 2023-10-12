class_name Main
extends Node2D

@onready var _matrix: CellGrid = $matrix as CellGrid
@onready var _tetronimo: CellGrid = $tetronimo as CellGrid
@onready var _camera: Camera2D = $camera as Camera2D

@onready var _tetris: Tetris = Tetris.new()
@onready var _matrix_width: int = _tetris.get_matrix_width()
@onready var _matrix_height: int = _tetris.get_matrix_height()
@onready var _tetronimo_max_width: int = _tetris.get_tetronimo_max_width()
@onready var _tetronimo_max_height: int = _tetris.get_tetronimo_max_height()
@onready var _sim_time_per_frame: float = _tetris.get_sim_time_per_frame()
@onready var _frame_accumulator: float = 0
@onready var _level: int = 0

func _ready() -> void:
    _tetronimo.init(_tetronimo_max_width, _tetronimo_max_height)
    _matrix.init(_matrix_width, _matrix_height)
    _camera.position += _matrix.grid_size / 2

func _process(delta: float) -> void:
    _tetris.poll_input()

    _frame_accumulator += delta
    while (_frame_accumulator > _sim_time_per_frame):
        _tetris.update_sim()
        _frame_accumulator -= _sim_time_per_frame
    
    _level = _tetris.get_level();
        
    _update_tetronimo()
    _update_matrix()
    
func _update_tetronimo() -> void:
    var color_index := _shape_to_color_index(_tetris.get_tetronimo_shape())
    for y in _tetronimo_max_height:
        for x in _tetronimo_max_width:
            var cell := _tetronimo.get_cell(x, y)
            if _tetris.get_tetronimo_cell(x, y):
                cell.set_color_index(color_index, _level)
            else:
                cell.set_color_index(Cell.ColorIndex.INVISIBLE, _level)
    var pos_x := _tetris.get_tetronimo_pos_x()
    var pos_y := _tetris.get_tetronimo_pos_y()
    _tetronimo.position = Vector2(pos_x, pos_y) * _tetronimo.cell_size
    
func _update_matrix() -> void:
    for y in _matrix_height:
        for x in _matrix_width:
            var cell : = _matrix.get_cell(x, y)
            if (_tetris.get_matrix_cell(x, y)):
                cell.set_color_index(_shape_to_color_index(_tetris.get_matrix_cell_shape(x, y)), _level)
            else:
                cell.set_color_index(Cell.ColorIndex.BACKGROUND, _level)

func _shape_to_color_index(shape: int) -> Cell.ColorIndex:
    match shape:
        0, 3, 6:
            return Cell.ColorIndex.SHAPE_0
        1, 4:
            return Cell.ColorIndex.SHAPE_1
        2, 5:
            return Cell.ColorIndex.SHAPE_2
        _:
            return Cell.ColorIndex.BACKGROUND
