set -e
cd asm
./build.sh
cd ..
gcc -Werror -I/usr/local/opt/openal-soft/include -I /opt/raylib/src -L/usr/local/opt/openal-soft/lib -lopenal -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo sound_binary_data.o image_binary_data.o /opt/raylib/release/libs/osx/libraylib.a -o three_nights_at_bennys_plaza osx_platform.c
#objdump -no-show-raw-insn -S -D ./osx/osx_$NAME.o > ./osx/asm_$NAME.asm
cp three_nights_at_bennys_plaza three_nights_at_bennys_plaza.app/Contents/MacOS/three_nights_at_bennys_plaza
./three_nights_at_bennys_plaza
