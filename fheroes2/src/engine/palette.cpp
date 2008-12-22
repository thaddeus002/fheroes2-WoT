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

#include "SDL.h"
#include "surface.h"
#include "error.h"
#include "palette_h2.h"
#include "palette.h"

Palette::Palette()
{
    const u16 size = 255;

    sdlpal = new SDL_Palette;
    sdlpal->ncolors = size;
    sdlpal->colors  = new SDL_Color[size];

    pal.resize(size);

    Surface sfa;
    sfa.CreateSurface(1, 1, DEFAULT_DEPTH, SDL_SWSURFACE|SDL_SRCALPHA);
    sfa.SetAlpha(255);

    const unsigned char *p = kb_pal;

    for(u16 ii = 0; ii < size; ++ii)
    {
        sdlpal->colors[ii].r = *p++;
        sdlpal->colors[ii].g = *p++;
        sdlpal->colors[ii].b = *p++;

        sdlpal->colors[ii].r <<= 2;
        sdlpal->colors[ii].g <<= 2;
        sdlpal->colors[ii].b <<= 2;

        pal[ii] = SDL_MapRGBA(sfa.surface->format, sdlpal->colors[ii].r, sdlpal->colors[ii].g, sdlpal->colors[ii].b, 0xFF);
    }
}

Palette::~Palette()
{
    if(sdlpal)
    {
	if(sdlpal->colors) delete [] sdlpal->colors;
	delete sdlpal;
    }
}

Palette & Palette::Get(void)
{
    static Palette pal_cache;

    return pal_cache;
}

u16 Palette::Size(void) const
{
    return pal.size();
}

u32 Palette::GetColor(u16 index) const
{
    return index < pal.size() ? pal[index] : 0;
}

const SDL_Palette * Palette::SDLPalette(void) const
{
    return sdlpal;
}
