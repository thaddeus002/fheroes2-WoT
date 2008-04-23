# makefile
# project: Free Heroes2
# libSDL C++ wrapper engine

TARGET = libengine
LIBS = $(SDL_LIBS)
CFLAGS = -O2 -Wall $(SDL_FLAGS) -I .

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
