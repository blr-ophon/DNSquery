#include "wrapper.h"

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

    uint32_t offset = 0;
    //Header 
    BE_msg.header.id = to_bigendian16(message->header.id);
    BE_msg.header.flags = to_bigendian16(message->header.id);
    BE_msg.header.qdcount = to_bigendian16(message->header.id);
    BE_msg.header.ancount = to_bigendian16(message->header.id);
    BE_msg.header.nscount = to_bigendian16(message->header.id);
    BE_msg.header.arcount = to_bigendian16(message->header.id);
    memcpy(&wrapped_msg[offset], &BE_msg.header, sizeof(struct DNSmsg_header));
    offset += sizeof(struct DNSmsg_header);

    //Question
    int q_namelen = strlen(message->question.name);
    memcpy(&wrapped_msg[offset], message->question.name, q_namelen);
    offset += q_namelen;

    BE_msg.question.qtype = to_bigendian16(message->question.qtype);
    BE_msg.question.qclass = to_bigendian16(message->question.qclass);
    memcpy(&wrapped_msg[offset], &BE_msg.question.qtype, 32);
    offset += 32;


    //Answer
    if(message->answer.name != NULL){
        int a_namelen = strlen(message->answer.name);
        memcpy(&wrapped_msg[offset], message->answer.name, a_namelen);
        offset += a_namelen;
    }

    BE_msg.answer.rtype = to_bigendian16(message->answer.rtype);
    BE_msg.answer.rclass = to_bigendian16(message->answer.rclass);
    memcpy(&wrapped_msg[offset], &BE_msg.answer.rtype, 32);
    offset += 32;

    BE_msg.answer.ttl = to_bigendian32(message->answer.ttl);
    BE_msg.answer.rdlength = to_bigendian32(message->answer.rdlength);
    memcpy(&wrapped_msg[offset], &BE_msg.answer.rtype, 48);
    offset += 48;

    memcpy(&wrapped_msg[offset], message->answer.rdata, message->answer.rdlength);

    return wrapped_msg;
}
uint16_t get_LE_Dword(uint32_t *dword, int *offset){
    uint32_t num = *dword;
    num = ((num>>24)&0xff) | ((num<<8)&0xff0000) | 
        ((num>>8)&0xff00) | ((num<<24)&0xff000000);
    *offset += 4;
    return num;
}

uint16_t get_LE_word(uint16_t *word, int *offset){
    uint16_t num = *word;
    num = (num >> 8) | (num << 8);
    *offset += 2;
    return num;
}

struct DNSmsg DNSmsg_unwrap(char *data, char *answer_databuf){
    struct DNSmsg unw_msg;
    memset(&unw_msg, 0, sizeof(struct DNSmsg));
    int offset = 0;

    //HEADER
    unw_msg.header.id = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.header.flags = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.header.qdcount = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.header.ancount = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.header.nscount = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.header.arcount = get_LE_word((uint16_t*)&data[offset], &offset);


    //QUESTION
    char qname_buf[255];
    int i;
    for(i = 0; data[offset] != '\0'; i++){
        qname_buf[i] = data[offset];
        offset ++;
    }
    unw_msg.question.name = NULL;
    if(i != 0){ //malloc(0) does not necessarilly returns NULL pointer
        unw_msg.question.name = (char*) malloc(i);
        strncpy(unw_msg.question.name, qname_buf, i);
    }
    
    unw_msg.question.qtype = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.question.qclass = get_LE_word((uint16_t*)&data[offset], &offset);


    //ANSWER
    char rname_buf[255];
    i = 0;
    for(; data[offset] != '\0'; i++){
        rname_buf[i] = data[offset];
        offset ++;
    }
    unw_msg.answer.name = NULL;
    if(i != 0){ //malloc(0) does not necessarilly returns NULL pointer
        unw_msg.answer.name = (char*) malloc(i);
        strncpy(unw_msg.answer.name, rname_buf, i);
    }

    unw_msg.answer.rtype = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.answer.rclass = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.answer.ttl = get_LE_Dword((uint32_t*)&data[offset], &offset);
    unw_msg.answer.rdlength = get_LE_word((uint16_t*)&data[offset], &offset);


    //DATA
    for(int i = 0; i < unw_msg.answer.rdlength; i++){
        answer_databuf[i] = data[offset++];
    }

    return unw_msg;
}
