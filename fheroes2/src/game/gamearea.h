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
#ifndef H2GAMEAREA_H
#define H2GAMEAREA_H

#include "gamedefs.h"
#include "rect.h"

class GameArea : protected Rect
{
public:
    static GameArea & Get(void);
    void Build(void);

    static const u8 NONE = 0x0;
    static const u8 LEFT = 0x01;
    static const u8 RIGHT = 0x02;
    static const u8 TOP = 0x04;
    static const u8 BOTTOM = 0x08;
    typedef u8 scroll_t;

    const Rect & GetRect(void) const { return *this; };

    static s16 x(void) { return Get().GetRect().x; };
    static s16 y(void) { return Get().GetRect().y; };
    static u16 w(void) { return Get().GetRect().w; };
    static u16 h(void) { return Get().GetRect().h; };

    static u16 GetLeftTopIndexMaps(void);
    static void SrcRectFixed(Rect & src, Point & dst, const u16 rw, const u16 rh);

    void Scroll(scroll_t scroll);
    void Center(const Point &pt);
    void CenterFromRadar(const Point &pt);

    void Redraw(const s16 rx = 0, const s16 ry = 0, const u16 rw = w(), const u16 rh = h()) const;
    void Redraw(const Rect & rt) const;


private:
    GameArea();

    s16 & gx;
    s16 & gy;
    u16 & gw;
    u16 & gh;
};

#endif
