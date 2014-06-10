/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <memory>
#include "surface.h"
#include "error.h"
#include "localevent.h"
#include "display.h"

#ifdef WITH_TTF
#include "SDL_ttf.h"
#endif

#ifdef WITH_IMAGE
#include "SDL_image.h"
#include "IMG_savepng.h"
#endif

namespace
{
    u32 default_depth = 16;
    RGBA default_color_key;
    SDL_Color* pal_colors = NULL;
    int pal_nums = 0;
}

void GetRGBAMask(u32 bpp, u32 & rmask, u32 & gmask, u32 & bmask, u32 & amask)
{
    switch(bpp)
    {
	case 32:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;

#else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif
	    break;

	case 24:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0x00fc0000;
            gmask = 0x0003f000;
            bmask = 0x00000fc0;
            amask = 0x0000003f;
#else
            rmask = 0x0000003f;
            gmask = 0x00000fc0;
            bmask = 0x0003f000;
            amask = 0x00fc0000;
#endif
	    break;

	case 16:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0x0000f000;
            gmask = 0x00000f00;
            bmask = 0x000000f0;
            amask = 0x0000000f;
#else
            rmask = 0x0000000f;
            gmask = 0x000000f0;
            bmask = 0x00000f00;
            amask = 0x0000f000;
#endif
	    break;

	default:
	    rmask = 0;
	    gmask = 0;
	    bmask = 0;
	    amask = 0;
	    break;
    }
}

u32 GetPixel24(u8* ptr)
{
    u32 color = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    color |= ptr[0];
    color <<= 8;
    color |= ptr[1];
    color <<= 8;
    color |= ptr[2];
#else
    color |= ptr[2];
    color <<= 8;
    color |= ptr[1];
    color <<= 8;
    color |= ptr[0];
#endif
    return color;                                                                                                
}                                                                                                                

void SetPixel24(u8* ptr, u32 color)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    ptr[2] = color;
    ptr[1] = color >> 8;
    ptr[0] = color >> 16;
#else
    ptr[0] = color;
    ptr[1] = color >> 8;
    ptr[2] = color >> 16;
#endif
}

RGBA::RGBA()
{
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.unused = 0;
}

RGBA::RGBA(int r, int g, int b, int a)
{
    color.r = r;
    color.g = g;
    color.b = b;
    color.unused = a;
}

int RGBA::GetColor(void) const
{
    int r = color.r;
    int g = color.g;
    int b = color.b;
    int a = color.unused;

    return (0xFF000000 & (r << 24)) | (0x00FF0000 & (g << 16)) | (0x0000FF00 & (b << 8)) | (0x000000FF & a);
}

int RGBA::r(void) const
{
    return color.r;
}

int RGBA::g(void) const
{
    return color.g;
}

int RGBA::b(void) const
{
    return color.b;
}

int RGBA::a(void) const
{
    return color.unused;
}

Surface::Surface() : surface(NULL)
{
}

Surface::Surface(u32 sw, u32 sh, bool amask) : surface(NULL)
{
    Set(sw, sh, amask);
}

Surface::Surface(const Surface & bs) : surface(NULL)
{
    Set(bs, true);
}

Surface::Surface(const std::string & file) : surface(NULL)
{
    Load(file);
}

Surface::~Surface()
{
    if(! isDisplay()) FreeSurface(*this);
}

bool Surface::isDisplay(void) const
{
    return false;
}

/* operator = */
Surface & Surface::operator= (const Surface & bs)
{
    Set(bs, true);
    return *this;
}

bool Surface::operator== (const Surface & bs) const
{
    return surface && bs.surface ? surface == bs.surface : false;
}

void Surface::SetDefaultPalette(SDL_Color* ptr, int num)
{
    pal_colors = ptr;
    pal_nums = num;
}

void Surface::SetDefaultColorKey(int r, int g, int b)
{
    default_color_key = RGBA(r, g, b);
}

void Surface::SetDefaultDepth(u32 depth)
{
    switch(depth)
    {
	case 8:
	case 16:
	case 24:
	case 32:
	    default_depth = depth;
	    break;

	default:
	    break;
    }
}

Size Surface::GetSize(void) const
{
    return Size(w(), h());
}

void Surface::Reset(void)
{
    FreeSurface(*this);
    surface = NULL; /* hard set: for ref copy */
}

void Surface::Set(SDL_Surface* sf)
{
    FreeSurface(*this);
    surface = sf;
}

