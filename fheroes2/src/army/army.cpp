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

#include "army.h"

const std::string & Army::String(Army::size_t size)
{
    static const std::string str_size[] = { "Few", "Several", "Pack", "Lots", "Horde", "Throng", "Swarm", "Zounds", "Legion" };

    switch(size)
    {
	case FEW:	return str_size[0];
        case SEVERAL:	return str_size[1];
        case PACK:	return str_size[2];
        case LOTS:	return str_size[3];
        case HORDE:	return str_size[4];
        case THRONG:	return str_size[5];
        case SWARM:	return str_size[6];
        case ZOUNDS:	return str_size[7];
        case LEGION:	return str_size[8];
    }

    return str_size[0];
}

Army::size_t Army::GetSize(u16 count)
{
    if(LEGION <= count)		return LEGION;
    else
    if(ZOUNDS <= count)		return ZOUNDS;
    else
    if(SWARM <= count)		return SWARM;
    else
    if(THRONG <= count)		return THRONG;
    else
    if(HORDE <= count)		return HORDE;
    else
    if(LOTS <= count)		return LOTS;
    else
    if(PACK <= count)		return PACK;
    else
    if(SEVERAL <= count)	return SEVERAL;

    return FEW;
}

Army::Troops::Troops(const Army::Troops & troops) : monster(troops.Monster()), count(troops.Count()), master_skill(NULL), pos(0,0)
{
}

Army::Troops & Army::Troops::operator= (const Army::Troops & troops)
{
    monster = troops.Monster();
    count = troops.Count();
    pos = troops.Position();

    return *this;
}

bool Army::Troops::PredicateIsValid(const Army::Troops & t)
{
    return Monster::UNKNOWN > t.Monster() && t.Count();
}

bool Army::Troops::isValid(void) const
{
    return Monster::UNKNOWN > monster && count;
}

bool Army::isValid(const Troops & army)
{
    return army.isValid();
}
