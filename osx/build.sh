set -e
trap 'echo "Returned $? <<< ${BASH_COMMAND}"' EXIT

env MACOSX_DEPLOYMENT_TARGET=10.9

NAME="vibrant"

cc -ggdb -g -c -I /opt/raylib/src osx_platform.c 
cc -g -std=c99 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo /opt/raylib/release/libs/osx/libraylib.a -o project osx_platform.o 
./project
#objdump -no-show-raw-insn -S -D ./osx/osx_$NAME.o > ./osx/asm_$NAME.asm

