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

#include "world.h"
#include "maps_tiles.h"
#include "object.h"

u32 Maps::Object::uniq = 0;

Maps::Object::Object() : type(MP2::OBJ_ZERO), id(++uniq)
{
}

Maps::Object::Object(const MP2::object_t obj) : type(obj), id(++uniq)
{
}

bool Maps::Object::isDayLife(const MP2::object_t obj)
{
    // FIXME: list day object life
    switch(obj)
    {
	default: break;
    }

    return false;
}

bool Maps::Object::isWeekLife(const MP2::object_t obj)
{
    // FIXME: list week object life
    switch(obj)
    {
	default: break;
    }

    return false;
}

bool Maps::Object::isMonthLife(const MP2::object_t obj)
{
    // FIXME: list month object life
    switch(obj)
    {
	default: break;
    }

    return false;
}

bool Maps::Object::isBattleLife(const MP2::object_t obj)
{
    // FIXME: list month object life
    switch(obj)
    {
	case MP2::OBJ_IDOL:
	case MP2::OBJ_FOUNTAIN:
	    return true;

	default: break;
    }

    return false;
}

bool Maps::VisitIndexObject::isDayLife(const VisitIndexObject & visit)
{
    return Maps::Object::isDayLife(visit.second);
}

bool Maps::VisitIndexObject::isWeekLife(const VisitIndexObject & visit)
{
    return Maps::Object::isWeekLife(visit.second);
}

bool Maps::VisitIndexObject::isMonthLife(const VisitIndexObject & visit)
{
    return Maps::Object::isMonthLife(visit.second);
}

bool Maps::VisitIndexObject::isBattleLife(const VisitIndexObject & visit)
{
    return Maps::Object::isBattleLife(visit.second);
}
