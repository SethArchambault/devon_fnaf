set -e
trap 'echo "Returned $? <<< ${BASH_COMMAND}"' EXIT

env MACOSX_DEPLOYMENT_TARGET=10.9


NAME="vibrant"

cc -ggdb -g -Werror -std=c99 -c -I/usr/local/opt/openal-soft/include -I /opt/raylib/src osx_platform.c 
cc -g -std=c99 -L/usr/local/opt/openal-soft/lib -lopenal -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo /opt/raylib/release/libs/osx/libraylib.a -o project osx_platform.o 
#objdump -no-show-raw-insn -S -D ./osx/osx_$NAME.o > ./osx/asm_$NAME.asm

