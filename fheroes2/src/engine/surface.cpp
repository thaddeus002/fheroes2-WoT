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
#include "palette.h"
#include "error.h"
#include "localevent.h"
#include "palette_h2.h"
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
    u8 default_depth = 16;
    std::vector<SDL_Color> pal_colors;
}

void LoadPalColors(void)
{
    u16 ncolors = ARRAY_COUNT(kb_pal) / 3;
    pal_colors.reserve(ncolors);

    for(u16 ii = 0; ii < ncolors; ++ii)
    {
	u16 index = ii * 3;
	SDL_Color cols;

	cols.r = kb_pal[index] << 2;
	cols.g = kb_pal[index + 1] << 2;
	cols.b = kb_pal[index + 2] << 2;

	pal_colors.push_back(cols);
    }
}

void GetRGBAMask(u8 bpp, u32 & rmask, u32 & gmask, u32 & bmask, u32 & amask)
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
#if SDL_VERSION_ATLEAST(1, 3, 0)
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	    rmask = 0x00ff0000;
	    gmask = 0x0000ff00;
	    bmask = 0x000000ff;
	    amask = 0x00000000;
#else
	    rmask = 0x000000ff;
	    gmask = 0x0000ff00;
	    bmask = 0x00ff0000;
	    amask = 0x00000000;
#endif
#else
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
#endif
	    break;

	case 16:
#if SDL_VERSION_ATLEAST(1, 3, 0)
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0x00007c00;
            gmask = 0x000003e0;
            bmask = 0x0000001f;
            amask = 0x00000000;
#else
            rmask = 0x0000001f;
            gmask = 0x000003e0;
            bmask = 0x00007c00;
            amask = 0x00000000;
#endif
#else
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

void SDLFreeSurface(SDL_Surface *sf)
{
    if(sf)
    {
	// clear static palette
	if(sf->format && 8 == sf->format->BitsPerPixel && pal_colors.size() &&
	    sf->format->palette && &pal_colors[0] == sf->format->palette->colors)
        {
    	    sf->format->palette->colors = NULL;
            sf->format->palette->ncolors = 0;
        }
	SDL_FreeSurface(sf);
    }
}

Surface::Surface() : surface(NULL)
{
}

Surface::Surface(const void* pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool amask) : surface(NULL)
{
    Set(pixels, width, height, bytes_per_pixel, amask);
}

Surface::Surface(u16 sw, u16 sh, bool amask) : surface(NULL)
{
    Set(sw, sh, amask);
}

Surface::Surface(const Surface & bs) : surface(NULL)
{
    Set(bs);
}

Surface::Surface(SDL_Surface* sf) : surface(NULL)
{
    Set(sf);
}

Surface::~Surface()
{
    if(! isDisplay())
	FreeSurface(*this);
}

/* operator = */
Surface & Surface::operator= (const Surface & bs)
{
    Set(bs);

    return *this;
}

void Surface::SetDefaultDepth(u8 depth)
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

u8 Surface::GetDefaultDepth(void)
{
    return default_depth;
}

void Surface::Set(SDL_Surface* sf)
{
    FreeSurface(*this);
    surface = sf ? sf : NULL;
}

void Surface::Set(const Surface & bs)
{
    FreeSurface(*this);

    if(bs.isValid())
    {
	surface = SDL_ConvertSurface(bs.surface, bs.surface->format, bs.surface->flags);

	if(!surface)
	    Error::Except(__FUNCTION__, SDL_GetError());
    }
}

void Surface::Set(u16 sw, u16 sh, bool amask)
{
    Set(sw, sh, default_depth, amask);
}

/* bpp: 8, 16, 24, 32 */
void Surface::Set(u16 sw, u16 sh, u8 bpp, bool amask0)
{
    FreeSurface(*this);
    CreateSurface(sw, sh, bpp, amask0);

    if(8 == bpp) LoadPalette();
    SetDefaultColorKey();
}

