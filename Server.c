#include <sys/types.h>                   
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 
#include <string.h> 

#define SERVER_PORT 8080

int action(int ,int);
void getName(int ,int);

int main (int argc, char **argv) {
	int socketC, client_len, socketD[2], player1 = 1, move = 0;   //player1 = stabilire chi e il giocatore1 - move = controllo in caso di vittoria                    
	struct sockaddr_in server, client;
	bool game = true;	//controllare lo stato del gioco (in gioco / concluso)
	
	//apertura socket
	printf ("socket()\n");
	if((socketC = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("\nErrore creazione socket");
		return(1);
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);
	
	//bind
	printf ("bind()\n");
	if (bind(socketC, (struct sockaddr*)&server, sizeof server) == -1){
		perror("\nErrore bind");
		return(2);
	}
	
	//listen
	printf ("listen()\n");
	listen(socketC, 2);           

	//accept
  	printf ("accept()\n");
  	for(int i = 0 ; i < 2 ; i++){
		client_len = sizeof(client);
		if ((socketD[i] = accept(socketC, (struct sockaddr*)&client, &client_len)) < 0)
		{
			perror("\nErrore accept");
			return(3);
		}
		
		send(socketD[i], &player1, 1, 0);	//Invio 1 G1, 0 G2
		player1--;		
	}
	
	//Scambio dei nomi
	getName(socketD[0] , socketD[1]);
	getName(socketD[1] , socketD[0]);

	while(game){
		move = action(socketD[0], socketD[1]);	//mossa giocatore 1
		if(move == 2){
			game =false;
			printf("\nfine game\n");

			move = 0;
		}
		
		if(move != 0){ 
			move = action(socketD[1], socketD[0]);	//mossa giocatore 2
			if(move == 2){
				game =false;
				printf("\nfine game\n");

				move = 0;
			}
		}
	}
	
	//chiusura socket
	close(socketD[0]);
	close(socketD[1]);
	close(socketC);

	return 0; 
}

//azioni client (invio coordinate)
int action(int in ,int out){
	int x, y, shot; 	
	
  	if(recv(in, &x, 1, 0) < 0){
		perror("Errore nella ricezione dei dati");
  	}

	if(recv(in, &y, 1, 0) < 0){
		perror("Errore nella ricezione dei dati");
	} 

	if(send(out, &x, 1, 0) < 0){
		perror("Errore nel'invio dei dati");
	}
	if(send(out, &y, 1, 0) < 0){
		perror("Errore nel'invio dei dati");
	}

	if(recv(out, &shot, 1, 0) < 0){
		perror("Errore nella ricezione dei dati");
	} else {
		if(send(in, &shot, 1, 0) < 0){
			perror("Errore nel'invio dei dati");
		}
	}
	
	if(shot == 2){
		return 2;	//uno dei client ha perso
	} else {
		return 1;
	}
}

//ricezione e invio dei nickname
void getName(int in ,int out){
	int nameLen;	//lunghezza nome
	char buffer[32] = "";	//buffer in cui viene salvato temporaneamente il nome
	
  	if(recv(in, &nameLen, 2, 0) < 0){
		perror("Errore nella ricezione dei dati");
  	}
	if(recv(in, &buffer, nameLen, 0) < 0){
		perror("Errore nella ricezione dei dati");
  	}

	if(send(out, &nameLen, 2, 0) < 0){	
		perror("Errore nel'invio dei dati");
	}
	if(send(out, &buffer, nameLen, 0) < 0){
		perror("Errore nel'invio dei dati");
	}
}
