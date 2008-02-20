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

#include "surface.h"
#include "error.h"
#include "palette.h"

Palette::Palette()
{
    ncolors = 0;
    colors  = NULL;
}

Palette::Palette(const std::vector<char> & v)
{
    ncolors = v.size() / 3;
    
    if(ncolors)
    {
	pal.resize(ncolors);

	Surface surface(1, 1, true);
        surface.SetAlpha(255);

    	colors = new SDL_Color[ncolors];
	const char * p = & v[0];

	for(u16 ii = 0; ii < ncolors; ++ii)
	{
	    colors[ii].r = *p++;
    	    colors[ii].g = *p++;
    	    colors[ii].b = *p++;

	    colors[ii].r <<= 2;
    	    colors[ii].g <<= 2;
    	    colors[ii].b <<= 2;

    	    pal[ii] = surface.MapRGB(colors[ii].r, colors[ii].g, colors[ii].b);
	}
    }
}

Palette::Palette(const Palette & p)
{
    ncolors = p.ncolors;
    colors = new SDL_Color[ncolors];

    memcpy(colors, p.colors, ncolors * sizeof(SDL_Color));

    pal.resize(ncolors);

    Surface surface(1, 1, true);
    surface.SetAlpha(255);

    for(u16 ii = 0; ii < ncolors; ++ii)
	pal[ii] = surface.MapRGB(colors[ii].r, colors[ii].g, colors[ii].b);
}

Palette::~Palette()
{
    if(colors) delete [] colors;
}

Palette & Palette::operator= (const Palette & p)
{
    if(colors) delete [] colors;

    ncolors = p.ncolors;
    colors = new SDL_Color[ncolors];

    memcpy(colors, p.colors, ncolors * sizeof(SDL_Color));

    pal.resize(ncolors);

    Surface surface(1, 1, true);
    surface.SetAlpha(255);

    for(u16 ii = 0; ii < ncolors; ++ii)
	pal[ii] = surface.MapRGB(colors[ii].r, colors[ii].g, colors[ii].b);

    return *this;
}

void Palette::Load(const std::vector<char> & v)
{
    if(colors) delete [] colors;

    ncolors = v.size() / 3;

    if(ncolors)
    {
	pal.resize(ncolors);

	Surface surface(1, 1, true);
	surface.SetAlpha(255);

	colors = new SDL_Color[ncolors];
	const char * p = & v[0];

	for(u16 ii = 0; ii < ncolors; ++ii)
	{
	    colors[ii].r = *p++;
    	    colors[ii].g = *p++;
    	    colors[ii].b = *p++;

	    colors[ii].r <<= 2;
    	    colors[ii].g <<= 2;
    	    colors[ii].b <<= 2;

	    pal[ii] = surface.MapRGB(colors[ii].r, colors[ii].g, colors[ii].b);
	}

	return;
    }

    Error::Warning("Palette::Load: size == 0");
}

u32 Palette::Size(void) const
{
    return ncolors;
}

const SDL_Color * Palette::SDLColors(void) const
{
    return colors;
}

u32 Palette::Color(const u16 index) const
{
    return index < ncolors ? pal[index] : 0;
}
