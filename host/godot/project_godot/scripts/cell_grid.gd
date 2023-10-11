class_name CellGrid
extends Node2D

@onready var grid_size: Vector2 = Vector2.ZERO
@onready var cell_size: Vector2 = Vector2.ZERO

@export var cell_scene: PackedScene 
@onready var _cells: Array = []

func init(width: int, height: int, init_visible: bool = true, init_color_index: Cell.ColorIndex = Cell.ColorIndex.BACKGROUND):
    var cell = cell_scene.instantiate() as Cell
    cell_size = cell.texture.get_size() / Vector2(cell.hframes, cell.vframes)
    
    _cells = []
    for y in height:
        _cells.push_back([])
        for x in width:
            _cells[y].push_back(_init_cell(x, y, init_visible, init_color_index))
            
    grid_size = Vector2(_cells[0].size(), _cells.size()) * cell_size    

func _init_cell(x: int, y: int, init_visible: bool, init_color_index: Cell.ColorIndex):
    var cell = cell_scene.instantiate() as Cell
    add_child(cell)
    cell.position = Vector2(x, y) * cell_size
    cell.visible = init_visible
    cell.set_color_index(init_color_index)
    return cell

func get_cell(x: int, y: int):
    return _cells[y][x]
