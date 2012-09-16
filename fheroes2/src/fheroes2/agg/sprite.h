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
#ifndef H2SPRITE_H
#define H2SPRITE_H

#include "display.h"
#include "gamedefs.h"

class Sprite : public Surface
{
public:
    Sprite();
    Sprite(const Sprite &);

    void SetOffset(s16, s16);

    s16 x(void) const{ return offsetX; }
    s16 y(void) const{ return offsetY; }

    void Blit(Surface & = Display::Get()) const;
    void Blit(s16, s16, Surface & = Display::Get()) const;
    void Blit(const Point &, Surface & = Display::Get()) const;
    void Blit(const Rect & srt, s16, s16, Surface & = Display::Get()) const;
    void Blit(const Rect & srt, const Point &, Surface & = Display::Get()) const;
    void Blit(u8 alpha, s16, s16, Surface & = Display::Get()) const;
    void Blit(u8 alpha, const Rect & srt, const Point &, Surface & = Display::Get()) const;

    u32 GetMemoryUsage(void) const;
    void ScaleMinifyByTwo(void);

    static void DrawICN(u16 icn, Surface & sf, const u8* buf, const u32 size, bool reflect);
    static void AddonExtensionModify(Sprite & sp, u16 icn, u16 index);

private:

    s16		offsetX;
    s16		offsetY;
};

#endif
