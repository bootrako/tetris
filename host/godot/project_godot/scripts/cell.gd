class_name Cell
extends Sprite2D

func set_active(is_active: bool):
    visible = is_active
    
func is_active():
    return visible
