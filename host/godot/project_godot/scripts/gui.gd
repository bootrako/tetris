class_name GUI
extends Control

@export var _tetris: Tetris
@export var _lines_label: Label
@export var _top_score_label: Label
@export var _score_label: Label
@export var _level_label: Label
@export var _next_tetronimo: CellGrid
@export var _stats_num: VBoxContainer
@export var _stats_shape: Control
@export var _game_over_panel: Panel

@onready var _cur_level: int = _tetris.sim.get_level()
@onready var _tetronimo_max_width: int = _tetris.sim.get_tetronimo_max_width()
@onready var _tetronimo_max_height: int = _tetris.sim.get_tetronimo_max_height()
@onready var _next_tetronimo_init_pos: Vector2 = _next_tetronimo.position
@onready var _spawned_num_per_shape: Array[int] = []

func _ready() -> void:
    _next_tetronimo.init_grid(_tetronimo_max_width, _tetronimo_max_height)
    for index in _stats_num.get_children():
        _spawned_num_per_shape.append(0)
    
    _set_next_tetronimo(_cur_level)
    _set_spawned_stats()
    _set_shapes_for_stats(_cur_level)
    _load_top_score()

func _process(_delta: float) -> void:
    if _tetris.tetronimo_spawned:
        var lines := _tetris.sim.get_lines()
        var score := _tetris.sim.get_score()
        var level := _tetris.sim.get_level()
        _set_label_int(_lines_label, lines)
        _set_label_int(_score_label, score)
        _set_label_int(_level_label, level)
        _set_next_tetronimo(level)
        _set_spawned_stats()
        if level != _cur_level:
            _set_shapes_for_stats(level)
            _cur_level = level
    
    if _tetris.sim.is_game_over() && !_game_over_panel.visible:
        _save_top_score()
        _game_over_panel.visible = true
    
func _set_next_tetronimo(level: int) -> void:
    var shape := _tetris.sim.get_next_tetronimo_shape()
    var color_index := _tetris.shape_to_color_index(shape)
    for y in _tetronimo_max_height:
        for x in _tetronimo_max_width:
            var cell := _next_tetronimo.get_cell(x, y)
            if _tetris.sim.get_next_tetronimo_cell(x, y):
                cell.set_color_index(color_index, level)
            else:
                cell.set_color_index(Cell.ColorIndex.INVISIBLE, level)
    
    _next_tetronimo.position = _get_next_tetronimo_offset(shape)

func _set_spawned_stats() -> void:
    var shape := _tetris.sim.get_tetronimo_shape()
    _spawned_num_per_shape[shape] += 1
    _set_label_int(_stats_num.get_children()[shape] as Label, _spawned_num_per_shape[shape])
    
func _set_shapes_for_stats(level: int):
    for stat_shape in _stats_shape.get_children():
        for stat_shape_child in stat_shape.get_children():
            var cell_stat := stat_shape_child as CellStat
            cell_stat.set_level(level)

func _set_label_int(label: Label, value: int) -> void:
    label.text = str(value).pad_zeros(label.text.length())

func _get_next_tetronimo_offset(shape: int) -> Vector2:
    if shape == 3 || shape == 6:
        return _next_tetronimo_init_pos + Vector2(4, 0)
    else:
        return _next_tetronimo_init_pos

func _load_top_score() -> void:
    if !FileAccess.file_exists("user://top_score.save"):
        return
    var save_file := FileAccess.open("user://top_score.save", FileAccess.READ)
    _top_score_label.text = save_file.get_line()

func _save_top_score() -> void:
    if _score_label.text <= _top_score_label.text:
        return
    var save_file := FileAccess.open("user://top_score.save", FileAccess.WRITE)
    save_file.store_line(_score_label.text)
