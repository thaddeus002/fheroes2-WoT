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

#include "captain.h"

Captain::Captain() : Skill::Primary()
{
}

void Captain::SetRace(Race::race_t race)
{
    switch(race)
    {
	case Race::KNGT:
	    attack	= 1;
	    defence	= 1;
	    power	= 1;
	    knowledge	= 1;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	case Race::BARB:
	    attack	= 1;
	    defence	= 1;
	    power	= 1;
	    knowledge	= 1;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	case Race::SORC:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	case Race::WRLK:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	case Race::WZRD:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	case Race::NECR:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    morale	= Morale::NORMAL;
	    luck	= Luck::NORMAL;
	    break;

	default:
	    break;
    }

}

u8 Captain::GetAttack(void) const
{
    return attack;
}

u8 Captain::GetDefense(void) const
{
    return defence;
}

u8 Captain::GetPower(void) const
{
    return power;
}

u8 Captain::GetKnowledge(void) const
{
    return knowledge;
}

Morale::morale_t Captain::GetMorale(void) const
{
    return morale;
}

Luck::luck_t Captain::GetLuck(void) const
{
    return luck;
}
