set -e
cc generate_asm.c -o exec_generate_asm
./exec_generate_asm 
cc generate_sound_binary_vars.c -o exec_generate_sound_binary_vars
./exec_generate_sound_binary_vars > ../sound_binary_vars.h
cc generate_sound_asm.c -o exec_generate_sound_asm
./exec_generate_sound_asm > sound_binary_data.asm


nasm -fmacho64 ./image_binary_data.asm -o ../image_binary_data.o

nasm -fmacho64 ./sound_binary_data.asm -o ../sound_binary_data.o
echo "done"
