class_name Matrix
extends CellGrid

@export var _tetris: Tetris

@onready var _matrix_width: int = _tetris.sim.get_matrix_width()
@onready var _matrix_height: int = _tetris.sim.get_matrix_height()

func _ready():
    init_grid(_matrix_width, _matrix_height)
    position -= grid_size / 2

func _process(_delta):
    var level := _tetris.sim.get_level()
    for y in _matrix_height:
        for x in _matrix_width:
            var cell := get_cell(x, y)
            if (_tetris.sim.get_matrix_cell(x, y)):
                var color_index := _tetris.shape_to_color_index(_tetris.sim.get_matrix_cell_shape(x, y))
                cell.set_color_index(color_index, level)
            else:
                cell.set_color_index(Cell.ColorIndex.BACKGROUND, level)
