# makefile
# project: Free Heroes2 Tools
#

TARGETS := extractor 82m2wav til2bmp icn2bmp
LIBS := ../engine/libengine.a $(shell sdl-config --libs)
CFLAGS := -O2 -Wall -I ../engine $(shell sdl-config --cflags)

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
