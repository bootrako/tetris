@echo off

if "%~1" == "" set profile_path="../editor.py"
if "%~1" == "editor" set profile_path="../editor.py"
if "%~1" == "editor_debug" set profile_path="../editor_debug.py"
if "%~1" == "export" set profile_path="../export.py"

pushd godot
scons profile=%profile_path%
popd