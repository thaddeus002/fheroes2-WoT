# makefile
# project: Free Heroes2
#

TARGET := fheroes2
LUA_LIBS := -Wl,-Bstatic -llua -Wl,-Bdynamic
#LUA_LIBS := `pkg-config lua --libs`
LUA_FLAGS :=
SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)

export SDL_FLAGS SDL_LIBS LUA_LIBS BUILD_DATE

all:
	$(MAKE) -C dist
	cp dist/$(TARGET) .

clean:
	$(MAKE) -C dist clean
	$(MAKE) -C src/engine clean
	$(MAKE) -C src/tools clean

