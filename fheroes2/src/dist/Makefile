# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# SCRIPTING: build with lua support
#

TARGET := fheroes2
LIBENGINE := ../engine/libengine.a
LIBS := $(LIBENGINE) $(SDL_LIBS)
CFLAGS = -I../engine

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -Wall -Wextra $(SDL_FLAGS)
else
CFLAGS := $(CFLAGS) -O2 -Wall $(SDL_FLAGS)
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

ifdef SCRIPTING
CFLAGS := $(CFLAGS) $(LUA_FLAGS) -DWITH_LUA
LIBS := $(LIBS) $(LUA_LIBS)
endif

CFLAGS := $(CFLAGS) -DBUILD_DATE=$(BUILD_DATE)

CC := g++

SOURCEROOT := ../fheroes2
SOURCEDIR  := $(shell ls $(SOURCEROOT))

SEARCH := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, $(SOURCEDIR)))

all: $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH)))) $(LIBENGINE)
	@echo "lnk: $@"
	$(CC) $^ -o $@ $(LIBS)

VPATH := $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

%.o: %.cpp
	@echo "cxx: $@"
	@$(CC) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o *.d *.exe $(TARGET)
