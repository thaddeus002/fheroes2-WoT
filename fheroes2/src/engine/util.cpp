/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>           *
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

#include "SDL.h"
#include "types.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

const uint16_t DEFAULT_COLOR_KEY16 = 0xf0f0;
const uint32_t DEFAULT_COLOR_KEY32 = 0xff00ff00;

const uint32_t RMASK16 = 0x0000f000;
const uint32_t GMASK16 = 0x00000f00;
const uint32_t BMASK16 = 0x000000f0;
const uint32_t AMASK16 = 0x0000000f;

const uint32_t RMASK32 = 0xff000000;
const uint32_t GMASK32 = 0x00ff0000;
const uint32_t BMASK32 = 0x0000ff00;
const uint32_t AMASK32 = 0x000000ff;

#else

const uint32_t DEFAULT_COLOR_KEY16 = 0x0f0f;
const uint32_t DEFAULT_COLOR_KEY32 = 0x00ff00ff;

const uint32_t RMASK16 = 0x0000000f;
const uint32_t GMASK16 = 0x000000f0;
const uint32_t BMASK16 = 0x00000f00;
const uint32_t AMASK16 = 0x0000f000;

const uint32_t RMASK32 = 0x000000ff;
const uint32_t GMASK32 = 0x0000ff00;
const uint32_t BMASK32 = 0x00ff0000;
const uint32_t AMASK32 = 0xff000000;

#endif

void DELAY(u32 ticks)
{
    SDL_Delay(ticks);
}

uint16_t Swap16(uint16_t val)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return SDL_Swap16(val);
#else
    return val;
#endif
}

uint32_t Swap32(uint32_t val)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return SDL_Swap32(val);
#else
    return val;
#endif
}

uint16_t Load16(uint8_t *p)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return (uint16_t)((*(p + 1) << 8) | *p);
#else
    return *(uint16_t *)p;
#endif
}

uint32_t Load32(uint8_t *p)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return (uint32_t)((*(p + 3) << 24) | (*(p + 2) << 16) | (*(p + 1) << 8) | *p);
#else
    return *(uint32_t *)p;
#endif
}
