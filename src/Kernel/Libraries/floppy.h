#ifndef FLOPPY_H
#define FLOPPY_H

#include <stdint.h>
#include "hwio.h"

static uint8_t primary_floppy;
static uint8_t heads;
static uint8_t cylinders;
static uint8_t sectPerTrack;

static inline uint8_t cmosflp() {
    outb(0x70, 0x10);  // beg cmos for floppy drives
    uint8_t output = inb(0x71); // get that output
    return output;  // ret output
}

static inline int setFloppyDataTypes() {
    uint8_t cmosFloppyType = cmosflp();
    heads = 0x02;
    primary_floppy = cmosFloppyType >> 4;

    if (primary_floppy == 0x00) {
        return 1;
    } else if (primary_floppy == 0x01) {
        cylinders = 0x28;
        sectPerTrack = 0x09;
        return 0;
    } else if (primary_floppy == 0x02) {
        cylinders = 0x50;
        sectPerTrack = 0x0F;
        return 0;
    } else if (primary_floppy == 0x03) {
        cylinders = 0x50;
        sectPerTrack = 0x09;
        return 0;
    } else if (primary_floppy == 0x04) {
        cylinders = 0x50;
        sectPerTrack = 0x12;
        return 0;
    } else if (primary_floppy == 0x05) {
        cylinders = 0x50;
        sectPerTrack = 0x24;
        return 0;
    }

    return 1;
}

#endif
