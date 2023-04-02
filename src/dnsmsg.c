#include "dnsmsg.h"

//Creates a DNS question with information provided by user
void DNSmsg_createQuery(struct DNSmsg *message, char *hostname, char *record_type){

    //Header section
    memset(message, 0, sizeof(struct DNSmsg));
    message->header.id = 0xabcd;
    message->header.flags |= HF_RD;
    message->header.qdcount = 0x01; 


    //Question section
    int namelen = strlen(hostname);
    if(namelen > MAX_NAMELEN) namelen = MAX_NAMELEN;
    char *enc_name = (char *) malloc(namelen + 1);
    DNSmsg_nameEncode(hostname, namelen, enc_name);

    message->question.name = enc_name;
    message->question.qtype = DNSmsg_getRecordCode(record_type);
    message->question.qclass = 1;    //internet
     

    //Answer section
    message->answer.name = (char*) malloc(1);
    message->answer.name[0] = '\0';
}

uint16_t DNSmsg_getRecordCode(char *record_type){
    if(strstr(record_type, "A") && strlen(record_type) == 1){
        return 1;
    }
    if(strstr(record_type, "CNAME") && strlen(record_type) == 5){
        return 5;
    }
    if(strstr(record_type, "MX") && strlen(record_type) == 2){
        return 15;
    }
    if(strstr(record_type, "TXT") && strlen(record_type) == 3){
        return 16;
    }
    if(strstr(record_type, "AAAA") && strlen(record_type) == 4){
        return 28;
    }
    if(strstr(record_type, "ANY") && strlen(record_type) == 3){
        return 255;
    }
    fprintf(stderr, "\n*INVALID RECORD TYPE\nValid types:\n"
            "'A', 'CNAME', 'MX', 'TXT', 'AAAA', 'ANY'\n\n");
    return 0;
}

//Encode hostname to the way the DNS server expects it.
void DNSmsg_nameEncode(const char *const name, size_t namelen, char *buf){
    int i = 0;
    char s[2] = ".";

    char name_cpy[MAX_NAMELEN];
    memcpy(name_cpy, name, namelen);
    char *token = strtok(name_cpy, s);
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

void DNSmsg_freeNames(struct DNSmsg *msg){
    free(msg->question.name);
    free(msg->answer.name);
}
