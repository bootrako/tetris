class_name CellGrid
extends Node2D

@onready var grid_size: Vector2 = Vector2.ZERO
@onready var cell_size: Vector2 = Vector2.ZERO

@export var cell_scene: PackedScene 
@onready var _cells: Array = []

func init(width: int, height: int) -> void:
    var cell := cell_scene.instantiate() as Cell
    cell_size = cell.texture.get_size() / Vector2(cell.hframes, cell.vframes)
    
    _cells = []
    for y in height:
        var cell_row := [] as Array[Cell]
        for x in width:
            cell_row.push_back(_init_cell(x, y))
        _cells.push_back(cell_row)
    
    var cell_row := _cells[0] as Array[Cell]
    grid_size = Vector2(cell_row.size(), _cells.size()) * cell_size    

func _init_cell(x: int, y: int) -> Cell:
    var cell := cell_scene.instantiate() as Cell
    cell.position = Vector2(x, y) * cell_size
    add_child(cell)
    return cell

func get_cell(x: int, y: int) -> Cell:
    return _cells[y][x] as Cell
