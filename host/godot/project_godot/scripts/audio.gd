class_name Audio
extends Node2D

@export var _tetris: Tetris
@export var _music_options: Array[AudioStream]
@export var _music: AudioStreamPlayer2D
@export var _locked: AudioStreamPlayer2D
@export var _clear: AudioStreamPlayer2D
@export var _clear4: AudioStreamPlayer2D
@export var _gameover: AudioStreamPlayer2D
@export var _levelup: AudioStreamPlayer2D
@export var _move: AudioStreamPlayer2D
@export var _rotate: AudioStreamPlayer2D

@onready var _is_gameover: bool = false
@onready var _level: int = 0

func _ready() -> void:
    _music.stream = _music_options.pick_random()
    _music.play()

func _process(_delta) -> void:
    if _tetris.tetronimo_locked:
        _locked.play()
    
    if !_tetris.matrix_rows_cleared.is_empty():
        if _tetris.matrix_rows_cleared.size() == 4:
            _clear4.play()
        else:
            _clear.play()
    
    if _tetris.tetronimo_spawned:
        var prev_level := _level
        _level = _tetris.sim.get_level()
        if _level != prev_level:
            _levelup.play()
    
    if _tetris.tetronimo_moved_horiz:
        _move.play()
    
    if _tetris.tetronimo_rotated:
        _rotate.play()
    
    if !_is_gameover && _tetris.sim.is_game_over():
        _gameover.play()
        _is_gameover = true
