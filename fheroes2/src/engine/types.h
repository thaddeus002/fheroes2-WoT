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

typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;

#define MAXU16   0xFFFF
#define MAXU32   0xFFFFFFFF

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#define SEPARATOR       '\\'
#else
#define SEPARATOR       '/'
#endif

void DELAY(uint32_t);

extern const uint32_t DEFAULT_COLOR_KEY16;
extern const uint32_t DEFAULT_COLOR_KEY32;

extern const uint32_t RMASK16;
extern const uint32_t GMASK16;
extern const uint32_t BMASK16;
extern const uint32_t AMASK16;

extern const uint32_t RMASK32;
extern const uint32_t GMASK32;
extern const uint32_t BMASK32;
extern const uint32_t AMASK32;

uint16_t Swap16(uint16_t);
uint32_t Swap32(uint32_t);

uint16_t Load16(uint8_t *p);
uint32_t Load32(uint8_t *p);

#define SWAP16(X)    X=Swap16(X)
#define SWAP32(X)    X=Swap32(X)

#define LOAD16(p, b) b=Load16((uint8_t *)p)
#define LOAD32(p, b) b=Load32((uint8_t *)p)

#endif
