all: main.c helper.c inputhandler.c outputhandler.c helper.h inputhandler.h outputhandler.h
	gcc -c -o main.o main.c
	gcc -c -o helper.o helper.c
	gcc -c -o inputhandler.o inputhandler.c
	gcc -c -o outputhandler.o outputhandler.c
	gcc helper.o inputhandler.o outputhandler.o main.o

helper: helper.c
	gcc -DHEADER_TEST helper.c

inputhandler: inputhandler.c
	gcc -DHEADER_TEST inputhandler.c

outputhandler: outputhandler.c
	gcc -DHEADER_TEST outputhandler.c

# 클린업
clean:
	rm -f *.o

