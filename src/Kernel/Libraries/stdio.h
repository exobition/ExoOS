#include <stdint.h>
#include "hwio.h"

#define VGAMemory 0xB8000
#define VGAMax 0xB9000

#define VGAWidth 80
#define VGAHeight 25

#define VGAColor 0x07

#define PS2_DATA 0x60
#define PS2_STATUS_COMMAND 0x64

#define PS2_STATUS_IBF (1 << 1)
#define PS2_STATUS_OBF (1 << 0)

#define COM1_PORT 0x3F8

#define BUFFER_SIZE 1024

const char ascii_lower[58] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  39,  '`',
    1,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',  1,   '*',
    0,   ' '
};

const char ascii_upper[58] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  '"',  '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',  0,   '*',
    0,   ' '
};

int isShiftHeld = 0;
int inputCMD = 0;

const char *helpMSG = "#############################################################\n#   whichOS -- Displays current operating system version.   #\n#           whichSHELL -- Displays current Shell.           #\n#      Exit -- Makes it safe to turn off the computer.      #\n#         help -- Displays commands and their uses.         #\n# echo -- sends what you type after echo to standard output #\n#############################################################";

volatile uint8_t *video = (volatile uint8_t*)VGAMemory;
volatile uint8_t Nl = 0xA0;
volatile uint8_t color = VGAColor;

char cmdBuffer[BUFFER_SIZE] = {0};

int exit = 0;

int counter = 0;

int i = 0;

void abort() {
    while(1) {
        
    }
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

static inline void  enableCursor(uint8_t cursorStart, uint8_t cursorEnd) { // from here to

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursorEnd); // setting the scanline end 

    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursorStart); // setting the scanline start

    prints("VGA cursor enabled\r\n\0");
}

static inline void disableCursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    prints("VGA cursor disabled\r\n\0");
}

