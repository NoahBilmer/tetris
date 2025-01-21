REM Note that the PLATFORM_WEB #define in globals.h should be uncommented in
REM game to work properly.
echo Compiling for web...
emcc -o tetris.html -s EXPORTED_FUNCTIONS=["_free","_malloc","_main"] -s EXPORTED_RUNTIME_METHODS=ccall ^
game\src\main.c game\src\board.c game\src\state.c game\src\draw.c ^
-Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result ^
-Os -I. -I C:\raylib\raylib\src -I C:\raylib\raylib\src\external -L. ^
-L C:\raylib\raylib\src -s USE_GLFW=3 -s ASYNCIFY -s ASSERTIONS=1 ^
 -s FORCE_FILESYSTEM=1 --preload-file resources\ ^
 --shell-file C:\raylib\raylib\src\shell.html ^
C:\raylib\raylib\src\web\libraylib.a -DPLATFORM_WEB ^

