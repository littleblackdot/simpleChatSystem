CC = gcc
object1 = server.o dataParse.o myNet.o cJSON.o queue.o serverWork.o myThreadPool.o List.o
object2 = client.o dataParse.o cJSON.o clientWork.o
binary_file : $(object1) $(object2)
	$(CC) $(object1) -o server -lpthread -lsqlite3 -g
	$(CC) $(object2) -o client -lpthread -g
server.o : server.c
	$(CC) -c server.c -g

dataParse.o : dataParse.c
	$(CC) -c dataParse.c -g

myNet.o : myNet.c
	$(CC) -c myNet.c -g

cJSON.o : cJSON.c
	$(CC) -c cJSON.c -g

queue.o : queue.c
	$(CC) -c queue.c -g

serverWork.o : serverWork.c
	$(CC) -c serverWork.c -g

myThreadPool.o : myThreadPool.c
	$(CC) -c myThreadPool.c -lpthread -g

clientWork.o : clientWork.c
	$(CC) -c clientWork.c -g

List.o : List.c
	$(CC) -c List.c -g
	
.PHONY:
clean:
	rm -rf $(object1) $(object2) binary_file
