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
#ifndef H2MAPSDATA_H
#define H2MAPSDATA_H

#include <vector>
#include "gamedefs.h"
#include "surface.h"
#include "rect.h"
#include "maps.h"
#include "mp2.h"

/* class Data */
class MapsData
{
public:
    MapsData(const std::string &filename);
    ~MapsData();

    static u16 w(void){ return width; };
    static u16 h(void){ return height; };

    const Surface & GetTilesSurface(void) const{ return *tiles; };
    u16 GetGround(u16 index) const{ return index < vec_mapstiles.size() ? vec_mapstiles[index].ground : 0; };
    u8  GetObject(u16 index) const{ return index < vec_mapstiles.size() ? vec_mapstiles[index].object : 0; };

    bool Movement(u16 ax, u16 ay) const{ return Movement(ax * width + ay); };
    bool Movement(u16 index) const;

    void Redraw(const Rect &rt, const Point &pt = Point(0, 0)) const;

private:
    typedef struct { Point coord; Surface *sprite; } levelsprite_t;

    Surface *tiles;
    std::vector<Maps::tiles_t> vec_mapstiles;

    static u16 width;
    static u16 height;
};

#endif
