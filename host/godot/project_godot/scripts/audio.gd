class_name Audio
extends Node2D

const _fast_music_threshold: int = 4

@export var _tetris: Tetris
@export var _music_options: Array[AudioStream]
@export var _music_options_fast: Array[AudioStream]
@export var _music: AudioStreamPlayer2D
@export var _music_fast: AudioStreamPlayer2D
@export var _locked: AudioStreamPlayer2D
@export var _clear: AudioStreamPlayer2D
@export var _clear4: AudioStreamPlayer2D
@export var _gameover: AudioStreamPlayer2D
@export var _levelup: AudioStreamPlayer2D
@export var _move: AudioStreamPlayer2D
@export var _rotate: AudioStreamPlayer2D

@onready var _is_gameover: bool = false
@onready var _level: int = 0
@onready var _matrix_width: int = _tetris.sim.get_matrix_width()
@onready var _music_index: int = randi() % _music_options.size()
@onready var _fast_music: bool = false

func _ready() -> void:
    _music.stream = _music_options[_music_index]
    _music_fast.stream = _music_options_fast[_music_index]
    _set_music(_fast_music)

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
        
        var is_above_fast_threshold := false
        for x in _matrix_width:
            if _tetris.sim.get_matrix_cell(x, _fast_music_threshold):
                is_above_fast_threshold = true
                break
        if is_above_fast_threshold != _fast_music:
            _set_music(is_above_fast_threshold)
    
    if _tetris.tetronimo_moved_horiz:
        _move.play()
    
    if _tetris.tetronimo_rotated:
        _rotate.play()
    
    if !_is_gameover && _tetris.sim.is_game_over():
        _gameover.play()
        _music.stop()
        _music_fast.stop()
        _is_gameover = true

func _set_music(fast: bool) -> void:
    if fast:
        _music.stop()
        _music_fast.play()
    else:
        _music_fast.stop()
        _music.play()
    _fast_music = fast
