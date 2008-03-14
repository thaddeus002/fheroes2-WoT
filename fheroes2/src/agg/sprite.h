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

#include "surface.h"
#include "icn.h"
#include "gamedefs.h"

class Sprite : public Surface
{

public:
    enum { REFLECT = 0x01, GREEN2RED = 0x02, WHITE2YELLOW = 0x04 } flag_t;

    /* icn sprite constructor */
    Sprite(const ICN::Header & header, const char *data, const u32 size, const u8 modify = 0);

    s16 x(void) const{ return offsetX; };
    s16 y(void) const{ return offsetY; };

    static void DrawICN(Surface & sf, const u32 size, const u8 *vdata, const u8 modify);

private:
    s16		offsetX;
    s16		offsetY;
};

#endif
