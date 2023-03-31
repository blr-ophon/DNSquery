#include "dnsquery.h"

#define DNS_PORT 53

uint16_t to_bigendian16(uint16_t num){
    return (num >> 8) | (num << 8);
}

uint16_t to_bigendian32(uint16_t num){
    return ((num>>24)&0xff) | ((num<<8)&0xff0000) | 
        ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int main(void){
    
    struct DNSmsg message;

    //Set header
    memset(&message, 0, sizeof(message));
    message.header.id = 0xabcd;
    message.header.flags |= HF_RD;
    message.header.qdcount = 0x01; 
    
    char name[20] = "www.example.com";
    char enc_name[20];
    name_encode(name, enc_name);
    //printf("%s\n", buf);
    message.question.name = enc_name;
    message.question.qtype = 1;
    message.question.qclass = 1;


    return 0;
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
