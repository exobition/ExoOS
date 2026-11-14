#ifndef IAO_H
#define IAO_H

static inline uint32_t atoi(const char* str) {

    int sign = 1;
    uint32_t integer = 0x00;

    for ( int i = 0; str[i] != 0x00; i++ ) {
        integer += str[i] - '0';
        integer *= 16;
    }

    return integer;

}

static inline int itoa(const int num) {

}

static inline int itoh(const int num) {

}

#endif