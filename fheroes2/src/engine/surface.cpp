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

#include <algorithm>
#include "surface.h"
#include "palette.h"
#include "error.h"

#ifdef WITH_TTF
#include "SDL_ttf.h"
#endif

#include "IMG_savepng.h"

Surface::Surface() : surface(NULL), videosurface(false)
{
}

Surface::Surface(const unsigned char * pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool alpha) : videosurface(false)
{
    surface = SDL_CreateRGBSurfaceFrom((void *) pixels, width, height, bytes_per_pixel * 8, width * bytes_per_pixel,
		RMASK32, GMASK32, BMASK32, alpha ? AMASK32 : 0);
}

Surface::Surface(u16 sw, u16 sh, u8 depth, u32 fl) : videosurface(false)
{
    CreateSurface(sw, sh, depth,  fl);
    LoadPalette();
}

Surface::Surface(u16 sw, u16 sh, bool alpha) : videosurface(false)
{
    CreateSurface(sw, sh, DEFAULT_DEPTH, alpha ? SDL_SRCALPHA|SDL_SWSURFACE : SDL_SWSURFACE);
    LoadPalette();
}

Surface::Surface(const Surface & bs) : videosurface(false)
{
    surface = bs.valid() ?
	SDL_ConvertSurface(const_cast<SDL_Surface *>(bs.GetSurface()), const_cast<SDL_PixelFormat *>(bs.GetPixelFormat()), bs.flags()) : NULL;
    LoadPalette();
}

Surface::Surface(SDL_Surface * sf) : videosurface(false)
{
    surface = sf ? sf : NULL;
    LoadPalette();
}

Surface::~Surface()
{
    if(surface && !videosurface) SDL_FreeSurface(surface);

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

    LoadPalette();

    return *this;
}

void Surface::Set(u16 sw, u16 sh, bool alpha)
{
    if(surface) SDL_FreeSurface(surface);

    CreateSurface(sw, sh, DEFAULT_DEPTH, alpha ? SDL_SRCALPHA|SDL_SWSURFACE : SDL_SWSURFACE);
    LoadPalette();
}

void Surface::Set(u16 sw, u16 sh, u8 depth, u32 fl)
{
    if(surface) SDL_FreeSurface(surface);

    CreateSurface(sw, sh, depth,  fl);
    LoadPalette();
}

u16 Surface::w(void) const
{
    return surface ? surface->w : 0;
}

u16 Surface::h(void) const
{
    return surface ? surface->h : 0;
}

u8 Surface::depth(void) const
{
    return surface ? surface->format->BitsPerPixel : 0;
}

bool Surface::alpha(void) const
{
    return SDL_SRCALPHA & surface->flags;
}

u32 Surface::flags(void) const
{
    return surface->flags;
}

u32 Surface::MapRGB(u8 r, u8 g, u8 b, u8 a) const
{
    return (SDL_SRCALPHA & surface->flags) ? SDL_MapRGBA(surface->format, r, g, b, a) : SDL_MapRGB(surface->format, r, g, b);
}

void Surface::GetRGB(u32 pixel, u8 *r, u8 *g, u8 *b, u8 *a) const
{
    return (SDL_SRCALPHA & surface->flags && a) ? SDL_GetRGBA(pixel, surface->format, r, g, b, a) : SDL_GetRGB(pixel, surface->format, r, g, b);
}

/* create new surface */
void Surface::CreateSurface(u16 sw, u16 sh, u8 dp, u32 fl)
{
    u32 rmask = 0;
    u32 gmask = 0;
    u32 bmask = 0;
    u32 amask = 0;

    switch(dp)
    {
	case 32:
	    rmask = RMASK32;
	    gmask = GMASK32;
	    bmask = BMASK32;
	    amask = SDL_SRCALPHA & fl ? AMASK32 : 0;
	    break;
	case 24:
	    rmask = RMASK24;
	    gmask = GMASK24;
	    bmask = BMASK24;
	    amask = SDL_SRCALPHA & fl ? AMASK24 : 0;
	    break;
	case 16:
	    rmask = RMASK16;
	    gmask = GMASK16;
	    bmask = BMASK16;
	    amask = SDL_SRCALPHA & fl ? AMASK16 : 0;
	    break;
	default: break;
    }

    surface = SDL_CreateRGBSurface(fl, sw, sh, dp, rmask, gmask, bmask, amask);

    if(!surface) Error::Warning("Surface::CreateSurface: empty surface, error:" + std::string(SDL_GetError()));
}

void Surface::LoadPalette(void)
{
    if(surface && 8 == surface->format->BitsPerPixel)
    {
	const SDL_Palette *pal = Palette::Get().SDLPalette();

        if(pal) SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, pal->colors, 0, pal->ncolors);
    }
}

