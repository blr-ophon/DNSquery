#include "udpclient.h"

#define DNS_PORT "53"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Provide DNS server address\n");
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo *peer_addrs;
    int rv = getaddrinfo(argv[1], DNS_PORT, &hints, &peer_addrs);
    if(rv < 0){
        printf("%s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *p;
    for(p = peer_addrs; p != NULL; p = p->ai_next){
        int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd < 0){
            printf("getaddrinfo error: %d\n", errno);
            continue;
        }
        break;
    }

    //configure query 
    struct DNSmsg message;
    DNSmsg_configure(&message);     
    DNSmsg_print(&message);
    uint8_t *data = DNSmsg_wrap(&message);
    char answer_databuf[256];
    struct DNSmsg tested = DNSmsg_unwrap(data, answer_databuf);
    DNSmsg_print(&tested);
    //uint8_t *query = DNSmsg_wrap(&message);     




    //free(query);
    DNSmsg_freeNames(&message);
    return 0;
}
