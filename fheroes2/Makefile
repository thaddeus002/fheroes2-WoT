# makefile
# project: Free Heroes2
#

TARGET = fheroes2
LIBS = `sdl-config --libs`
#CFLAGS = -O0 -g -Wall -fomit-frame-pointer `sdl-config --cflags`
CFLAGS = -O3 -Wall -fomit-frame-pointer `sdl-config --cflags` -mtune=pentium3 -march=i686

CC = gcc

all: $(TARGET)
	strip $(TARGET)

fheroes2: $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm *.o
	rm *.d
	rm $(TARGET)
