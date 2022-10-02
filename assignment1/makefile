CC := g++
CFLAGS := -Wall -O
EXTO := shell379

all:assign1

assign1: ./assign1.cpp ./pcontrol.cpp ./pfind.cpp
	${CC} ${CFLAGS} -o $(EXTO) ./assign1.cpp ./pcontrol.cpp ./pfind.cpp


submition:
	tar -czvf assignment1.tar makefile *.h *.cpp

clean:
	-rm shell379

