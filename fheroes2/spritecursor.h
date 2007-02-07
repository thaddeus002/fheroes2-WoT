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
#ifndef H2SPRITECURSOR_H
#define H2SPRITECURSOR_H

#include "sprite.h"
#include "rect.h"
#include "surface.h"
#include "background.h"
#include "gamedefs.h"

class SpriteCursor : public Surface
{

public:
    /* empty sprite cursor */
    SpriteCursor(const Rect  &rt) : Surface(rt.w, rt.h), rect(rt), background(rect){};
    /* sprite cursor */
    SpriteCursor(const Sprite &sprite, const Point &pt = Point(-1, -1)) : Surface(sprite), rect(pt.x, pt.y, sprite.w(), sprite.h()), background(rect){ Move(pt); };
    SpriteCursor(const Sprite &sprite, const Rect  &rt) : Surface(sprite), rect(rt), background(rect){ Move(Point(rt.x, rt.y)); };

    void Move(s16 ax, s16 ay);
    void Move(const Point &rt);
    void Redraw(void);

private:
    Rect rect;
    Background background;
};

#endif
