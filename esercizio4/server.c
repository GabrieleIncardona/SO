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
    LIST clienti = NewList();
    LIST fornitori = NewList();

    // Apertura del socket
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

        // Binding dell'indirizzo al socket
        if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
        {
            perror("Error on binding");
            exit(1);
        }
        
        // Numero massimo di connessioni in coda
        if ( listen( sockfd, BUF_SIZE ) == -1 ) 
        {
            perror("Error on listen");
            exit(1);
        }

        socklen_t address_size = sizeof( cli_addr );    
        
        ItemType buf;    
        printf("\nIn attesa di una nuova connessione...\n");
        
        // Accettazione di una nuova connessione        
        int newsockfd= accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
        if (newsockfd == -1) 
        {
            perror("Error on accept");
            exit(1);
        }
        
        // Ricezione del messaggio
        ssize_t receive = recv(newsockfd, &buf, sizeof(buf), 0);
        if ( receive == -1) 
        {
            perror("Error on receive");
            exit(1);
        }
        else if (receive != sizeof(ItemType)){
            perror("Non è un ItemType");
            exit(1);
        }

        buf.sockfd = newsockfd;

        if(buf.tipo == FORNITORE){
            LIST tmp = clienti;
            while(!isEmpty(tmp)){
                if(tmp->item.paziente <= buf.quantitaMax && tmp->item.paziente >= buf.quantitaMin){
                    char msg[BUF_SIZE]= "HAI RICEVUTO LE DOSI VOLUTE";
                    if ( send(tmp->item.sockfd, msg, BUF_SIZE, 0) == -1 ) 
                    {
                        perror("Error on send\n");
                        exit(1);
                    }

                    ItemType tmp2;
                    strcpy(tmp2.nome,tmp->item.nome);
                    tmp2.quantitaMax = tmp->item.paziente;
					tmp2.tipo = 1;
                    if ( send(buf.sockfd, &tmp2, sizeof(tmp2), 0) == -1 ) 
                    {
                        perror("Error on send\n");
                        exit(1);
                    }
                    buf.quantitaMax -= buf.quantitaMax - tmp->item.paziente;

                    close(tmp->item.sockfd);
                    clienti = Dequeue(clienti, tmp->item);
                }
                tmp = tmp->next;
            }
			PrintItem(buf);

            if(buf.quantitaMax > buf.quantitaMin){
                fornitori = EnqueueFirst(fornitori, buf);
            }
            else{
                ItemType tmp2;
                tmp2.tipo = -1;
                if ( send(buf.sockfd, &tmp2, sizeof(tmp2), 0) == -1 ) 
                {
                    perror("Error on send\n");
                    exit(1);
                }
                close(newsockfd);
            }
        }
        
        else if (buf.tipo == CLIENTE){
            LIST tmp = fornitori;
			int flag = 0;
            while(!isEmpty(tmp)){
                if(tmp->item.quantitaMax >= buf.paziente && tmp->item.quantitaMin <= buf.paziente){
					flag = 1;
                    char msg[BUF_SIZE]= "HAI RICEVUTO LE DOSI VOLUTE";
                    if ( send(newsockfd, msg, BUF_SIZE, 0) == -1 ) 
                    {
                        perror("Error on send\n");
                        exit(1);
                    }

                    ItemType tmp2;
                    strcpy(tmp2.nome, buf.nome);
                    tmp2.quantitaMax = buf.paziente;
					tmp2.tipo = 1;
                    if ( send(tmp->item.sockfd, &tmp2, sizeof(tmp2), 0) == -1 ) 
                    {
                        perror("Error on send\n");
                        exit(1);
                    }
					tmp->item.quantitaMax = tmp->item.quantitaMax - buf.paziente;
                    
                    close(newsockfd);
                    if(tmp->item.quantitaMax < tmp->item.quantitaMin){
						ItemType tmp2;
						tmp2.tipo = -1;
						if ( send(tmp->item.sockfd, &tmp2, sizeof(tmp2), 0) == -1 ) 
						{
							perror("Error on send\n");
							exit(1);
						}
                        close(tmp->item.sockfd);
                        fornitori = Dequeue(fornitori, tmp->item);
                        break;
                    }
                }
                tmp = tmp->next;
            }
			if(flag == 0){
				clienti = EnqueueFirst(clienti, buf);
			}
        }

        close(sockfd);
		PrintList(clienti);
    }
    return 0;
}