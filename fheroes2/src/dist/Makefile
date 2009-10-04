# makefile
# project: Free Heroes2
#

ifeq ($(PREFIX),)
PREFIX := /usr/local
endif

TARGET := fheroes2
LIBENGINE := ../engine/libengine.a
CFLAGS := $(CFLAGS) -I../engine

ifndef WITHOUT_XML
LIBENGINE := $(LIBENGINE) ../xmlccwrap/libxmlccwrap.a
CFLAGS := $(CFLAGS) -I../xmlccwrap
endif

ifdef PREFIX
CFLAGS := $(CFLAGS) -I$(PREFIX)/include
endif

SVN  := $(shell svnversions ..)
CFLAGS := $(CFLAGS) -DSVN_REVISION="\"$(SVN)\""

SOURCEROOT := ../fheroes2
SOURCEDIR  := $(shell ls $(SOURCEROOT))
POT        := $(TARGET).pot

SEARCH := $(addprefix $(SOURCEROOT)/, $(addsuffix /*.cpp, $(SOURCEDIR)))

all: $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH)))) $(LIBENGINE)
	@echo "lnk: $@"
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

pot: $(wildcard $(SEARCH))
	@echo "gen: $(POT)"
	@xgettext -d $(TARGET) -C -k_ -o $(POT) $(wildcard $(SEARCH))

VPATH := $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

%.o: %.cpp
	@echo "cxx: $@"
	@$(CXX) -c -MD $(addprefix -I$(SOURCEROOT)/, $(SOURCEDIR)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	rm -f *.pot *.o *.d *.exe $(TARGET)
