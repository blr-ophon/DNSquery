#include "hexdump.h"

//TODO: change name. add print and error translation to here

void msg_hexdump(uint8_t *msg, size_t n){
    for(size_t i = 0; i < (n/8); i++){
        for(int j = 0x0; j <= 0x7; j++){
            printf("%#4x ", msg[i*0x8 + j]);
        }
        printf("\n");
    }
}
