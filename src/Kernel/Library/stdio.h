#include <stdint.h>

#define VGAMemory 0xB8000
#define VGAMax 0xB9000

#define VGAWidth 80
#define VGAHeight 25

#define VGAColor 0x07

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
    for (int i = 0x00, n = 0; str[n] != '\0'; i += 2, n++) {
        if (str[n] == '\n') {
            n++;
            int bytes_into_row = i % 160;
            i += (160 - bytes_into_row);
        }
        video[i] = str[n];
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
    while(str[i] != '\0') {
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
    Ok I know exactly what you are thinking after reading this code, and yeah I admit that there are probably
    a lot of better and faster ways of doing this stuff but before you slam me with ways I could've done it gooder
    just know that I couldnt care less and it just works. Also a lot of the redundancy of this code is because of
    the compiler but you read my comments so you know that right? right...

    Well if you actually read this find a way to message me the word "banana".
*/