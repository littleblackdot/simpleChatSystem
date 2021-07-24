CC = gcc
object = server.o dataParse.o myNet.o cJSON.o queue.o

binary_file : $(object)
	$(CC) $(object) -o server

server.o : server.c
	$(CC) -c server.c  

dataParse.o : dataParse.c
	$(CC) -c dataParse.c

myNet.o : myNet.c
	$(CC) -c myNet.c

cJSON.o : cJSON.c
	$(CC) -c cJSON.c
queue.o : queue.c
	$(CC) -c queue.c
.PHONY:
clean:
	rm -rf $(object) binary_file
