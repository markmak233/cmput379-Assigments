CC		:= g++
CFLAGS	:= -Wall -O -std=c++11 -pthread
TARGET1 = server
TARGET2 = client

all: link

link: server.o client.o tands.o
	$(CC) $(CFLAGS) -o $(TARGET1) server.o tands.o
	$(CC) $(CFLAGS) -o $(TARGET2) client.o

server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) -c client.cpp

tands.o: tands.cpp
	$(CC) $(CFLAGS) -c tands.cpp


submit:
	tar -czvf assignment3.tar makefile *.cpp *.h readme.txt

clean:
	rm -f *.o prodcon
