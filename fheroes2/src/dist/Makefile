# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#

TARGET := fheroes2
LIBENGINE := ../engine/libengine.a
LIBS := $(LIBENGINE) $(SDL_LIBS)
CFLAGS = -I../engine

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -Wall $(SDL_FLAGS)
else
CFLAGS := $(CFLAGS) -O2 -Wall $(SDL_FLAGS)
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

ifdef DEVELOPMENT
CFLAGS := $(CFLAGS) $(LUA_FLAGS) -DWITH_LUA
LIBS := $(LIBS) $(LUA_LIBS)
endif

CFLAGS := $(CFLAGS) -DBUILD_DATE=$(BUILD_DATE)

CC := g++

SOURCEROOT := ../fheroes2
SOURCEDIR  := $(shell ls $(SOURCEROOT))
POT        := $(TARGET).pot

SEARCH := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, $(SOURCEDIR)))

all: $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH)))) $(LIBENGINE)
	@echo "lnk: $@"
	$(CC) $^ -o $@ $(LIBS)

pot: $(wildcard $(SEARCH))
	@echo "gen: $(POT)"
	@xgettext -d $(TARGET) -C -ktr -o $(POT) $(wildcard $(SEARCH))

VPATH := $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

%.o: %.cpp
	@echo "cxx: $@"
	@$(CC) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.pot *.o *.d *.exe $(TARGET)
