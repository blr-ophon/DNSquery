#include "msgoutput.h"

//Prints the content of a DNSmsg struct
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
    char q_name[MAX_NAMELEN]; 
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
    char a_name[MAX_NAMELEN]; 
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

    printf("DATA:\n\n");
    switch(msg->answer.rtype){ 
        case QTYPE_A:
            for(int i = 0; i < msg->answer.rdlength; i++){
                printf("%hhu.", msg->answer.rdata[i]);
            }
            break;
        case QTYPE_AAAA:
            for(int i = 0; i < msg->answer.rdlength; i+=2){
                printf("%02hhx%02hhx:", msg->answer.rdata[i],
                        msg->answer.rdata[i+1]);
            }
            break;
        case QTYPE_TXT:
            for(int i = 0; i < msg->answer.rdlength; i++){
                printf("%c", msg->answer.rdata[i]);
            }
            break;
        default:
            for(int i = 0; i < msg->answer.rdlength; i++){
                printf("%#4x ", msg->answer.rdata[i]);
            }
            break;
    }
    printf("\n\n");

}

//Hexdumps a byte array
void msg_hexdump(uint8_t *msg, size_t n){
    for(size_t i = 0; i <= (n/8); i++){
        for(int j = 0x0; j <= 0x7; j++){
            printf("%#4x ", msg[i*0x8 + j]);
        }
        printf("\n");
    }
}
