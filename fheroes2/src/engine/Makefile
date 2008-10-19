# makefile
# project: Free Heroes2
# libSDL C++ wrapper engine

TARGET = libengine
SDL_LIBS = $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS = $(shell sdl-config --cflags)
LIBS = $(SDL_LIBS)

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -Wall $(SDL_FLAGS)
else
CFLAGS := $(CFLAGS) -O2 -Wall $(SDL_FLAGS)
endif

CC = g++

all: $(TARGET).a $(TARGET).so

$(TARGET).so: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) -shared -fPIC -o $@ $^ $(LIBS)

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	ar crvs $@ $^

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a *.so *.d *.o
