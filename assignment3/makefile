CC		:= g++
CFLAGS	:= -Wall -O -std=c++11 -pthread
TARGET1 = server
TARGET2 = client

all: link

link: server.o client.o tands.o
	$(CC) $(CFLAGS) -o $(TARGET1) server.o tands.o
	$(CC) $(CFLAGS) -o $(TARGET2) client.o tands.o

server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) -c client.cpp

tands.o: tands.cpp
	$(CC) $(CFLAGS) -c tands.cpp

man:
	for name in ./*.man; do man -t $${name} > $${name}.ps; done
	for name in ./*.ps; do ps2pdf12 $${name}; done
	

submit:
	tar -czvf assignment3.tar makefile *.cpp *.h *.man readme.txt

clean:
	rm -f *.o ./server ./client *.ps
