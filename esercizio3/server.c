#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include "LIST.h"


#define BUF_SIZE 1000

int port = 8000;
int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int sockfd;
	srand(time(NULL));

	// Socket opening
	while(1){
		LIST giocatori = NewList();
		LIST giocatoriConnessi = NewList();
		for (int i = 0; i<= 3; i++){
			ItemType giocatore;
			sockfd =  socket( PF_INET, SOCK_STREAM, 0 ); 
			if ( sockfd == -1 ) 
			{
				perror("Error opening socket");
				exit(1);
			}
			
			int options = 1;
			if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
				perror("Error on setsockopt");
				exit(1);
			}

			bzero( &serv_addr, sizeof(serv_addr) );
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = INADDR_ANY;
			serv_addr.sin_port = htons(port);

			// Address bindind to socket
			if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
			{
				perror("Error on binding");
				exit(1);
			}
			
			// Maximum number of connection kept in the socket queue
			if ( listen( sockfd, BUF_SIZE ) == -1 ) 
			{
				perror("Error on listen");
				exit(1);
			}


			socklen_t address_size = sizeof( cli_addr );	
			
			char buf[BUF_SIZE];	
			printf("\nWaiting for a new connection...\n");
			
			// New connection acceptance		
			int newsockfd= accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
			if (newsockfd == -1) 
			{
				perror("Error on accept");
				exit(1);
			}
			
			bzero(buf, BUF_SIZE);
			giocatore.sockfd = newsockfd;
			
			// Message reception
			if ( recv( newsockfd, buf, BUF_SIZE, 0 ) == -1) 
			{
				perror("Error on receive");
				exit(1);
			}

			printf("Received from client: \"%s\"\n", buf);
			ItemType appoggio;
			strcpy(appoggio.nome, buf);
			appoggio.partite = 0;  // Inizializza a un valore predefinito
			appoggio.value = 0;    // Inizializza a un valore predefinito
			appoggio.sockfd = -1;  // Inizializza a un valore predefinito
			//PrintItem(appoggio);

			if (Find(giocatori, appoggio) != NULL && Find(giocatoriConnessi, appoggio)== NULL){
				PrintItem(*Find(giocatori, appoggio));
				strcpy(giocatore.nome, buf);
				giocatore.partite = Find(giocatori, appoggio)->partite;
				giocatore.value = Find(giocatori, appoggio)->value;
				EnqueueLast(giocatoriConnessi, giocatore);
			}
			else if(Find(giocatoriConnessi, appoggio)== NULL){
				strcpy(giocatore.nome, buf);
				//PrintList(giocatori);
				giocatore.partite = 0;
				giocatore.value = 0;
				//PrintItem(giocatore);
				giocatori = EnqueueFirst(giocatori, giocatore);
				PrintList(giocatori);
				giocatoriConnessi= EnqueueLast(giocatoriConnessi, giocatore);
				//PrintList(giocatoriConnessi);
			}
			else{
				printf("giocatore già connesso \n");
			}


			close(sockfd);
		}

		for(int punteggio = 3; punteggio>0; punteggio--){
			//printf("lunghezza: %d\n", getLength(giocatoriConnessi));
			int randomNumber = rand() % getLength(giocatoriConnessi);
			char buf[BUF_SIZE];
			LIST tmp = giocatoriConnessi;
			for (int j =0; j<randomNumber; j++){
				tmp = tmp->next;
			}
			if (punteggio == 3){
				strcpy(buf, "Sei arrivato primo");
			}
			else if(punteggio ==2)
				strcpy(buf, "Sei arrivato secondo");
			else
				strcpy(buf, "Sei arrivato terzo");
			int len = strlen(buf);
			if ( send( tmp->item.sockfd, buf, len + 1, 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}	
			LIST tmp2 = giocatori;
			while (strcmp(tmp2->item.nome, tmp->item.nome) != 0) {
				tmp2 = tmp2->next;
			}
			tmp2->item.partite = tmp2->item.partite + 1;
			tmp2->item.value = tmp2->item.value + punteggio;
			close(tmp->item.sockfd);
			giocatoriConnessi = Dequeue(giocatoriConnessi, tmp->item );
			
		}
		//printf("SONO QUI\n");
		char buf[BUF_SIZE] = "Hai perso";
		int len = strlen(buf);
		while(!isEmpty(giocatoriConnessi)){
			ItemType tmp = getHead(giocatoriConnessi);
			//printf("SONO QUI\n");
			if ( send( tmp.sockfd, buf, len + 1, 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}
			ItemType* tmp2 = Find(giocatori, tmp);
			tmp2->partite = tmp2->partite + 1;
			close(tmp2->sockfd);
			giocatoriConnessi = DequeueFirst(giocatoriConnessi);
		}
		printf("\n";)
		PrintList(giocatori);
		printf("\n");

	}
	return 0;
}



