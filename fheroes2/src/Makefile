# makefile
# project: Free Heroes2
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)

BUILD_DATE := $(shell date +%Y%m%d)

CFLAGS := -Wall -fsigned-char -DTIXML_USE_STL
LIBS := -lz

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g
else
CFLAGS := $(CFLAGS) -O2
endif

ifndef WITHOUT_MIXER
CFLAGS := $(CFLAGS) -DWITH_MIXER
SDL_LIBS := $(SDL_LIBS) -lSDL_mixer
endif

ifndef WITHOUT_IMAGE
CFLAGS := $(CFLAGS) -DWITH_IMAGE $(shell libpng12-config --cflags)
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(shell libpng12-config --libs)
endif

ifndef WITHOUT_UNICODE
CFLAGS := $(CFLAGS) -DWITH_TTF
SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
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

ifeq ($(PLATFORM),)
PLATFORM := unix
endif

ifeq ($(OS),Windows_NT)
PLATFORM := mingw
endif

include Makefile.$(PLATFORM)

export BUILD_DATE CFLAGS LIBS LINK SIMPLE_LINK_FLAGS LINK_FLAGS PLATFORM

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
