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
#ifndef H2SPRITE_H
#define H2SPRITE_H

#include "icn.h"
#include "gamedefs.h"

class Sprite : public Surface
{
public:
    /* icn sprite constructor */
    Sprite(const ICN::icn_t icn, const ICN::Header & header, const char *data, const u32 size, bool reflect = false);

    s16 x(void) const{ return offsetX; };
    s16 y(void) const{ return offsetY; };

    operator Point() const { return Point(offsetX, offsetY); }
    operator Size() const { return Size(w(), h()); }
    operator Rect() const { return Rect(offsetX, offsetY, w(), h()); }

    static void DrawICN(Surface & sf, const char *buf, const u32 size, bool reflect);

private:
    s16		offsetX;
    s16		offsetY;
};

#endif
