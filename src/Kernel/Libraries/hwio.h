#ifndef HWIO_H
#define HWIO_H

#include <stdint.h>

#define COM1_PORT 0x3F8

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
        "outw %0, %1"
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
    for (int i = 0; str[i] != '\0'; i++) { // checks if str[i] is at null terminator, otherwise just keeps trucking
        writeCharSerial(str[i]);
    }
}

#endif