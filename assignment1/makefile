CC := g++
CFLAGS := -Wall -O
EXTO := shell379

all:assign1

assign1: ./assign1.cpp ./pcontrol.cpp ./pfind.cpp
	${CC} ${CFLAGS} -o $(EXTO) ./assign1.cpp ./pcontrol.cpp ./pfind.cpp


submit:
	tar -czvf assignment1.tar makefile *.h *.cpp *.md

clean:
	-rm shell379

