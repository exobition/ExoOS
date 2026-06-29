# ExoOS - An operating system project

Its a hobby project so dont expect it to be insanely proficent, optimized, totally bug free, and be up to todays standards. (Or even expect it to be finished cuz I lose motivation easily sorry)

DISCLAMER:

Currently the iso does not work but i am still working on a way to make a bootable iso due to it being a pain in the ass. (nevermind it works now, just havent uploaded the new code n' stuff)

## Prerequisites

1. MUST have the 'nasm' package installed
2. MUST have the 'make' package installed
3. MUST have the 'qemu-desktop' package installed
4. MUST have the 'dosfstools' package installed
5. MUST have the 'coreutils' package installed (Usually installed on linux by default)
6. MUST have the 'cdrtools' package installed
7. IF you desire to edit the code, have a text editor, any will do.

### for arch linux
```bash
sudo pacman -S --needed nasm make qemu-desktop dosfstools coreutils cdrtools
```
### for debian-based systems
```bash
sudo apt update && sudo apt install -y nasm make qemu-system-x86 qemu-utils dosfstools coreutils genisoimage
```
### for fedora/REHL/CentOS
```bash
sudo dnf install -y nasm make qemu-system-x86 qemu-img dosfstools coreutils cdrtools
```
### for OpenSUSE
```bash
sudo zypper install -y nasm make qemu-x86 qemu-tools dosfstools coreutils cdrtools
```

### for windows
install wsl and a distro (I recommend ubuntu) and follow the corresponding tutorial

### for mac
¯\ _(ツ)_/¯

## Compiling

1. make sure you are in the root of the project directory
2. run code below
```bash
make buildOS
```

## Booting

 1. make sure you are in the root of the project directory
 2. run code below

### for booting DRIVE
```bash
make boot_drive
```
### for booting FLOPPY
```bash
make boot_floppy
```
### for booting CD (isnt working yet.)
```bash
make boot_cd
```

## Clearing build DIR

1. make sure you are in the root of the project directory
2. run code below

```bash
make clean
```

## Minimum specs

CPU: 32-bit or 64-bit
GPU: none
RAM: 8mb
STORAGE: >2kb

## Recommended specs

>potato

Image of development as of 17 Jan 2026 18:56

![current development lul XD](Screenshot_20260117_184304.png)
