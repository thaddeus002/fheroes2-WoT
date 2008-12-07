/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef H2TYPES_H
#define H2TYPES_H

#include "l10n.h"
#include "SDL.h"

typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;

typedef SDL_Color Colors;

#define MAXU16   0xFFFF
#define MAXU32   0xFFFFFFFF

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#define SEPARATOR       '\\'
#else
#define SEPARATOR       '/'
#endif

#define DELAY(X)	SDL_Delay(X)

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define DEFAULT_COLOR_KEY16	0xf0f0
#define DEFAULT_COLOR_KEY32	0xff00ff00

#define RMASK16			0x0000f000
#define GMASK16			0x00000f00
#define BMASK16			0x000000f0
#define AMASK16			0x0000000f

#define RMASK32			0xff000000
#define GMASK32			0x00ff0000
#define BMASK32			0x0000ff00
#define AMASK32			0x000000ff

#else

#define DEFAULT_COLOR_KEY16	0x0f0f
#define DEFAULT_COLOR_KEY32	0x00ff00ff

#define RMASK16			0x0000000f
#define GMASK16			0x000000f0
#define BMASK16			0x00000f00
#define AMASK16			0x0000f000

#define RMASK32			0x000000ff
#define GMASK32			0x0000ff00
#define BMASK32			0x00ff0000
#define AMASK32			0xff000000

#endif

uint16_t Swap16(uint16_t);
uint32_t Swap32(uint32_t);

uint16_t Load16(uint8_t *p);
uint32_t Load32(uint8_t *p);

#define SWAP16(X)    X=Swap16(X)
#define SWAP32(X)    X=Swap32(X)

#define LOAD16(p, b) b=Load16((uint8_t *)p)
#define LOAD32(p, b) b=Load32((uint8_t *)p)

#endif
