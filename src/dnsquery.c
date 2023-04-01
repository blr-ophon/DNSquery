#include "dnsquery.h"

//TODO: 255 and other numbers as constants on config.h
//TODO: User defined record types and question name
//TODO: Use htonl/ntohl
//TODO: Use char[255] on answer and question names, but only send necessary
//TODO: make qtype display record in string form on DNSmsg_print
//TODO: ncurses display
//TODO: check fixed size buffers


//Won't work:
//empty answer name on first query (for strlen on wrap)
//  -it either is completly empty or has a single null character
//  -handling it like it's completly empty
//  -now handling it with a single null character


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
    char name[20] = "www.google.com";
    char *enc_name = (char *) malloc(strlen(name) + 1);
    DNSmsg_nameEncode(name, strlen(name), enc_name);
    //printf("%s\n", buf);
    message->question.name = enc_name;
    message->question.qtype = QTYPE_A;     
    message->question.qclass = 1;    //internet
     

    //Answer
    message->answer.name = (char*) malloc(1);
    message->answer.name[0] = '\0';

    //FOR TEST
    
    //message->header.id= 0xabcd;
    //message->header.flags= 0xabcd;
    //message->header.qdcount = 0xabcd;
    //message->header.ancount = 0xabcd;
    //message->header.nscount = 0xabcd;
    //message->header.arcount = 0xabcd;

    //message->question.qtype = 0xabcd;
    //message->question.qclass = 0xabcd;

    //message->answer.rtype = 0xabcd;
    //message->answer.rclass = 0xabcd;
    //message->answer.ttl = 0xabcdef01;
    //message->answer.rdlength = 0x3;
    //message->answer.rdata = (char*) malloc(3);
    //message->answer.rdata = "123";
    
}

void DNSmsg_print(struct DNSmsg *msg){ 
    //HEADER
    printf("\n--HEADER--\nID:%#x\nFLAGS:%#x\nQDCOUNT:%#x\n"
            "ANCOUNT:%#x\nNSCOUNT:%#x\nARCOUNT:%#x\n",
            msg->header.id,
            msg->header.flags,
            msg->header.qdcount,
            msg->header.ancount,
            msg->header.nscount,
            msg->header.arcount
    );

    //QUESTION
    char q_name[255]; 
    if(msg->question.name != NULL){
        strncpy(q_name, msg->question.name, strlen(msg->question.name)+1);
    }else{
        q_name[0] = '\0';
    }
    printf("\n--QUESTION--\nNAME:%s\nQTYPE:%#x\nQCLASS:%#x\n",
            msg->question.name,
            msg->question.qtype,
            msg->question.qclass
    );

    //ANSWER
    char a_name[255]; 
    if(msg->answer.name != NULL){
        strncpy(a_name, msg->answer.name, strlen(msg->answer.name)+1);
    }else{
        a_name[0] = '\0';
    }
    printf("\n--ANSWER--\nNAME:%s\nTYPE:%#x\nCLASS:%#x\n"
            "TTL:%#x\nRDLENGTH:%#x\n",
            a_name,
            msg->answer.rtype,
            msg->answer.rclass,
            msg->answer.ttl,
            msg->answer.rdlength
    );

    printf("DATA:\n");
    for(int i = 0; i < msg->answer.rdlength; i++){
        printf("%c", msg->answer.rdata[i]);
    }
    printf("\n");
}

void DNSmsg_nameEncode(const char *const name, size_t namelen, char *buf){
    int i = 0;
    char s[2] = ".";

    char name_cpy[255];
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

