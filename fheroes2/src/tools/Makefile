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

extractor: extractor.o
	$(CC) $^ -o $@ $(LIBS)

82m2wav: 82m2wav.o
	$(CC) $^ -o $@ $(LIBS)

til2bmp: til2bmp.o
	$(CC) $^ -o $@ $(LIBS)

icn2bmp: icn2bmp.o
	$(CC) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o *.d *.exe $(TARGETS)
