class_name Cell
extends Sprite2D

enum ColorIndex { INVISIBLE, BACKGROUND, SHAPE_START, SHAPE_0 = SHAPE_START, SHAPE_1, SHAPE_2, COUNT }
const num_colors: int = ColorIndex.COUNT - ColorIndex.SHAPE_START

@onready var _num_levels: int = (hframes - ColorIndex.SHAPE_START) / num_colors

func set_color_index(color_index: ColorIndex, level: int) -> void:
    if color_index == ColorIndex.INVISIBLE || color_index == ColorIndex.BACKGROUND:
        frame = color_index
    else:
        frame = color_index + ((level % _num_levels) * num_colors)
