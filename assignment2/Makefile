CC		:= g++
CFLAGS	:= -Wall -O2
TARGET = prodcon

all: link

link: assignment2.o
	$(CC) $(CFLAGS) -o $(TARGET) assignment2.o 

assignment2.o: assignment2.cpp
	$(CC) $(CFLAGS) -c assignment2.cpp


submit:
	tar -cxvf assigment2.tar Makefile *.cpp *.h

clean:
	rm -f *.o prodcon
