#include "wrapper.h"

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

uint16_t get_LE_word(char *data, int *offset){
    uint16_t word = data[*offset] | ((uint16_t) data[(*offset)+1] << 8);
    *offset += 2;
    return word;
}

struct DNSmsg DNSmsg_unwrap(char *data, char *answer_databuf){
    struct DNSmsg unw_msg;
    memset(&unw_msg, 0, sizeof(struct DNSmsg));
    int offset = 0;

    //header
    unw_msg.header.id = get_LE_word(data, &offset);
    unw_msg.header.flags = get_LE_word(data, &offset);
    unw_msg.header.qdcount = get_LE_word(data, &offset);
    unw_msg.header.ancount = get_LE_word(data, &offset);
    unw_msg.header.nscount = get_LE_word(data, &offset);
    unw_msg.header.arcount = get_LE_word(data, &offset);

    return unw_msg;
}
