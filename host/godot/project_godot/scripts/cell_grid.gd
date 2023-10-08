class_name CellGrid
extends Node2D

@onready var grid_size: Vector2 = Vector2.ZERO
@onready var cell_size: Vector2 = Vector2.ZERO

@export var cell_scene: PackedScene 
@onready var _cells: Array = []

func init(width: int, height: int):
    _cells = []
    for y in height:
        _cells.push_back([])
        for x in width:
            _cells[y].push_back(_init_cell(x, y))
    cell_size = _cells[0][0].texture.get_size()
    grid_size = Vector2(_cells[0].size(), _cells.size()) * cell_size

func _init_cell(x: int, y: int):
    var cell = cell_scene.instantiate() as Cell
    add_child(cell)
    cell.position = Vector2(x, y) * cell.texture.get_size()
    cell.set_active(false)
    return cell
    
func set_cell_active(x: int, y: int, is_active: bool):
    _cells[y][x].set_active(is_active)

func copy_cell(dst_x: int, dst_y: int, src_x: int, src_y: int):
    var dst_cell = _cells[dst_y][dst_x] as Cell
    var src_cell = _cells[src_y][src_x] as Cell
    dst_cell.set_active(src_cell.is_active())
