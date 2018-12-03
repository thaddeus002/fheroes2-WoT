# makefile
# project: Free Heroes2 Tools
#

ifndef CXX
CXX := g++
endif

ifndef CC
CC := gcc
endif

YIMAGE_TARGETS := yImage.o yColor.o yImage_io.o
AUDIO_TARGETS := 82m2wav xmi2mid
GENERAL_TARGETS := extractor aggregator
TARGETS := $(GENERAL_TARGETS) $(IMAGE_TARGETS) $(AUDIO_TARGETS)
# What is ready
TARGETS := $(GENERAL_TARGETS)


CFLAGS := $(CFLAGS) -O2

LIBS := -lengine $(LIBS)
CFLAGS :=  -I../engine $(CFLAGS)
LDFLAGS := -L../engine $(LDFLAGS)



SDL_LIBS := $(shell sdl-config --libs)
SDL_FLAGS := $(shell sdl-config --cflags)


ifndef WITHOUT_MIXER
CFLAGS := $(CFLAGS) -DWITH_MIXER
SDL_LIBS := $(SDL_LIBS) -lSDL_mixer
endif

ifndef WITHOUT_IMAGE
CFLAGS := $(CFLAGS) -DWITH_IMAGE $(shell libpng-config --cflags) -DWITH_ZLIB
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(shell libpng-config --libs) -lz
endif



CFLAGS := $(SDL_FLAGS) $(CFLAGS)
LIBS := $(SDL_LIBS) $(LIBS)



all: $(TARGETS)
	@echo "Done"

extractor: extractor.c aggregation.o
	$(CC) -c $@.c -Wall -O2
	$(CC) -o $@ $@.o aggregation.o -lz

aggregator: aggregator.c aggregation.o
	$(CC) -c $@.c -Wall -O2
	$(CC) -o $@ $@.o aggregation.o -lz


aggregation.o : aggregation.c
	$(CC) -c aggregation.c -Wall -O2

YG_CFLAGS=$(shell libpng-config --cflags)
YG_LDFLAGS=$(shell libpng-config --ldflags)


$(AUDIO_TARGETS): $(addsuffix .cpp, $(AUDIO_TARGETS))
	#$(CXX) -c $@.cpp $(CFLAGS)
	$(CXX) -c $@.cpp -I/usr/include/SDL -I../engine  -O2
	#$(CXX) -o $@ $@.o $(LDFLAGS) $(LIBS)
	$(CXX) -o $@ $@.o -L../engine -lengine



convert: convert.cpp icn2img.o yImage.o palette.o
	$(CXX) -c $@.cpp
	$(CXX) -o $@ $@.o icn2img.o yImage.o palette.o $(shell libpng-config --ldflags)


palette.o : palette.cpp palette.h
	$(CXX) -c palette.cpp -IyImage


til2img: til2img.cpp palette.o $(YIMAGE_TARGETS)
	$(CXX) -c $@.cpp $(YG_CFLAGS) -DHAVE_LIBPNG -DWITH_IMAGE -IyImage
	$(CXX) -o $@ $@.o $(YIMAGE_TARGETS) palette.o -L/usr/lib -lpng

bmp2img: bmp2img.cpp $(YIMAGE_TARGETS) h2bmp.o
	$(CXX) -c $< $(YG_CFLAGS) -IyImage
	$(CXX) -o $@ $@.o $(YIMAGE_TARGETS) h2bmp.o $(YG_LDFLAGS)

icn2img: icn2img.cpp $(YIMAGE_TARGETS) palette.o h2icn.o
	$(CXX) -c $< -DHAVE_LIBPNG $(shell libpng-config --cflags) -IyImage
	$(CXX) -o $@ $@.o $(YIMAGE_TARGETS) palette.o h2icn.o $(YG_LDFLAGS)


$(YIMAGE_TARGETS): yImage/$(subst .o,.c,$@) yImage/$(subst .o,.h,$@)
	$(CC) -o $@ -c yImage/$(subst .o,.c,$@) -DHAVE_LIBPNG

h2bmp.o: h2bmp.cpp h2bmp.h
	$(CXX) -c $< -IyImage

h2icn.o: h2icn.cpp h2icn.h
	$(CXX) -c -DHAVE_LIBPNG $< -IyImage

.PHONY: clean

clean:
	rm -f *.o $(TARGETS)
