#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include "lIST.h"


#define BUF_SIZE 1000

int port = 8000;
int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int sockfd;
	LIST clienti = NewList();
	LIST magazzino = NewList();

	// Socket opening
	while(1){
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
		
		ItemType buf;	
		printf("\nWaiting for a new connection...\n");
		
		// New connection acceptance		
		int newsockfd= accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		
		// Message reception
		ssize_t receive = recv(newsockfd, &buf, sizeof(buf), 0);
		//PrintItem(buf);
		if ( receive == -1) 
		{
			perror("Error on receive");
			exit(1);
		}
		else if (receive != sizeof(ItemType)){
			perror("Non è un ItemType");
			exit(1);
		}

		if(buf.titolo[0] == 'C'){
			for(int i = 0; i<strlen(buf.titolo); i++)
				buf.titolo[i] = buf.titolo[i+1];
			ItemType* tmp = Find(magazzino, buf);
			if(tmp != NULL){
				tmp->quantita += buf.quantita; 
			}
			else
				magazzino = EnqueueOrdered(magazzino, buf);
			tmp = Find(clienti,buf);
			PrintItem(buf);
			ItemType* tmp2 = Find(magazzino, buf);
			while(tmp != NULL && tmp2->quantita != 0){
				char msg [20]= "Libro venduto";
				int len = strlen(msg);
				if ( send( tmp->sockfd, msg, len + 1, 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				tmp2->quantita--;
				close(tmp->sockfd);
				clienti = Dequeue(clienti, *tmp);
				tmp = Find(clienti,buf);
			} 
				char msg [20]= "Libro/i ricevuti";
				int len = strlen(msg);
				if ( send( newsockfd, msg, len + 1, 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
			close(newsockfd);
		}
		//Pensare a L ancora
		else if (buf.titolo[0] == 'L'){
			for(int i = 0; i<strlen(buf.titolo); i++)
				buf.titolo[i] = buf.titolo[i+1];
			ItemType* tmp = Find(magazzino, buf);
			if (tmp != NULL && tmp->quantita != 0){
				tmp->quantita--;
				char msg [20]= "Libro venduto";
				int len = strlen(msg);
				if ( send( newsockfd, msg, len + 1, 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				close(newsockfd);
			}
			else{
				buf.sockfd = newsockfd;
				clienti= EnqueueLast(clienti, buf);
			}
		}

		PrintList(magazzino);
		close(sockfd);

	}
	return 0;
}



