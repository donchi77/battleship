#include <stdio.h>                          // lezione lab 10 UA3 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h> 
bool a = true;
int len2, len, player1, x, y, x2, y2;

#define SOCKET_ERROR   ((int)-1)
#define SERVER_PORT 8080             // numero di porta del server
#define MAXSIZE 10                          // dimensione del messaggio

int main(int argc, char *argv[]){
  char indirizzoServer[] = "127.0.0.1";     // indirizzo del server 
  char messaggio[32] = "";                // messaggio da inviare 
  char buffer[MAXSIZE];                     // messaggio ricevuto 
  char ch, chMaiu[32] = "";                          // singolo carattere inviato e ricevuto 
  int ris, nwrite;                 
    
  int socketfd;                             // identificatore della socket
  struct sockaddr_in locale, remoto;        // dati dei socket 
  
  // settaggio del socket locale   
  locale.sin_family	     = AF_INET;
  locale.sin_addr.s_addr = htonl(INADDR_ANY);                      
  locale.sin_port	     = htons(0);
  // assegnazione parametri del server 
  remoto.sin_family	     = AF_INET;
  remoto.sin_addr.s_addr = inet_addr(indirizzoServer);
  remoto.sin_port		 = htons(SERVER_PORT);
  
  // impostazione del transport endpoint 
  printf("creazione del socket()\n");
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == SOCKET_ERROR){
    printf ("fallito socket(), errore: %d \"%s\"\n", errno, strerror(errno));
    return(1);
  }
 
  // leghiamo l'indirizzo al transport endpoint 
  printf("bind()\n");
  ris = bind(socketfd, (struct sockaddr*) &locale, sizeof(locale));
  if (ris == SOCKET_ERROR){
    printf ("fallito il bind(), errore: %d \"%s\"\n", errno, strerror(errno));
    fflush(stdout);  
    return(3);
  }
 
  // richiesta di connessione al server   
  printf("connect()\n");
  ris = connect(socketfd, (struct sockaddr*) &remoto, sizeof(remoto));
  if (ris == SOCKET_ERROR){
    printf("fallita la connect(), errore: %d \"%s\"\n", errno, strerror(errno));
    fflush(stdout);  
    return(4);
  }

	if (read(socketfd, &player1, 1) > 0){     
		if(player1 == 1){    
			printf("\nGiocatore 1\n"); 
		} else {
			printf("\nGiocatore 2\n"); 
		}
	} 
	if(player1 == 1){
		printf("\ninserire il nickname: ");
		scanf("%s",&messaggio);
		len = strlen(messaggio);
		write(socketfd, &len, 2); 
		write(socketfd, &messaggio, len); 
		printf("\nAttendendo l'altro giocatore ...");
		printf("\n");
		printf("\n");
		if (read(socketfd, &len2, 2) > 0){          
		}  
		if (read(socketfd, &chMaiu, len2) > 0){  
		}  
	} else {
		printf("\nAttendendo l'altro giocatore ...");
		printf("\n");
		printf("\n");
		if (read(socketfd, &len2, 2) > 0){          
		}  
		if (read(socketfd, &chMaiu, len2) > 0){  
		}  
		printf("\ninserire il nickname: ");
		scanf("%s",&messaggio);
		len = strlen(messaggio);
		write(socketfd, &len, 2); 
		write(socketfd, &messaggio, len); 
	}
       	
	while(a){
		if(player1 == 1){
			printf("\ninserire x: ");
			scanf("%d",&x);
			write(socketfd, &x, 1); 
			printf("\ninserire y: ");
			scanf("%d",&y);
			write(socketfd, &y, 1); 
			printf("\nAttendendo l'altro giocatore ...");
			printf("\n");
			printf("\n");
			if (read(socketfd, &x2, 1) > 0){          
				printf("\nx arrivata: %d\n", x2); 
			}
			if (read(socketfd, &y2, 1) > 0){          
				printf("\ny arrivata: %d\n", y2); 
			}
		} else {
			printf("\nAttendendo l'altro giocatore ...");
			printf("\n");
			printf("\n");
			if (read(socketfd, &x2, 1) > 0){          
				printf("\nx arrivata: %d\n", x2); 
			}
			if (read(socketfd, &y2, 1) > 0){          
				printf("\ny arrivata: %d\n", y2); 
			}
			printf("\ninserire x: ");
			scanf("%d",&x);
			write(socketfd, &x, 1); 
			printf("\ninserire y: ");
			scanf("%d",&y);
			write(socketfd, &y, 1); 
		}
	}

  // chiusura socket 
  close(socketfd);
  return(0);
}
