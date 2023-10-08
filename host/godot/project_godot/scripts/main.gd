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
    _tetronimo.init(_tetronimo_max_width, _tetronimo_max_height)
    _matrix.init(_matrix_width, _matrix_height)
    _camera.position += _matrix.grid_size / 2
    
    _update_tetronimo()

func _process(delta):
    _tetris.poll_input()

    _frame_accumulator += delta
    while (_frame_accumulator > _sim_time_per_frame):
        _tetris.update_sim()
        _frame_accumulator -= _sim_time_per_frame
        
    _update_tetronimo()
    
func _update_tetronimo():
    for y in _tetronimo_max_height:
        for x in _tetronimo_max_width:
            _tetronimo.set_cell_active(x, y, _tetris.get_tetronimo_value(x, y))
    var pos_x = _tetris.get_tetronimo_pos_x()
    var pos_y = _tetris.get_tetronimo_pos_y()
    _tetronimo.position = Vector2(pos_x, pos_y) * _tetronimo.cell_size
    


