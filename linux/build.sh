set -e
cc -g -std=c99 -c -I /opt/raylib/src linux_platform.c -o ./obj/linux_platform.o
cc -o linux_platform  obj/linux_platform.o -s -Wall -std=c99 -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
./linux_platform

#workds:cc -o linux_platform  obj/linux_platform.o -s -Wall -std=c99 -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
# works: gcc -o linux_platform  obj/linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP
