# makefile

TARGET = libxmlccwrap
CFLAGS := $(shell xml2-config --cflags)
LIBS := -O2 -Wall -I . $(shell xml2-config --libs)

CC = g++

all: $(TARGET).so $(TARGET).a

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