void Surface::Set(const Surface & bs, bool refcopy)
{
    FreeSurface(*this);

    if(bs.isValid())
    {
	if(refcopy)
	{
	    surface = bs.surface;
	    if(surface) surface->refcount += 1;
	}
	else
	{
	    surface = SDL_ConvertSurface(bs.surface, bs.surface->format, bs.surface->flags);

	    if(!surface)
		Error::Except(__FUNCTION__, SDL_GetError());
	}
    }
}

void Surface::Set(u32 sw, u32 sh, bool amask)
{
    Set(sw, sh, default_depth, amask);
}

void Surface::Set(u32 sw, u32 sh, u32 bpp /* bpp: 8, 16, 24, 32 */, bool amask0)
{
    FreeSurface(*this);

    u32 rmask, gmask, bmask, amask;
    GetRGBAMask(bpp, rmask, gmask, bmask, amask);

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, sw, sh, bpp, rmask, gmask, bmask, (amask0 ? amask : 0));

    if(!surface)
	Error::Except(__FUNCTION__, SDL_GetError());

    if(8 == bpp) SetPalette();

    if(default_color_key.GetColor())
    {
	Fill(default_color_key);
	SetColorKey(default_color_key);
    }
}

void Surface::Set(const void* pixels, u32 width, u32 height, u32 bytes_per_pixel /* 1, 2, 3, 4 */, bool amask0)
{
    FreeSurface(*this);

    const u32 bpp = 8 * bytes_per_pixel;
    u32 rmask, gmask, bmask, amask;
    GetRGBAMask(bpp, rmask, gmask, bmask, amask);

    if(8 == bpp)
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp, rmask, gmask, bmask, (amask0 ? amask : 0));
    else
	surface = SDL_CreateRGBSurfaceFrom(const_cast<void *>(pixels), width, height, bpp, width * bytes_per_pixel,
		rmask, gmask, bmask, (amask0 ? amask : 0));

    if(!surface)
	Error::Except(__FUNCTION__, SDL_GetError());

    if(8 == bpp)
    {
	SetPalette();
	Lock();
	std::memcpy(surface->pixels, pixels, width * height);
	Unlock();
    }
}

bool Surface::isValid(void) const
{
    return surface && surface->format;
}

bool Surface::Load(const std::string & fn)
{
    FreeSurface(*this);

#ifdef WITH_IMAGE
    surface = IMG_Load(fn.c_str());
#else
    surface = SDL_LoadBMP(fn.c_str());
#endif

    if(!surface)
	Error::Message(__FUNCTION__, SDL_GetError());

    return surface;
}

bool Surface::Save(const std::string & fn) const
{
    int res = 0;

#ifdef WITH_IMAGE
    res = IMG_SavePNG(fn.c_str(), surface, -1);
#else
    res = SDL_SaveBMP(surface, fn.c_str());
#endif

    if(0 != res)
    {
	Error::Message(__FUNCTION__, SDL_GetError());
	return false;
    }

    return true;
}

bool Surface::isRefCopy(void) const
{
    return surface ? 1 < surface->refcount : false;
}

int Surface::w(void) const
{
    return surface ? surface->w : 0;
}

int Surface::h(void) const
{
    return surface ? surface->h : 0;
}

u32 Surface::depth(void) const
{
    return isValid() ? surface->format->BitsPerPixel : 0;
}

u32 Surface::amask(void) const
{
    return isValid() ? surface->format->Amask : 0;
}

u32 Surface::alpha(void) const
{
    return isValid() ? surface->format->alpha : 0;
}

u32 Surface::MapRGB(const RGBA & color) const
{
    return amask() ? SDL_MapRGBA(surface->format, color.r(), color.g(), color.b(), color.a()) : SDL_MapRGB(surface->format, color.r(), color.g(), color.b());
}

RGBA Surface::GetRGB(u32 pixel) const
{
    u8 r, g, b, a;

    if(amask())
    {
	SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
	return RGBA(r, g, b, a);
    }

    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return RGBA(r, g, b);
}

/* load static palette (economize 1kb for each surface) only 8bit color! */
void Surface::SetPalette(void)
{
    if(isValid() &&
	pal_colors && pal_nums && surface->format->palette)
    {
	if(surface->format->palette->colors &&
	    pal_colors != surface->format->palette->colors) SDL_free(surface->format->palette->colors);

        surface->format->palette->colors = pal_colors;
        surface->format->palette->ncolors = pal_nums;
    }
}

/* format surface */
void Surface::SetDisplayFormat(void)
{
    if(isValid())
	Set(amask() ? SDL_DisplayFormatAlpha(surface) : SDL_DisplayFormat(surface));
}

