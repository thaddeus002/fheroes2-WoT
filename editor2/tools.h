#ifndef _TOOLS_H
#define _TOOLS_H

#include "gamedefs.h"
#include "agg.h"

#define FONT_LETTER_BEGIN	0x20
#define FONT_LETTER_END		0x7F

typedef enum {FONT_BIG, FONT_SMALL} ENUMFONT;

BOOL ValidPoint(SDL_Rect *, Uint16, Uint16);
BOOL CompareRect(SDL_Rect *, SDL_Rect *);

Uint16 GetHiUint32(Uint32);
Uint16 GetLoUint32(Uint32);
Uint8  GetHiUint16(Uint16);
Uint8  GetLoUint16(Uint16);
Uint32 PackUint32(Uint16, Uint16);
Uint16 PackUint16(Uint8, Uint8);
Uint32 GetSizeSurface(SDL_Surface *);

void FillSPRITE(AGGSPRITE *, const char *, Uint16);

void PrintText(SDL_Surface *, SDL_Rect *, const char *, ENUMFONT);

#endif
