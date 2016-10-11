all: client server

SERVER_OBJS = main.o System.o DB.o Lists.o
CLIENT_OBJS = client.o

SERVER_SOURCE	= main.cpp System.cpp DB.cpp Lists.cpp
CLIENT_SOURCE = client.cpp

SERVER_HEADER  = DB.h Lists.h System.h

SERVER_OUT = bankserver
CLIENT_OUT = bankclient

CC	= g++
FLAGS	= -c -lpthread

client: $(CLIENT_OBJS)
	$(CC) -g $(CLIENT_OBJS) -o $(CLIENT_OUT)

server: $(SERVER_OBJS)
	$(CC) -g $(SERVER_OBJS) -lpthread -o $(SERVER_OUT)


main.o: main.cpp
		$(CC) $(FLAGS) main.cpp

System.o: System.cpp
		$(CC) $(FLAGS) System.cpp

DB.o: DB.cpp
		$(CC) $(FLAGS) DB.cpp

Lists.o: Lists.cpp
		$(CC) $(FLAGS) Lists.cpp

client.o: client.cpp
		$(CC) $(FLAGS) client.cpp


clean:
	rm -f $(CLIENT_OBJS) $(CLIENT_OUT)
	rm -f $(SERVER_OBJS) $(SERVER_OUT)
