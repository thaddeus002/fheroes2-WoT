# makefile
# project: Free Heroes2
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS := $(shell sdl-config --cflags)
PNG_LIBS := $(shell libpng12-config --libs)
PNG_FLAGS := $(shell libpng12-config --cflags)

BUILD_DATE := $(shell date +%Y%m%d)

CFLAGS := -Wall -fsigned-char
LIBS :=

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g
else
CFLAGS := $(CFLAGS) -O2
endif

ifndef WITHOUT_PNG
CFLAGS := $(CFLAGS) -DWITH_PNG $(PNG_FLAGS)
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(PNG_LIBS)
endif

ifndef WITHOUT_UNICODE
CFLAGS := $(CFLAGS) -DWITH_TTF
SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
ifeq ($(OS), Windows_NT)
LIBS := $(LIBS) -lintl -liconv
endif
endif

ifndef WITHOUT_NETWORK
CFLAGS := $(CFLAGS) -DWITH_NET
SDL_LIBS := $(SDL_LIBS) -lSDL_net
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

CFLAGS := $(SDL_FLAGS) $(CFLAGS)
LIBS := $(SDL_LIBS) $(LIBS)

export BUILD_DATE CFLAGS LIBS

.PHONY: clean

all:
	$(MAKE) -C xmlccwrap
	$(MAKE) -C engine
	$(MAKE) -C dist
	$(MAKE) -C tools
ifndef WITHOUT_UNICODE
	$(MAKE) -C dist pot
endif

clean:
	$(MAKE) -C xmlccwrap clean
	$(MAKE) -C tools clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
