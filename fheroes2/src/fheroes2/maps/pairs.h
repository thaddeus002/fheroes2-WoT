/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#ifndef H2PAIRS_H
#define H2PAIRS_H

#include <utility>
#include "maps_tiles.h"

class IndexDistance : public std::pair<u16, u16>
{
    public:
    IndexDistance() : std::pair<u16, u16>(MAXU16, 0) {};
    IndexDistance(u16 i, u16 d) : std::pair<u16, u16>(i, d) {};

    static bool Longest(const IndexDistance & id1, const IndexDistance & id2){ return id1.second > id2.second; };
};

class IndexObject : public std::pair<u16, MP2::object_t>
{
    public:
    IndexObject() : std::pair<u16, MP2::object_t>(MAXU16, MP2::OBJ_ZERO) {};
    IndexObject(const std::pair<u16, MP2::object_t> & pair) : std::pair<u16, MP2::object_t>(pair) {};
    IndexObject(const u16 index, const MP2::object_t object) : std::pair<u16, MP2::object_t>(index, object) {};
    IndexObject(const Maps::Tiles & tile) : std::pair<u16, MP2::object_t>(tile.GetIndex(), tile.GetObject()) {};

    bool isIndex(u16 index) const { return index == first; };
    bool isObject(MP2::object_t object) const { return object == second; };
};

#endif
