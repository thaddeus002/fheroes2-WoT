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

u8 Captain::GetAttack(Race::race_t race)
{
    switch(race)
    {
	case Race::KNGT:	return 1;
	case Race::BARB:	return 1;
	case Race::SORC:	return 0;
	case Race::WRLK:	return 0;
	case Race::WZRD:	return 0;
	case Race::NECR:	return 0;
	default: break;
    }

    return 0;
}

u8 Captain::GetDefense(Race::race_t race)
{
    switch(race)
    {
	case Race::KNGT:	return 1;
	case Race::BARB:	return 1;
	case Race::SORC:	return 0;
	case Race::WRLK:	return 0;
	case Race::WZRD:	return 0;
	case Race::NECR:	return 0;
	default: break;
    }

    return 0;
}

u8 Captain::GetPower(Race::race_t race)
{
    switch(race)
    {
	case Race::KNGT:	return 1;
	case Race::BARB:	return 1;
	case Race::SORC:	return 2;
	case Race::WRLK:	return 2;
	case Race::WZRD:	return 2;
	case Race::NECR:	return 2;
	default: break;
    }

    return 0;
}

u8 Captain::GetKnowledge(Race::race_t race)
{
    switch(race)
    {
	case Race::KNGT:	return 1;
	case Race::BARB:	return 1;
	case Race::SORC:	return 2;
	case Race::WRLK:	return 2;
	case Race::WZRD:	return 2;
	case Race::NECR:	return 2;
	default: break;
    }

    return 0;
}
