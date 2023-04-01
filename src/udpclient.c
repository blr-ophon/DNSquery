#include "udpclient.h"

#define DNS_PORT "53"

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Provide DNS server address\n");
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo *peer_addrs;
    int rv = getaddrinfo(argv[1], DNS_PORT, &hints, &peer_addrs);
    if(rv < 0){
        fprintf(stderr, "%s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct addrinfo *p;
    for(p = peer_addrs; p != NULL; p = p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd < 0){
            fprintf(stderr, "getaddrinfo error: %d\n", errno);
            continue;
        }
        break;
    }
    if(p == NULL){
        fprintf(stderr, "Failed creating sockets\n");
        exit(EXIT_FAILURE);
    }

    //configure query 
    struct DNSmsg message;
    DNSmsg_configure(&message);     
    DNSmsg_print(&message);

    //for test
    //DNSmsg_print(&message);
    //uint8_t *data = DNSmsg_wrap(&message);
    //char answer_databuf[256];
    //struct DNSmsg tested = DNSmsg_unwrap(data, answer_databuf);
    //DNSmsg_print(&tested);
    
    //wrap query and send to DNS server
    uint8_t *query = DNSmsg_wrap(&message);     
    size_t query_size = DNSmsg_getWrappedSize(&message);
    int bytes_sent = sendto(sockfd, query, query_size, 0, p->ai_addr, p->ai_addrlen);
    if(bytes_sent < 0){
        fprintf(stderr, "sendto() error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Sent %d bytes\n", bytes_sent);
    msg_hexdump(query, query_size);

    //Receive response from DNS server
    uint8_t recv_buf[1024] = {0};
    int bytes_recv = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, 0, 0);
    if(bytes_recv < 0){
        fprintf(stderr, "recvfrom() error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Received %d bytes\n", bytes_recv);
    msg_hexdump(recv_buf, bytes_recv);

    char answer_databuf[256];
    struct DNSmsg tested = DNSmsg_unwrap(recv_buf, answer_databuf);
    DNSmsg_print(&tested);


    free(query);
    DNSmsg_freeNames(&message);
    return 0;
}
