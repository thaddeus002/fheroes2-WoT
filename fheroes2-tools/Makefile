# makefile
# project: Free Heroes2 Tools
#

ifndef CXX
CXX := g++
endif

ifndef CC
CC := gcc
endif

IMAGE_TARGETS := bmp2img til2img
AUDIO_TARGETS := 82m2wav xmi2mid
GENERAL_TARGETS := extractor
# extractor_lod
PROGS_YANN := bmp2img extractor_lod
TARGETS := $(GENERAL_TARGETS) $(LOD_TARGETS) $(IMAGE_TARGETS) $(AUDIO_TARGETS)


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

#ifndef WITHOUT_UNICODE
#CFLAGS := $(CFLAGS) -DWITH_TTF
#SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
#endif

#ifndef WITHOUT_NETWORK
#CFLAGS := $(CFLAGS) -DWITH_NET_DISABLE
#SDL_LIBS := $(SDL_LIBS) -lSDL_net
#endif

#ifndef WITHOUT_XML
#CFLAGS := $(CFLAGS) -DWITH_XML
#endif

#ifndef WITHOUT_ZLIB
#CFLAGS := $(CFLAGS) -DWITH_ZLIB
#LIBS := $(LIBS) -lz
#endif



CFLAGS := $(SDL_FLAGS) $(CFLAGS)
LIBS := $(SDL_LIBS) $(LIBS)



all: $(TARGETS)
#all: extractor_lod

$(GENERAL_TARGETS): $(addsuffix .c, $(GENERAL_TARGETS)) aggregation.o aggregation_lod.o
	$(CC) -c $@.c -Wall -O2
	$(CC) -o $@ $@.o aggregation.o aggregation_lod.o -lz

#extractor_lod: extractor_lod.cpp aggregation_lod.o
#	$(CXX) -c $@.c -Wall -fsigned-char -O2  -I../libzpipe
#	$(CXX) -o $@ $@.o aggregation_lod.o -L../libzpipe  -lzpipe -lz

aggregation.o : aggregation.c
	$(CC) -c aggregation.c -Wall -O2

aggregation_lod.o : aggregation_lod.c
	$(CC) -c aggregation_lod.c -Wall -O2

YG_CFLAGS=$(shell libpng-config --cflags)
YG_LDFLAGS=$(shell libpng-config --ldflags)


$(AUDIO_TARGETS): $(addsuffix .cpp, $(AUDIO_TARGETS))
	#$(CXX) -c $@.cpp $(CFLAGS)
	$(CXX) -c $@.cpp -I/usr/include/SDL -I../engine  -O2
	#$(CXX) -o $@ $@.o $(LDFLAGS) $(LIBS)
	$(CXX) -o $@ $@.o -L../engine -lengine


$(IMAGE_TARGETS): $(addsuffix .cpp, $(IMAGE_TARGETS)) yImage.o
	$(CXX) -c $@.cpp $(YG_CFLAGS) $(SDL_FLAGS) -DHAVE_LIBPNG -DWITH_IMAGE
	#$(CXX) -o $@ $@.o yImage.o $(YG_LDFLAGS) $(SDL_LIBS)
	$(CXX) -o $@ $@.o yImage.o -L../engine -lengine -L/usr/lib -lpng


til2img: til2img.cpp palette.o yImage.o
	$(CXX) -c $@.cpp $(YG_CFLAGS) -DHAVE_LIBPNG -DWITH_IMAGE
	$(CXX) -o $@ $@.o yImage.o palette.o -L/usr/lib -lpng

convert: convert.cpp icn2img.o yImage.o palette.o
	$(CXX) -c $@.cpp
	$(CXX) -o $@ $@.o icn2img.o yImage.o palette.o $(shell libpng-config --ldflags)

icn2img.o : icn2img.cpp icn2img.h
	#$(CXX) -c icn2img.cpp -DHAVE_LIBPNG -I/usr/local/include/libpng15 -D_GNU_SOURCE=1 -D_REENTRANT
	$(CXX) -c icn2img.cpp -DHAVE_LIBPNG $(shell libpng-config --cflags)



palette.o : palette.c palette.h
	$(CXX) -c palette.c



#bmp2img: bmp2img.cpp yImage.o
#	$(CXX) -c $@.cpp $(YG_CFLAGS) $(SDL_FLAGS)
#	#$(CXX) -o $@ $@.o yImage.o $(YG_LDFLAGS) $(SDL_LIBS)
#	$(CXX) -o $@ $@.o yImage.o -L../engine -lengine -L/usr/lib -lpng15

#icn2img: icn2img.cpp yImage.o
#	$(CXX) -c $@.cpp -DWITH_IMAGE $(YG_CFLAGS) $(SDL_FLAGS)
#	#$(CXX) -o $@ $@.o yImage.o $(YG_LDFLAGS) $(SDL_LIBS)
#	$(CXX) -o $@ $@.o yImage.o -L../engine -lengine -L/usr/lib -lpng15

yImage.o: yImage.c yImage.h
	$(CXX) -c yImage.c -DHAVE_LIBPNG


.PHONY: clean

clean:
	rm -f *.o *.exe $(TARGETS)
