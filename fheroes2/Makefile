# makefile
#

TARGET = fheroes2
LIBS = `sdl-config --libs`
CFLAGS = -O3 -Wall `sdl-config --cflags` -DBUILD_DATE=`date +%Y%m%d`

CC = g++

all: $(TARGET)
	strip $(TARGET)

fheroes2: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	rm -f $(TARGET)
