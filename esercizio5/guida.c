#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"


#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{
	char str[22];
	int quantitaMax;
	int quantitaMin;
	
	if (argc < 4) {
		printf("Inserire nome e persone\n");
		exit(0);
	}
	else{
		strcpy(str, argv[1] );
		quantitaMax = atoi(argv[2]);
		quantitaMin = atoi(argv[3]);
	}
	
		
	struct sockaddr_in serv_addr;
 	struct hostent* server;	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}
	ItemType msg;
	strcpy(msg.nome, str);
	msg.quantitaMin = quantitaMin;
	msg.quantitaMax = quantitaMax;
	msg.tipo = GUIDA;

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Message sent. Waiting for response...\n");
	int buf = 0;

	if ( recv(sockfd, &buf, sizeof(int), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}
	printf("\nLa guida \"%s\" ha servito %d persone\n", str , buf);

	close(sockfd);

	return 0;
}