static inline void moveCursor(int x, int y) {
    uint16_t pos = y * VGAWidth + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
} // here is all from wiki.osdev.org

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
    for (int n = 0; str[n] != '\0'; i += 2, n++) { // loop that stops when str[n] reached null terminator
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

void PS2WaitWrite() {
    while (inb(PS2_STATUS_COMMAND) & PS2_STATUS_IBF) {

    }
}

void PS2WaitRead() {
    while (!(inb(PS2_STATUS_COMMAND) & PS2_STATUS_OBF)) {

    }
}

static inline void setPS2ConfigByte(uint8_t controllerConfigByte) {
    PS2WaitWrite();
    outb(PS2_STATUS_COMMAND, 0x20); // getting old PS/2 configuration byte
    PS2WaitRead();
    uint8_t CCB = inb(PS2_DATA);
    if (CCB == controllerConfigByte) {  // Testing if its already how i like if
        prints("PS/2 Controller Configuration Byte is already in correct configuration\r\n\0");
    } else { // if not then send the right one
        PS2WaitWrite();
        outb(0x64, 0x60);
        PS2WaitWrite();
        outb(0x60, controllerConfigByte);
        prints("PS/2 Controller Configuration Byte has been set\r\n\0");
    }
}

static inline void initPS2Controller() {
    prints("Begining PS/2 initialization\r\n\0");
    PS2WaitWrite();
    outb(PS2_STATUS_COMMAND, 0xA7); // disabling PS/2 devices
    PS2WaitWrite();
    outb(PS2_STATUS_COMMAND, 0xAD);
    prints("PS/2 devices disabled\r\n\0");
    inb(PS2_DATA); // flushing the data register
    prints("Begining PS/2 Configuration\r\n\0");
    
    setPS2ConfigByte(0x64);

    prints("Self-testing PS/2 controller\r\n\0");
    PS2WaitWrite();
    outb(PS2_STATUS_COMMAND, 0xAA); // getting the numbers that tell us if the PS/2 controller is ok or fried
    PS2WaitRead();
    uint8_t selfTestOutput = inb(PS2_DATA);

    if (selfTestOutput == 0x55) { // actually checking if its ok
        prints("PS/2 controller is OK\r\n\0");
        PS2WaitWrite();
        outb(PS2_STATUS_COMMAND, 0xA7);
        prints("Disabled 2nd PS/2 channel\r\nTesting PS/2 port\r\n\0");
        PS2WaitWrite();
        outb(PS2_STATUS_COMMAND, 0xAB); // disabling 2nd playstation channel
        PS2WaitRead();
        uint8_t PS2PortTestOutput = inb(PS2_DATA);
        if (PS2PortTestOutput == 0x00) {  // detecting el problemo with PS/2 port
            PS2WaitWrite();
            outb(PS2_STATUS_COMMAND, 0xAE);
            PS2WaitWrite();
            outb(PS2_STATUS_COMMAND, 0xFF);
            prints("PS/2 port is ready for use\r\n\0"); // alright everything ok
        } else {
            prints("Problem detected with PS/2 port, aborting...\r\n\0"); // gulp
            abort();
        }
    } else {
        prints("PS/2 controller failed self-test, aborting...\r\n\0"); // ruh roh raggy, scoobs we gotta get outta here
        abort();
    }
}

static inline int strcmp(const char *buffer, int buffer_index, const char *string) {
    for (int count = 0; count <= buffer_index; count++) { // all you need to know is that this checks if string is equivilant to buffer of what is inputed by user
        if (buffer[count] != string[count]) { // check if it isnt equivalent to string
            if (buffer[count] == ' ') { // check if its just an argument
                return 0;
            }
            return 1;
        }
    }

    return 0;
}


static inline void shellCmds() {
    if (strcmp(cmdBuffer, counter, "Exit") == 0) { // they are if statements checking wether or not the input matches command
        clr();
        printf("It is now safe to shut down your PC");
        while (1) {
            asm volatile("hlt");
        }
    } 
    if (strcmp(cmdBuffer, counter, "echo") == 0) {
        char string[1024];
        for (int count = 5; count <= counter; count++) {
            string[count - 5] = cmdBuffer[count]; // copy everything after echo into string minus "echo "
        }
        printf(string); // echo it out to std output
        return;
    }
    if (strcmp(cmdBuffer, counter, "whichOS") == 0) {
        printf("ExoOS ver. 4.2.0");
        return;
    }
    if (strcmp(cmdBuffer, counter, "whichSHELL") == 0) {
        printf("ExoSHELL v1.1.0");
        return;
    }
    if (strcmp(cmdBuffer, counter, "clear") == 0) {
        clr();
        printf("User $ ");
        return
    }
    if (strcmp(cmdBuffer, counter, "help") == 0) {
        printf(helpMSG); // help msg was a little big
        return;
    }
    printf("That is not recognized as a valid command.");
}

static inline void printc(const char c) {
    if (i > 4000) { // clear screen if it reaches the end
        clr();
        i = 0;
    }
    if (c == '\n') { // detects if there is a newline
        int bytes_into_row = i % 160; // finds amount needed to add to i to get to a newline
        i += (160 - bytes_into_row); // actually getting to the newline
        shellCmds(); // check if it matches one of the many expansive shell commands
        if (counter != 0) {
            for (int count = 0; count <= counter; count++) {
                cmdBuffer[count] = 0x00;
            }
        }
        printf("\nUser $ \0"); // larping shell commands
        counter = 0; // counter is zero
        return; // "Back to where thou doth come thence" - some old xenophobic english dude
    }
    if (c == '\b') {
        if (counter == 0) { // Checking if nothing has been typed
            return; // If it has then be the old xenophobic english dude
        }
        i -= 2; // decrement vga index one character
        cmdBuffer[counter] = 0x00;
        counter--; // decrement counter by one
        video[i] = 0x00; // erasing evidence
        video[i + 1] = color; // erasing evidence X2
        return; // return dont print
    }
    cmdBuffer[counter] = c; // write to command buffer
    video[i] = c; // write to 
    video[i + 1] = color; // VGA memory
    i += 2; // Eke two
    counter++; // make sure to mark your new characters
}

static inline void scanf() {
    PS2WaitRead(); // wait
    uint8_t scancode = inb(PS2_DATA); // getting the scancode
    if(scancode == 0x2A || scancode == 0x36){ // if left or right shift is pressed
        isShiftHeld = 1; // note that
        return; // gtfo
    }
    if (isShiftHeld == 0) { // checking if shift is being held
        if(scancode & 0x80) { // getting rid of that release code
            prints("Character from PS/2 input printed\r\n\0");
            return; // "Back to where thou doth come thence" - another old xenophobic english dude
        }
        printc(ascii_lower[scancode]); // print it
    } else {
        if (scancode & 0x80) { // getting rid of that release code
            if (scancode == 0xB6 || scancode == 0xAA) { // if the scancode is the release code of left or right shift then do below
                isShiftHeld = 0; // "WRITE THAT DOWN WRITE THAT DOWN!" - spongebob
                return;
            }
            prints("Character from PS/2 input printed\r\n\0");
            return; // "Back to where thou doth come thence" - another old xenophobic english dude
        }
        printc(ascii_upper[scancode]); // PRINT IT
    }
}

static inline void keyboard() {
    while (exit == 0) {
        scanf(); // its scanf and always has been
    }
}

/*
    Thank you for reading the source code!
*/