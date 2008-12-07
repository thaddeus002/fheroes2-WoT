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

#include "types.h"

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
