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

#include <cstdlib>
#include "SDL.h"
#include "surface.h"
#include "agg.h"
#include "error.h"
#include "til.h"

namespace TIL
{
    static const tilmap_t tilmap[] = 
    {
	{ CLOF32,	"CLOF32.TIL" },
	{ GROUND32,	"GROUND32.TIL" },
	{ STON,		"STON.TIL" },
    };
};

const std::string  & TIL::GetString(const til_t til)
{
    return tilmap[til].string;
}

void TIL::Reflect(Surface & sf_dst, const Surface & sf_src, const u8 shape)
{
    // valid sf_src
    if(sf_src.w() != sf_src.h())
    {
	Error::Warning("TILE::Rotate: incorrect size");

	return;
    }

    if(sf_src.depth() != 8)
    {
	Error::Warning("TILE::Rotate: use only 8 bpp");

	return;
    }

    const u8 tile_width = sf_src.w();
    const u8 tile_height = sf_src.h();

    // valid sf_dst
    if(!sf_dst.valid() || sf_dst.w() != tile_width || sf_dst.h() != tile_height)
    {
        sf_dst = Surface(tile_width, tile_height, 8, SDL_SWSURFACE);
        sf_dst.LoadPalette(AGG::Cache::Get().GetPAL());
    }

    const char * src = reinterpret_cast<const char *>(sf_src.pixels());
    char * dst = const_cast<char *>(reinterpret_cast<const char *>(sf_dst.pixels()));

    s16 x, y;

    char * dst2 = NULL;

    sf_dst.Lock();

    // draw tiles
    switch(shape % 4)
    {
        // normal
	case 0:
	    memcpy(dst, src, tile_width * tile_height);
            break;

        // vertical reflect
        case 1:
	{
	    dst2 = dst + tile_width * (tile_height - 1);

	    for(int i = 0; i < tile_height; i++)
	    {
    		memcpy(dst2, src, tile_width);

    		src += tile_width;
    		dst2 -= tile_width;
	    }
	}
            break;

        // horizontal reflect
        case 2:
            for(y = 0; y < tile_height; ++y)
                for(x = tile_width - 1; x >= 0; --x)
                {
		    dst2 = dst + y * tile_width + x;
		    *dst2 = *src;
                    ++src;
                }
            break;

        // any variant
        case 3:
            for(y = tile_height - 1; y >= 0; --y)
                for( x = tile_width - 1; x >= 0; --x)
                {
		    dst2 = dst + y * tile_width + x;
		    *dst2 = *src;
                    ++src;
                }
            break;
    }

    sf_dst.Unlock();
}
