#ifndef _TOOLS_H
#define _TOOLS_H

#define ICNRLE_DEBUG 0

typedef struct __attribute__((packed)){
    Uint16		type;
    Uint32		size;
    Uint16		rez1;
    Uint16		rez2;
    Uint32		offset;
}bitmapfileheader;

typedef struct {
    Uint32		size;
    Uint32		width;
    Uint32		height;
    Uint16		planes;
    Uint16		bitcount;
    Uint32		compression;
    Uint32		sizeimage;
    Uint32		xpels;
    Uint32		ypels;
    Uint32		colorsused;
    Uint32		colorsimportant;
}bitmapinfoheader;

typedef struct{
    Uint8	b;
    Uint8	g;
    Uint8	r;
    Uint8	null;
}bitmapcolor;

typedef struct __attribute__((packed)){
    bitmapfileheader fileheader;
    bitmapinfoheader infoheader;
    bitmapcolor	     color[256];
}bitmapheader;

typedef struct __attribute__((packed)){
	Uint16		unknown1;
	Uint16		unknown2;
	Uint16		w;
	Uint16		h;
	Uint8		unknown3;
	Uint32		offset;
}icnspriteheader;

void DrawScene(SDL_Surface *, Uint8 *, Uint32);
void DrawPixel(SDL_Surface *, Uint16, Uint16, Uint8);
void Slock(SDL_Surface *);
void Sulock(SDL_Surface *);

#endif
