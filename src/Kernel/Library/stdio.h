#include <stdint.h>

#define VGAMemory 0xB8000
#define VGAMax 0xB9000

#define VGAWidth 80
#define VGAHeight 25

#define VGAColor 0x08

#define COM1_PORT 0x3F8

volatile uint8_t *video = (volatile uint8_t*)VGAMemory;
volatile uint8_t Nl = 0xA0;
volatile uint8_t color = VGAColor;

static inline void clr() {
    video = (volatile uint8_t*)VGAMemory;

    int i = 0;
    while(i != 4000) {
        video[i] = ' ';
        i++;
        video[i] = 0x00;
        i++;
    }
}

static inline void printf(const char *str) {
    for (int i = 0x00, n = 0; str[n] != '\0'; i += 2, n++) { // loop that stops when str[n] reached null terminator
        if (i >= 4000) { // if i reaches the end of the first vga page clear screen and set i back to original.
            clr();
            i = 0;
        }
        if (str[n] == '\n') { // newline sheisse, love it or hate it its alright
            n++;
            int bytes_into_row = i % 160;
            i += (160 - bytes_into_row);
        }
        video[i] = str[n];  // core dont touch
        video[i + 1] = color;
    }
}


static inline uint8_t inb(uint16_t port) {
    uint8_t result; // OH MY GOD OH JESUS ITS AN UNSIGNED INTEGER!!!

    asm volatile (  // Let er' rip
        "inb %1, %0"
        :"=a"(result)
        :"Nd"(port)
    );
    return result;
}

static inline void outb(uint16_t port, uint8_t dataWrite) {
    asm volatile (  // Let er' rip 2nd edition
        "outb %0, %1"
        :
        :"a"(dataWrite), "Nd"(port)
    );
}

static inline void outw(uint16_t port, uint16_t dataWrite) {
    asm volatile (  // Let er' rip 3nd edition
        "outb %0, %1"
        :
        :"a"(dataWrite), "Nd"(port)
    );
}

static inline void  enableCursor(uint8_t cursorStart, uint8_t cursorEnd) { // from here to
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursorStart);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursorEnd);
}

static inline void disableCursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

static inline void moveCharacter(int x, int y) {
    uint16_t pos = y * VGAWidth + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
} // here is all from wiki.osdev.org 

static inline void initSerial() {
    outb(COM1_PORT + 1, 0x00); // STOP THE INTERRUPTS!
    outb(COM1_PORT + 3, 0x80); // initializing the serial port
    outb(COM1_PORT + 1, 0x01);
    outb(COM1_PORT + 0, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xC7);
    outb(COM1_PORT + 4, 0x0B); // alright im done.
}

static inline int isTransmitEmpty(){
    return inb(COM1_PORT + 5) & 0x20; // return 0 if safe to write
}

static inline void writeCharSerial(char c){
    while(isTransmitEmpty() == 0); // check if its save to write

    outb(COM1_PORT, c); // read name of function for what this does
}

static inline void prints(const char* str) { 
    int i = 0;
    while (str[i] != '\0') { // loop that sends letters to serial ports until it reaches a null terminator
        writeCharSerial(str[i]);
        i++;
    }
}

static inline void printchar(char character, uint8_t color) { // dont use this, its more of a proof of concept and as useless as a 2 month old toddler.
    //set the grandpa video memory to the variable the extra cool way
    volatile uint8_t *video = (volatile uint8_t*)VGAMemory;
    
    video[0] = character; // writing stuff to vga
    video[1] = color;

}

/*
    Thank you for reading the source code!
*/