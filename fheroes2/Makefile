# makefile
# project: Free Heroes2
#

TARGET := fheroes2

all:
	make -C src
	cp src/$(TARGET) .
	strip $(TARGET)

clean:
	make -C src clean
