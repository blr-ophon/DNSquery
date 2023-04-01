#include "dnsquery.h"

//TODO: User defined record types


//Won't work:
//question and answer names initialized with local strings
//empty answer name on first query (for strlen on wrap)

void DNSmsg_configure(struct DNSmsg *message){

    //Header section
    memset(message, 0, sizeof(struct DNSmsg));
    message->header.id = 0xabcd;
    message->header.flags |= HF_RD;
    message->header.qdcount = 0x01; 
    
    //Question section
    char name[20] = "www.example.com";
    char enc_name[20];
    name_encode(name, enc_name);
    //printf("%s\n", buf);
    message->question.name = enc_name;
    message->question.qtype = QTYPE_A;     
    message->question.qclass = 1;    //internet
     
    //Answer
    char answer_emptyname = '\0';
    message->answer.name = &answer_emptyname;
                                     
}

void name_encode(char *name, char *buf){
    int i = 0;
    char s[2] = ".";

    char *token = strtok(name, s);
    buf[i] = strlen(token);
    strncpy(&buf[i+1], token, strlen(token));
    i+= strlen(token)+1;

    while(token != NULL){
        token = strtok(NULL, s);
        if(token == NULL) break;
        buf[i] = strlen(token);
        strncpy(&buf[i+1], token, strlen(token));
        i+= strlen(token)+1;
    }
    buf[i] = 0;
}

uint16_t to_bigendian16(uint16_t num){
    return (num >> 8) | (num << 8);
}

uint32_t to_bigendian32(uint32_t num){
    return ((num>>24)&0xff) | ((num<<8)&0xff0000) | 
        ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

uint8_t *DNSmsg_wrap(const struct DNSmsg *const message){
    //Size = struct + name strings + data - 2*(pointer size)
    uint8_t *wrapped_msg = (uint8_t*) malloc(sizeof(struct DNSmsg) +
            strlen(message->question.name) + strlen(message->answer.name) +
            message->answer.rdlength - 2*sizeof(void *));

    struct DNSmsg BE_msg;
    memset(&BE_msg, 0, sizeof(struct DNSmsg));

    //Header 
    BE_msg.header.id = to_bigendian16(message->header.id);
    BE_msg.header.flags = to_bigendian16(message->header.id);
    BE_msg.header.qdcount = to_bigendian16(message->header.id);
    BE_msg.header.ancount = to_bigendian16(message->header.id);
    BE_msg.header.nscount = to_bigendian16(message->header.id);
    BE_msg.header.arcount = to_bigendian16(message->header.id);
    memcpy(wrapped_msg, &BE_msg.header, sizeof(struct DNSmsg_header));

    //Question
    memcpy(wrapped_msg, message->question.name, strlen(message->question.name));


    //Answer
    memcpy(wrapped_msg, message->answer.name, strlen(message->answer.name));

    return wrapped_msg;
}
