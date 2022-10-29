CC		:= g++
CFLAGS	:= -Wall -O -std=c++11 -pthread
TARGET = prodcon

all: link

link: assignment2.o thread_man.o tands.o
	$(CC) $(CFLAGS) -o $(TARGET) assignment2.o thread_man.o tands.o

assignment2.o: assignment2.cpp
	$(CC) $(CFLAGS) -c assignment2.cpp

thread_man.o: thread_man.cpp
	$(CC) $(CFLAGS) -c thread_man.cpp

tands.o: tands.cpp
	$(CC) $(CFLAGS) -c tands.cpp


submit:
	tar -czvf assignment2.tar makefile *.cpp *.h readme.txt

clean:
	rm -f *.o prodcon