u32 Surface::GetColorKey(void) const
{
    return isValid() && (surface->flags & SDL_SRCCOLORKEY) ? surface->format->colorkey : 0;
}

void Surface::SetColorKey(const RGBA & color)
{
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, MapRGB(color));
}

/* draw u32 pixel */
void Surface::SetPixel4(s32 x, s32 y, u32 color)
{
    u32* bufp = static_cast<u32 *>(surface->pixels) + y * (surface->pitch >> 2) + x;
    *bufp = color;
}

/* draw u24 pixel */
void Surface::SetPixel3(s32 x, s32 y, u32 color)
{
    u8* bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3; 
    SetPixel24(bufp, color);
}

/* draw u16 pixel */
void Surface::SetPixel2(s32 x, s32 y, u32 color)
{
    u16* bufp = static_cast<u16 *>(surface->pixels) + y * (surface->pitch >> 1) + x;
    *bufp = static_cast<u16>(color);
}

/* draw u8 pixel */
void Surface::SetPixel1(s32 x, s32 y, u32 color)
{
    u8* bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;
    *bufp = static_cast<u8>(color);
}

/* draw pixel */
void Surface::SetPixel(int x, int y, const RGBA & color)
{
    SetPixel(x, y, MapRGB(color));
}

void Surface::SetPixel(int x, int y, u32 color)
{
    if(x < w() && y < h())
    {
	if(isRefCopy()) Set(*this, false);

	switch(depth())
	{
	    case 8:	SetPixel1(x, y, color);	break;
	    case 16:	SetPixel2(x, y, color);	break;
	    case 24:	SetPixel3(x, y, color);	break;
	    case 32:	SetPixel4(x, y, color);	break;
	    default: break;
	}
	if(isDisplay()) Display::Get().AddUpdateRect(x, y, 1, 1);
    }
    else
	Error::Except(__FUNCTION__, "out of range");
}

u32 Surface::GetPixel4(s32 x, s32 y) const
{
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * (surface->pitch >> 2) + x;
    return *bufp;
}

u32 Surface::GetPixel3(s32 x, s32 y) const
{
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3; 
    return GetPixel24(bufp);
}

