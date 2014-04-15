server : sig.o util.o echo_server.o echo_client.o
	gcc -o echo_server sig.o util.o echo_server.o
	gcc -o echo_client util.o echo_client.o

sig.o : util.o sig.c sig.h
	gcc -c sig.c

util.o : util.c util.h
	gcc -c util.c

echo_server.o : echo_server.c
	gcc -c echo_server.c

echo_client.o : echo_client.c
	gcc -c echo_client.c
