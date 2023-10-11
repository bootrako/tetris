class_name Cell
extends Sprite2D

enum ColorIndex { BACKGROUND, SHAPE_0, SHAPE_1, SHAPE_2 }

func set_color_index(color: ColorIndex):
    if color == ColorIndex.BACKGROUND:
        frame = 0
    else:
        frame = 1
        
