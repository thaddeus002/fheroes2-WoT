# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
#

TARGET := fheroes2
LIBS := $(SDL_LIBS) $(LUA_LIBS)

ifdef DEBUG
CFLAGS := -O0 -g -Wall $(SDL_FLAGS) $(LUA_FLAGS)
else
CFLAGS := -O2 -Wall $(SDL_FLAGS) $(LUA_FLAGS)
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

CC := g++

SOURCEROOT := ../src
SOURCEDIR := engine agg algorithm army castle dialog editor game gui \
	heroes image kingdom maps monster objects resource scripting spell system

SEARCH := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, $(SOURCEDIR)))
ENGINE := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, engine))

all: $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH))))
	@echo "lnk: $@"
	$(CC) $^ -o $@ $(LIBS)

VPATH := $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

settings.o: settings.cpp
	@echo "cxx: $@"
	@$(CC) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS) -DBUILD_DATE=$(BUILD_DATE)

%.o: %.cpp
	@echo "cxx: $@"
	@$(CC) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.o *.d *.exe $(TARGET)
