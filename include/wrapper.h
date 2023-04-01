#ifndef WRAPPER_H
#define WRAPPER_H

#include "dnsquery.h"

//uint8_t *DNSmsg_wrap(const struct DNSmsg *const message);
uint8_t *DNSmsg_wrap(struct DNSmsg *message);

struct DNSmsg DNSmsg_unwrap(uint8_t *data, char *answer_databuf);

#endif
