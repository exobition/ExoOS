#ifndef IAO_H
#define IAO_H

static inline uint32_t atoi(const char* str) {

    uint32_t integer = 0;

    for ( int i = 0; str[i] != 0x00; i++ ) {
        if ( str[i] != '0' || str[i] != '1' || str[i] != '2' || str[i] != '3' || str[i] != '4' || str[i] != '5' || str[i] != '6' || str[i] != '7' || str[i] != '8' || str[i] != '9' ) {
            continue;
        }
        integer += str[i] - '0';
        integer *= 10;
    }

    return integer;

}

static inline char* htoa(const uint8_t num) {

    char out[2];

    if ( num & 0x0F < 0x0A) {
        out[0] = ( num & 0x0F ) + 0x30;
    } else {
        out[1] = ( ( num & 0x0F ) - 0x0A) + 0x61;
    }
    if ( num >> 4 < 0x0A) {
        out[0] = ( num >> 4)  + 0x30;
    } else {
        out[1] = ( ( num >> 4 ) - 0x0A ) + 0x61;
    }

} 

static inline char* itoa(const int num) {

}

static inline int itoh(const int num) {

}

#endif