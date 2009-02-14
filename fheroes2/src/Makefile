# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs) -lSDL_image -lSDL_mixer
SDL_FLAGS := $(shell sdl-config --cflags)
PNG_LIBS := $(shell libpng12-config --libs)
PNG_FLAGS := $(shell libpng12-config --cflags)
BUILD_DATE := $(shell date +%Y%m%d)
CFLAGS := $(SDL_FLAGS) $(PNG_FLAGS) -Wall -fsigned-char
LIBS := $(SDL_LIBS) $(PNG_LIBS)

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g
else
CFLAGS := $(CFLAGS) -O2
endif

ifndef WITHOUT_UNICODE
CFLAGS := $(CFLAGS) -DWITH_TTF
LIBS := $(LIBS) -lSDL_ttf
ifeq ($(OS), Windows_NT)
LIBS := $(LIBS) -lintl -liconv
endif
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
