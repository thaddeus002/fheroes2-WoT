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
#include "maps.h"
#include "mapsdata.h"
#include "rect.h"

class GameArea
{
public:
    GameArea(const MapsData & data);
    
    typedef enum { LEFT, RIGHT, TOP, BOTTOM } scroll_t;

    static const Rect & GetRect(void){ return pos; }

    Rect GetPosition(void) const{ return Rect(BORDERWIDTH, BORDERWIDTH, pos.w * TILEWIDTH, pos.h * TILEWIDTH); }

    void Scroll(GameArea::scroll_t scroll);

    void Center(const Point &pt);
    void CenterFromRadar(const Point &pt);

    void Redraw(void);
    void Redraw(const Rect &area_rt);

private:
    const MapsData & maps;

    static Rect pos;
};

#endif
