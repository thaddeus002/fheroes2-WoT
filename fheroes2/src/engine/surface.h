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
#include "font.h"
#include "types.h"

struct Point;
struct Rect;
struct SDL_Surface;

class RGBA
{
public:
    RGBA();
    RGBA(int r, int g, int b, int a = 0);

    SDL_Color operator() (void) const { return color; }

    int		GetColor(void) const;
    int         r(void) const;
    int         g(void) const;
    int         b(void) const;
    int         a(void) const;

protected:
    SDL_Color   color;
};

#define ColorBlack RGBA(0,0,0)

class Surface
{
public:
    Surface();
    Surface(u32 sw, u32 sh, bool amask = false);
    Surface(const std::string &);

    Surface(const Surface &);

    Surface & operator= (const Surface &);
    bool operator== (const Surface &) const;
    SDL_Surface* operator() (void) const { return surface; }

    virtual ~Surface();
    virtual bool isDisplay(void) const;

    void Set(const void* pixels, u32 width, u32 height, u32 bytes_per_pixel /* 1, 2, 3, 4 */, bool amask);  /* agg: create raw tile */
    void Set(u32 sw, u32 sh, bool amask = false);
    void Set(SDL_Surface*);
    void Reset(void);

    bool Load(const std::string &);
    bool Save(const std::string &) const;

    int w(void) const;
    int h(void) const;
    u32 depth(void) const;
    u32 amask(void) const;
    u32 alpha(void) const;
    Size GetSize(void) const;
    bool isRefCopy(void) const;

    bool isValid(void) const;

    u32	 MapRGB(const RGBA &) const;
    RGBA GetRGB(u32 pixel) const;

    void SetColorKey(const RGBA &);
    u32	 GetColorKey(void) const;

    void Blit(Surface &) const;
    void Blit(s32, s32, Surface &) const;
    void Blit(const Point &, Surface &) const;
    void Blit(const Rect & srt, s32, s32, Surface &) const;
    void Blit(const Rect & srt, const Point &, Surface &) const;
    void Blit(u32 alpha, s32, s32, Surface &) const;
    void Blit(u32 alpha, const Rect & srt, const Point &, Surface &) const;

    void Fill(const RGBA &);
    void FillRect(const Rect &, const RGBA &);

    void SetDisplayFormat(void);
    void SetAlpha(u32 level);
    void ResetAlpha(void);

    void SetPixel(int x, int y, u32 color);
    void SetPixel(int x, int y, const RGBA &);
    u32  GetPixel(int x, int y) const;

    void Lock(void) const;
    void Unlock(void) const;

    virtual u32  GetMemoryUsage(void) const;
    std::string Info(void) const;

    static Surface Reflect(const Surface &, u32 shape /* 0: none, 1 : vert, 2: horz, 3: both */);
    static Surface Rotate(const Surface &, u32 parm /* 0: none, 1 : 90 CW, 2: 90 CCW, 3: 180 */);
    static Surface Stencil(const Surface &, const RGBA &);
    static Surface Contour(const Surface &, const RGBA &);
    static Surface Scale(const Surface &, s32, s32);
    static Surface GrayScale(const Surface &);
    static Surface Sepia(const Surface &);
    static Surface RectBorder(const Size &, const RGBA &, const RGBA &, bool solid);
    static Surface RectBorder(const Size &, const RGBA &, bool solid);

#ifdef WITH_TTF
    static Surface RenderText(const SDL::Font &, const std::string &, const RGBA &, bool solid /* or blended */);
    static Surface RenderChar(const SDL::Font &, char, const RGBA &, bool solid /* or blended */);
    static Surface RenderUnicodeText(const SDL::Font &, const std::vector<u16> &, const RGBA &, bool solid /* or blended */);
    static Surface RenderUnicodeChar(const SDL::Font &, u16, const RGBA &, bool solid /* or blended */);
#endif

    static void ChangeColor(const RGBA &, const RGBA &, Surface &);
    static void DrawLine(const Point &, const Point &, const RGBA &, Surface &);

    static void SetDefaultPalette(SDL_Color*, int);
    static void SetDefaultDepth(u32);
    static void SetDefaultColorKey(int, int, int);
    static void Swap(Surface &, Surface &);
    
protected:
    static void BlitSurface(const Surface &, SDL_Rect*, Surface &, SDL_Rect*);
    static void FreeSurface(Surface &);

    void Set(const Surface &, bool refcopy);
    void Set(u32 sw, u32 sh, u32 bpp /* bpp: 8, 16, 24, 32 */, bool amask);
    void SetPalette(void);

    void SetPixel4(s32 x, s32 y, u32 color);
    void SetPixel3(s32 x, s32 y, u32 color);
    void SetPixel2(s32 x, s32 y, u32 color);
    void SetPixel1(s32 x, s32 y, u32 color);

    u32 GetPixel4(s32 x, s32 y) const;
    u32 GetPixel3(s32 x, s32 y) const;
    u32 GetPixel2(s32 x, s32 y) const;
    u32 GetPixel1(s32 x, s32 y) const;

    SDL_Surface* surface;
};

#endif
