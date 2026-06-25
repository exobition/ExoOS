build:
	nasm src/main.asm -f bin -o build/main.bin 
	dd if=/dev/zero of=build/image.img bs=512 count=2880
	dd if=build/main.bin of=build/image.img bs=512 count=1 conv=notrunc
	mkfs.fat -F 12 -n "ExoOS" /build/image.img
	qemu-img create -f qcow2 build/ExoDisk.qcow2 256M
	mkisofs \
  -o build/cd.iso \
  -b main.bin \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  build

clean:
	sudo rm -rf ~/osdev/ExoOS/build/*

boot_drive:
	qemu-system-i386 -m 512M -enable-kvm -hda build/ExoDisk.qcow2 -fda build/image.img

boot_floppy:
	qemu-system-i386 -fda  build/image.img

boot_cd:
	qemu-system-i386 -cdrom build/cd.iso
