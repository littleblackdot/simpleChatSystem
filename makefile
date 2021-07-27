CC = gcc
object1 = server.o dataParse.o myNet.o cJSON.o queue.o serverWork.o myThreadPool.o List.o
object2 = client.o dataParse.o cJSON.o clientWork.o
binary_file : $(object1) $(object2)
	$(CC) $(object1) -o server -lpthread -lsqlite3
	$(CC) $(object2) -o client
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

serverWork.o : serverWork.c
	$(CC) -c serverWork.c

myThreadPool.o : myThreadPool.c
	$(CC) -c myThreadPool.c -lpthread

clientWork.o : clientWork.c
	$(CC) -c clientWork.c

List.o : List.c
	$(CC) -c List.c
	
.PHONY:
clean:
	rm -rf $(object1) $(object2) binary_file
