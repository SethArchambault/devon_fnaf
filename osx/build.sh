set -e
gcc -Werror -I/usr/local/opt/openal-soft/include -I /opt/raylib/src -L/usr/local/opt/openal-soft/lib -lopenal -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo /opt/raylib/release/libs/osx/libraylib.a -o project osx_platform.c
#objdump -no-show-raw-insn -S -D ./osx/osx_$NAME.o > ./osx/asm_$NAME.asm
./project