u32 Surface::GetPixel2(s32 x, s32 y) const
{
    u16* bufp = static_cast<u16 *>(surface->pixels) + y * (surface->pitch >> 1) + x;
    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel1(s32 x, s32 y) const
{
    u8* bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;
    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel(int x, int y) const
{
    if(x < w() && y < h())
    {
	switch(depth())
	{
	    case 8:	return GetPixel1(x, y);
	    case 16:	return GetPixel2(x, y);
	    case 24:	return GetPixel3(x, y);
	    case 32:	return GetPixel4(x, y);
	    default: break;
	}
    }
    else
	Error::Except(__FUNCTION__, "out of range");
    
    return 0;
}

/* fill colors surface */
void Surface::Fill(const RGBA & col)
{
    FillRect(Rect(0, 0, w(), h()), col);
}

/* rect fill colors surface */
void Surface::FillRect(const Rect & rect, const RGBA & col)
{
    if(isRefCopy()) Set(*this, false);
    SDL_Rect dstrect = SDLRect(rect);
    SDL_FillRect(surface, &dstrect, MapRGB(col));
    if(isDisplay()) Display::Get().AddUpdateRect(rect.x, rect.y, rect.w, rect.h);
}

bool BlitCheckVariant(const Surface & sf, s32 x, s32 y, u32 variant)
{
    u32 pixel = sf.GetPixel(x, y);

    switch(variant)
    {
        // blit: skip alpha
        case 0: return (pixel & sf.amask()) == sf.amask();
        // blit: reset alpha
        case 1: return pixel != sf.GetColorKey();
        // blit: only alpha
        case 2: return (pixel & sf.amask()) != sf.amask();
        //
        default: break;
    }

    // skip colorkey
    return pixel != sf.GetColorKey();
}

void SetAmask(u8* ptr, u32 w, const SDL_PixelFormat* format, u32 alpha)
{
    while(w--)
    {
        switch(format->BitsPerPixel)
        {
            case 16:
                *reinterpret_cast<u16*>(ptr) |= ((static_cast<u16>(alpha) >> format->Aloss) << format->Ashift);
                break;

            case 24:
                SetPixel24(ptr, GetPixel24(ptr) | ((static_cast<u32>(alpha) >> format->Aloss) << format->Ashift));
                break;

            case 32:
                *reinterpret_cast<u32*>(ptr) |= ((static_cast<u32>(alpha) >> format->Aloss) << format->Ashift);
                break; 

            default:
                break;
        }
        ptr += format->BytesPerPixel;
    }
}

/* my alt. variant: for RGBA <-> RGB */
void Surface::BlitSurface(const Surface & sf1, SDL_Rect* srt, Surface & sf2, SDL_Rect* drt)
{
    if(sf2.isRefCopy()) sf2.Set(sf2, false);

    if(sf1.depth() == sf2.depth() &&
	// RGBA <-> RGB
	(((0 != sf1.amask() && 0 == sf2.amask()) ||
	 (0 == sf1.amask() && 0 != sf2.amask())) ||
	// depth 24, RGB <-> RGB
	 (sf1.depth() == 24 && 0 == sf1.amask() && 0 == sf2.amask())))
    {
        SDL_Rect rt1 = SDLRect(0, 0, sf1.w(), sf1.h());
        SDL_Rect rt2 = SDLRect(0, 0, sf2.w(), sf2.h());
        if(!srt) srt = &rt1;
        if(!drt) drt = &rt2;

        if(srt->w == 0) srt->w = sf1.w();
        if(srt->h == 0) srt->h = sf1.h();
        if(drt->w == 0) drt->w = sf2.w();
        if(drt->h == 0) drt->h = sf2.h();

        if(srt != &rt1)
	{
            Rect tmp = Rect::Get(Rect(*srt), Rect(rt1), true);

	    srt->x = tmp.x;
	    srt->y = tmp.y;
	    srt->w = tmp.w;
	    srt->h = tmp.h;
	}

        if(drt != &rt2)
	{
            Rect tmp = Rect::Get(Rect(*drt), Rect(rt2), true);

	    drt->x = tmp.x;
	    drt->y = tmp.y;
	    drt->w = tmp.w;
	    drt->h = tmp.h;
	}

        srt->w = std::min(srt->w, drt->w);
        srt->h = std::min(srt->h, drt->h);
        drt->w = srt->w;
        drt->h = srt->h;

	const SDL_Surface* ss = sf1.surface;
	SDL_Surface* ds = sf2.surface;

        s32 x, y;
	u32 variant = 0;

	// RGB -> RGB
	if(24 == sf1.depth() &&
	    0 == sf1.amask() && 0 == sf2.amask())
	    variant = 4;
	else
	// RGBA -> RGB
	if(0 != sf1.amask())
	    variant = ss->flags & SDL_SRCALPHA ? 0 : 1;
	else
	// RGB -> RGBA
	if(0 != sf2.amask())
	    variant = 1;

        sf2.Lock();

        for(y = 0; y < srt->h; ++y)
        {
            x = 0;

            while(x < srt->w)
            {
                if(! BlitCheckVariant(sf1, srt->x + x, srt->y + y, variant))
                    x++;
                else
                {
                    u32 w = 0;
                    while(x + w < srt->w &&
			BlitCheckVariant(sf1, srt->x + x + w, srt->y + y, variant)) ++w;

                    u8* sptr = reinterpret_cast<u8*>(ss->pixels) + (srt->y + y) * ss->pitch + (srt->x + x) * ss->format->BytesPerPixel;
                    u8* dptr = reinterpret_cast<u8*>(ds->pixels) + (drt->y + y) * ds->pitch + (drt->x + x) * ds->format->BytesPerPixel;

                    std::memcpy(dptr, sptr, w * ds->format->BytesPerPixel);

                    // RGB -> RGBA only
                    if(0 != sf2.amask()) SetAmask(dptr, w, ds->format, 0xFF);

                    x += w;
                }
            }
        }

        sf2.Unlock();
    }
    else
        SDL_BlitSurface(sf1.surface, srt, sf2.surface, drt);
}

/* blit */
void Surface::Blit(Surface & dst) const
{
    if(dst.isDisplay())
    {
	SDL_BlitSurface(surface, NULL, dst.surface, NULL);
	Display::Get().AddUpdateRect(0, 0, w(), h());
    }
    else
	BlitSurface(*this, NULL, dst, NULL);
}

/* blit */
void Surface::Blit(s32 dst_ox, s32 dst_oy, Surface & dst) const
{
    SDL_Rect dstrect = SDLRect(dst_ox, dst_oy, surface->w, surface->h);

    if(dst.isDisplay())
    {
	SDL_BlitSurface(surface, NULL, dst.surface, &dstrect);
	Display::Get().AddUpdateRect(dst_ox, dst_oy, surface->w, surface->h);
    }
    else
	BlitSurface(*this, NULL, dst, &dstrect);
}

/* blit */
void Surface::Blit(const Rect &src_rt, s32 dst_ox, s32 dst_oy, Surface & dst) const
{
    SDL_Rect dstrect = SDLRect(dst_ox, dst_oy, src_rt.w, src_rt.h);
    SDL_Rect srcrect = SDLRect(src_rt);

    if(dst.isDisplay())
    {
	SDL_BlitSurface(surface, &srcrect, dst.surface, &dstrect);
	Display::Get().AddUpdateRect(dst_ox, dst_oy, src_rt.w, src_rt.h);
    }
    else
	BlitSurface(*this, &srcrect, dst, &dstrect);
}

void Surface::Blit(const Point & dpt, Surface & dst) const
{
    Blit(dpt.x, dpt.y, dst);
}

void Surface::Blit(const Rect & srt, const Point & dpt, Surface & dst) const
{
    Blit(srt, dpt.x, dpt.y, dst);
}

void Surface::Blit(u32 alpha0, const Rect & srt, const Point & dpt, Surface & dst) const
{
    if(alpha0 == 255)
        Blit(srt, dpt, dst);
    else
    if(amask())
    {
        Surface tmp(w(), h(), false);
	Blit(tmp);
        tmp.SetAlpha(alpha0);
        tmp.Blit(srt, dpt, dst);
    }
    else
    {
        const u32 tmp = alpha();
        const_cast<Surface &>(*this).SetAlpha(alpha0);
        Blit(srt, dpt, dst);
        const_cast<Surface &>(*this).SetAlpha(tmp);
    }
}

void Surface::Blit(u32 alpha0, s32 dstx, s32 dsty, Surface & dst) const
{
    Blit(alpha0, Rect(0, 0, w(), h()), Point(dstx, dsty), dst);
}

void Surface::SetAlpha(u32 level)
{
    if(isValid())
    {
        if(isRefCopy()) Set(*this, false);
	SDL_SetAlpha(surface, SDL_SRCALPHA, level);
    }
}

void Surface::ResetAlpha(void) 
{
    if(isValid())
    {
        if(isRefCopy()) Set(*this, false);
	SDL_SetAlpha(surface, 0, 255);
    }
}

void Surface::Lock(void) const
{
    if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
}

void Surface::Unlock(void) const
{
    if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}

void Surface::FreeSurface(Surface & sf)
{
    if(sf.surface)
    {
	if(sf.isRefCopy())
	    --sf.surface->refcount;
	else
	{
    	    // clear static palette
    	    if(sf.surface->format && 8 == sf.surface->format->BitsPerPixel && pal_colors && pal_nums &&
        	sf.surface->format->palette && pal_colors == sf.surface->format->palette->colors)
    	    {
        	sf.surface->format->palette->colors = NULL;
        	sf.surface->format->palette->ncolors = 0;
    	    }

	    SDL_FreeSurface(sf.surface);
	    sf.surface = NULL;
	}
    }
}

void Surface::ChangeColor(const RGBA & col1, const RGBA & col2, Surface & sf)
{
    if(sf.isRefCopy()) sf.Set(sf, false);

    u32 fc = sf.MapRGB(col1);
    u32 tc = sf.MapRGB(col2);

    if(sf.amask())
    {
	fc |= sf.amask();
	tc |= sf.amask();
    }

    sf.Lock();
    if(fc != tc)
    for(int y = 0; y < sf.h(); ++y)
	for(int x = 0; x < sf.w(); ++x)
	    if(fc == sf.GetPixel(x, y)) sf.SetPixel(x, y, tc);
    sf.Unlock();
}

Surface Surface::GrayScale(const Surface & sf)
{
    Surface dst;
    dst.Set(sf, false);

    RGBA col;
    const u32 colkey = sf.GetColorKey();
    u32 pixel = 0;

    dst.Lock();
    for(int y = 0; y < sf.h(); ++y)
	for(int x = 0; x < sf.w(); ++x)
    {
	pixel = sf.GetPixel(x, y);
	if(pixel != colkey)
	{
	    col = sf.GetRGB(pixel);
	    int z = col.r() * 0.299f + col.g() * 0.587f + col.b() * 0.114f;
	    pixel = sf.MapRGB(RGBA(z, z, z, col.a()));
	    dst.SetPixel(x, y, pixel);
	}
    }
    dst.Unlock();
    return dst;
}

Surface Surface::Sepia(const Surface & sf)
{
    Surface dst;
    dst.Set(sf, false);

    RGBA col;
    u32 pixel = 0;

    dst.Lock();
    for(int x = 0; x < sf.w(); x++)
        for(int y = 0; y < sf.h(); y++)
        {
            pixel = sf.GetPixel(x, y);
            col = sf.GetRGB(pixel);
    
            //Numbers derived from http://blogs.techrepublic.com.com/howdoi/?p=120
            #define CLAMP255(val) std::min<u16>((val), 255)
            int outR = CLAMP255(static_cast<u16>(col.r() * 0.693f + col.g() * 0.769f + col.b() * 0.189f));
            int outG = CLAMP255(static_cast<u16>(col.r() * 0.449f + col.g() * 0.686f + col.b() * 0.168f));
            int outB = CLAMP255(static_cast<u16>(col.r() * 0.272f + col.g() * 0.534f + col.b() * 0.131f));
            pixel = sf.MapRGB(RGBA(outR, outG, outB, col.a()));
            dst.SetPixel(x, y, pixel);
            #undef CLAMP255
        }
    dst.Unlock();
    return dst;
}

void Surface::DrawLine(const Point & p1, const Point & p2, const RGBA & color, Surface & sf)
{
    if(sf.isRefCopy()) sf.Set(sf, false);

    int x1 = p1.x; int y1 = p1.y;
    int x2 = p2.x; int y2 = p2.y;

    const u32 col = sf.MapRGB(color);
    const int dx = std::abs(x2 - x1);
    const int dy = std::abs(y2 - y1);

    sf.Lock();
    if(dx > dy)
    {
	int ns = std::div(dx, 2).quot;

	for(int i = 0; i <= dx; ++i)
	{
	    sf.SetPixel(x1, y1, col);
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
	int ns = std::div(dy, 2).quot;

	for(int i = 0; i <= dy; ++i)
	{
	    sf.SetPixel(x1, y1, col);
	    y1 < y2 ? ++y1 : --y1;
	    ns -= dx;
	    if(ns < 0)
	    {
		x1 < x2 ? ++x1 : --x1;
		ns += dy;
	    }
	}
    }
    sf.Unlock();
}

Surface Surface::Stencil(const Surface & src, const RGBA & color)
{
    Surface dst;
    dst.Set(src.w(), src.h(), src.depth(), false); // same depth for src and dst
    const u32 clkey = src.GetColorKey();
    const u32 col2 = dst.MapRGB(color);

    src.Lock();
    dst.Lock();

    for(int y = 0; y < src.h(); ++y)
    {
        for(int x = 0; x < src.w(); ++x)
        {
            u32 pixel = src.GetPixel(x, y);
            if(clkey != pixel)
	    {
		if(src.alpha())
		{
		    RGBA col = src.GetRGB(pixel);
		    // skip shadow
		    if(col.a() < 200) continue;
		}

                dst.SetPixel(x, y, col2);
            }
        }
    }

    dst.Unlock();
    src.Unlock();

    return dst;
}

Surface Surface::Contour(const Surface & src, const RGBA & color)
{
    const RGBA fake = RGBA(0x00, 0xFF, 0xFF);
    Surface dst;
    dst.Set(src.w() + 2, src.h() + 2, src.depth(), false); // same depth for src and dst
    Surface trf = Stencil(src, fake);
    const u32 clkey = trf.GetColorKey();
    const u32 fake2 = trf.MapRGB(fake);
    const u32 col = dst.MapRGB(color);

    trf.Lock();
    dst.Lock();

    for(int y = 0; y < trf.h(); ++y)
    {
        for(int x = 0; x < trf.w(); ++x)
        {
            if(fake2 == trf.GetPixel(x, y))
            {
                if(0 == x) dst.SetPixel(x, y, col);
                else if(trf.w() - 1 == x) dst.SetPixel(x + 1, y, col);
                else if(0 == y) dst.SetPixel(x, y, col);
                else if(trf.h() - 1 == y) dst.SetPixel(x, y + 1, col);
                else {
                    if(0 < x && clkey == trf.GetPixel(x - 1, y)) dst.SetPixel(x - 1, y, col);
                    if(trf.w() - 1 > x && clkey == trf.GetPixel(x + 1, y)) dst.SetPixel(x + 1, y, col);

                    if(0 < y && clkey == trf.GetPixel(x, y - 1)) dst.SetPixel(x, y - 1, col);
                    if(trf.h() - 1 > y && clkey == trf.GetPixel(x, y + 1)) dst.SetPixel(x, y + 1, col);
                }
            }
        }
    }

    trf.Unlock();
    dst.Unlock();

    return dst;
}

Surface Surface::Reflect(const Surface & sf_src, u32 shape)
{
    Surface sf_dst;

    if(sf_src.isValid())
    {
	sf_dst.Set(sf_src, false);

	sf_src.Lock();
	sf_dst.Lock();

	switch(shape % 4)
	{
    	    // normal
	    default:
		// skip double copy
        	break;
    
	    // vertical reflect
    	    case 1:
        	for(int yy = 0; yy < sf_src.h(); ++yy)
            	    for(int xx = 0; xx < sf_src.w(); ++xx)
			sf_dst.SetPixel(xx, sf_src.h() - yy - 1, sf_src.GetPixel(xx, yy));
        	break;

    	    // horizontal reflect
    	    case 2:
        	for(int yy = 0; yy < sf_src.h(); ++yy)
            	    for(int xx = 0; xx < sf_src.w(); ++xx)
			sf_dst.SetPixel(sf_src.w() - xx - 1, yy, sf_src.GetPixel(xx, yy));
		break;

    	    // both variants
	    case 3:
        	for(int yy = 0; yy < sf_src.h(); ++yy)
            	    for(int xx = 0; xx < sf_src.w(); ++xx)
			sf_dst.SetPixel(sf_src.w() - xx - 1, sf_src.h() - yy - 1, sf_src.GetPixel(xx, yy));
    		break;
	}

	sf_src.Unlock();
	sf_dst.Unlock();
    }
    else
	Error::Message(__FUNCTION__, "incorrect param");

    return sf_dst;
}

Surface Surface::Rotate(const Surface & sf_src, u32 parm)
{
    Surface sf_dst;

    if(sf_src.isValid())
    {
	// 90 CW or 90 CCW
	if(parm == 1 || parm == 2)
	{
	    sf_dst.Set(sf_src.h(), sf_src.w());

	    sf_src.Lock();
	    sf_dst.Lock();

    	    for(int yy = 0; yy < sf_src.h(); ++yy)
        	for(int xx = 0; xx < sf_src.w(); ++xx)
		    if(parm == 1)
			sf_dst.SetPixel(yy, sf_src.w() - xx - 1, sf_src.GetPixel(xx, yy));
		    else
			sf_dst.SetPixel(sf_src.h() - yy - 1, xx, sf_src.GetPixel(xx, yy));

	    sf_src.Unlock();
	    sf_dst.Unlock();
	}
	else
	if(parm == 3)
	    sf_dst = Reflect(sf_src, 3);
    }
    else
	std::cerr << __FUNCTION__ << "incorrect param" << std::endl;

    return sf_dst;
}

u32 Surface::GetMemoryUsage(void) const
{
    u32 res = sizeof(surface);

    if(surface)
    {
	res += sizeof(SDL_Surface) + sizeof(SDL_PixelFormat) + surface->pitch * surface->h;

	if(surface->format && surface->format->palette &&
	    (! pal_colors || pal_colors != surface->format->palette->colors))
	    res += sizeof(SDL_Palette) + surface->format->palette->ncolors * sizeof(SDL_Color);
    }

    return res;
}

std::string Surface::Info(void) const
{
    std::ostringstream os;

    if(isValid())
    {
	os << 
	    "flags" << "(" << surface->flags << ", " << (surface->flags & SDL_SRCALPHA ? "SRCALPHA" : "") << (surface->flags & SDL_SRCCOLORKEY ? "SRCCOLORKEY" : "") << "), " <<
	    "w"<< "(" << surface->w << "), " <<
	    "h"<< "(" << surface->h << "), " <<
	    "size" << "(" << GetMemoryUsage() << "), " <<
	    "bpp" << "(" << depth() << "), " <<
	    "Amask" << "(" << "0x" << std::setw(8) << std::setfill('0') << std::hex << surface->format->Amask << "), " <<
	    "colorkey" << "(" << "0x" << std::setw(8) << std::setfill('0') << surface->format->colorkey << "), " << std::dec <<
	    "alpha" << "(" << alpha() << "), ";
    }
    else
	os << "invalid surface";

    return os.str();
}

u32 AVERAGE(SDL_PixelFormat* fm, u32 c1, u32 c2)
{
    if(c1 == c2) return c1;
    if(c1 == SDL_MapRGBA(fm, 0xFF, 0x00, 0xFF, 0)) c1 = 0;
    if(c2 == SDL_MapRGBA(fm, 0xFF, 0x00, 0xFF, 0)) c2 = 0;

#define avr(a, b) ((a + b) >> 1)
    u8 r1, g1, b1, a1;
    SDL_GetRGBA(c1, fm, &r1, &g1, &b1, &a1);
    u8 r2, g2, b2, a2;
    SDL_GetRGBA(c2, fm, &r2, &g2, &b2, &a2);
    return SDL_MapRGBA(fm, avr(r1, r2), avr(g1, g2), avr(b1, b2), avr(a1, a2));
}

/* scale surface */
Surface Surface::Scale(const Surface & src, s32 width, s32 height)
{
    Surface res;
    res.Set(width, height, src.depth(), src.amask());

    if(width >= 2 && height >= 2)
    {
	float stretch_factor_x = width / static_cast<float>(src.w());
	float stretch_factor_y = height / static_cast<float>(src.h());

	for(s32 yy = 0; yy < src.h(); yy++)
    	    for(s32 xx = 0; xx < src.w(); xx++)
        	for(s32 oy = 0; oy < stretch_factor_y; ++oy)
            	    for(s32 ox = 0; ox < stretch_factor_x; ++ox)
            	    {
                	res.SetPixel(static_cast<s32>(stretch_factor_x * xx) + ox,
                    	    static_cast<s32>(stretch_factor_y * yy) + oy, src.GetPixel(xx, yy));
            	    }
    }

    return res;
}

void Surface::Swap(Surface & sf1, Surface & sf2)
{
    std::swap(sf1.surface, sf2.surface);
}

Surface Surface::RectBorder(const Size & sz, const RGBA & color, bool solid)
{
    return RectBorder(sz, default_color_key, color, solid);
}

Surface Surface::RectBorder(const Size & sz, const RGBA & fill, const RGBA & col, bool solid)
{
    Surface sf(sz.w, sz.h);
    const u32 color = sf.MapRGB(col);

    sf.Fill(fill);
    sf.Lock();

    if(solid)
    {
	for(u32 i = 0; i < sz.w; ++i)
        {
    	    sf.SetPixel(i, 0, color);
            sf.SetPixel(i, sz.h - 1, color);
        }

        for(u32 i = 0; i < sz.h; ++i)
        {
            sf.SetPixel(0, i, color);
    	    sf.SetPixel(sz.w - 1, i, color);
        }
    }
    else
    {
	for(u32 i = 0; i < sz.w; ++i)
	{
    	    sf.SetPixel(i, 0, color);
    	    if(i + 1 < sz.w) sf.SetPixel(i + 1, 0, color);
    	    i += 3;
	}
	for(u32 i = 0; i < sz.w; ++i)
	{
    	    sf.SetPixel(i, sz.h - 1, color);
    	    if(i + 1 < sz.w) sf.SetPixel(i + 1, sz.h - 1, color);
    	    i += 3;
	}
	for(u32 i = 0; i < sz.h; ++i)
	{
    	    sf.SetPixel(0, i, color);
    	    if(i + 1 < sz.h) sf.SetPixel(0, i + 1, color);
    	    i += 3;
	}
	for(u32 i = 0; i < sz.h; ++i)
	{
    	    sf.SetPixel(sz.w - 1, i, color);
    	    if(i + 1 < sz.h) sf.SetPixel(sz.w - 1, i + 1, color);
    	    i += 3;
	}
    }

    sf.Unlock();
    return sf;
}

#ifdef WITH_TTF
Surface Surface::RenderText(const SDL::Font & font, const std::string & msg, const RGBA & clr, bool solid)
{
    Surface res;

    if(font())
    {
	res.Set(solid ? TTF_RenderUTF8_Solid(font(), msg.c_str(), clr()) :
			TTF_RenderUTF8_Blended(font(), msg.c_str(), clr()));
    }

    return res;
}

Surface Surface::RenderChar(const SDL::Font & font, char ch, const RGBA & clr, bool solid)
{
    Surface res;
    char buf[2] = { '\0', '\0' };
         buf[0] = ch;

    if(font())
    {
	res.Set(solid ? TTF_RenderUTF8_Solid(font(), buf, clr()) :
			TTF_RenderUTF8_Blended(font(), buf, clr()));
    }

    return res;
}

Surface Surface::RenderUnicodeText(const SDL::Font & font, const std::vector<u16> & msg, const RGBA & clr, bool solid)
{
    Surface res;

    if(font())
    {
	res.Set(solid ? TTF_RenderUNICODE_Solid(font(), &msg[0], clr()) :
			TTF_RenderUNICODE_Blended(font(), &msg[0], clr()));
    }

    return res;
}

Surface Surface::RenderUnicodeChar(const SDL::Font & font, u16 ch, const RGBA & clr, bool solid)
{
    Surface res;
    u16 buf[2] = { L'\0', L'\0' };
        buf[0] = ch;

    if(font())
    {
	res.Set(solid ? TTF_RenderUNICODE_Solid(font(), buf, clr()) :
			TTF_RenderUNICODE_Blended(font(), buf, clr()));
    }

    return res;
}
#endif
