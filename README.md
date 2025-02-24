# Tetris
This is a tetris clone built in C using [raylib](https://www.raylib.com/), which is a library that provides a set of basic tools for game development. For this project I only used it's drawing and input polling functionality. You can play the game in your browser [here](https://noahbilmer.github.io/portfolio/tetris), or download the .exe in the releases section.

# Development Enviorment 

Refer to https://github.com/raysan5/raylib/wiki to setup the development enviorment with your desired IDE. 
 
# Compiling for Windows (using VSCode) 

- run `premake-mingw.bat` to setup the make file correctly (This only needs to be done if something about the project structure changes, i.e adding/deleting source files)
- run `make`
- run the executeable in _bin/Debug/tetris.exe

# Compiling to WebAssembly

- Enusre web assembly is setup correctly (You need to install emscripten as well as compile the library with some specific flags enabled, refer to https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5).)
- Run `compile_for_web.bat`

# Screenshot
![screenshot](https://github.com/user-attachments/assets/a929ed76-8c1d-46a1-aba4-b12b6ef62c7e)

