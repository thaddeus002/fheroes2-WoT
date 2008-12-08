# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#

TARGET := fheroes2
LUA_LIBS := -Wl,-Bstatic -llua -Wl,-Bdynamic
LUA_FLAGS :=
SDL_LIBS := $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS := $(shell sdl-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)

ifdef DEVELOPMENT
SDL_FLAGS := $(SDL_FLAGS) -DWITH_TTF
SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
endif

export SDL_FLAGS SDL_LIBS LUA_LIBS BUILD_DATE

all:
	$(MAKE) -C engine
	$(MAKE) -C dist
	$(MAKE) -C tools
ifdef DEVELOPMENT
	$(MAKE) -C dist pot
endif

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
