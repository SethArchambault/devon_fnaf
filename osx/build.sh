set -e
cc -Werror -dynamiclib -o libproject2.dylib project2.c
./full_build.sh

