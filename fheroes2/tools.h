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

#ifndef _TOOLS_H
#define _TOOLS_H

#include "gamedefs.h"
#include "agg.h"

#define FONT_LETTER_BEGIN	0x20
#define FONT_LETTER_END		0x7F
#define FONT_WIDTHBIG           0x0E 
#define FONT_WIDTHSMALL         0x07
#define FONT_HEIGHTBIG          0x10
#define FONT_HEIGHTSMALL        0x0A 

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

void	PrintText(SDL_Surface *, SDL_Rect *, const char *, ENUMFONT);
Uint16	GetLengthText(const char *, ENUMFONT);

#endif
