#include "wrapper.h"

static uint16_t to_bigendian16(uint16_t num){
    return (num >> 8) | (num << 8);
}

static uint32_t to_bigendian32(uint32_t num){
    return ((num>>24)&0xff) | ((num<<8)&0xff0000) | 
        ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

static uint32_t get_LE_Dword(uint32_t *dword, int *offset){
    uint32_t num = *dword;
    num = ((num>>24)&0xff) | ((num<<8)&0xff0000) | 
        ((num>>8)&0xff00) | ((num<<24)&0xff000000);
    *offset += 4;
    return num;
}

static uint16_t get_LE_word(uint16_t *word, int *offset){
    uint16_t num = *word;
    num = (num >> 8) | (num << 8);
    *offset += 2;
    return num;
}

size_t DNSmsg_getWrappedSize(struct DNSmsg *message){
    size_t q_name_len = message->question.name == NULL? 0 : strlen(message->question.name) + 1;
    size_t a_name_len = message->answer.name == NULL? 0 : strlen(message->answer.name) + 1;
    size_t msg_size = sizeof(struct DNSmsg) + q_name_len + a_name_len
         + message->answer.rdlength - 2*sizeof(void *);
    //printf("%d\n", msg_size);
    return msg_size;
}

uint8_t *DNSmsg_wrap(const struct DNSmsg *const message){
    size_t q_name_len = message->question.name == NULL? 0 : strlen(message->question.name) + 1;
    size_t a_name_len = message->answer.name == NULL? 0 : strlen(message->answer.name) + 1;

    //Size = struct + name strings + data - 2*(pointer size)
    size_t msg_size = sizeof(struct DNSmsg) + q_name_len + a_name_len
         + message->answer.rdlength - 2*sizeof(void *);
    uint8_t *wrapped_msg = (uint8_t*) malloc(msg_size);

    struct DNSmsg BE_msg;
    memset(&BE_msg, 0, sizeof(struct DNSmsg));

    uint32_t offset = 0;
    //Header 
    BE_msg.header.id = to_bigendian16(message->header.id);
    BE_msg.header.flags = to_bigendian16(message->header.flags);
    BE_msg.header.qdcount = to_bigendian16(message->header.qdcount);
    BE_msg.header.ancount = to_bigendian16(message->header.ancount);
    BE_msg.header.nscount = to_bigendian16(message->header.nscount);
    BE_msg.header.arcount = to_bigendian16(message->header.arcount);
    memcpy(&wrapped_msg[offset], &BE_msg.header, sizeof(struct DNSmsg_header));
    offset += sizeof(struct DNSmsg_header);

    //Question
    if(message->question.name != NULL){
        memcpy(&wrapped_msg[offset], message->question.name, q_name_len);
        offset += q_name_len;
    }

    BE_msg.question.qtype = to_bigendian16(message->question.qtype);
    BE_msg.question.qclass = to_bigendian16(message->question.qclass);
    memcpy(&wrapped_msg[offset], &BE_msg.question.qtype, 2);
    offset += 2;
    memcpy(&wrapped_msg[offset], &BE_msg.question.qclass, 2);
    offset += 2;


    //Answer
    if(message->answer.name != NULL){
        memcpy(&wrapped_msg[offset], message->answer.name, a_name_len);
        offset += a_name_len;
    }

    BE_msg.answer.rtype = to_bigendian16(message->answer.rtype);
    BE_msg.answer.rclass = to_bigendian16(message->answer.rclass);
    memcpy(&wrapped_msg[offset], &BE_msg.answer.rtype, 2);
    offset += 2;
    memcpy(&wrapped_msg[offset], &BE_msg.answer.rclass, 2);
    offset += 2;

    BE_msg.answer.ttl = to_bigendian32(message->answer.ttl);
    BE_msg.answer.rdlength = to_bigendian16(message->answer.rdlength);
    memcpy(&wrapped_msg[offset], &BE_msg.answer.ttl, 4);
    offset += 4;
    memcpy(&wrapped_msg[offset], &BE_msg.answer.rdlength, 2);
    offset += 2;

    memcpy(&wrapped_msg[offset], message->answer.rdata, message->answer.rdlength);

    return wrapped_msg;
}

struct DNSmsg DNSmsg_unwrap(uint8_t *data, char *answer_databuf){
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
    qname_buf[i] = data[offset++];  //append '\0'
    unw_msg.question.name = NULL;
    if(i != 0){ //malloc(0) does not necessarilly returns NULL pointer
        unw_msg.question.name = (char*) malloc(i);
        strncpy(unw_msg.question.name, qname_buf, i);
    }
    
    unw_msg.question.qtype = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.question.qclass = get_LE_word((uint16_t*)&data[offset], &offset);


    //ANSWER
    if(!(data[offset] & 0xc0)){    //if not a pointer/ if it's a string
        char rname_buf[255];
        i = 0;
        for(; data[offset] != '\0'; i++){
            rname_buf[i] = data[offset];
            offset ++;
        }
        qname_buf[i] = data[offset++];  //append '\0'
        if(i != 0){ //malloc(0) does not necessarilly returns NULL pointer
            unw_msg.answer.name = (char*) malloc(i);
            strncpy(unw_msg.answer.name, rname_buf, i);
        }
    }else{
        //rest of the bits and next byte are a pointer
        offset += 2;
    }

    unw_msg.answer.rtype = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.answer.rclass = get_LE_word((uint16_t*)&data[offset], &offset);
    unw_msg.answer.ttl = get_LE_Dword((uint32_t*)&data[offset], &offset);
    unw_msg.answer.rdlength = get_LE_word((uint16_t*)&data[offset], &offset);


    //DATA
    for(int i = 0; i < unw_msg.answer.rdlength; i++){
        answer_databuf[i] = data[offset++];
    }
    unw_msg.answer.rdata = answer_databuf;

    return unw_msg;
}
