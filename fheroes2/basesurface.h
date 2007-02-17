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
#ifndef H2BASESURFACE_H
#define H2BASESURFACE_H

#include "rect.h"
#include "gamedefs.h"

#define DEFAULT_DEPTH           16              // Surface use bits color

class BaseSurface
{
public:
    BaseSurface() : surface(NULL){};
    BaseSurface(u16 sw, u16 sh, u8 depth, u32 fl){ CreateSurface(sw, sh, depth,  fl); };
    BaseSurface(u16 sw, u16 sh, bool alpha = false){ CreateSurface(sw, sh, DEFAULT_DEPTH, alpha ? SDL_SRCALPHA|SDL_SWSURFACE : SDL_SWSURFACE); };
    BaseSurface(const BaseSurface & bs){ if(bs.valid()) surface = SDL_ConvertSurface(const_cast<SDL_Surface *>(bs.GetSurface()), const_cast<SDL_PixelFormat *>(bs.GetPixelFormat()), bs.flags()); };

    BaseSurface & operator= (const BaseSurface & bs);

    u16 w(void) const{ return surface ? surface->w : 0; };
    u16 h(void) const{ return surface ? surface->h : 0; };
    Size size(void) const { return surface ? Size(surface->w, surface->h) : Size(); };

    bool valid(void) const{ return surface ? true : false; };
    u32 flags(void) const{ return surface->flags; };
    u32 MapRGB(u8 r, u8 g, u8 b, u8 a = 0) const{ return a ? SDL_MapRGBA(surface->format, r, g, b, a) : SDL_MapRGB(surface->format, r, g, b); };

    void Blit(const BaseSurface &src);
    void Blit(const BaseSurface &src, s16 dst_ox, s16 dst_oy);
    void Blit(const BaseSurface &src, const Point &dst_pt){ Blit(src, dst_pt.x, dst_pt.y); };
    void Blit(const BaseSurface &src, const Rect &src_rt, s16 dst_ox, s16 dst_oy);
    void Blit(const BaseSurface &src, const Rect &src_rt, const Point &dst_pt){ Blit(src, src_rt, dst_pt.x, dst_pt.y); };

    bool SaveBMP(const char *fn) const{ return SDL_SaveBMP(surface, fn) ? false : true; };
    const void *pixels(void) const{ return surface->pixels; };

    void Fill(u32 color);
    void Fill(u8 r, u8 g, u8 b){ Fill(MapRGB(r, g, b)); };

    void LoadPalette(const SDL_Color *colors);
    void SetDisplayFormat(void);
    void SetColorKey(void);
    void SetColorKey(u32 color){ SDL_SetColorKey(surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, color); };
    void SetPixel2(u16 x, u16 y, u32 color);
    void SetPixel1(u16 x, u16 y, u8 color);

    void Lock(void){ if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface); };
    void Unlock(void){ if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface); };

protected:
    void CreateSurface(const Size &sz, u8 dp, u32 fl){ CreateSurface(sz.w, sz.h, dp, fl); };
    void CreateSurface(u16 sw, u16 sh, u8 dp, u32 fl);
    void FreeSurface(void){ if(surface) SDL_FreeSurface(surface); surface = NULL; }
    const SDL_Surface *GetSurface(void) const{ return surface; };
    const SDL_PixelFormat *GetPixelFormat(void) const{ return surface->format; };

    SDL_Surface *surface;
};

#endif
