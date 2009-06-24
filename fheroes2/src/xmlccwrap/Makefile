# makefile
# xml wrapper
# http://www.sourceforge.net/projects/tinyxml

TARGET	:= libxmlccwrap
CC 	:= g++

all: $(TARGET).so $(TARGET).a

$(TARGET).so: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(LINK) -o $@ $^ $(SIMPLE_LINK_FLAGS) -lz

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	ar crvs $@ $^
	ranlib $@

%.o: %.cpp
	$(CC) -c -fPIC -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a *.so *.d *.o
