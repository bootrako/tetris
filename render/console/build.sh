mkdir -p obj
mkdir -p lib
mkdir -p bin

# tetris sim (static lib)
tetris_sim_srcs=(tetris_ctx.c tetris_events.c tetris_matrix.c tetris_sim.c tetris_tetronimo.c tetris_utils.c)
for src in "${tetris_sim_srcs[@]}"
do
    tetris_sim_objs="$tetris_sim_objs obj/$src.o"
    if [ "$1" == "release" ]; then
        clang -O3 -DNDEBUG -c -o obj/$src.o ../../sim/src/$src -Wall -Werror -I../../sim/include
    else
        clang -g -c -o obj/$src.o ../../sim/src/$src -Wall -Werror -I../../sim/include
    fi
done
ar r lib/tetris_sim.lib $tetris_sim_objs

# console app (exe)
console_render_srcs=(main.c)
if [ "$1" == "release" ]; then
    clang -O3 -DNDEBUG ${console_render_srcs[@]} -o bin/tetris_console.exe -Llib -ltetris_sim -I../../sim/include
else
    clang -g ${console_render_srcs[@]} -o bin/tetris_console.exe -Llib -ltetris_sim -I../../sim/include
fi