/* format surface */
void Surface::SetDisplayFormat(void)
{
    SDL_Surface *osurface = surface;
    surface = (osurface->flags & SDL_SRCALPHA ? SDL_DisplayFormatAlpha(osurface) : SDL_DisplayFormat(osurface));
    if(osurface) SDL_FreeSurface(osurface);
}

u32 Surface::GetColor(u16 index) const
{
    if(! surface) return 0;

    return 8 == surface->format->BitsPerPixel ? index : Palette::Get().GetColor(index);
}

u32 Surface::GetColorKey(void) const
{
    if(! surface) return 0;

//    return surface->flags & SDL_SRCALPHA ? SDL_MapRGBA(surface->format, 0xFF, 0x00, 0xFF, 0) :
//			    SDL_MapRGB(surface->format, 0xFF, 0x00, 0xFF);
    return SDL_MapRGBA(surface->format, 0xFF, 0x00, 0xFF, 0);
}

/* set color key */
void Surface::SetColorKey(void)
{
    if(surface)
    {
	const u32 clkey = GetColorKey();
	Fill(clkey);
	SetColorKey(clkey);
    }
}

void Surface::SetColorKey(u8 r, u8 g, u8 b)
{
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, MapRGB(r, g, b));
}

void Surface::SetColorKey(u32 color)
{
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, color);
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
void Surface::SetPixel1(u16 x, u16 y, u32 color)
{
    if(x > surface->w || y > surface->h) return;

    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;

    *bufp = static_cast<u8>(color);
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

u32 Surface::GetPixel4(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * surface->pitch / 4 + x;

    return *bufp;
}

u32 Surface::GetPixel3(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;

    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3; 

    u32 color = 0;

    if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
    {
        color |= bufp[2];
        color <<= 8;
        color |= bufp[1];
        color <<= 8;
        color |= bufp[0];
    }
    else
    { 
        color |= bufp[0];
        color <<= 8;
        color |= bufp[1];
        color <<= 8;
        color |= bufp[2];
    }
    
    return color;
}

u32 Surface::GetPixel2(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * surface->pitch / 2 + x;

    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel1(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;

    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel(u16 x, u16 y) const
{
    switch(surface->format->BytesPerPixel)
    {
	case 1:	return GetPixel1(x, y);
	case 2:	return GetPixel2(x, y);
	case 3:	return GetPixel3(x, y);
	case 4:	return GetPixel4(x, y);
	default: break;
    }
    
    return 0;
}

/* fill colors surface */
void Surface::Fill(u32 color)
{
    Rect dstrect(0, 0, surface->w, surface->h);

    SDL_FillRect(surface, dstrect.SDLRect(), color);
}

/* rect fill colors surface */
void Surface::FillRect(u32 color, const Rect & src)
{
    SDL_FillRect(surface, const_cast<Rect &>(src).SDLRect(), color);
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

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), NULL, surface, dstrect.SDLRect());
}

/* blit */
void Surface::Blit(const Surface &src, const Rect &src_rt, s16 dst_ox, s16 dst_oy)
{
    Rect dstrect(dst_ox, dst_oy, src_rt.w, src_rt.h);

    SDL_BlitSurface(const_cast<SDL_Surface *>(src.GetSurface()), const_cast<Rect &>(src_rt).SDLRect(), surface, dstrect.SDLRect());
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

bool Surface::SaveBMP(const char *fn) const
{
    return !surface || SDL_SaveBMP(surface, fn) ? false : true;
}

bool Surface::SavePNG(const char *fn) const
{
#ifdef WITH_PNG
    return !surface || IMG_SavePNG(fn, surface, -1) ? false : true;
#else
    return false;
#endif
}

const void *Surface::pixels(void) const
{
    return surface ? surface->pixels : NULL;
}

void Surface::SetAlpha(u8 level)
{
    if(!surface) return;
    SDL_SetAlpha(surface, SDL_SRCALPHA, level);
}

void Surface::ResetAlpha(void)
{
    if(!surface) return;
    SDL_SetAlpha(surface, 0, 0);
    surface->format->Amask = 0;
}

void Surface::Lock(void)
{
    if(SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);
}

void Surface::Unlock(void)
{
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
}

void Surface::FreeSurface(void)
{
    if(surface)
        SDL_FreeSurface(surface);
    surface = NULL;
}

const SDL_PixelFormat *Surface::GetPixelFormat(void) const
{
    return surface->format;
}

void Surface::ChangeColor(u32 fc, u32 tc)
{
    if(!surface) return;

    Lock();
    if(fc != tc)
    for(u16 y = 0; y < surface->h; ++y)
	for(u16 x = 0; x < surface->w; ++x)
	    if(fc == GetPixel(x, y)) SetPixel(x, y, tc);
    Unlock();
}

void Surface::GrayScale(void)
{
    if(!surface) return;

    u8 r, g, b, z;
    
    Lock();
    for(u16 y = 0; y < surface->h; ++y)
	for(u16 x = 0; x < surface->w; ++x)
    {
	GetRGB(GetPixel(x, y), &r, &g, &b);
	z = static_cast<u8>(0.299 * r + 0.587 * g + 0.114 * b);
	r = z;
	g = z;
	b = z;
	SetPixel(x, y, MapRGB(r, g, b));
    }
    Unlock();
}

void Surface::Sepia(void)
{
    if(!surface) return;

    Lock();
    for(u16 x = 0; x < surface->w; x++)
        for(u16 y = 0; y < surface->h; y++)
        {
            u32 pixel = GetPixel(x, y);
            u8 r, g, b;
            GetRGB(pixel, &r, &g, &b);
    
            //Numbers derived from http://blogs.techrepublic.com.com/howdoi/?p=120
            #define CLAMP255(val) static_cast<u8>(std::min<u16>((val), 255))
            u8 outR = CLAMP255(static_cast<u16>(r * 0.693f + g * 0.769f + b * 0.189f));
            u8 outG = CLAMP255(static_cast<u16>(r * 0.449f + g * 0.686f + b * 0.168f));
            u8 outB = CLAMP255(static_cast<u16>(r * 0.272f + g * 0.534f + b * 0.131f));
            pixel = MapRGB(outR, outG, outB);
            SetPixel(x, y, pixel);
            #undef CLAMP255
        }
    Unlock();
}

void Surface::DrawLine(const Point & p1, const Point & p2, u32 c)
{
    DrawLine(p1.x, p1.y, p2.x, p2.y, c);
}

void Surface::DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u32 c)
{
    const u16 dx = std::abs(x2 - x1);
    const u16 dy = std::abs(y2 - y1);

    Lock();
    if(dx > dy)
    {
	s16 ns = std::div(dx, 2).quot;

	for(u16 i = 0; i <= dx; ++i)
	{
	    SetPixel(x1, y1, c);
	    x1 < x2 ? ++x1 : --x1;
	    ns -= dy;
	    if(ns < 0)
	    {
		y1 < y2 ? ++y1 : --y1;
		ns += dx;
	    }
	}
    }
    else
    {
	s16 ns = std::div(dy, 2).quot;

	for(u16 i = 0; i <= dy; ++i)
	{
	    SetPixel(x1, y1, c);
	    y1 < y2 ? ++y1 : --y1;
	    ns -= dx;
	    if(ns < 0)
	    {
		x1 < x2 ? ++x1 : --x1;
		ns += dy;
	    }
	}
    }
    Unlock();
}

void Surface::MakeStencil(Surface & dst, u32 col) const
{
    if(!surface) return;

    dst.Set(surface->w, surface->h);
    dst.SetColorKey();
    const u32 clkey = GetColorKey();

    dst.Lock();
    for(u16 y = 0; y < surface->h; ++y)
	for(u16 x = 0; x < surface->w; ++x)
    {
	if(clkey != GetPixel(x, y))
	{
	    dst.SetPixel(x, y, col);
	}
    }
    dst.Unlock();
}

void Surface::MakeContour(Surface & dst, u32 col) const
{
    dst.Set(surface->w + 2, surface->h + 2);
    dst.SetColorKey();

    Surface trf;
    u32 fake = MapRGB(0x00, 0xFF, 0xFF);

    MakeStencil(trf, fake);
    const u32 clkey = trf.GetColorKey();
    dst.Lock();
    for(u16 y = 0; y < trf.h(); ++y)
	for(u16 x = 0; x < trf.w(); ++x)
    {
	if(fake == trf.GetPixel(x, y))
	{
	    if(0 == x) dst.SetPixel(x, y, col);
	    else
	    if(trf.w() - 1 == x) dst.SetPixel(x + 2, y, col);
	    else
	    if(0 == y) dst.SetPixel(x, y, col);
	    else
	    if(trf.h() - 1 == y) dst.SetPixel(x, y + 2, col);
	    else
	    {
		if(0 < x && clkey == trf.GetPixel(x - 1, y)) dst.SetPixel(x, y + 1, col);
		if(trf.w() - 1 > x && clkey == trf.GetPixel(x + 1, y)) dst.SetPixel(x + 2, y + 1, col);

		if(0 < y && clkey == trf.GetPixel(x, y - 1)) dst.SetPixel(x + 1, y, col);
		if(trf.h() - 1 > y && clkey == trf.GetPixel(x, y + 1)) dst.SetPixel(x + 1, y + 2, col);
	    }
	}
    }
    dst.Unlock();
}
