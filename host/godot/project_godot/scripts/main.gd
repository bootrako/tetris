class_name Main
extends Node2D

@onready var _tetris: Tetris = Tetris.new()
@onready var _matrix: CellGrid = $matrix
@onready var _tetronimo: CellGrid = $tetronimo
@onready var _camera: Camera2D = $camera
@onready var _matrix_width = _tetris.get_matrix_width()
@onready var _matrix_height = _tetris.get_matrix_height()
@onready var _tetronimo_max_width = _tetris.get_tetronimo_max_width()
@onready var _tetronimo_max_height = _tetris.get_tetronimo_max_height()
@onready var _sim_time_per_frame = _tetris.get_sim_time_per_frame()
@onready var _frame_accumulator: float = 0

func _ready():
    _tetronimo.init(_tetronimo_max_width, _tetronimo_max_height, false)
    _matrix.init(_matrix_width, _matrix_height, true, Cell.ColorIndex.BACKGROUND)
    _camera.position += _matrix.grid_size / 2

func _process(delta):
    _tetris.poll_input()

    _frame_accumulator += delta
    while (_frame_accumulator > _sim_time_per_frame):
        _tetris.update_sim()
        _frame_accumulator -= _sim_time_per_frame
        
    _update_tetronimo()
    _update_matrix()
    
func _update_tetronimo():
    for y in _tetronimo_max_height:
        for x in _tetronimo_max_width:
            _tetronimo.get_cell(x, y).visible = _tetris.get_tetronimo_cell(x, y) 
            _tetronimo.get_cell(x, y).set_color_index(_shape_to_color_index(_tetris.get_tetronimo_shape()))
    var pos_x = _tetris.get_tetronimo_pos_x()
    var pos_y = _tetris.get_tetronimo_pos_y()
    _tetronimo.position = Vector2(pos_x, pos_y) * _tetronimo.cell_size

func _update_matrix():
    for y in _matrix_height:
        for x in _matrix_width:
            if (!_tetris.get_matrix_cell(x, y)):
                _matrix.get_cell(x, y).set_color_index(Cell.ColorIndex.BACKGROUND)
            else:
                _matrix.get_cell(x, y).set_color_index(_shape_to_color_index(_tetris.get_matrix_cell_shape(x, y)))

func _shape_to_color_index(shape: int):
    if shape == 0 || shape == 3 || shape == 6:
        return Cell.ColorIndex.SHAPE_0
    elif shape == 1 || shape == 4:
        return Cell.ColorIndex.SHAPE_1
    elif shape == 2 || shape == 5:
        return Cell.ColorIndex.SHAPE_2
    else:
        return Cell.ColorIndex.BACKGROUND
