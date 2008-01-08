# makefile
# project: Free Heroes2
#

TARGET := fheroes2
LIBS := $(shell sdl-config --libs) ./engine/libengine.a
#CFLAGS = $(CFLAGS) -O0 -g -Wall $(shell sdl-config --cflags) -I ./engine
#CFLAGS = $(CFLAGS) -O2 -Wall $(shell sdl-config --cflags) -I ./engine -DBUILD_RELEASE
CFLAGS := $(CFLAGS) -O2 -Wall $(shell sdl-config --cflags) -I ./engine

CC := g++

SOURCEDIR := ./agg ./algorithm ./army ./castle ./dialog ./editor \
	./game ./gui ./heroes ./image ./kingdom ./maps ./monster \
	./resource ./system

SEARCH := $(addsuffix /*.cpp, $(SOURCEDIR))

all: libengine $(TARGET) toolsprog

toolsprog:
	make -C tools

libengine:
	make -C engine

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH))))
	$(CC) $^ -o $@ $(LIBS)

VPATH := $(SOURCEDIR)

fheroes2.o: fheroes2.cpp
	$(CC) -c -MD $(addprefix -I, $(SOURCEDIR)) $< $(CFLAGS) -DBUILD_DATE=$(shell date +%Y%m%d)

%.o: %.cpp
	$(CC) -c -MD $(addprefix -I, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o *.d $(TARGET)
	make -C engine clean
	make -C tools clean
