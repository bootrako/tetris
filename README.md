# Tetris

## Summary

This project is a SNES Tetris clone.

There are three separate subprojects:

1. Tetris simulation project, which is game engine / renderer agnostic.
2. Console host project, which runs the Tetris simulation on the command line.
3. Godot host project, which runs the Tetris simulation in a Godot game.

Read my blog posts explaining why I structured the project this way and what I thought of using Godot:
* [decouple game from engine](https://bootrako.github.io/decouple-game-from-engine/)
* [godot first impressions](https://bootrako.github.io/godot-first-impressions/)

## Building

### Console Host

* Make sure you have [clang](https://clang.llvm.org/) installed. You'll want to use [MinGW](https://www.mingw-w64.org/) if you're on windows.
* Run `build.sh` in a bash shell.

### Godot Host

* Make sure you have installed the prerequisites for [building Godot from source](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html).
* Run `build.cmd` in Windows command prompt to build the editor.
* Run `run.cmd` in Windows command prompt to open the project in the Godot editor.

## Attribution

I used [this Nintendo Tetris AI blogpost](https://meatfighter.com/nintendotetrisai/) for reference on SNES Tetris simulation behavior and sprites.

I got the SFX and music from [Zophar's Domain](https://www.zophar.net/music/nintendo-nes-nsf/tetris-1989-Nintendo).