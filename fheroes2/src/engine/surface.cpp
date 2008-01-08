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

Surface::Surface() : surface(NULL)
{
}

Surface::Surface(const unsigned char * pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool alpha)
{
    surface = SDL_CreateRGBSurfaceFrom((void *) pixels, width, height, bytes_per_pixel * 8, width * bytes_per_pixel,
		RMASK32, GMASK32, BMASK32, alpha ? AMASK32 : 0);

    if(!surface) Error::Warning("SurfaceFrom: empty surface, error: " + std::string(SDL_GetError()));
}

Surface::Surface(u16 sw, u16 sh, u8 depth, u32 fl)
{
    CreateSurface(sw, sh, depth,  fl);
}

Surface::Surface(u16 sw, u16 sh, bool alpha)
{
    CreateSurface(sw, sh, DEFAULT_DEPTH, alpha ? SDL_SRCALPHA|SDL_SWSURFACE : SDL_SWSURFACE);
}

Surface::Surface(const Surface & bs)
{
    if(bs.valid())
	surface = SDL_ConvertSurface(const_cast<SDL_Surface *>(bs.GetSurface()), const_cast<SDL_PixelFormat *>(bs.GetPixelFormat()), bs.flags());
}


Surface::~Surface()
{
    if(surface)
	SDL_FreeSurface(surface);

    surface = NULL;
}

/* operator = */
Surface & Surface::operator= (const Surface & bs)
{
    if(surface) SDL_FreeSurface(surface);

    if(bs.valid())
    {
	surface = SDL_ConvertSurface(const_cast<SDL_Surface *>(bs.GetSurface()), const_cast<SDL_PixelFormat *>(bs.GetPixelFormat()), bs.flags());

	if(!surface) Error::Warning("Surface: operator, error: " + std::string(SDL_GetError()));

    }
    else
	Error::Warning("Surface: operator, empty surface");

    return *this;
}

/* create new surface */
void Surface::CreateSurface(u16 sw, u16 sh, u8 dp, u32 fl)
{
    u32 rmask = RMASK16;
    u32 gmask = GMASK16;
    u32 bmask = BMASK16;
    u32 amask = SDL_SRCALPHA & fl ? AMASK16 : 0;

    surface = SDL_CreateRGBSurface(fl, sw, sh, dp, rmask, gmask, bmask, amask);

    if(!surface) Error::Warning("Surface::CreateSurface: empty surface, error:" + std::string(SDL_GetError()));
}

/* load  palette for 8bit surface */
void Surface::LoadPalette(const SDL_Color *colors, u32 ncolor)
{
    if(!colors) Error::Warning("Surface::LoadPalette: empty palette.");

    else

    if(!valid()) Error::Warning("Surface::LoadPalette: invalid surface.");

    else

    SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, const_cast<SDL_Color *>(colors), 0, ncolor);
}

/* format surface */
void Surface::SetDisplayFormat(void)
{
    SDL_Surface *osurface = surface;

    surface = (osurface->flags & SDL_SRCALPHA ? SDL_DisplayFormatAlpha(osurface) : SDL_DisplayFormat(osurface));

    if(!surface) Error::Warning("Surface::SetDisplayFormat: empty surface, error:" + std::string(SDL_GetError()));

    SDL_FreeSurface(osurface);
}

/* set color key */
void Surface::SetColorKey(void)
{
    if(surface)
    {
	switch(surface->format->BitsPerPixel)
	{
	    case 16:
		Fill(DEFAULT_COLOR_KEY16);
		SetColorKey(DEFAULT_COLOR_KEY16);
		break;

	    case 32:
		Fill(DEFAULT_COLOR_KEY32);
		SetColorKey(DEFAULT_COLOR_KEY32);
		break;

	    default:
		break;
	}
    }
}

/* draw u32 pixel */
void Surface::SetPixel4(u16 x, u16 y, u32 color)
{
    if(x > surface->w || y > surface->h) return;
    
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * surface->pitch / 4 + x;

    *bufp = color;
}

/* draw u24 pixel */
void Surface::SetPixel3(u16 x, u16 y, u32 color)
{
    if(x > surface->w || y > surface->h) return;

    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3; 

    if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
    {
        bufp[0] = color;
        bufp[1] = color >> 8;
        bufp[2] = color >> 16;
    }
    else
    { 
	bufp[2] = color;
	bufp[1] = color >> 8;
        bufp[0] = color >> 16;
    }
}

/* draw u16 pixel */
void Surface::SetPixel2(u16 x, u16 y, u32 color)
{
    if(x > surface->w || y > surface->h) return;
    
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * surface->pitch / 2 + x;

    *bufp = static_cast<u16>(color);
}

/* draw u8 pixel */
void Surface::SetPixel1(u16 x, u16 y, u8 color)
{
    if(x > surface->w || y > surface->h) return;

    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;

    *bufp = color;
}

/* draw pixel */
void Surface::SetPixel(u16 x, u16 y, u32 color)
{
    switch(surface->format->BytesPerPixel)
    {
	case 1:	SetPixel1(x, y, color);	break;
	case 2:	SetPixel2(x, y, color);	break;
	case 3:	SetPixel3(x, y, color);	break;
	case 4:	SetPixel4(x, y, color);	break;
	default: break;
    }
}

/* fill colors surface */
void Surface::Fill(u32 color)
{
    Rect dstrect(0, 0, surface->w, surface->h);

    SDL_FillRect(surface, &dstrect, color);
}

/* rect fill colors surface */
void Surface::FillRect(u32 color, const Rect & src)
{
    SDL_FillRect(surface, const_cast<Rect *>(&src), color);
}

/* blit */
void Surface::Blit(const Surface &src)
{
    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), NULL, surface, NULL);
}

/* blit */
void Surface::Blit(const Surface &src, s16 dst_ox, s16 dst_oy)
{
    Rect dstrect(dst_ox, dst_oy, surface->w, surface->h);

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), NULL, surface, &dstrect);
}

/* blit */
void Surface::Blit(const Surface &src, const Rect &src_rt, s16 dst_ox, s16 dst_oy)
{
    Rect dstrect(dst_ox, dst_oy, src_rt.w, src_rt.h);

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), const_cast<Rect *>(&src_rt), surface, &dstrect);
}

/* scaled from surface */
void Surface::ScaleFrom(const Surface & bs)
{
    if(bs.w() <= w() || bs.h() <= h()) Error::Warning("Surface::ScaleFrom: incorrect size.");
    
    u16 *p_src = NULL;
    u16 *p_dst = NULL;

    u16 width = 0;
    u16 height = 0;

    u8 count = 0;

    Surface & src = const_cast<Surface &>(bs);
    
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
