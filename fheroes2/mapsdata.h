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
#include "mp2.h"

#define TILEWIDTH     32
#define TILEWIDTH    32

/* class Tiles */
class MapsTiles : public Point
{
public:
    MapsTiles(Point pt, MP2::tile_t tile) : Point(x, y) {};
    u16 GetGrounds(void) const{ return ground; };

private:
    u16 ground;
};

/* class Data */
class MapsData
{
public:
    MapsData(const std::string &filename);

    static u16 w(void){ return width; }
    static u16 h(void){ return height; }

    const Surface & GetSurface(void) const{ return tiles; }

    const MapsTiles & GetTiles(u16 index) const{ return vec_tiles.size() > index ? vec_tiles[index] : vec_tiles.back(); }
    const MapsTiles & GetTiles(u32 ax, u32 ay) const{ return vec_tiles.size() > ax * ay ? vec_tiles[ax * width + ay] : vec_tiles.back(); }

    void Redraw(const Rect &rt, const Point &pt = Point(0, 0)) const;

private:
    Surface tiles;
    std::vector<MapsTiles> vec_tiles;

    static u16 width;
    static u16 height;
};

#endif
