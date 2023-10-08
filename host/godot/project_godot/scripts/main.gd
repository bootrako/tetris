class_name Main
extends Node2D

@onready var _tetris: Tetris = Tetris.new()
@onready var _matrix: CellGrid = $matrix
@onready var _tetronimo: CellGrid = $tetronimo

func _ready():
    var tetronimo_max_width = _tetris.get_tetronimo_max_width()
    var tetronimo_max_height = _tetris.get_tetronimo_max_height()
    _tetronimo.init(tetronimo_max_width, tetronimo_max_height)
    
    var matrix_width = _tetris.get_matrix_width()
    var matrix_height = _tetris.get_matrix_height()
    _matrix.init(matrix_width, matrix_height)
