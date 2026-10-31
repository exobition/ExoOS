#include <stdint.h>
#include "Libraries/stdio.h"

int main() {
    clr();
    initSerial();
    prints("Kernel loaded into ram successfully\r\nSerial initialized\r\n\0");
    printf("#########################\n# Welcome to ExoOS v0.2 #\n#########################\n\0");
}

