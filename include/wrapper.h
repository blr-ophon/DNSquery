#ifndef WRAPPER_H
#define WRAPPER_H

#include "dnsmsg.h"


size_t DNSmsg_getWrappedSize(struct DNSmsg *message);

uint8_t *DNSmsg_wrap(const struct DNSmsg *const message);

struct DNSmsg DNSmsg_unwrap(uint8_t *data, char *answer_databuf);

#endif
