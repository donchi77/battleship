#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#include "error_handler.h"

#define MAX_CHAR_IP 16
#define MAX_CHAR_PORT 6
#define SOCKET_ERROR -1

void usage(char *prg_name);
void fatal(char *);
void debug(char *);
void *errorchecked_malloc(unsigned int);
void check_args(int, char *[], char *, char *);

int main(int argc, char *argv[]) {
    char *IP = (char *) errorchecked_malloc(MAX_CHAR_IP); 
    char *port = (char *) errorchecked_malloc(MAX_CHAR_PORT); 
    char *end;
    int sockfd; 
    struct sockaddr_in serveraddr;
    
    check_args(argc, argv, IP, port);
    debug("Correct arguments.\n");
    //TODO: init_struct(serveraddr);

    
    memset(&serveraddr, 0, sizeof(serveraddr)); 
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(*IP); 
    serveraddr.sin_port = htons(strtol(port, &end, 10));
    debug("IP and port assigned.\n");
    debug("Struct sockaddr_in initialized successfully.\n");
    free(IP);
    free(port);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
        fatal("Socket creation error.\n");
    
    if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) { 
        fatal("Connect to server failed.\n");
    }

    char c = 'c';
    send(sockfd, &c, 1, 0);
    recv(sockfd, &c, 1, 0);
    printf("car ricevuto: %c\n", c);

    close(sockfd);
      
    return 0; 
}

void usage(char *prg_name) {
    printf("Usage: %s [-i] IP [-p] Port\n", prg_name);
    exit(0);
}

void check_args(int argc, char *argv[], char *ip, char *port) {
    int opt = 0;

    if (argc < 2) 
        usage(argv[0]);

    while ((opt = getopt (argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i':
                strcpy(ip, optarg);
                break;
            
            case 'p':
                strcpy(port, optarg);
                break;
            
            case '?':
                if (optopt == 'i' || optopt == 'p')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character \'%c\'.\n", optopt);

            default:
                exit(0);
        }
    }
    
    for(; optind < argc; optind++){      
        printf("Extra arguments: %s\n", argv[optind]);  
    } 
}