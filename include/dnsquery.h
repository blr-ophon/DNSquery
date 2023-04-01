#ifndef DNSQUERY_H
#define DNSQUERY_H

//TODO: Modularize:
//-udp client file
//-DNSmsg configuration and encoding
//-DNSmsg receiving
//-Interface (if using curses)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct DNSmsg_header{
    uint16_t id;            //any 16bit value chosen by the client
    uint16_t flags;
    uint16_t qdcount;       //Questions per message (always 1)
    uint16_t ancount;       //Number of answers
    uint16_t nscount;
    uint16_t arcount;       //Number of Records
};


struct DNSmsg_question{
    char *name;          //Hostname encoded
    uint16_t qtype;         //Record type asked. 1 for A records
    uint16_t qclass;        //set to 1 to indicate the Internet
};
//hostname is encoded by first being broken into labels, then have
//each label prepended by 1 byte indicating the label length. Ends
//with a 0 byte.

struct DNSmsg_answer{
    char *name;          //Hostname encoded
    uint16_t rtype;         //Data interpretation (endianness)
    uint16_t rclass;        //set to 1 to indicate the Internet
    uint32_t ttl;           //time to live
    uint16_t rdlength;      //size of following data 
    char *rdata;
};

struct DNSmsg{
    struct DNSmsg_header header;
    struct DNSmsg_question question;
    struct DNSmsg_answer answer;
};

typedef enum{
    HF_QR = (1 << 0),           //Query(0)/Response(1)
    HF_OPCODE_STD = (0 << 1),   //Standard query
    HF_OPCODE_REV = (1 << 1),   //Reverse query
    HF_OPCODE_STR = (2 << 1),   //Status request
    HF_AA = (1 << 5),   //Authoritative Answer
    HF_TC = (1 << 6),   //Truncated (must be resent through TCP)
    HF_RD = (1 << 7),   //Recursion
    HF_RA = (1 << 8),   //Indicates in response if server supports recursion
    HF_RCODE_NE         = (0 << 12),    //No error 
    HF_RCODE_FORMAT     = (1 << 12),    //Format Error
    HF_RCODE_SERVER     = (2 << 12),    //Server Failure
    HF_RCODE_NAME       = (3 << 12),    //Name error
    HF_RCODE_NOTIMP     = (4 << 12),    //Not Implemented
    HF_RCODE_RFSD       = (5 << 12),    //Refused
}HEADER_FLAGS;

typedef enum{
    QTYPE_A     = 1,
    QTYPE_MX    = 15,
    QTYPE_TXT   = 16,
    QTYPE_AAAA  = 28,
    QTYPE_ANY   = 255
}QTYPE;

void DNSmsg_freeNames(struct DNSmsg *msg);

void DNSmsg_configure(struct DNSmsg *message);

void name_encode(char *name, char *buf);

uint16_t to_bigendian16(uint16_t num);

uint32_t to_bigendian32(uint32_t num);

#endif