void Surface::Set(const void* pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool amask0)
{
    FreeSurface(*this);

    switch(bytes_per_pixel)
    {
	case 1:
	    CreateSurface(width, height, 8, false);
	    LoadPalette();
	    Lock();
	    std::memcpy(surface->pixels, pixels, width * height);
	    Unlock();
	    break;

	default:
	{
	    u32 rmask, gmask, bmask, amask;
	    GetRGBAMask(bytes_per_pixel * 8, rmask, gmask, bmask, amask);
	    surface = SDL_CreateRGBSurfaceFrom(const_cast<void *>(pixels), width, height, 8 * bytes_per_pixel, width * bytes_per_pixel,
		rmask, gmask, bmask, (amask0 ? amask : 0));
	}
	break;
    }

    if(!surface)
	Error::Except(__FUNCTION__, SDL_GetError());
}

/* create new surface */
void Surface::CreateSurface(u16 sw, u16 sh, u8 bpp, bool amask0)
{
    u32 rmask, gmask, bmask, amask;
    GetRGBAMask(bpp, rmask, gmask, bmask, amask);
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, sw, sh, bpp, rmask, gmask, bmask, (amask0 ? amask : 0));

    if(!surface)
	Error::Except(__FUNCTION__, SDL_GetError());
}

bool Surface::isValid(void) const
{
    return surface;
}

bool Surface::isDisplay(void) const
{
    return surface && Display::Get().surface == surface;
}

bool Surface::Load(const char* fn)
{
    FreeSurface(*this);

#ifdef WITH_IMAGE
    if(fn) surface = IMG_Load(fn);
#else
    if(fn) surface = SDL_LoadBMP(fn);
#endif
    return surface;
}

bool Surface::Load(const std::string & str)
{
    return Load(str.c_str());
}

bool Surface::Save(const char *fn) const
{
#ifdef WITH_IMAGE
    return !surface || !fn || IMG_SavePNG(fn, surface, -1) ? false : true;
#else
    return !surface || !fn || SDL_SaveBMP(surface, fn) ? false : true;
#endif
}

bool Surface::Save(const std::string & str) const
{
    return Save(str.c_str());
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
    return isValid() ? surface->format->BitsPerPixel : 0;
}

u32 Surface::amask(void) const
{
    return isValid() ? surface->format->Amask : 0;
}

u8 Surface::alpha(void) const
{
    if(isValid())
    {
#if SDL_VERSION_ATLEAST(1, 3, 0)
	u8 alpha = 0;
	SDL_GetSurfaceAlphaMod(surface, &alpha);
	return alpha;
#else
	return surface->format->alpha;
#endif
    }
    return 0;
}

u32 Surface::MapRGB(u8 r, u8 g, u8 b, u8 a) const
{
    return amask() ? SDL_MapRGBA(surface->format, r, g, b, a) : SDL_MapRGB(surface->format, r, g, b);
}

void Surface::GetRGB(u32 pixel, u8 *r, u8 *g, u8 *b, u8 *a) const
{
    return amask() && a ? SDL_GetRGBA(pixel, surface->format, r, g, b, a) : SDL_GetRGB(pixel, surface->format, r, g, b);
}


/* load static palette (economize 1kb for each surface) only 8bit color! */
void Surface::LoadPalette(void)
{
    if(pal_colors.empty())
	LoadPalColors();

    if(surface->format->palette)
    {
    	if(surface->format->palette->colors && &pal_colors[0] != surface->format->palette->colors) SDL_free(surface->format->palette->colors);
    	surface->format->palette->colors = &pal_colors[0];
	surface->format->palette->ncolors = pal_colors.size();
    }
}

/* format surface */
void Surface::SetDisplayFormat(void)
{
    if(surface)
    {
	SDL_Surface *osurface = surface;
	surface = amask() ? SDL_DisplayFormatAlpha(osurface) : SDL_DisplayFormat(osurface);
	if(osurface) SDLFreeSurface(osurface);
    }
}

u32 Surface::GetColorIndex(u16 index) const
{
    if(isValid())
    {
	if(8 == depth())
	    return index;
	else
	{
	    index *= 3;

    	    if(static_cast<u16>(ARRAY_COUNT(kb_pal)) > index + 2)
		return MapRGB(kb_pal[index] << 2, kb_pal[index + 1] << 2, kb_pal[index + 2] << 2);
	}
    }
    return 0;
}

