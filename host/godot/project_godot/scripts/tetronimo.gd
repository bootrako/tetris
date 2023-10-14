class_name Tetronimo
extends CellGrid

@export var _tetris: Tetris
@export var _matrix: Matrix

@onready var _tetronimo_max_width: int = _tetris.sim.get_tetronimo_max_width()
@onready var _tetronimo_max_height: int = _tetris.sim.get_tetronimo_max_height()

func _ready() -> void:
    init_grid(_tetronimo_max_width, _tetronimo_max_height)
    _process_tetronimo()

func _process(_delta) -> void:
    if (_tetris.tetronimo_spawned || _tetris.tetronimo_moved):
        _process_tetronimo()
    
func _process_tetronimo() -> void:
    var color_index := _tetris.shape_to_color_index(_tetris.sim.get_tetronimo_shape())
    var level := _tetris.sim.get_level()
    
    for y in _tetronimo_max_height:
        for x in _tetronimo_max_width:
            var cell := get_cell(x, y)
            if _tetris.sim.get_tetronimo_cell(x, y):
                cell.set_color_index(color_index, level)
            else:
                cell.set_color_index(Cell.ColorIndex.INVISIBLE, level)
                
    var pos_x := _tetris.sim.get_tetronimo_pos_x()
    var pos_y := _tetris.sim.get_tetronimo_pos_y()
    position = _matrix.position + (Vector2(pos_x, pos_y) * cell_size)
