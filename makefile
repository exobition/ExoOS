all: clean buildOS

buildOS:
	nasm src/stage1.asm -f bin -o build/stage1.bin
	nasm src/stage2.asm -f bin -o build/stage2.bin
	nasm src/kernel.asm -f bin -o build/kernel.bin
	dd if=/dev/zero of=build/image.img bs=512 count=2880
	mkfs.fat -F 12 -n "ExoOS" build/image.img
	cat build/stage1.bin build/stage2.bin build/kernel.bin > build/os_flat.bin
	dd if=build/os_flat.bin of=build/image.img bs=512 conv=notrunc
	rm build/stage1.bin build/stage2.bin build/kernel.bin build/os_flat.bin
	qemu-img create -f qcow2 build/ExoDisk.qcow2 256M
	cp build/image.img build/iso/
	mkisofs -o build/cd.iso -b image.img -R -J build/iso/

clean:
	rm -rf build/*
	mkdir build/iso

boot_drive:
	qemu-system-i386 -m 512M -enable-kvm -hda build/ExoDisk.qcow2 -fda build/image.img

boot_floppy:
	qemu-system-i386 -fda  build/image.img

boot_cd:
	qemu-system-i386 -cdrom build/cd.iso
