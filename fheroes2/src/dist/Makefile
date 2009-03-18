# makefile
# project: Free Heroes2
#

TARGET := fheroes2
LIBENGINE := ../engine/libengine.a ../xmlccwrap/libxmlccwrap.a
LIBS := $(LIBENGINE) $(LIBS)
CFLAGS := $(CFLAGS) -I../engine -I../xmlccwrap -I/usr/local/include -DBUILD_DATE=$(BUILD_DATE)

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
	@xgettext -d $(TARGET) -C -k_ -o $(POT) $(wildcard $(SEARCH))

VPATH := $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

%.o: %.cpp
	@echo "cxx: $@"
	@$(CC) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.pot *.o *.d *.exe $(TARGET)
