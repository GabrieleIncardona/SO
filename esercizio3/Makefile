all:	client server 

client:	client.o
	gcc -Wall -o client client.o

server:	server.o list.o
	gcc -Wall -o server server.o list.o
server.o: server.c list.h
	gcc -Wall -c server.c
client.o: client.c list.h
	gcc -Wall -c client.c
list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f client server *~

