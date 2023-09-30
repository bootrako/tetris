mkdir -p obj
mkdir -p lib
mkdir -p bin

# tetris sim (static lib)
tetris_sim_srcs=(tetris_matrix.c tetris_sim.c tetris_tetronimo.c tetris_utils.c)
for src in "${tetris_sim_srcs[@]}"
do
    tetris_sim_objs="$tetris_sim_objs obj/$src.o"
    clang -g -c -o obj/$src.o ../../sim/src/$src -Wall -Werror -I../../sim/include
done
ar r lib/tetris_sim.lib $tetris_sim_objs

# console app (exe)
clang -g main.c -o bin/tetris_console.exe -Llib -ltetris_sim -I../../sim/include