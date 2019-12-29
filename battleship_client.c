#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "error_handler.h"

#define MAX_CHAR_IP 16
#define MAX_CHAR_PORT 6
#define SOCKET_ERROR -1

void usage(char *prg_name);
void fatal(char *);
void debug(char *);
void *errorchecked_malloc(unsigned int);
void check_args(int, char *[], char *, char *);
int init_struct(struct sockaddr_in, char *, char *);

int main(int argc, char *argv[]) {








    // Allocazione dinamica nel Segment Heap dell'IP e porta del server.
    // I valori sono passati come argomenti e gestiti con il metodo check_args.
    char *IP = (char *) errorchecked_malloc(MAX_CHAR_IP); 
    char *port = (char *) errorchecked_malloc(MAX_CHAR_PORT);
    // Identificatore socket.
    int sockfd; 
    // Struttura contenente le informazioni di rete del server.
    struct sockaddr_in serveraddr;
    
    check_args(argc, argv, IP, port);   // Controllo argomenti.
    debug("Correct arguments.\n");

    init_struct(serveraddr, IP, port);    // Inizializzazione struttura del server.
    debug("IP and port assigned.\n");
    debug("Struct sockaddr_in initialized successfully.\n");

    // La memoria viene liberta.
    free(IP);
    free(port);

    // Creazione della socket.
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        fatal("Socket creation error.\n");
    debug("Socket created.\n");

    // Connessione al server (CR).
    if ((connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))) == SOCKET_ERROR)
        fatal("Connect to server failed.\n");
    debug("Connected to server.\n");

    // Chiusura della socket.
    close(sockfd);
      
    return 0; 
}

void usage(char *prg_name) {
    // Informazioni sull'avvio.
    printf("Usage: %s [-i] IP [-p] Port\n", prg_name);
    exit(0);
}

void check_args(int argc, char *argv[], char *ip, char *port) {
    int opt = 0;

    // Verifica che i parametri sono 2.
    if (argc < 2) 
        usage(argv[0]);

    // Lettura dei parametri.
    while ((opt = getopt (argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i':
                strcpy(ip, optarg); // Copia dei valori nelle stringhe.
                break;
            
            case 'p':
                strcpy(port, optarg);
                break;
            
            case '?':   // Verifica che gli argomenti sono corretti/sconosciuti.
                if (optopt == 'i' || optopt == 'p')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character \'%c\'.\n", optopt);

            default:
                exit(0);
        }
    }
    
    // Argomenti eccessivi inutili. 
    for(; optind < argc; optind++){      
        printf("Extra arguments: %s\n", argv[optind]);  
    } 
}

int init_struct(struct sockaddr_in srvaddr, char *IP, char *port) {
    // Puntatore usato per la conversione 'string' to 'int'.
    char *end;

    memset(&srvaddr, 0, sizeof(srvaddr)); 

    srvaddr.sin_family = AF_INET;                        // Famiglia.
    srvaddr.sin_addr.s_addr = inet_addr(IP);             // IP.
    srvaddr.sin_port = htons(strtol(port, &end, 10));    // Porta.
}