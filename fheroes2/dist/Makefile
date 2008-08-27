# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# SCRIPTING: build with lua support
#

TARGET := fheroes2
LIBS := $(SDL_LIBS)

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -Wall $(SDL_FLAGS)
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

SOURCEROOT := ../src
SOURCEDIR := engine ai agg algorithm army castle dialog editor game gui \
	heroes image kingdom maps monster objects resource scripting spell system

SEARCH := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, $(SOURCEDIR)))
ENGINE := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, engine))

all: $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH))))
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
