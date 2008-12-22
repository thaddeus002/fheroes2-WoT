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
#ifndef H2SURFACE_H
#define H2SURFACE_H

#include "rect.h"
#include "types.h"

#define DEFAULT_DEPTH           16              // Surface use bits color
#define SWSURFACE		SDL_SWSURFACE

class Palette;
class Point;
class Rect;
struct SDL_Surface;
struct SDL_PixelFormat;

#ifdef WITH_TTF
namespace SDL { class Font; };
#endif

class Surface
{
public:
    Surface();
    Surface(const unsigned char * pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool alpha);
    Surface(u16 sw, u16 sh, u8 depth, u32 fl);
    Surface(u16 sw, u16 sh, bool alpha = false);
    Surface(const Surface & bs);
    Surface(SDL_Surface * sf);

    ~Surface();

    Surface & operator= (const Surface & bs);
    void Set(u16 sw, u16 sh, bool alpha = false);

    u16 w(void) const;
    u16 h(void) const;
    u8  depth(void) const;

    bool valid(void) const{ return surface ? true : false; };
    bool alpha(void) const;
    u32 flags(void) const;
    u32 MapRGB(u8 r, u8 g, u8 b, u8 a = 0) const;
    void GetRGB(u32 pixel, u8 *r, u8 *g, u8 *b, u8 *a = NULL) const;

    void Blit(const Surface &src);
    void Blit(const Surface &src, s16 dst_ox, s16 dst_oy);
    void Blit(const Surface &src, const Point &dst_pt){ Blit(src, dst_pt.x, dst_pt.y); };
    void Blit(const Surface &src, const Rect &src_rt, s16 dst_ox, s16 dst_oy);
    void Blit(const Surface &src, const Rect &src_rt, const Point &dst_pt){ Blit(src, src_rt, dst_pt.x, dst_pt.y); };

    void ScaleFrom(const Surface & bs);
    
    bool SaveBMP(const char *fn) const;
    const void *pixels(void) const;
    const SDL_Surface *GetSurface(void) const{ return surface; };

    void Fill(u32 color);
    void Fill(u8 r, u8 g, u8 b){ Fill(MapRGB(r, g, b)); };

    void FillRect(u32 color, const Rect & src);
    void FillRect(u8 r, u8 g, u8 b, const Rect & src){ FillRect(MapRGB(r, g, b), src); };

    void SetDisplayFormat(void);
    void SetColorKey(void);
    void SetColorKey(u8 r, u8 g, u8 b);
    void SetColorKey(u32 color);
    void SetAlpha(u8 level);
    void ResetAlpha(void);
    void SetPixel4(u16 x, u16 y, u32 color);
    void SetPixel3(u16 x, u16 y, u32 color);
    void SetPixel2(u16 x, u16 y, u32 color);
    void SetPixel1(u16 x, u16 y, u32 color);
    void SetPixel(u16 x, u16 y, u32 color);
    
    void LoadPalette(void);
    u32 GetColorKey(void) const;
    u32 GetColor(u16) const;

    u32 GetPixel4(u16 x, u16 y) const;
    u32 GetPixel3(u16 x, u16 y) const;
    u32 GetPixel2(u16 x, u16 y) const;
    u32 GetPixel1(u16 x, u16 y) const;
    u32 GetPixel(u16 x, u16 y) const;

    void DrawLine(const Point &, const Point &, u32);
    void DrawLine(u16, u16, u16, u16, u32);

    void MakeStencil(Surface &, u32) const;

    void ChangeColor(u32, u32);
    void GrayScale(void);
    void Sepia(void);
    
    void Lock(void);
    void Unlock(void);

protected:
#ifdef WITH_TTF
    friend class SDL::Font;
#endif
    friend class Palette;

    void CreateSurface(const Rect &sz, u8 dp, u32 fl){ CreateSurface(sz.w, sz.h, dp, fl); };
    void CreateSurface(u16 sw, u16 sh, u8 dp, u32 fl);
    void FreeSurface(void);
    const SDL_PixelFormat *GetPixelFormat(void) const;

    SDL_Surface *surface;
    
    bool videosurface;
};

#endif
