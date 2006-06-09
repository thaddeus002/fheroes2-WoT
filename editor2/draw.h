#ifndef _DRAW_H
#define _DRAW_H

#include "agg.h"

#define ICNRLE_DEBUG 0

void DrawSceneVar2(SDL_Surface *, Uint8 *, Uint32);
void DrawPixel(SDL_Surface *, Uint16, Uint16, Uint8);
void DrawPixelAlpha(SDL_Surface *, Uint16, Uint16, Uint8, Uint8, Uint8, Uint8);
void DrawSprite(SDL_Rect *, AGGSPRITE *);
void DrawTILBmp(SDL_Surface *, Uint8 *, Uint16, Uint8);
void DrawMapObject(SDL_Rect *, Uint8, Uint8);
void DrawAnimationFrame(SDL_Rect *, Uint8, Uint8, Uint8);

Uint16 GetPixel(SDL_Surface *, Uint16, Uint16);
void PutPixel(SDL_Surface *, Uint16, Uint16, Uint16);

void ScaleSurface(SDL_Surface *, SDL_Surface *);

void LockSurface(SDL_Surface *);
void UnlockSurface(SDL_Surface *);

#endif
