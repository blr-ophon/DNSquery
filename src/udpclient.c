#include "udpclient.h"

int main(int argc, char *argv[]){
    if(argc < 4){
        fprintf(stderr, "Usage: dnsq <DNS server> <hostname> <record type>");
        exit(EXIT_FAILURE);
    }

    //get address info
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

    //Creae socket with the first working address
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

    //Create a query with user provided info
    struct DNSmsg message;
    DNSmsg_createQuery(&message, argv[2], argv[3]);     
    

    //wrap query and send to DNS server
    uint8_t *query = DNSmsg_wrap(&message);     
    size_t query_size = DNSmsg_getWrappedSize(&message);
    int bytes_sent = sendto(sockfd, query, query_size, 0, p->ai_addr, p->ai_addrlen);
    if(bytes_sent < 0){
        fprintf(stderr, "sendto() error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Sent %d bytes\n", bytes_sent);


    //Receive response from DNS server
    uint8_t recv_buf[MSG_BUF_SIZE] = {0};
    int bytes_recv = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, 0, 0);
    if(bytes_recv < 0){
        fprintf(stderr, "recvfrom() error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Received %d bytes\n", bytes_recv);


    //Output to user
    msg_hexdump(recv_buf, bytes_recv);
    char answer_databuf[ANSWER_DATABUF_SIZE];
    struct DNSmsg answer = DNSmsg_unwrap(recv_buf, answer_databuf);
    DNSmsg_print(&answer);

    //Free memory
    DNSmsg_freeNames(&message);
    free(query);
    DNSmsg_freeNames(&answer);
    return 0;
}