u32 Surface::GetColorKey(void) const
{
    if(isValid())
    {
#if SDL_VERSION_ATLEAST(1, 3, 0)
	u32 res = 0;
	SDL_GetColorKey(surface, &res);
	return res;
#else
	return (surface->flags & SDL_SRCCOLORKEY) ? surface->format->colorkey : 0;
#endif
    }
    return 0;
}

/* set color key */
void Surface::SetDefaultColorKey(void)
{
    if(isValid())
    {
	const u32 clkey = amask() ? SDL_MapRGBA(surface->format, 0xFF, 0, 0xFF, 0) : 
				    SDL_MapRGB(surface->format, 0xFF, 0, 0xFF);
	Fill(clkey);
	SetColorKey(clkey);
    }
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

    SetPixel24(bufp, color);
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
    if(isDisplay()) Display::Get().AddUpdateRect(x, y, 1, 1);
}

u32 Surface::GetPixel4(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * (surface->pitch >> 2) + x;

    return *bufp;
}

u32 Surface::GetPixel3(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3; 

    return GetPixel24(bufp);
}

u32 Surface::GetPixel2(u16 x, u16 y) const
{
    if(x > surface->w || y > surface->h) return 0;
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * (surface->pitch >> 1) + x;

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
    SDL_Rect dstrect = {0, 0, surface->w, surface->h};

    SDL_FillRect(surface, &dstrect, color);
    if(isDisplay()) Display::Get().AddUpdateRect(0, 0, surface->w, surface->h);
}

void Surface::Fill(u8 r, u8 g, u8 b)
{
    Fill(MapRGB(r, g, b));
}

/* rect fill colors surface */
void Surface::FillRect(u32 color, const Rect & rect)
{
    SDL_Rect dstrect = {rect.x, rect.y, rect.w, rect.h};
    SDL_FillRect(surface, &dstrect, color);
    if(isDisplay()) Display::Get().AddUpdateRect(rect.x, rect.y, rect.w, rect.h);
}

void Surface::FillRect(u8 r, u8 g, u8 b, const Rect & src)
{
    FillRect(MapRGB(r, g, b), src);
}

bool BlitCheckVariant(const Surface & sf, u16 x, u16 y, u8 variant)
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

void SetAmask(u8* ptr, u16 w, const SDL_PixelFormat* format, u8 alpha)
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
    if(sf1.depth() == sf2.depth() &&
	// RGBA <-> RGB
	(((0 != sf1.amask() && 0 == sf2.amask()) ||
	 (0 == sf1.amask() && 0 != sf2.amask())) ||
	// depth 24, RGB <-> RGB
	 (sf1.depth() == 24 && 0 == sf1.amask() && 0 == sf2.amask())))
    {
        SDL_Rect rt1 = {0, 0, sf1.w(), sf1.h()};
        SDL_Rect rt2 = {0, 0, sf2.w(), sf2.h()};
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

        u16 x, y;
	u8 variant = 0;

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
                    int w = 0;
                    while(BlitCheckVariant(sf1, srt->x + x + w, srt->y + y, variant) && x + w < srt->w) ++w;

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
void Surface::Blit(s16 dst_ox, s16 dst_oy, Surface & dst) const
{
    SDL_Rect dstrect = {dst_ox, dst_oy, surface->w, surface->h};

    if(dst.isDisplay())
    {
	SDL_BlitSurface(surface, NULL, dst.surface, &dstrect);
	Display::Get().AddUpdateRect(dst_ox, dst_oy, surface->w, surface->h);
    }
    else
	BlitSurface(*this, NULL, dst, &dstrect);
}

/* blit */
void Surface::Blit(const Rect &src_rt, s16 dst_ox, s16 dst_oy, Surface & dst) const
{
    SDL_Rect dstrect = {dst_ox, dst_oy, src_rt.w, src_rt.h};
    SDL_Rect srcrect = {src_rt.x, src_rt.y, src_rt.w, src_rt.h};

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

void Surface::Blit(u8 alpha0, const Rect & srt, const Point & dpt, Surface & dst) const
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
        const u8 tmp = alpha();
        const_cast<Surface &>(*this).SetAlpha(alpha0);
        Blit(srt, dpt, dst);
        const_cast<Surface &>(*this).SetAlpha(tmp);
    }
}

