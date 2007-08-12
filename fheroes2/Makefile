# makefile
# project: Free Heroes2
#

TARGET = fheroes2
LIBS = $(shell sdl-config --libs)
CFLAGS = -O0 -g -Wall $(shell sdl-config --cflags)
#CFLAGS = -O3 -Wall $(shell sdl-config --cflags) -DBUILD_RELEASE

CC = g++

all: $(TARGET)

$(TARGET): $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) $^ -o $@ $(LIBS)

fheroes2.o: fheroes2.cpp
	$(CC) -c -MD $< $(CFLAGS) -DBUILD_DATE=$(shell date +%Y%m%d)

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
