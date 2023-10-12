class_name Cell
extends Sprite2D

enum ColorIndex { INVISIBLE, BACKGROUND, SHAPE_0, SHAPE_1, SHAPE_2 }

func set_color_index(color_index: ColorIndex, level: int) -> void:
    if color_index == ColorIndex.INVISIBLE || color_index == ColorIndex.BACKGROUND:
        frame = color_index
    else:
        var level_index := level % 10
        var shape_index := color_index - 1
        frame = (level_index * 3) + shape_index + 1