void Surface::Blit(u8 alpha0, s16 dstx, s16 dsty, Surface & dst) const
{
    Blit(alpha0, Rect(0, 0, w(), h()), Point(dstx, dsty), dst);
}

void Surface::SetAlpha(u8 level)
{
    if(isValid())
    {
#if SDL_VERSION_ATLEAST(1, 3, 0)
	SDL_SetSurfaceAlphaMod(surface, level);
#else
	SDL_SetAlpha(surface, SDL_SRCALPHA, level);
#endif
    }
}

void Surface::ResetAlpha(void) 
{
    if(isValid())
    {
#if SDL_VERSION_ATLEAST(1, 3, 0)
	SDL_SetSurfaceAlphaMod(surface, 255);
#else
	SDL_SetAlpha(surface, 0, 255);
#endif
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
	SDLFreeSurface(sf.surface);
	sf.surface = NULL;
    }
}

s32 Surface::GetIndexColor(u32 col) const
{
    for(std::vector<SDL_Color>::iterator
	it = pal_colors.begin(); it != pal_colors.end(); ++it)
	if(MapRGB((*it).r, (*it).g, (*it).b) == col)
	    return std::distance(pal_colors.begin(), it);
    return -1;
}

void Surface::ChangeColorIndex(u32 fc, u32 tc)
{
    ChangeColor(GetColorIndex(fc), GetColorIndex(tc));
}

