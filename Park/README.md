# OpenGL Neighborhood Park Scene

Computer Graphics: Assignment 2

Semester 2, 2021

Author: Lakshan Martin

Submission: 18th October 2021

## Purpose

This application is a demonstration of how a variety of geometric transformations, lighting and texture usage can be used through OpenGL to create animated computer graphic scenes. 

## Required installations

Compilation and execution of this application requires the following installations:

- gcc
- cmake
- g++
- libglfw3
- libglfw3-dev
- libassimp-dev
- libxinerama-dev
- libxi-dev
- libxcursor-dev
- libxxf86vm-dev

## How to compile & run

In order to compile the application, please enter the following commands from the project root directory:

```bash
cd build/
cmake..
make
```

To execute the application, please enter the following commands from the ```build``` directory:

```bash
cd bin/assignment/
./assignment__park
```

## Controls

Upon executing the application, the scene will be display within a window. The camera angle can be controlled with mouse movements. 

### Keyboard movement

```W``` - Forwards

```A``` - Left strafe

```S``` - Backwards

```D``` - Right strafe

```SPACEBAR``` - Up

```X``` - Down

```LSHIFT``` - Double movement speed

### Keyboard actions

```F``` - Stop light source from following camera position and remain stationary. Press again to follow camera position. 

```K``` - Reduce brightness of light source

```L``` - Increase brightness of light source

```P``` - Toggle camera between perspective and orthographic projection

```R``` - Play 2nd animation. Press again to reset back to 1st animation

```O``` - Toggle between a brightening and darkening of the scene





