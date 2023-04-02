#ifndef MSGOUTPUT_H
#define MSGOUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "dnsmsg.h"

void DNSmsg_print(struct DNSmsg *msg);

void msg_hexdump(uint8_t *msg, size_t n);


#endif
