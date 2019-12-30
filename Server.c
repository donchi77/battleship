#include <sys/types.h>                   
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 
#include <string.h> 

#define SERVER_PORT 8081

bool check = true;
char namePlayer1[32] = "", namePlayer2[32] = "";

void action(int ,int);
void getName(int ,int, int);
//void getName1(int ,int);
//void getName2(int ,int);

int main (int argc, char **argv) {
	int socketC, client_len, socketD[2];                      
	struct sockaddr_in server, client;
	
	printf ("socket()\n");
	if((socketC = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("chiamata della system call socket() fallita");
		return(1);
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);
	
	printf ("bind()\n");
	if (bind(socketC, (struct sockaddr*)&server, sizeof server) == -1){
		perror("chiamata della bind() fallita");
		return(2);
	}
	
	printf ("listen()\n");
	listen(socketC, 2);           

  	printf ("accept()\n");
  	for(int i = 0 ; i < 2 ; i++){
	  client_len = sizeof(client);
	  if ((socketD[i] = accept(socketC, (struct sockaddr*)&client, &client_len)) < 0)
	  {
	    perror("connessione non accettata");
	    return(3);
	  }
	}

	getName(socketD[0], socketD[1], 1);
	getName(socketD[1], socketD[0], 2);
	printf("\n");

	printf("\nIl nome di p1 e': %s", namePlayer1);
	printf("\nIl nome di p2 e': %s", namePlayer2);

	printf("\n");
	printf("\n");

	while(check){
		action(socketD[0], socketD[1]);
		action(socketD[1], socketD[0]);

		//controllo caso di vittoria
	}
	
	close(socketD[0]);
	close(socketD[1]);
}

void action(int in ,int out){
	char a;
	
  	if(recv(in, &a, 1, 0) > 0){
	    printf("\nil carattere ricevuto da p1 e': %c", a);
	    send(out, &a, 1, 0);
  	}
}

void getName(int in ,int out, int saveName){
	int nameLen;
	char buffer[32] = "";
	
  	if(recv(in, &nameLen, 2, 0) > 0){
	    printf("\nLunghezza nome di p%d e': %d",saveName, nameLen);
  	}
	fflush(stdout);

	if(recv(in, &buffer, nameLen, 0) > 0){
		printf("\nIl buffer ora e': %s", buffer);
  	}
	fflush(stdout);

	send(out, &nameLen, 2, 0);
	fflush(stdout);
	send(out, &buffer, nameLen, 0);
	fflush(stdout);	

	if(saveName == 1){
		strcpy(namePlayer1, buffer);
	} else {
		strcpy(namePlayer2, buffer);
	}
}

/*void getName1(int in ,int out){
	int nameLen;
	char buffer1[32];
	
  	if(recv(in, &nameLen, 2, 0) > 0){
	    printf("\nLunghezza nome di p1 e': %d", nameLen);
  	}

	if(recv(in, &buffer1, nameLen, 0) > 0){
	    printf("\nIl nome di p1 e': %s", buffer1);
  	}

	send(out, &nameLen, 2, 0);
	send(out, &buffer1, nameLen, 0);

	strcpy(namePlayer1, buffer1);
	fflush(stdout);
}

void getName2(int in ,int out){
	int nameLen;
	char buffer2[32];
	
  	if(recv(in, &nameLen, 2, 0) > 0){
	    printf("\nLunghezza nome di p2 e': %d", nameLen);
  	}

	if(recv(in, &buffer2, nameLen, 0) > 0){
	    printf("\nIl nome di p2 e': %s", buffer2);
  	}

	send(out, &nameLen, 2, 0);
	send(out, &buffer2, nameLen, 0);

	strcpy(namePlayer2, buffer2);
	fflush(stdout);
}*/
