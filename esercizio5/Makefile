all: guida cliente server 

cliente: cliente.o list.o
	gcc -Wall -o cliente cliente.o list.o

guida: guida.o list.o
	gcc -Wall -o guida guida.o list.o

server: server.o list.o
	gcc -Wall -o server server.o list.o

server.o: server.c list.h
	gcc -Wall -c server.c

cliente.o: cliente.c list.h
	gcc -Wall -c cliente.c  # Fixed typo

guida.o: guida.c list.h
	gcc -Wall -c guida.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f cliente guida server *.o *~
