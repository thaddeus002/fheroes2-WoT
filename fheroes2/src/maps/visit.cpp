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

#include "world.h"
#include "maps_tiles.h"
#include "visit.h"

Maps::VisitIndexObject::VisitIndexObject() : std::pair<u16, MP2::object_t>(MAXU16, MP2::OBJ_ZERO)
{
}

Maps::VisitIndexObject::VisitIndexObject(const std::pair<u16, MP2::object_t> & pair) : std::pair<u16, MP2::object_t>(pair)
{
}

Maps::VisitIndexObject::VisitIndexObject(const u16 index, const MP2::object_t object) : std::pair<u16, MP2::object_t>(index, object)
{
}

Maps::VisitIndexObject::VisitIndexObject(const Maps::Tiles & tile) : std::pair<u16, MP2::object_t>(tile.GetIndex(), tile.GetObject())
{
}

bool Maps::VisitIndexObject::isDayLife(const VisitIndexObject & visit)
{
    return MP2::isDayLife(visit.second);
}

bool Maps::VisitIndexObject::isWeekLife(const VisitIndexObject & visit)
{
    return MP2::isWeekLife(visit.second);
}

bool Maps::VisitIndexObject::isMonthLife(const VisitIndexObject & visit)
{
    return MP2::isMonthLife(visit.second);
}

bool Maps::VisitIndexObject::isBattleLife(const VisitIndexObject & visit)
{
    return MP2::isBattleLife(visit.second);
}
