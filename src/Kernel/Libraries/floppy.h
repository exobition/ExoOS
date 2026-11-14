#ifndef FLOPPY_H
#define FLOPPY_H

#include <stdint.h>
#include "hwio.h"

#define SECTOR_SIZE 512

static uint8_t primary_floppy;
static uint8_t heads = 2;
static uint8_t cylinders;
static uint8_t sectPerTrack;
static uint8_t driveNum = 0x00;

static uint8_t cylinder;
static uint8_t head;
static uint8_t sector;

static uint8_t st0;
static uint8_t st1;
static uint8_t st2;
static uint8_t cyl;
static uint8_t head;
static uint8_t sect;
static uint8_t sectSize;

static int isFloppyInit = 0;
static int isReadingData = 0;
static int isReadingResult = 0;

enum optionBits {
    MT  = 0x80,  // Multitrack bit value
    MFM = 0x40,  // Magnetic encoding mode (always set to 1)
    SK  = 0x20   // Skip
};

enum FloppyReg {
    statRegA         = 0x3F0,   // ro
    statRegB         = 0x3F1,   // ro
    digitalOutReg    = 0x3F2,   // rw
    tapeDriveReg     = 0x3F3,   // rw
    mainStatReg      = 0x3F4,   // ro
    dataRateSelReg   = 0x3F4,   // wo
    dataFIFO         = 0x3F5,   // rw
    digitalInReg     = 0x3F7,   // ro
    configControlReg = 0x3F7    // wo
};

enum FloppyCommands {
   readTrack =                  0x02,
   specify =                    0x03,
   senseDriveStatus =           0x04,
   writeData =                  0x05,
   readData =                   0x06,
   recalibrate =                0x07,
   senseInterrupt =             0x08,
   writeDeletedData =           0x09,
   readID =                     0x0A,
   readDeletedData =            0x0C,
   formatTrack =                0x0D,
   dumpReg =                    0x0E,
   seek =                       0x0F,
   version =                    0x10,
   scanEqual =                  0x11,
   perpenDICularMode =          0x12, // hah dick
   configure =                  0x13,
   lock =                       0x14,
   verify =                     0x16,
   scanLowOrEqual =             0x19,
   scanHighOrEqual =            0x1D
};

static inline void fdcWait() {
    int timeout = 0;

    if ( isReadingData == 0 && isReadingResult == 0 ) {
        while ( !(inb(mainStatReg) & 0x80) ) {
            timeout++;
            if (timeout == 100000) {
                break;
            }
        }
    } else if ( isReadingData == 1 ) {
        while ( !(inb(mainStatReg) & 0xC0) ) {
            timeout++;
            if (timeout == 100000) {
                break;
            }
        }
    } else if ( isReadingResult == 1 ) {
        while ( !(inb(mainStatReg) & 0xE0) ) {
            timeout++;
            if (timeout == 100000) {
                break;
            }
        }
    } else {
        prints("Genuinely what the fuck are you waiting for? EVERYTHING IS OKAY.\r\n\0");
    }
}

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

static inline void LBAToCHS(uint32_t lba){

    cylinder    = lba / (heads * sectPerTrack);
    head        = ((lba % (heads * sectPerTrack)) / sectPerTrack);
    sector      = ((lba % (heads * sectPerTrack)) % sectPerTrack + 1);

    return;
}

static inline uint32_t CHSToLBA(uint32_t cyl, uint32_t head, uint32_t sect) {
    return ((cyl * heads + head) * sectPerTrack) + sect - 1;
}

static inline int read(uint32_t LBA, uint8_t mem[512]) {

    if ( mem == 0x00 ) {
        return -1;
    }

    LBAToCHS(LBA);

    fdcWait();

    outb(dataFIFO, MT | MFM | readData);

    fdcWait();

    outb(dataFIFO, (head << 2) | driveNum);

    fdcWait();

    outb(dataFIFO, cylinder);

    fdcWait();

    outb(dataFIFO, head);

    fdcWait();

    outb(dataFIFO, sector); 

    fdcWait();

    outb(dataFIFO, 0x02);

    fdcWait();

    outb(dataFIFO, sectPerTrack); // idk if this is technically correct but fuck it we ball

    fdcWait();

    outb(dataFIFO, 0x1B);

    fdcWait();

    outb(dataFIFO, 0xFF); 

    isReadingData = 1;

    for ( unsigned int i = 0; i <= 511; i++ ) {
        fdcWait();

        mem[i] = inb(dataFIFO);
    }

    isReadingData = 0;
    isReadingResult = 1;

    fdcWait();

    st0 = inb(dataFIFO);

    fdcWait();

    st1 = inb(dataFIFO);

    fdcWait();

    st2 = inb(dataFIFO);

    fdcWait();

    cyl = inb(dataFIFO);

    fdcWait();

    head = inb(dataFIFO);

    fdcWait();

    sect = inb(dataFIFO);

    fdcWait();

    sectSize = inb(dataFIFO);

    isReadingResult = 0;

    return 0;

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
        
        fdcWait();

        outb(dataFIFO, 0x03);

        fdcWait();

        outb(dataFIFO, 0xDF);

        fdcWait();

        outb(dataFIFO, 0x03);

        outb(digitalOutReg, 0x14);
        
        flpMotorDelay();

        fdcWait();

        outb(dataFIFO, recalibrate);

        fdcWait();

        outb(dataFIFO, 0x00);

        fdcWait();

        outb(dataFIFO, senseInterrupt);

        int timeout = 0;
        while ( !(inb(mainStatReg) & 0x40) || timeout < 100000 ) {
            // just wait
            timeout++;
        }
        
        return;
    }
}

#endif