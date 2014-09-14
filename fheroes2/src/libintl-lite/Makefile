# makefile
# libintl lite
# https://sourceforge.net/projects/libintl-lite/

TARGET	:= libintl

all: $(TARGET).a

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(AR) crvs $@ $^

%.o: %.cpp
	$(CXX) -c -MD $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.a *.so *.d *.o
