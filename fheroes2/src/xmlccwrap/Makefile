# makefile
# xml wrapper
# http://www.sourceforge.net/projects/tinyxml

TARGET	:= libxmlccwrap
CC 	:= g++

ifdef DEBUG
CFLAGS	:= -O0 -g
else
CFLAGS	:= -O2
endif

all: $(TARGET).so $(TARGET).a

$(TARGET).so: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(CC) -shared -fPIC -o $@ $^

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	ar crvs $@ $^

%.o: %.cpp
	$(CC) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a *.so *.d *.o
