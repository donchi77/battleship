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
#include "layout_handler.h"

#define MAX_CHAR_IP 16
#define MAX_CHAR_PORT 6
#define MAX_CHAR_USR 32
#define SOCKET_ERROR -1
#define DIM_TABLE 9
#define SHIPS 6

void usage(char *prg_name);
void fatal(char *);
void debug(char *);
void *errorchecked_malloc(unsigned int);

void clear_console();
void wait_player();
void newline(int );

void check_args(int, char *[], char *, char *);
int start(char []);
void show_info();

void init_tables();
void set_coords();
void print_tables();

int init_struct(struct sockaddr_in *, char *, char *);

int your_table[DIM_TABLE][DIM_TABLE];
int enemy_table[DIM_TABLE][DIM_TABLE];

int main(int argc, char *argv[]) {
    char nickname[MAX_CHAR_USR]; // Nickname del giocatore.
    // Allocazione dinamica nel Segment Heap dell'IP e porta del server.
    // I valori sono passati come argomenti e gestiti con il metodo check_args.
    char *IP = (char *) errorchecked_malloc(MAX_CHAR_IP); 
    char *port = (char *) errorchecked_malloc(MAX_CHAR_PORT);
    // Identificatore socket.
    int sockfd; 
    // Struttura contenente le informazioni di rete del server.
    struct sockaddr_in serveraddr;
    struct sockaddr_in *ptr_srvstrc = &serveraddr;
    
    check_args(argc, argv, IP, port);   // Controllo argomenti.
    debug("Correct arguments. Game starting.\n\n");

    if (start(nickname))    // Nickname e regolamento.
        show_info();

    init_tables();
    set_coords();
    print_tables();

    if (init_struct(ptr_srvstrc, IP, port)) {   // Inizializzazione struttura del server.
        debug("IP and port assigned.\n");
        debug("Struct sockaddr_in initialized successfully.\n");
    }   
        
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

int start(char player[]) {
    char ch = 'n';

    printf("]::::::::::::::::::::::::[ BATTAGLIA NAVALE ]::::::::::::::::::::::::[\n\n");
    printf("\tInserire nickname giocatore: ");
    fgets(player, MAX_CHAR_USR, stdin);

    printf("\tBenvenuto ");
    puts(player);
    printf("\tDesideri leggere il regolamento? [y/n]: ");
    ch = getchar();

    if (ch == 'y' || ch =='Y')
        show_info();
    else if (ch == 'n' || ch == 'N')
        return 0;

    return 0;
}

void show_info() {
    printf("\n]::::::[ REGOLAMENTO ]::::::[\n\n");

    printf("----> Come giocare.\n");
    printf("\t1) Impostare le coordinate delle navi.\n");
    printf("\t2) Selezionare le coordinate da attaccare.\n");
    printf("\t3) Sperare che l'altro non colpisca le tue navi.\n\n");
    printf("----> Come leggerlo.\n");
    printf("\t- / : acqua.\n");
    printf("\t- = : nave.\n");
    printf("\t- X : nave affondata.\n");
    printf("\t- . : ancora da colpire.\n\n");
    printf("----> Come vincere/perdere.\n");
    printf("\t1) Se le tutte le navi nemiche sono affondate, complimenti.\n");
    printf("\t2) In caso contrario, intuiscilo.\n\n");
    printf("----> Oggetti a disposizione.\n");
    printf("\ta) Cinque navi. Ogni nave occupa un quadrato.\n");
    printf("\tb) Due tabelloni: il tuo e il suo.\n\n");

    printf("\tBUONA FORTUNA\n");
    wait_player();
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

int init_struct(struct sockaddr_in *ptr_srv, char *IP, char *port) {
    // Puntatore usato per la conversione 'string' to 'int'.
    char *end;
    memset(ptr_srv, 0, sizeof(ptr_srv)); 

    ptr_srv -> sin_family = AF_INET;                        // Famiglia.
    ptr_srv -> sin_addr.s_addr = inet_addr(IP);             // IP.
    ptr_srv -> sin_port= htons(strtol(port, &end, 10));    // Porta.

    return 1;
}

void init_tables() {
    // Se 0 allora acqua, se 1 allora nave.
    memset(your_table, 0, DIM_TABLE*(sizeof(your_table[0])));
    memset(enemy_table, 0, DIM_TABLE*(sizeof(enemy_table[0])));
    
    debug("Matrix initialized with 0s.\n");
}

void set_coords() {
    int ships_remains = SHIPS;

    printf("\n]::::::[ FASE 1: COORDINATE ]::::::[\n\n");

    printf("\tInserire le coordinate delle tue navi.\n\n");
    while(ships_remains > 0) {
        int x = 1, y = 1, isOkay = 0;

        printf("\tNavi rimaste: %d.\n", ships_remains);

        while (!isOkay) {
            do {
                printf("\tInserire la %d nave [x]: ", ships_remains);
                scanf("%d", &x);
            } while(x < 0 || x > DIM_TABLE - 1);

            do {
                printf("\tInserire la %d nave [y]: ", ships_remains);
                scanf("%d", &y);
            } while(y < 0 || y > DIM_TABLE - 1);

            if (your_table[x][y]) {
                printf("\tCoordinata gia' occupata!\n");
                isOkay = 0;
            } else {
                your_table[x][y] = 1;
                isOkay = 1;
                printf("\tNave impostata!\n\n");
            }
        }

        ships_remains--;
    }

    debug("Ships setted.\n");
}

void print_tables() {
    printf("\n]::::::[ TABELLONI ]::::::[\n\n");
    printf("----> Il tuo tabellone: \n");
    int i, j;
    newline(DIM_TABLE);
    for (i = 0; i < DIM_TABLE; i++) {
        for (j = 0; j < DIM_TABLE; j++) {
            if (your_table[i][j]) 
                j == 0 ? printf("| = ") : printf(" = ");
            else
                j == 0 ? printf("| / ") : printf(" / ");

            printf("|");
        }
        newline(DIM_TABLE);
    }

    printf("\n----> Il tabellone avversario: \n");
    newline(DIM_TABLE);
    for (i = 0; i < DIM_TABLE; i++) {
        for (j = 0; j < DIM_TABLE; j++) {
                j == 0 ? printf("| . ") : printf(" . ");

            printf("|");
        }
        newline(DIM_TABLE);
    }
}
