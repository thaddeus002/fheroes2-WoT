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

class SpriteCursor
{
public:
    /* sprite cursor */
    SpriteCursor(const Surface &cursor, const Point & pt = Point()) : spriteCursor(cursor), background(pt, cursor.w(), cursor.h()){};
    SpriteCursor(const Surface &cursor, s16 x, s16 y) : spriteCursor(cursor), background(x, y, cursor.w(), cursor.h()){};

    u16  w(void) const{ return spriteCursor.w(); };
    u16  h(void) const{ return spriteCursor.h(); };

    void Move(s16 ax, s16 ay);
    void Move(const Point &pt);
    void Move(const Rect &rt);
    void Hide(void);
    void Show(void);
    void Show(const Point &pt);
    void Show(s16 ax, s16 ay);

    void Redraw(void);
    void Change(const Surface &cursor);
    const Rect & GetRect(void) const{ return background.GetRect(); };

private:
    const Surface & spriteCursor;
    Background background;
};

#endif
