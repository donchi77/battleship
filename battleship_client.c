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
#define SHIPS 8

void check_args(int, char *[], char *, char *);
void usage(char *prg_name);
void fatal(char *);
void debug(char *);
void *errorchecked_malloc(unsigned int);

void clear_console();
void wait_player();
void newline(int);

int start(char *);
void show_info();

void init_tables();
void set_coords();
int insertXY(char);
void print_tables();

void connect_to_server(char *, char *, char *);
int init_struct(struct sockaddr_in *, char *, char *);
void send_coords(int, int *, int *, char *);
int recv_check(int, char *, char *);
void recv_coords(int, int *, int *, int *);
int verify(int, int *, int *);

int your_table[DIM_TABLE][DIM_TABLE];
int enemy_table[DIM_TABLE][DIM_TABLE];

int main(int argc, char *argv[]) {
    char *nickname = (char *) errorchecked_malloc(MAX_CHAR_USR); // Nickname del giocatore.
    // Allocazione dinamica nel Segment Heap dell'IP e porta del server.
    // I valori sono passati come argomenti e gestiti con il metodo check_args.
    char *IP = (char *) errorchecked_malloc(MAX_CHAR_IP); 
    char *port = (char *) errorchecked_malloc(MAX_CHAR_PORT);
    
    check_args(argc, argv, IP, port);   // Controllo argomenti.
    debug("Correct arguments. Game starting.\n\n");
    // -----> FASE 1: COORDINATE
    if (start(nickname))    // Nickname e regolamento.
        show_info();

    init_tables();
    set_coords();
    print_tables();
 
    // -----> FASE 2: MATCHMAKING
    connect_to_server(IP, port, nickname);
      
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

int start(char *player) {
    char ch = 'n';
    int len = 0;

    printf("]::::::::::::::::::::::::[ BATTAGLIA NAVALE ]::::::::::::::::::::::::[\n\n");
    printf("\tInserire nickname giocatore: ");
    if (fgets(player, MAX_CHAR_USR, stdin) == NULL)
        fatal("Input error.");
    player[strcspn(player, "\n")] = 0;

    printf("\tBenvenuto %s\n", player);
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

void init_tables() {
    // Se 0 allora acqua, se 1 allora nave.
    memset(your_table, 0, DIM_TABLE*(sizeof(your_table[0])));
    memset(enemy_table, 2, DIM_TABLE*(sizeof(enemy_table[0])));
    
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
            x = insertXY('x');
            y = insertXY('y');

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

int insertXY(char which) {
    int value;

    do {
        printf("\tInserire %c: ", which);
        scanf("%d", &value);
    } while(value < 0 || value > DIM_TABLE - 1);

    return value;
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

    wait_player();

    printf("\n----> Il tabellone avversario: \n");
    newline(DIM_TABLE);
    for (i = 0; i < DIM_TABLE; i++) {
        for (j = 0; j < DIM_TABLE; j++) {
                if (enemy_table[i][j] == 2) 
                    j == 0 ? printf("| . ") : printf(" . ");
                else if (enemy_table[i][j] == 1)
                    j == 0 ? printf("| X ") : printf(" X ");
                else
                    j == 0 ? printf("| / ") : printf(" / ");

            printf("|");
        }
        newline(DIM_TABLE);
    }

    wait_player();
}

void connect_to_server(char *IP, char *port, char *nickname) {
    // Identificatore socket.
    int sockfd; 
    // Lunghezza nickname.
    int len = strlen(nickname);
    // Chi inizia.
    int isFirst = 0;
    // Struttura contenente le informazioni di rete del server.
    struct sockaddr_in serveraddr;
    struct sockaddr_in *ptr_srvstrc = &serveraddr;
    // Nickname avversario.
    int len_enemy = 0;
    char *nick_enemy = (char *) errorchecked_malloc(MAX_CHAR_USR);
    // Navi affondate.
    int ships_destroyed = 8;
    // Fine gioco.
    int endgame = 0;

    if (init_struct(ptr_srvstrc, IP, port)) {   // Inizializzazione struttura del server.
        debug("IP and port assigned.\n");
        debug("Struct sockaddr_in initialized successfully.\n");
    }   
        
    // La memoria viene liberta.
    free(IP);
    free(port);
    debug("Dinamic strings deallocated.\n");

    printf("\n]::::::[ FASE 2: MATCHMAKING ]::::::[\n\n");

    // Creazione della socket.
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        fatal("Socket creation error.\n");
    debug("Socket created.\n");

    // Connessione al server (CR).
    if ((connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))) == SOCKET_ERROR)
        fatal("Connect to server failed.\n");
    debug("Connected to server.\n");

    if ((send(sockfd, &len, 2, 0)) == SOCKET_ERROR) 
        fatal("Impossible sending nickname length.\n");
    else {
        debug("Nickname length sent.\n");
        if ((send(sockfd, nickname, len, 0)) == SOCKET_ERROR)
            fatal("Impossible sending nickname.\n"); 
        else {
            debug("Nickane sent.\n");
            
            if ((recv(sockfd, &isFirst, 1, 0)) == SOCKET_ERROR)
                fatal("Impossible receiving who starts.\n");
            debug("Received who starts.\n");
        }  
    }

    if (isFirst)
        printf("\n\t%s, inizi per primo!\n", nickname);
    else
        printf("\n\t%s, inizi per secondo!\n", nickname);
    
    printf("\n\tIn attesa dell'avversario...\n");
        
    if (recv(sockfd, &len_enemy, 2, 0) == SOCKET_ERROR)
        fatal("Impossible reading other player nickname length.\n");
    else {
        debug("Nickname length received.\n");

        if (recv(sockfd, nick_enemy, len_enemy, 0) == SOCKET_ERROR)
            fatal("Impossible reading other player nickname.\n");
        
        debug("Nickname received.");
        nick_enemy[strcspn(nick_enemy, "\n")] = 0;
        printf("Sei contro %s!\n", nick_enemy);

        wait_player();
    }

    printf("\n]::::::[ FASE 3: BATTAGLIA ]::::::[\n\n");
    while (ships_destroyed != 0 && !endgame) {
        int hit = 0;
        int x = 0, y = 0;

        if (isFirst) {
            send_coords(sockfd, &x, &y, nickname);

            hit = recv_check(sockfd, nickname, nick_enemy);

            endgame = verify(hit, &x, &y);

            if (!endgame)
                recv_coords(sockfd, &x, &y, &ships_destroyed);
        } else {
            if (!endgame){
                recv_coords(sockfd, &x, &y, &ships_destroyed);
            	if(ships_destroyed != 0) 
			    send_coords(sockfd, &x, &y, nickname);

			    hit = recv_check(sockfd, nickname, nick_enemy);
			    
			    endgame = verify(hit, &x, &y);
		} else {
			endgame = 1;
		}
        }

        print_tables();
    }

    if (ships_destroyed)
        printf("\t\n\nCOMPLIMENTI HAI VINTO!!\n");
    else
        printf("\t\n\nHAI PERSO!\n");

    free(nick_enemy);
    free(nickname);

    wait_player();
    // Chiusura della socket.
    close(sockfd);
}

int init_struct(struct sockaddr_in *ptr_srv, char *IP, char *port) {
    // Puntatore usato per la conversione 'string' to 'int'.
    char *end;
    memset(ptr_srv, 0, sizeof(ptr_srv)); 

    ptr_srv -> sin_family = AF_INET;                        // Famiglia.
    ptr_srv -> sin_addr.s_addr = inet_addr(IP);             // IP.
    ptr_srv -> sin_port= htons(strtol(port, &end, 10));     // Porta.

    return 1;
}

void send_coords(int sockfd, int *x, int *y, char *nick) {
    printf("\t%s tocca a te!\n", nick);
    printf("\tInserisci le coordinate da attaccare.\n\n");
    
    *x = insertXY('x');
    *y = insertXY('y');
    if (send(sockfd, x, 1, 0) == SOCKET_ERROR)
        fatal("Impossible sending X.\n");
    if (send(sockfd, y, 1, 0) == SOCKET_ERROR)
        fatal("Impossible sending Y.\n");
    debug("X & Y sent.\n");
}

int recv_check(int sockfd, char *nick, char *nick_en) {
    int hit = 0;

    printf("\t\n%s attendi la risposta di %s.\n", nick, nick_en);
    if (recv(sockfd, &hit, 1, 0) == SOCKET_ERROR)
        fatal("Impossible receiving hit.\n");
    debug("Hit received.\n");

    return hit;
}

void recv_coords(int sockfd, int *x, int *y, int *ships_des) {
    if (recv(sockfd, x, 1, 0) == SOCKET_ERROR)
        fatal("Impossible reading X.\n");
    if (recv(sockfd, y, 1, 0) == SOCKET_ERROR)
        fatal("Impossible reading Y.\n");
    debug("X & Y received.\n");
    int check = 0;
    if (your_table[*x][*y]){
        printf("\t\nNave colpita!\n");
        your_table[*x][*y] = 0;
        *ships_des=*ships_des-1;
        if (*ships_des > 0)
            check = 1;
        else 
            check = 2;

        if (send(sockfd, &check, 1, 0) == SOCKET_ERROR)
            fatal("Impossible sending check.\n");
        debug("Check sent.\n");
    } else {
        printf("\t\nNave mancata!\n");

        if (send(sockfd, &check, 1, 0) == SOCKET_ERROR)
            fatal("Impossible sending check.\n");
        debug("Check sent.\n");
    }
}

int verify(int hit, int *x, int *y) {
    if (hit == 0) {
        enemy_table[*x][*y] = 0;
        printf("\t\nMancato!\n");
        return 0;
    }

    if (hit == 1) {
        enemy_table[*x][*y] = 1;
        printf("\t\nColpito!\n");
        return 0;
    }
        
    if (hit == 2) {
        printf("\t\nHai vinto!\n");
        return 1;
    }
}
