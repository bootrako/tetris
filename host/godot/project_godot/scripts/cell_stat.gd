class_name CellStat
extends Sprite2D

@onready var _init_frame := frame
@onready var _level_count := hframes / Cell.num_colors

func set_level(level: int) -> void:
    frame = _init_frame + ((level % _level_count) * Cell.num_colors)
