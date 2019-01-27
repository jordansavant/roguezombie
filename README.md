# Rogue Zombie

A fully networked, multiplayer roguelike game built in C++ using the SFML library.

It has been a perpetual work in progress for several years when the inspiration hits me.

There are several wrapper build repositories for building this game on Mac, Windows and Linux.

<img src="https://media.giphy.com/media/1Bek1Q5vb7tWHlsvEp/giphy.gif" width="500" />

## Synopsis

You awaken as an experimentation of Hades Corp (C) gone awry, now some sort of undead being you must find your way out of Hades Corp into the world above.

Demon spawn, undead, menacing security, traps and mazelike puzzles face your escape.

## Game Elements

Hades Corp tower is a procedurally generated series of connected dungeons using the XoGeni codebase.

Enemies have various levels of functionally assigned AI routines to dynamically adjust enemy detection, intelligent pathfinding and combat decisions.

Outside of combat the world proceeds in free time, where you can navigate the level at any pace. Once combat is detected a queue of combat begins.

The game supports networked multiplayer games but has not been optimized in some time.

## Building

This repository contains the cross-platform compatible C++ used to build this game on most sytems.

There are four additional repositories that contain build details for these environments specifically:

- Visual Studio 2017: https://github.com/jordansavant/roguezombie.windows_visualcpp17_x86
- Visual Studio 2010: https://github.com/jordansavant/roguezombie.windows_visualcpp10_x86
- Mac OS X: https://github.com/jordansavant/roguezombie.macosx10.8_clang_xuni
- Linux: https://github.com/jordansavant/roguezombie.linux_gcc_x64

The game uses SFML, the Open GL library so use its installation guides as additional reference:

- https://www.sfml-dev.org

## Media

https://www.youtube.com/watch?v=5urZK-CNOFo

<img src="https://media.giphy.com/media/1Bek1Q5vb7tWHlsvEp/giphy.gif" width="500" />

<img src="https://media.giphy.com/media/5UudDFsjKINvU2nK9s/giphy.gif" width="500" />

<img src="https://media.giphy.com/media/QJsUHfH4Lie30R61Vz/giphy.gif" width="500" />

<img src="https://media.giphy.com/media/tZqWkDeFzmclavakLI/giphy.gif" width="500" />
