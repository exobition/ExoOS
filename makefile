OPEN:
	nvim ~/Documents/Projects/src/main.asm

BUILD:
	nasm ~/Documents/Projects/src/main.asm -f bin -o ~/Documents/Projects/build/main.bin
	cp ~/Documents/Projects/build/main.bin ~/Documents/Projects/build/image.img

CLEAN:
	rm -rf ~/Documents/Projects/build/*

BOOT:
	qemu-system-i386 -hda ~/Documents/Projects/build/image.img
