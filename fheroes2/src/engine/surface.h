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
class Sprite;

class Surface
{
public:
    Surface();
    Surface(const void* pixels, u32 width, u32 height, u32 bytes_per_pixel, bool amask);
    Surface(u32 sw, u32 sh, bool amask = false);
    Surface(const std::string &);

    Surface(const Surface &);

    Surface & operator= (const Surface &);
    bool operator== (const Surface &);

    virtual ~Surface();
    virtual bool isDisplay(void) const;

    void Set(u32 sw, u32 sh, bool amask = false);
    void Set(SDL_Surface*);
    void Set(u32 sw, u32 sh, u32 bpp /* 8, 16, 24, 32 */, bool amask); 
    void Set(const void* pixels, u32 width, u32 height, u32 bytes_per_pixel /* 1, 2, 3, 4 */, bool amask); 
    void Reset(void);

    bool Load(const char*);
    bool Load(const std::string &);

    bool Save(const char*) const;
    bool Save(const std::string &) const;

    int w(void) const;
    int h(void) const;
    u32 depth(void) const;
    u32 amask(void) const;
    u32 alpha(void) const;
    Size GetSize(void) const;
    bool isRefCopy(void) const;

    bool isValid(void) const;
    u32 MapRGB(int r, int g, int b, int a = 0) const;
    void GetRGB(u32 pixel, u8 *r, u8 *g, u8 *b, u8 *a = NULL) const;

    void Blit(Surface &) const;
    void Blit(s32, s32, Surface &) const;
    void Blit(const Point &, Surface &) const;
    void Blit(const Rect & srt, s32, s32, Surface &) const;
    void Blit(const Rect & srt, const Point &, Surface &) const;
    void Blit(u32 alpha, s32, s32, Surface &) const;
    void Blit(u32 alpha, const Rect & srt, const Point &, Surface &) const;

    const SDL_Surface* SDLSurface(void) const{ return surface; };

    void Fill(u32 color);
    void Fill(int r, int g, int b);

    void FillRect(u32 color, const Rect & src);
    void FillRect(int r, int g, int b, const Rect & src);

    void SetDisplayFormat(void);
    void SetColorKey(u32 color);
    void SetAlpha(u32 level);
    void ResetAlpha(void);
    void SetPixel(int x, int y, u32 color);
    
    u32 GetColorKey(void) const;
    u32 GetColorIndex(u32) const;
    s32 GetIndexColor(u32) const;
    u32 GetPixel(int x, int y) const;

    void ChangeColor(u32, u32);
    void ChangeColorIndex(u32, u32);
    
    void Lock(void) const;
    void Unlock(void) const;

    virtual u32  GetMemoryUsage(void) const;
    std::string Info(void) const;

    static Surface Reflect(const Surface &, u32 shape /* 0: none, 1 : vert, 2: horz, 3: both */);
    static Surface Rotate(const Surface &, u32 parm /* 0: none, 1 : 90 CW, 2: 90 CCW, 3: 180 */);
    static Surface Stencil(const Surface &, u32);
    static Surface Contour(const Surface &, u32);
    static Surface Scale(const Surface &, s32, s32);
    static Surface GrayScale(const Surface &);
    static Surface Sepia(const Surface &);
    static Surface RectBorder(u32, u32, u32 fill, u32 color, bool solid);
    static Surface RectBorder(u32, u32, u32 color, bool solid);

    static void DrawLine(const Point &, const Point &, u32, Surface &);
    static void DrawLine(s32, s32, s32, s32, u32, Surface &);

    static void SetDefaultDepth(u32);
    static u32  GetDefaultDepth(void);
    static void SetDefaultColorKey(int r, int g, int b);
    static u32  GetDefaultColorKey(void);
    static void FreeSurface(Surface &);
    static void Swap(Surface &, Surface &);
    
protected:
    void Set(const Surface &, bool refcopy);
    void SetPixel4(s32 x, s32 y, u32 color);
    void SetPixel3(s32 x, s32 y, u32 color);
    void SetPixel2(s32 x, s32 y, u32 color);
    void SetPixel1(s32 x, s32 y, u32 color);
    u32 GetPixel4(s32 x, s32 y) const;
    u32 GetPixel3(s32 x, s32 y) const;
    u32 GetPixel2(s32 x, s32 y) const;
    u32 GetPixel1(s32 x, s32 y) const;
    void LoadPalette(void);
    void CreateSurface(u32 sw, u32 sh, u32 bpp, bool amask);
    static void BlitSurface(const Surface &, SDL_Rect*, Surface &, SDL_Rect*);

    friend class Display;

    SDL_Surface* surface;
};

#endif