void Surface::ChangeColor(u32 fc, u32 tc)
{
    if(amask())
    {
	fc |= amask();
	tc |= amask();
    }

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

    u8 a, r, g, b, z;

    const u32 colkey = GetColorKey();
    u32 color = 0;

    Lock();
    for(u16 y = 0; y < surface->h; ++y)
	for(u16 x = 0; x < surface->w; ++x)
    {
	color = GetPixel(x, y);
	if(color == colkey) continue;
	GetRGB(color, &r, &g, &b, &a);
	z = static_cast<u8>(0.299 * r + 0.587 * g + 0.114 * b);
	r = z;
	g = z;
	b = z;
	SetPixel(x, y, MapRGB(r, g, b, a));
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

void Surface::MakeStencil(Surface & dst, const Surface & src, u32 col)
{
    dst.Set(src.surface->w, src.surface->h, false);
    const u32 clkey = src.GetColorKey();
    u8 r, g, b, a;

    src.Lock();
    dst.Lock();

    for(u16 y = 0; y < src.surface->h; ++y)
    {
        for(u16 x = 0; x < src.surface->w; ++x)
        {
            u32 pixel = src.GetPixel(x, y);
            if(clkey != pixel)
	    {
		if(src.alpha())
		{
		    src.GetRGB(pixel, &r, &g, &b, &a);
		    // skip shadow
		    if(a < 200) continue;
		}

                dst.SetPixel(x, y, col);
            }
        }
    }

    dst.Unlock();
    src.Unlock();
}

void Surface::MakeContour(Surface & dst, const Surface & src, u32 col)
{
    dst.Set(src.surface->w + 2, src.surface->h + 2, false);

    Surface trf;
    u32 fake = src.MapRGB(0x00, 0xFF, 0xFF);

    MakeStencil(trf, src, fake);
    const u32 clkey = trf.GetColorKey();

    trf.Lock();
    dst.Lock();

    for(u16 y = 0; y < trf.h(); ++y)
    {
        for(u16 x = 0; x < trf.w(); ++x)
        {
            if(fake == trf.GetPixel(x, y))
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
}

void Surface::Reflect(Surface & sf_dst, const Surface & sf_src, const u8 shape)
{
    if(sf_src.isValid())
    {
	sf_dst.Set(sf_src);

	const char* src = static_cast<const char*>(sf_src.surface->pixels);
	char* dst = static_cast<char*>(sf_dst.surface->pixels);
	const s32 size = sf_src.surface->h * sf_src.surface->pitch;
	const u8 bpp = sf_src.surface->format->BytesPerPixel;

	sf_dst.Lock();

	switch(shape % 4)
	{
    	    // normal
	    default:
		std::memcpy(dst, src, size);
        	break;

    	    // vertical reflect
    	    case 1:
		for(s32 offset = 0; offset < size; offset += sf_src.surface->pitch)
    		    std::memcpy(dst + size - sf_src.surface->pitch - offset,
			src + offset, sf_src.surface->pitch);
        	break;

    	    // horizontal reflect
    	    case 2:
        	for(s32 offset = 0; offset < size; offset += bpp)
		    std::memcpy(dst + (offset % sf_src.surface->pitch) +
			size - sf_src.surface->pitch - static_cast<s32>(offset / sf_src.surface->pitch) * sf_src.surface->pitch, src + size - offset - bpp, bpp);
		break;

    	    // both variants
	    case 3:
        	for(s32 offset = 0; offset < size; offset += bpp)
		    std::memcpy(dst + offset, src + size - offset - bpp, bpp);
    		break;
	}

	sf_dst.Unlock();
    }
    else
	std::cerr << "Surface::TILReflect: " << "incorrect param" << std::endl;
}

u32 Surface::GetSize(void) const
{
    u32 res = 0;

    if(surface)
    {
	res = sizeof(SDL_Surface) + sizeof(SDL_PixelFormat) + surface->pitch * surface->h;
	if(surface->format->palette) res += sizeof(SDL_Palette) + surface->format->palette->ncolors * sizeof(SDL_Color);
    }

    return res;
}

std::string Surface::Info(void) const
{
    std::ostringstream os;

    if(isValid())
    {
#if SDL_VERSION_ATLEAST(1, 3, 0)
	os << "FIXME: SDL-1.3";
#else
	os << 
	    "flags" << "(" << surface->flags << ", " << (surface->flags & SDL_SRCALPHA ? "SRCALPHA" : "") << (surface->flags & SDL_SRCCOLORKEY ? "SRCCOLORKEY" : "") << "), " <<
	    "w"<< "(" << surface->w << "), " <<
	    "h"<< "(" << surface->h << "), " <<
	    "size" << "(" << GetSize() << "), " <<
	    "bpp" << "(" << static_cast<int>(surface->format->BitsPerPixel) << "), " <<
	    "Amask" << "(" << "0x" << std::setw(8) << std::setfill('0') << std::hex << surface->format->Amask << "), " <<
	    "colorkey" << "(" << "0x" << std::setw(8) << std::setfill('0') << surface->format->colorkey << "), " << std::dec <<
	    "alpha" << "(" << static_cast<int>(surface->format->alpha) << "), ";
#endif
    }
    else
	os << "invalid";

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
void Surface::ScaleMinifyByTwo(Surface & sf_dst, const Surface & sf_src, bool event)
{
    if(!sf_src.isValid()) { std::cerr << __FUNCTION__ << "invalid surface" << std::endl; return; };
    u16 x, y, x2, y2;

    u8 mul = 2;
    u16 w = sf_src.w() / mul;
    u16 h = sf_src.h() / mul;

    if(2 > w || 2 > h){ std::cerr << __FUNCTION__ << "small size" << std::endl; return; };

    sf_dst.Set(w, h, sf_src.depth(), sf_src.amask());

    sf_dst.Lock();
    sf_src.Lock();
    for(y = 0; y < h; y++)
    {
       y2 = mul * y;
       for(x = 0; x < w; x++)
       {
	    x2 = mul * x;
	    const u32 & p = AVERAGE(sf_src.surface->format, sf_src.GetPixel(x2, y2), sf_src.GetPixel(x2 + 1, y2));
	    const u32 & q = AVERAGE(sf_src.surface->format, sf_src.GetPixel(x2, y2 + 1), sf_src.GetPixel(x2 + 1, y2 + 1));
	    sf_dst.SetPixel(x, y, AVERAGE(sf_src.surface->format, p, q));
	    if(event) LocalEvent::Get().HandleEvents(false);
       }
    }
    sf_src.Unlock();
    sf_dst.Unlock();
}

void Surface::Swap(Surface & sf1, Surface & sf2)
{
    std::swap(sf1.surface, sf2.surface);
}
