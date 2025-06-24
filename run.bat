nasm boot.asm -f bin -o bin/boot.bin
nasm kernEntry.asm -f elf -o bin/kernEntry.o
i686-elf-gcc -ffreestanding -m32 -g -Iincludes -c kernel.c -o bin/kernel.o
i686-elf-gcc -ffreestanding -m32 -g -Iincludes -c inoutlib.c -o bin/inoutlib.o
nasm zeroes.asm -f bin -o bin/zeroes.bin

i686-elf-ld -o bin/kernel.bin -Ttext 0x1000 bin/kernEntry.o bin/kernel.o bin/inoutlib.o --oformat binary

cd bin

copy /b boot.bin + kernel.bin + zeroes.bin fullKernel.bin

qemu-system-i386.exe fullKernel.bin
