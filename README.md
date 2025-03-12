# InterstellarAssault

[![Build](https://github.com/robcicle/InterstellarAssault/actions/workflows/build.yml/badge.svg)](https://github.com/robcicle/InterstellarAssault/actions/workflows/build.yml)

**Object-Orientated C++ Programming DirectX11 Game Project**

A fresh take on the original concept of SpaceInvaders whilst incorporating new approaches.
Now being used as the main project for 3D Game Development LUA Scripting Project.
Also upgraded to use CMake.

**The project was originally part of a separate GitHub repository and has since been moved to its own.**
**Hence why ALOT of commits are missing, these current ones relate to the LUA Scripting Project and CMake upgrade.**

## Features

- 2D and 3D graphics rendering using DirectXTK.
- Model importing via assimp.
- Scripting functionality using Lua.
- Custom game logic and mechanics related to Space Invaders.

## Building the Project

### Prerequisites

- CMake 3.28 or higher
- A C++ compiler (e.g., GCC, Clang, MSVC)
- DirectXTK Jun2022 (fetched in CMake solution generation)
- Assimp 5.2.5 (fetched in CMake solution generation)
- Lua 5.4.4 (fetched in CMake solution generation)

### Building with CMake

1. Clone the repository and initialize submodules:

    ```sh
    git clone https://github.com/robcicle/InterstellarAssault.git
    cd InterstellarAssault
    ```

2. Create a build directory and navigate to it:

    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:

    ```sh
    cmake ..
    ```

4. Build the project:

    ```sh
    cmake --build .
    ```

5. Run the executable:

    ```sh
    ./bin/Debug/InterstellarAssault.exe
    ```
