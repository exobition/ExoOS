all: clean buildOS

buildOS:
	nasm src/Bootloader/stage1.asm -f bin -o build/Bootloader/stage1.bin
	nasm src/Bootloader/stage2.asm -f bin -o build/Bootloader/stage2.bin
	nasm src/Kernel/kernelstub.asm -f elf32 -o build/Kernel/Object/kernelstub.o
	gcc -c src/Kernel/kernel.c -o build/Kernel/Object/kernel.o -m32 -ffreestanding -fno-pic -fno-stack-protector -O2 -Wall -Wextra
	ld -m elf_i386 -T linker.ld --oformat binary -o build/Kernel/kernel.bin build/Kernel/Object/kernelstub.o build/Kernel/Object/kernel.o
	dd if=/dev/zero of=build/image.img bs=512 count=2880
	# mkfs.fat -F 12 -n "ExoOS" build/image.img  <-- USELESS!!!
	cat build/Bootloader/stage1.bin build/Bootloader/stage2.bin > build/Bootloader/full_bootloader.bin
	cat build/Bootloader/full_bootloader.bin build/Kernel/kernel.bin > build/os_flat.bin
	dd if=build/os_flat.bin of=build/image.img bs=512 conv=notrunc
	rm build/Bootloader/stage1.bin build/Bootloader/stage2.bin build/Bootloader/full_bootloader.bin
	qemu-img create -f qcow2 build/ExoDisk.qcow2 256M

clean:
	rm -rf build/*
	mkdir build/Bootloader
	mkdir -p build/Kernel/Object

boot_drive:
	qemu-system-i386 -m 512M -enable-kvm -hda build/ExoDisk.qcow2 -fda build/image.img -serial stdio

boot_floppy:
	qemu-system-i386 -fda  build/image.img -serial stdio

boot_cd:
	qemu-system-i386 -cdrom build/cd.iso -serial stdio
