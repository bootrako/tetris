if [ -z "$1" ]; then
    exe=godot/bin/godot.windows.editor.x86_64.exe
elif [ "$1" == "editor" ]; then
    exe=godot/bin/godot.windows.editor.x86_64.exe
elif [ "$1" == "editor_debug" ]; then
    exe=godot/bin/godot.windows.editor.dev.x86_64.exe
else
    echo "invalid argument $1"
    exit 1
fi

./$exe -e --path project_godot