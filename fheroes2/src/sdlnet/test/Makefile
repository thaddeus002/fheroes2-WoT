# makefile

TARGETS	:= server client
CFLAGS	:= -O0 -g -Wall $(shell sdl-config --cflags) -I ..
LIBS	:= $(shell sdl-config --libs) ../libsdlnet.a -lSDL_net
CC 	:= g++

all: $(TARGETS)

$(TARGETS): $(addsuffix .cpp, $(TARGETS)) $(LIBENGINE)
	$(CC) -c $@.cpp $(CFLAGS)
	$(CC) -o $@ $@.o $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *.d *.exe $(TARGETS)
