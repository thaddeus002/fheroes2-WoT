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

#ifndef H2MAPSVISIT_H
#define H2MAPSVISIT_H

#include <utility>
#include "mp2.h"
#include "gamedefs.h"

namespace Maps
{

    class VisitIndexObject : public std::pair<u16, MP2::object_t>
    {
	public:
	VisitIndexObject();
	VisitIndexObject(const std::pair<u16, MP2::object_t> & pair);
	VisitIndexObject(const u16 index, const MP2::object_t object);
	VisitIndexObject(const Maps::Tiles & tile);

	static bool isDayLife(const VisitIndexObject & visit);
	static bool isWeekLife(const VisitIndexObject & visit);
	static bool isMonthLife(const VisitIndexObject & visit);
	static bool isBattleLife(const VisitIndexObject & visit);
    };
};

#endif
