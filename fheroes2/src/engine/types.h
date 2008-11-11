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

#include "SDL.h"
#include "SDL_mixer.h"
#include "l10n.h"

typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;

#define MAXU16   65535
#define MAXU32   4294967295

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#define SEPARATOR       '\\'
#else
#define SEPARATOR       '/'
#endif


#define KEY_RETURN	SDLK_RETURN
#define KEY_ESCAPE	SDLK_ESCAPE

#define DELAY(X)	SDL_Delay(X)

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define DEFAULT_COLOR_KEY16	0xf0f0
#define DEFAULT_COLOR_KEY32	0xff00ff00

#define RMASK16 0x0000f000
#define GMASK16 0x00000f00
#define BMASK16 0x000000f0
#define AMASK16 0x0000000f

#define RMASK32 0xff000000
#define GMASK32 0x00ff0000
#define BMASK32 0x0000ff00
#define AMASK32 0x000000ff

#define SWAP16(X)    X=SDL_Swap16(X)
#define SWAP32(X)    X=SDL_Swap32(X)

// slow implementation uint16_t = *(uint8_t *)
#define LOAD16(p, b)	b=(((uint16_t)(((b) & 0) | *(((uint8_t *)p) + 1)) << 8) | *(((uint8_t *)p)))
// slow implementation uint32_t = *(uint8_t *)
#define LOAD32(p, b)	b=(((((((uint32_t)(((b) & 0) | *(((uint8_t *)p) + 3)) << 8) | *(((uint8_t *)p) + 2)) << 8) | *(((uint8_t *)p) + 1)) << 8) | *((uint8_t *)p))

#else

#define DEFAULT_COLOR_KEY16	0x0f0f
#define DEFAULT_COLOR_KEY32	0x00ff00ff

#define RMASK16 0x0000000f
#define GMASK16 0x000000f0
#define BMASK16 0x00000f00
#define AMASK16 0x0000f000

#define RMASK32 0x000000ff
#define GMASK32 0x0000ff00
#define BMASK32 0x00ff0000
#define AMASK32 0xff000000

#define SWAP16(X) ;
#define SWAP32(X) ;

// implementation uint16_t = *(uint8_t *)
#define LOAD16(p, b)	b=(*(uint16_t *)(p))

// implementation uint32_t = *(uint8_t *)
#define LOAD32(p, b)	b=(*(uint32_t *)(p))

#endif

#endif
