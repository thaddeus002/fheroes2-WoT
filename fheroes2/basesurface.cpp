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
#include "agg.h"
#include "error.h"

/* operator equals */
BaseSurface & BaseSurface::operator= (const BaseSurface & bs)
{
    if(! bs.valid()) Error::Except("BaseSurface(const BaseSurface &): empty surface");

    if(valid()) FreeSurface();
    surface = SDL_ConvertSurface(const_cast<SDL_Surface *>(bs.GetSurface()), const_cast<SDL_PixelFormat *>(bs.GetPixelFormat()), bs.flags());

    if(!surface) Error::Except("BaseSurface(const BaseSurface &): " + std::string(SDL_GetError()));
    return *this;
}

/* create new surface */
void BaseSurface::CreateSurface(u16 sw, u16 sh, u8 dp, u32 fl)
{
    u32 rmask = SDL_SRCALPHA & fl ? RMASK : 0;
    u32 gmask = SDL_SRCALPHA & fl ? GMASK : 0;
    u32 bmask = SDL_SRCALPHA & fl ? BMASK : 0;
    u32 amask = SDL_SRCALPHA & fl ? AMASK : 0;

    surface = SDL_CreateRGBSurface(fl, sw, sh, dp, rmask, gmask, bmask, amask);

    if(!surface) Error::Except("BaseSurface::CreateSurface: " + std::string(SDL_GetError()));
}

/* load  palette for 8bit surface */
void BaseSurface::LoadPalette(const SDL_Color *colors)
{
    if(!colors) Error::Except("BaseSurface::LoadPalette: empty palette.");
    if(!valid()) Error::Except("BaseSurface::LoadPalette: invalid surface.");

    SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, const_cast<SDL_Color *>(colors), 0, AGGSIZEPALETTE);
}

/* format surface */
void BaseSurface::SetDisplayFormat(void)
{
    SDL_Surface *osurface = surface;
    surface = (osurface->flags & SDL_SRCALPHA ? SDL_DisplayFormatAlpha(osurface) : SDL_DisplayFormat(osurface));

    SDL_FreeSurface(osurface);
}

/* draw u16 pixel */
void BaseSurface::SetPixel2(u16 x, u16 y, u32 color)
{
    if(x > surface->w || y > surface->h) return;
    
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * surface->pitch / 2 + x;

    *bufp = static_cast<u16>(color);
}

/* draw u8 pixel */
void BaseSurface::SetPixel1(u16 x, u16 y, u8 color)
{
    if(x > surface->w || y > surface->h) Error::Except("BaseSurface::SetPixel1: out of range");

    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;

    *bufp = color;
}

/* set color key */
void BaseSurface::SetColorKey(void)
{
    Fill(AGG::GetColorKey());
    SetColorKey(AGG::GetColorKey());
}

/* fill colors surface */
void BaseSurface::Fill(u32 color)
{
    SDL_Rect dstrect = {0, 0, surface->w, surface->h};

    SDL_FillRect(surface, &dstrect, color);
}

/* rect fill colors surface */
void BaseSurface::FillRect(u32 color, const Rect & src)
{
    SDL_Rect dstrect = {src.x, src.y, src.w, src.h};

    SDL_FillRect(surface, &dstrect, color);
}

/* blit */
void BaseSurface::Blit(const BaseSurface &src)
{
    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), NULL, surface, NULL);
}

/* blit */
void BaseSurface::Blit(const BaseSurface &src, s16 dst_ox, s16 dst_oy)
{
    SDL_Rect dstrect = { dst_ox, dst_oy, surface->w, surface->h };

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), NULL, surface, &dstrect);
}

/* blit */
void BaseSurface::Blit(const BaseSurface &src, const Rect &src_rt, s16 dst_ox, s16 dst_oy)
{
    SDL_Rect srcrect = { src_rt.x, src_rt.y, src_rt.w, src_rt.h };
    SDL_Rect dstrect = { dst_ox, dst_oy, src_rt.w, src_rt.w };

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), &srcrect, surface, &dstrect);
}

/* scaled from surface */
void BaseSurface::ScaleFrom(const BaseSurface & bs)
{
    if(bs.w() <= w() || bs.h() <= h()) Error::Warning("BaseSurface::ScaleFrom: incorrect size.");
    
    u16 *p_src = NULL;
    u16 *p_dst = NULL;

    u16 width = 0;
    u16 height = 0;

    u8 count = 0;

    BaseSurface & src = const_cast<BaseSurface &>(bs);
    
    src.Lock();
    p_src = static_cast<u16 *>(const_cast<void *>(src.pixels()));

    // count min iteration    
    width = src.w();
    for(;;)
    {
	++count;
	if((width >>= 1) <= w()) break;
    }
    width = src.w() / 2;
    height = src.h() / 2;

    bool first = true;

    while(count)
    {
	p_dst = new u16[width * height];

	// iteration 2х2 -> 1
	u32 index = 0;
	u16 width2 = 2 * width;

	for(u16 dstY = 0; dstY < height; ++dstY)
	    for(u16 dstX = 0; dstX < width; ++dstX)
	    {
		u16 dstX2 = dstX * 2;
		u16 dstY2 = dstY * 2;

		u16 color = 0;

		if((color = p_src[width2 * dstY2 + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
		    color == p_src[width2 * dstY2 + dstX2 + 1] || color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;
		else
		if((color = p_src[width2 * dstY2 + dstX2 + 1]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
	    	    color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;
		else
		if((color = p_src[width2 * (dstY2 + 1) + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1])
		    p_dst[index] = color;
		else
		    p_dst[index] = p_src[width2 * (dstY2 + 1) + dstX2 + 1];

		++index;
	    }

	if(!first && NULL != p_src) delete [] p_src;
	first = false;
	p_src = p_dst;
	p_dst = NULL;

	width = width / 2;
	height = height / 2;

	--count;
    }

    src.Unlock();

    Lock();
    memcpy(const_cast<void *>(pixels()), p_src, sizeof(u16) * (width * 2) * (height * 2));
    Unlock();

    delete [] p_src;
}
