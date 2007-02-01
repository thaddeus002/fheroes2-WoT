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
#include "sdlmm.h"

typedef char		s8;
typedef unsigned char	u8;
typedef short		s16;
typedef unsigned short	u16;
typedef int		s32;
typedef unsigned int	u32;

namespace Display
{
    typedef enum { SMALL = 640, MEDIUM = 800, LARGE = 1024, XLARGE = 1280 } resolution_t;

    void SetVideoMode(resolution_t mode);

    void HideCursor(void);
    void ShowCursor(void);
    void SetCaption(const std::string &caption);
};

static SDLmm::Display &display = SDLmm::Display::GetDisplay();

// PUBLIC
#define GAME_VERSION		20070129	// Version


// PRIVATE
#define DEFAULT_DEPTH		16		// Surface use bits color
#define	DEFAULT_SHADOW_ALPHA	0x40		// shadow alpha

#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define RMASK 0x0000f000 
#define GMASK 0x00000f00 
#define BMASK 0x000000f0 
#define AMASK 0x0000000f 
#else 
#define RMASK 0x0000000f 
#define GMASK 0x000000f0 
#define BMASK 0x00000f00 
#define AMASK 0x0000f000 
#endif 

#endif
