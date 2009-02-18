# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS := $(shell sdl-config --cflags)
PNG_LIBS := $(shell libpng12-config --libs)
PNG_FLAGS := $(shell libpng12-config --cflags)
XML_FLAGS := $(shell xml2-config --cflags)
XML_LIBS := $(shell xml2-config --libs)

BUILD_DATE := $(shell date +%Y%m%d)

CFLAGS := $(CFLAGS) -Wall -fsigned-char
LIBS := $(LIBS)

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g
else
CFLAGS := $(CFLAGS) -O2
endif

ifndef WITHOUT_PNG
CFLAGS := $(CFLAGS) -DWITH_PNG $(PNG_FLAGS)
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(PNG_LIBS)
endif

ifndef WITHOUT_XML
CFLAGS := $(CFLAGS) -DWITH_XML $(XML_FLAGS) -I../xmlccwrap
LIBS := $(LIBS)../xmlccwrap/libxmlccwrap.a $(XML_LIBS)
endif

ifndef WITHOUT_UNICODE
CFLAGS := $(CFLAGS) -DWITH_TTF
SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
ifeq ($(OS), Windows_NT)
LIBS := $(LIBS) -lintl -liconv
endif
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

CFLAGS := $(SDL_FLAGS) $(CFLAGS)
LIBS := $(SDL_LIBS) $(LIBS)

export BUILD_DATE CFLAGS LIBS

.PHONY: clean

all:
ifndef WITHOUT_XML
	$(MAKE) -C xmlccwrap
endif
	$(MAKE) -C engine
	$(MAKE) -C dist
	$(MAKE) -C tools
	$(MAKE) -C dist pot

clean:
	$(MAKE) -C xmlccwrap clean
	$(MAKE) -C tools clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
