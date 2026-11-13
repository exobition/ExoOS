#ifndef IAO_H
#define IAO_H

static inline int atoi(const char* str) {

    int i = 0;
    int sign = 1;
    int atoi = 0;

    if ( str[0] == '-' ) {
        sign = 0;
        str++;
    }

    for ( ; str[i] != 0x00; i++ ) {
        atoi *= 10;
        atoi += str[i] - '0';
    }
    if ( sign == 0 ) {
        atoi *= -1;
    }

    return atoi;

}

static inline int itoa(const int num) {

}

static inline int itoh(const int num) {

}

#endif