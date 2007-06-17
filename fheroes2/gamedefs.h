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
#ifndef H2GAMEDEFS_H
#define H2GAMEDEFS_H

#include "SDL.h"

typedef char		s8;
typedef unsigned char	u8;
typedef short		s16;
typedef unsigned short	u16;
typedef int		s32;
typedef unsigned int	u32;

namespace Font
{
    typedef enum { SMALL, BIG } type_t;
};

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#define MAXU16   65535
#define MAXU32   4294967295

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define RMASK 0x0000f000
#define GMASK 0x00000f00
#define BMASK 0x000000f0
#define AMASK 0x0000000f

#define SWAP16(X)    X=SDL_Swap16(X)
#define SWAP32(X)    X=SDL_Swap32(X)

#else

#define RMASK 0x0000000f
#define GMASK 0x000000f0
#define BMASK 0x00000f00
#define AMASK 0x0000f000

#define SWAP16(X) ;
#define SWAP32(X) ;

#endif

#endif
