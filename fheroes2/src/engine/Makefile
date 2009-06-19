# makefile
# project: Free Heroes2
# libSDL C++ wrapper engine

TARGET := libengine
CC := g++

all: $(TARGET).a $(TARGET).so

$(TARGET).so: $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 
	$(LINK) $(LINK_FLAGS) -o $@ $^

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 
	ar crvs $@ $^
	ranlib $@

%.o: %.cpp
	$(CC) -c -fPIC -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a *.so *.d *.o
