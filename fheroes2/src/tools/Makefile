# makefile
# project: Free Heroes2 Tools
#

TARGETS := extractor 82m2wav til2bmp icn2bmp xmi2mid package
LIBENGINE := ../engine/libengine.a
LIBS := $(LIBENGINE) $(SDL_LIBS)
CFLAGS = -I../engine

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -Wall $(SDL_FLAGS)
else
CFLAGS := $(CFLAGS) -O2 -Wall $(SDL_FLAGS)
endif

CC := g++

all: $(TARGETS)

$(TARGETS): $(addsuffix .cpp, $(TARGETS)) $(LIBENGINE)
	$(CC) -c $@.cpp $(CFLAGS)
	$(CC) -o $@ $@.o $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *.exe $(TARGETS)
