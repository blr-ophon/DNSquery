#include "dnsquery.h"

//TODO: User defined record types
//TODO: Use htonl/ntohl
//TODO: Use char[255] on answer and question names, but only send necessary


//Won't work:
//question and answer names initialized with local strings
//empty answer name on first query (for strlen on wrap)
//  -it either is completly empty or has a single null character
//  -handling it like it's completly empty


void DNSmsg_freeNames(struct DNSmsg *msg){
    free(msg->question.name);
    free(msg->answer.name);
}

void DNSmsg_configure(struct DNSmsg *message){

    //Header section
    memset(message, 0, sizeof(struct DNSmsg));
    message->header.id = 0xabcd;
    message->header.flags |= HF_RD;
    message->header.qdcount = 0x01; 
    
    //Question section
    char name[20] = "www.example.com";
    char *enc_name = (char *) malloc(strlen(name) + 1);
    name_encode(name, enc_name);
    //printf("%s\n", buf);
    message->question.name = enc_name;
    message->question.qtype = QTYPE_A;     
    message->question.qclass = 1;    //internet
     
    /*
    //Answer
    char answer_emptyname = '\0';
    message->answer.name = &answer_emptyname;
    */
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

