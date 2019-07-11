set -e
cc generate_asm.c -o exec_generate_asm
./exec_generate_asm 
nasm -fmacho64 ./image_binary_data.asm -o ../image_binary_data.o
echo "done"
