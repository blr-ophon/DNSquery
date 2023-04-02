#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "dnsmsg.h"
#include "wrapper.h"
#include "msgoutput.h"

#define DNS_PORT "53"
#define MSG_BUF_SIZE 1024
#define ANSWER_DATABUF_SIZE 256

#endif
