class_name Matrix
extends CellGrid

@export var _tetris: Tetris

@onready var _matrix_width: int = _tetris.sim.get_matrix_width()
@onready var _matrix_height: int = _tetris.sim.get_matrix_height()

@onready var _is_animating_rows_cleared: bool = false
@onready var _rows_cleared_animation_frame: int = 0
@onready var _rows_cleared: PackedInt32Array = []

func _ready() -> void:
    init_grid(_matrix_width, _matrix_height)
    position -= grid_size / 2
    _set_matrix()

func _process(_delta) -> void:
    if !_tetris.matrix_rows_cleared.is_empty():
        _is_animating_rows_cleared = true
        _rows_cleared_animation_frame = 0
        _rows_cleared = _tetris.matrix_rows_cleared
         
    if _tetris.tetronimo_locked:
        _set_matrix()
        
    if _tetris.tetronimo_spawned:
        _is_animating_rows_cleared = false
        _set_matrix()
    
    if _is_animating_rows_cleared:
        _animate_rows_cleared()

func _set_matrix() -> void:
    var level := _tetris.sim.get_level()
    for y in _matrix_height:
        for x in _matrix_width:
            var cell := get_cell(x, y)
            if _tetris.sim.get_matrix_cell(x, y):
                var color_index := _tetris.shape_to_color_index(_tetris.sim.get_matrix_cell_shape(x, y))
                cell.set_color_index(color_index, level)
            else:
                cell.set_color_index(Cell.ColorIndex.BACKGROUND, level)

func _animate_rows_cleared() -> void:
    var half_width := _matrix_width / 2
    var rows_cleared_animation_index := _rows_cleared_animation_frame / 5
    if rows_cleared_animation_index >= half_width:
        _is_animating_rows_cleared = false
        return 
    var l_index := half_width - 1 - rows_cleared_animation_index
    var r_index := half_width + rows_cleared_animation_index
    var level := _tetris.sim.get_level()
    for row in _rows_cleared:
        var l_cell := get_cell(l_index, row)
        var r_cell := get_cell(r_index, row)
        l_cell.set_color_index(Cell.ColorIndex.BACKGROUND, level)
        r_cell.set_color_index(Cell.ColorIndex.BACKGROUND, level)
    _rows_cleared_animation_frame += _tetris.num_frames_simmed
