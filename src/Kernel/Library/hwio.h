#include <stdint.h>

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