class_name Options
extends Node2D

@onready var _paused: bool = false
@onready var _muted: bool = false

func _process(_delta):
    if Input.is_action_just_pressed("restart"):
        get_tree().reload_current_scene()
    
    if Input.is_action_just_pressed("pause"):
        _paused = !_paused
        get_tree().paused = _paused
    
    if Input.is_action_just_pressed("mute"):
        _muted = !_muted
        AudioServer.set_bus_mute(0, _muted)
