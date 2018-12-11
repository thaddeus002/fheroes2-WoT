# makefile
# xml wrapper
# http://www.sourceforge.net/projects/tinyxml

TARGET	:= libtinyxml
CFLAGS := -Wall -O2

all: $(TARGET).a

$(TARGET).a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	$(AR) crvs $@ $^

%.o: %.cpp
	$(CXX) -c $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.a *.o
