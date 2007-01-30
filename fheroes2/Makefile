# makefile
#

TARGET = fheroes2
LIBS = `sdl-config --libs` `sdlmm-config --libs`
CFLAGS = -O0 -g -Wall -fomit-frame-pointer `sdl-config --cflags` `sdlmm-config --cflags`
#CFLAGS = -O3 -Wall -fomit-frame-pointer `sdl-config --cflags` `sdlmm-config --cflags` -march=i686

CC = g++

all: $(TARGET)
#	strip $(TARGET)

fheroes2: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm *.o
	rm *.d
	rm $(TARGET)
