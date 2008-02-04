# makefile
# project: Free Heroes2
# libSDL C++ wrapper engine

TARGET = libengine
LIBS = $(shell sdl-config --libs)
CFLAGS = -O2 -Wall $(shell sdl-config --cflags) -I .

CC = g++

all: $(TARGET).a

$(TARGET).so: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) -shared -fPIC -o $@ $^ $(LIBS)

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	ar crvs $@ $^

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a
	rm -f *.so
	rm -f *.d
	rm -f *.o
