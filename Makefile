TARGET = 82m2wav extractor showicn unicn unicn2bmp
LIBS = `sdl-config --libs`
CFLAGS = -O2 -Wall -fomit-frame-pointer `sdl-config --cflags`
CC = gcc

all: showicn unicn unicn2bmp extractor 82m2wav

extractor:
	$(CC) $(CFLAGS) -o extractor extractor.c

82m2wav:
	$(CC) $(CFLAGS) -o 82m2wav 82m2wav.c

unicn: unicn.o tools.o
	$(CC) $(CFLAGS) -o unicn unicn.o tools.o $(LIBS)

unicn2bmp: unicn2bmp.o tools.o
	$(CC) $(CFLAGS) -o unicn2bmp unicn2bmp.o tools.o $(LIBS)

showicn: showicn.o tools.o
	$(CC) $(CFLAGS) -o showicn showicn.o tools.o $(LIBS)

tools.o: tools.c
	$(CC) $(CFLAGS) -c tools.c -o tools.o

unicn.o: unicn.c
	$(CC) $(CFLAGS) -c unicn.c -o unicn.o

unicn2bmp.o: unicn2bmp.c
	$(CC) $(CFLAGS) -c unicn2bmp.c -o unicn2bmp.o

showicn.o: showicn.c
	$(CC) $(CFLAGS) -c showicn.c -o showicn.o

.PHONY: clean

clean:
	rm *.o 
	rm $(TARGET)
