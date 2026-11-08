#ifndef FLOPPY_H
#define FLOPPY_H

#include <stdint.h>
#include "hwio.h"

static uint8_t primary_floppy;
static uint8_t heads;
static uint8_t cylinders;
static uint8_t sectPerTrack;

static int timeout = 0;
static int isFloppyInit = 0;

enum FloppyReg {
    statRegA         = 0x3F0,
    statRegB         = 0x3F1,
    digitalOutReg    = 0x3F2,
    tapeDriveReg     = 0x3F3,
    mainStatReg      = 0x3F4,
    dataRateSelReg   = 0x3F4,
    dataFIFO         = 0x3F5,
    digitalInReg     = 0x3F7,
    configControlReg = 0x3F7
};

static inline uint8_t cmosIn(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static inline void flpMotorDelay() {
    while (cmosIn(0x0A) & 0x80);

    uint8_t startSecond = cmosIn(0x00);

    while (1) {

        while (cmosIn(0x0A) & 0x80);

        if (cmosIn(0x00) != startSecond) {
            break;
        }
    }
}

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
        return 2;
    } else if (primary_floppy == 0x02) {
        cylinders = 0x50;
        sectPerTrack = 0x0F;
        return 3;
    } else if (primary_floppy == 0x03) {
        cylinders = 0x50;
        sectPerTrack = 0x09;
        return 4;
    } else if (primary_floppy == 0x04) {
        cylinders = 0x50;
        sectPerTrack = 0x12;
        return 5;
    } else if (primary_floppy == 0x05) {
        cylinders = 0x50;
        sectPerTrack = 0x24;
        return 6;
    }

    return 1;
}

static inline void LBAToCHS(uint32_t lba, uint8_t *cyl, uint8_t *head, uint8_t *sector){
    *cyl    = lba / (2 * sectPerTrack);
    *head   = ((lba % (2 * sectPerTrack)) / sectPerTrack);
    *sector = ((lba % (2 * sectPerTrack)) % sectPerTrack + 1);
}

static inline uint32_t CHSToLBA(uint32_t cyl, uint32_t head, uint32_t sect) {
    return ((cyl * heads + head) * sectPerTrack) + sect - 1;
}

static inline void initFDC() {
    int flpType = setFloppyDataTypes();

    if (flpType != 1 && isFloppyInit == 0) {
        isFloppyInit++;

        outb(digitalOutReg, 0x00);

        inb(0x80);
        inb(0x80);
        inb(0x80);
        inb(0x80);

        outb(digitalOutReg, 0x04);

        outb(configControlReg, 0x00);

        if (flpType == 2 || flpType == 4) {
            outb(configControlReg, 0x02);
        } else if (flpType == 3 || flpType == 5) {
            outb(configControlReg, 0x00);
        } else if (flpType == 6) {
            outb(configControlReg, 0x03);
        } else {
            return;
        }
        
        while ((inb(mainStatReg) & 0xC0) != 0x80) {
            timeout++;
            if (timeout == 1000000) {
                break;
            }
        }

        outb(dataFIFO, 0x03);

        while ((inb(mainStatReg) & 0xC0) != 0x80) {
            timeout++;
            if (timeout == 1000000) {
                break;
            }
        }

        outb(dataFIFO, 0xDF);

        while ((inb(mainStatReg) & 0xC0) != 0x80) {
            timeout++;
            if (timeout == 1000000) {
                break;
            }
        }

        outb(dataFIFO, 0x03);

        outb(digitalOutReg, 0x14);
        
        flpMotorDelay();
        
        return;
    }
}

#endif