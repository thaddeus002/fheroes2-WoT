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

class GameArea
{
public:
    GameArea();

    typedef enum { LEFT, RIGHT, TOP, BOTTOM } scroll_t;

    static const Rect & GetRect(void){ return area_pos; };

    void Scroll(scroll_t scroll);

    void Center(const Point &pt);
    void CenterFromRadar(const Point &pt);

    void Redraw(const Rect & area_rt = Rect(0, 0, area_pos.w, area_pos.h));
    void RedrawAnimation(void);

private:
    static Rect area_pos;
    static u32 animation_ticket;
};

#endif
