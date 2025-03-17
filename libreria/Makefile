all: fornitore cliente server 

cliente: cliente.o list.o
	gcc -Wall -o cliente cliente.o list.o

fornitore: fornitore.o list.o
	gcc -Wall -o fornitore fornitore.o list.o

server: server.o list.o
	gcc -Wall -o server server.o list.o

server.o: server.c list.h
	gcc -Wall -c server.c

cliente.o: cliente.c list.h
	gcc -Wall -c cliente.c  # Fixed typo

fornitore.o: fornitore.c list.h
	gcc -Wall -c fornitore.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f cliente fornitore server *.o *~
