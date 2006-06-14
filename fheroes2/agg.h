/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#ifndef _AGG_H
#define _AGG_H

#include "SDL.h"
#include "gamedefs.h"

#define AGGSIZENAME     15

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

BOOL            InitAGG();
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
