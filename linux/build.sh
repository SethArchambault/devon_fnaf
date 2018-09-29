set -e
trap 'echo "Returned $? <<< ${BASH_COMMAND}"' EXIT

cc -ggdb -g -std=c99 -c -I /opt/raylib/src linux_platform.c -o ./obj/linux_platform.o
gcc -o linux_platform  obj/linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP
./linux_platform

#cc -ggdb -g -std=c99 -c -I /opt/raylib/src linux_platform.c -o ./obj/linux_platform.o
#cc -g -std=c99 -I/usr/local/include -isystem. -isystem/opt/raylib/src -isystem/opt/raylib/release/incude -isystem/opt/raylib/src/external -L. -L/usr/local/lib -L /opt/raylib/release/libs/linux -L/opt/raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o project ./obj/linux_platform.o 
#gcc -o core/core_input_mouse core/core_input_mouse.c -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I/usr/local/include -isystem. -isystem/opt/raylib/src -isystem/opt/raylib/release/include -isystem/opt/raylib/src/external -L. -L/usr/local/lib -L/opt/raylib/release/libs/linux -L/opt/raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP


#gcc -c linux_platform.c -o linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -DPLATFORM_DESKTOP
#gcc -o linux_platform  linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP
#make linux_platform make[1]: Entering directory '/home/pi/seth/devon_fnaf/linux' gcc -o linux_platform  linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP
#gcc -o linux_platform  linux_platform.o -O1 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -D_DEFAULT_SOURCE -I. -I/opt/raylib/release/include -I/opt/raylib/src -I/opt/raylib/src/external -L. -L/opt/raylib/release/libs/linux -L/opt/raylib/src  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP
