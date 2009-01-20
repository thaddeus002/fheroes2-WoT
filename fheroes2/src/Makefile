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
#LUA_LIBS := $(shell pkg-config lua --libs)
LUA_FLAGS :=
SDL_LIBS := $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS := $(shell sdl-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)
CFLAGS := $(SDL_FLAGS) -Wall -fsigned-char
LIBS := $(SDL_LIBS)

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g
else
CFLAGS := $(CFLAGS) -O2
endif

ifdef DEVELOPMENT
CFLAGS := $(CFLAGS) $(LUA_FLAGS) -DWITH_LUA -DWITH_TTF
LIBS := $(LIBS) $(LUA_LIBS) -lSDL_ttf
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

export BUILD_DATE CFLAGS LIBS

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
