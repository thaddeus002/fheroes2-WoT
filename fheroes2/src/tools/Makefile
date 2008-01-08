# makefile
# project: Free Heroes2 Tools
#

TARGETS := extractor 82m2wav til2bmp icn2bmp
LIBS := $(shell sdl-config --libs) ../engine/libengine.a
CFLAGS := -O2 -Wall -I ../system -I ../engine $(shell sdl-config --cflags)

CC := g++

all: libengine $(TARGETS)

libengine:
	make -C ../engine

extractor: extractor.o
	$(CC) $^ -o $@

82m2wav: 82m2wav.o
	$(CC) $^ -o $@

til2bmp: til2bmp.o
	$(CC) $^ -o $@ $(LIBS)

icn2bmp: icn2bmp.o
	$(CC) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o *.d $(TARGETS)
