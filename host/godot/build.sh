if [ -z "$1" ]; then
    profile=editor
elif [ "$1" == "editor" ]; then
    profile=editor
elif [ "$1" == "editor_debug" ]; then
    profile=editor_debug
else
    echo "invalid argument $1"
    exit 1
fi

pushd godot

scons profile=../$profile

popd