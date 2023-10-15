@echo off

if "%~1" == "" set exe_path="godot/bin/godot.windows.editor.x86_64.exe"
if "%~1" == "editor" set exe_path="godot/bin/godot.windows.editor.x86_64.exe"
if "%~1" == "editor_debug" set exe_path="godot/bin/godot.windows.dev.editor.x86_64.exe"

%exe_path% -e --path project_godot