#include <stdint.h>
#include "Libraries/main.h"

void main() {
    clr();
    initSerial();
    prints("Kernel loaded into ram successfully\r\nSerial initialized\r\n\0");
    disableCursor();
    printf("###########################\n# Welcome to ExoOS v5.0.0 #\n###########################\n\0");
    printf("\nUser $ ");
    // note NEVER DOUBLE \n (as in printf("\n\n");) it will NOT work as intended,
    // instead use printf("\n"); and then another printf("\n")' if you need double
    // newlines, if you do print a double \n it WILL set i (vga memory index) to 0 
    // and you will overwrite stuff that was printed back when i was 0.  beware.

    // nevermind it now tries to print \n like its a regular character (if you double \n) so just space em out or smth
    //tl;dr DONT USE \n\n
    initPS2Controller();
    keyboard();
}
