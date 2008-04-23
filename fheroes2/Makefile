# makefile
# project: Free Heroes2
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)
#BUILD_DATE := 20080418

export SDL_FLAGS SDL_LIBS BUILD_DATE

all:
	$(MAKE) -C dist
	cp dist/$(TARGET).exe .

clean:
	$(MAKE) -C dist clean
	$(MAKE) -C src/engine clean
	$(MAKE) -C src/tools clean
