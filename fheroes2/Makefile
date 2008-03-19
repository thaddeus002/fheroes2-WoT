# makefile
# project: Free Heroes2
#

TARGET := fheroes2

all:
	make -C dist
	cp dist/$(TARGET) .

clean:
	make -C dist clean
	make -C src/engine clean
	make -C src/tools clean
