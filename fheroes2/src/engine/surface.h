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
#ifndef H2SURFACE_H
#define H2SURFACE_H

#include <string>
#include "rect.h"
#include "types.h"

struct Point;
struct Rect;
struct SDL_Surface;
class Display;

class Surface
{
public:
    Surface();
    Surface(const void* pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool amask);
    Surface(u16 sw, u16 sh, bool amask = false);
    Surface(const Surface & bs);
    Surface(SDL_Surface* sf);

    virtual ~Surface();

    Surface & operator= (const Surface & bs);
    void Set(const Surface &);
    void Set(SDL_Surface* sf);
    void Set(u16 sw, u16 sh, bool amask = false);
    void Set(u16 sw, u16 sh, u8 bpp, bool amask); /* bpp: 8, 16, 24, 32 */
    void Set(const void* pixels, unsigned int width, unsigned int height, unsigned char bytes_per_pixel, bool amask); /* bytes_per_pixel: 1, 2, 3, 4 */

    bool Load(const char*);
    bool Load(const std::string &);

    bool Save(const char*) const;
    bool Save(const std::string &) const;

    u16 w(void) const;
    u16 h(void) const;
    u8  depth(void) const;
    u32 amask(void) const;
    u8  alpha(void) const;
    Size GetSize(void) const;

    bool isValid(void) const;
    virtual bool isDisplay(void) const;
    u32 MapRGB(u8 r, u8 g, u8 b, u8 a = 0) const;
    void GetRGB(u32 pixel, u8 *r, u8 *g, u8 *b, u8 *a = NULL) const;

    void Blit(Surface &) const;
    void Blit(s16, s16, Surface &) const;
    void Blit(const Point &, Surface &) const;
    void Blit(const Rect & srt, s16, s16, Surface &) const;
    void Blit(const Rect & srt, const Point &, Surface &) const;
    void Blit(u8 alpha, s16, s16, Surface &) const;
    void Blit(u8 alpha, const Rect & srt, const Point &, Surface &) const;

    //const SDL_Surface* GetSurface(void) const{ return surface; };

    void Fill(u32 color);
    void Fill(u8 r, u8 g, u8 b);

    void FillRect(u32 color, const Rect & src);
    void FillRect(u8 r, u8 g, u8 b, const Rect & src);

    void SetDisplayFormat(void);
    void SetDefaultColorKey(void);
    void SetColorKey(u32 color);
    void SetAlpha(u8 level);
    void ResetAlpha(void);
    void SetPixel(u16 x, u16 y, u32 color);
    
    u32 GetColorKey(void) const;
    u32 GetColorIndex(u16) const;
    s32 GetIndexColor(u32) const;
    u32 GetPixel(u16 x, u16 y) const;

    void DrawLine(const Point &, const Point &, u32);
    void DrawLine(u16, u16, u16, u16, u32);


    void ChangeColor(u32, u32);
    void ChangeColorIndex(u32, u32);
    void GrayScale(void);
    void Sepia(void);
    
    void Lock(void) const;
    void Unlock(void) const;

    u32  GetMemoryUsage(void) const;
    std::string Info(void) const;

    static void Reflect(Surface &, const Surface &, u8 shape /* 0: none, 1 : vert, 2: horz, 3: both */);
    static void Rotate(Surface &, const Surface &, u8 parm /* 0: none, 1 : 90 CW, 2: 90 CCW, 3: 180 */);

    static void MakeStencil(Surface &, const Surface &, u32);
    static void MakeContour(Surface &, const Surface &, u32);

    static void ScaleMinifyByTwo(Surface & sf_dst, const Surface & sf_src, bool event = false);
    static void SetDefaultDepth(u8);
    static u8   GetDefaultDepth(void);
    static void FreeSurface(Surface &);
    static void Swap(Surface &, Surface &);
    
protected:
    void SetPixel4(u16 x, u16 y, u32 color);
    void SetPixel3(u16 x, u16 y, u32 color);
    void SetPixel2(u16 x, u16 y, u32 color);
    void SetPixel1(u16 x, u16 y, u32 color);
    u32 GetPixel4(u16 x, u16 y) const;
    u32 GetPixel3(u16 x, u16 y) const;
    u32 GetPixel2(u16 x, u16 y) const;
    u32 GetPixel1(u16 x, u16 y) const;
    void LoadPalette(void);
    void CreateSurface(u16 sw, u16 sh, u8 bpp, bool amask);
    static void BlitSurface(const Surface &, SDL_Rect*, Surface &, SDL_Rect*);

    friend class Display;

    SDL_Surface* surface;
};

#endif
