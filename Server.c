#include <sys/types.h>                   
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 

#define SERVER_PORT 50000

bool check = true;

void player1(int ,int);
void player2(int ,int);

int main (int argc, char **argv) {
	int socketAttesa, client_len, socketComunica;
	int asciLetto;                       
	struct sockaddr_in server, client;
	
	printf ("socket()\n");
	if((socketAttesa = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("chiamata della system call socket() fallita");
		return(1);
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);
	
	printf ("bind()\n");
	if (bind(socketAttesa, (struct sockaddr*)&server, sizeof server) == -1){
		perror("chiamata della bind() fallita");
		return(2);
	}
	
	printf ("listen()\n");
	listen(socketAttesa, 2);           

  	printf ("accept()\n");
  	for(int i = 0 ; i < 2 ; i++){
	  client_len = sizeof(client);
	  if ((socketComunica = accept(socketAttesa, (struct sockaddr*)&client, &client_len)) < 0)
	  {
	    perror("connessione non accettata");
	    return(3);
	  }
	}
	
	printf("KEKW");
	
	while(check){
		printf("monkaS");
		player1(socketComunica, socketComunica);
		player2(socketComunica, socketComunica);
		
		//controllo caso di vittoria
	}
	
	close(socketComunica);
	close(socketComunica);
}

void player1(int in ,int out){
	char p1;
	
	printf("monkaS_1");
	
  	if(recv(in, &p1, 1, 0) > 0){
	    printf("\nil carattere ricevuto da p1 e': %c", p1);
	    send(out, &p1, 1, 0);
  	}
}

void player2(int in ,int out){
	char p2;
	
	printf("monkaS_2");
	
  	if(recv(in, &p2, 1, 0) > 0){
	    printf("\nil carattere ricevuto da p2 e': %c ", p2);
	    send(out, &p2, 1, 0);
  	}
}
