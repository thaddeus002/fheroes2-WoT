# makefile
# project: Free Heroes2
#

TARGET := fheroes2
LUA_LIBS := -Wl,-Bstatic -llua -Wl,-Bdynamic
LUA_FLAGS :=
SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)

export SDL_FLAGS SDL_LIBS LUA_LIBS BUILD_DATE

all:
	$(MAKE) -C engine
	$(MAKE) -C dist
	$(MAKE) -C tools

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
