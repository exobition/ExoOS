OPEN:
	nvim src/main.asm

MAKE_QDRIVE:
	qemu-img create -f qcow2 build/ExoDisk.qcow2 256M

BUILD:
	nasm src/main.asm -f bin -o build/main.bin 
	dd if=/dev/zero of=build/image.img bs=512 count=2880
	dd if=build/main.bin of=build/image.img bs=512 count=1 conv=notrunc
	mkfs.fat -F 12 -n "ExoOS" /build/image.img
	mkisofs \
  -o build/cd.iso \
  -b main.bin \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  build

CLEAN:
	rm -rf ~/osdev/ExoOS/build/*

BOOT_DRIVE:
	qemu-system-i386 -m 512M -enable-kvm -hda build/ExoDisk.qcow2 -fda build/image.img

BOOT_FLOPPY:
	qemu-system-i386 -fda  build/image.img

BOOT_CD:
	qemu-system-i386 -cdrom build/cd.iso
