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
#include "palette.h"

Palette::Palette()
{
    sdlpal = new SDL_Palette;
    sdlpal->ncolors = 0;
    sdlpal->colors  = NULL;
}

Palette::Palette(const std::vector<char> & v)
{
    sdlpal->ncolors = v.size() / 3;
    
    if(sdlpal->ncolors)
    {
	pal.resize(sdlpal->ncolors);

	Surface surface(1, 1, true);
        surface.SetAlpha(255);

    	sdlpal->colors = new SDL_Color[sdlpal->ncolors];
	const char * p = & v[0];

	for(u16 ii = 0; ii < sdlpal->ncolors; ++ii)
	{
	    sdlpal->colors[ii].r = *p++;
    	    sdlpal->colors[ii].g = *p++;
    	    sdlpal->colors[ii].b = *p++;

	    sdlpal->colors[ii].r <<= 2;
    	    sdlpal->colors[ii].g <<= 2;
    	    sdlpal->colors[ii].b <<= 2;

    	    pal[ii] = surface.MapRGB(sdlpal->colors[ii].r, sdlpal->colors[ii].g, sdlpal->colors[ii].b);
	}
    }
}

Palette::Palette(const Palette & p)
{
    sdlpal->ncolors = p.sdlpal->ncolors;
    sdlpal->colors = new SDL_Color[sdlpal->ncolors];

    memcpy(sdlpal->colors, p.sdlpal->colors, sdlpal->ncolors * sizeof(SDL_Color));

    pal.resize(sdlpal->ncolors);

    Surface surface(1, 1, true);
    surface.SetAlpha(255);

    for(u16 ii = 0; ii < sdlpal->ncolors; ++ii)
	pal[ii] = surface.MapRGB(sdlpal->colors[ii].r, sdlpal->colors[ii].g, sdlpal->colors[ii].b);
}

Palette::~Palette()
{
    if(sdlpal->colors) delete [] sdlpal->colors;
    delete sdlpal;
}

Palette & Palette::operator= (const Palette & p)
{
    if(sdlpal->colors) delete [] sdlpal->colors;

    sdlpal->ncolors = p.sdlpal->ncolors;
    sdlpal->colors = new SDL_Color[sdlpal->ncolors];

    memcpy(sdlpal->colors, p.sdlpal->colors, sdlpal->ncolors * sizeof(SDL_Color));

    pal.resize(sdlpal->ncolors);

    Surface surface(1, 1, true);
    surface.SetAlpha(255);

    for(u16 ii = 0; ii < sdlpal->ncolors; ++ii)
	pal[ii] = surface.MapRGB(sdlpal->colors[ii].r, sdlpal->colors[ii].g, sdlpal->colors[ii].b);

    return *this;
}

void Palette::Load(const std::vector<char> & v)
{
    if(sdlpal->colors) delete [] sdlpal->colors;

    sdlpal->ncolors = v.size() / 3;

    if(sdlpal->ncolors)
    {
	pal.resize(sdlpal->ncolors);

	Surface surface(1, 1, true);
	surface.SetAlpha(255);

	sdlpal->colors = new SDL_Color[sdlpal->ncolors];
	const char * p = & v[0];

	for(u16 ii = 0; ii < sdlpal->ncolors; ++ii)
	{
	    sdlpal->colors[ii].r = *p++;
    	    sdlpal->colors[ii].g = *p++;
    	    sdlpal->colors[ii].b = *p++;

	    sdlpal->colors[ii].r <<= 2;
    	    sdlpal->colors[ii].g <<= 2;
    	    sdlpal->colors[ii].b <<= 2;

	    pal[ii] = surface.MapRGB(sdlpal->colors[ii].r, sdlpal->colors[ii].g, sdlpal->colors[ii].b);
	}

	return;
    }

    Error::Warning("Palette::Load: size == 0");
}

u32 Palette::Size(void) const
{
    return sdlpal->ncolors;
}

const SDL_Color * Palette::SDLColors(void) const
{
    return sdlpal->colors;
}

u32 Palette::Color(const u16 index) const
{
    return index < sdlpal->ncolors ? pal[index] : 0;
}
