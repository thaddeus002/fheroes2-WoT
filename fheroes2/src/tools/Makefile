# makefile
# project: Free Heroes2 Tools
#

TARGETS := extractor 82m2wav til2bmp icn2bmp
SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)
LIBS := ../engine/libengine.a $(SDL_LIBS)
CFLAGS := -O2 -Wall -I ../engine $(SDL_FLAGS)

CC := g++

all: libengine $(TARGETS)

libengine:
	make -C ../engine

$(TARGETS): $(addsuffix .cpp, $(TARGETS))
	$(CC) -c $@.cpp $(CFLAGS)
	$(CC) -o $@ $@.o $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *.exe $(TARGETS)
