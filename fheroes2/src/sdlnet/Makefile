# makefileё

TARGET	:= libsdlnet
LIBS	:= $(shell sdl-config --libs) -lSDL_net
CFLAGS	:= -O0 -g -Wall $(shell sdl-config --cflags)

CC	:= g++

all: $(TARGET).so $(TARGET).a examples

examples: $(TARGET).a
	make -C test

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
	make -C test clean
