#ifndef _AGG_H
#define _AGG_H

#define AGGSIZENAME     15

#include "SDL.h"
#include "gamedefs.h"

typedef enum {ICN, WAV, TIL, BMP, XMI, BIN, PAL, FNT, UNK} AGGTYPE;

typedef struct {
    char	name[AGGSIZENAME];
    Uint16	number;
} AGGSPRITE;

typedef struct {
    Uint16      offsetX;
    Uint16      offsetY;
    SDL_Surface *surface;
    void        *next;
} ICNHEADER;

BOOL            InitAGG(const char *);
void            FreeAGG(void);
void		PreloadObject(const char *);
void		FreeObject(const char *);

SDL_Surface *   GetICNSprite(AGGSPRITE *);
ICNHEADER *     GetICNHeader(AGGSPRITE *);
Uint8 *		GetTILData(const char *);

SDL_AudioCVT *  GetAudioCVT(const char *);

AGGTYPE		ExistsAGGName(const char *);

SDL_Color *     GetPalette(void);

Uint32		GetCurrentSizeMemory(void);

#endif
