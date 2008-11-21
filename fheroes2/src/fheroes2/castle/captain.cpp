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

#include "morale.h"
#include "luck.h"
#include "castle.h"
#include "army.h"
#include "captain.h"

Captain::Captain(const Castle & c) : home(c)
{
    switch(home.GetRace())
    {
	case Race::KNGT:
	    attack	= 1;
	    defence	= 1;
	    power	= 1;
	    knowledge	= 1;
	    break;

	case Race::BARB:
	    attack	= 1;
	    defence	= 1;
	    power	= 1;
	    knowledge	= 1;
	    break;

	case Race::SORC:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    break;

	case Race::WRLK:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    break;

	case Race::WZRD:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    break;

	case Race::NECR:
	    attack	= 0;
	    defence	= 0;
	    power	= 2;
	    knowledge	= 2;
	    break;

	default:
	    break;
    }
}

bool Captain::isValid(void) const
{
    return home.isBuild(Castle::BUILD_CAPTAIN);
}

u8 Captain::GetAttack(void) const
{
    return isValid() ? attack : 0;
}

u8 Captain::GetDefense(void) const
{
    return isValid() ? defence : 0;
}

u8 Captain::GetPower(void) const
{
    return isValid() ? power : 0;
}

u8 Captain::GetKnowledge(void) const
{
    return isValid() ? knowledge : 0;
}

Morale::morale_t Captain::GetMorale(void) const
{
    s8 result = Morale::NORMAL;

    // check castle morale modificators
    result += home.GetMoraleWithModificators();

    // check from army morale modificators
    result += Army::GetMoraleWithModificators(home.GetArmy());

    // result
    if(result < Morale::AWFUL)  return Morale::TREASON;
    else
    if(result < Morale::POOR)   return Morale::AWFUL;
    else
    if(result < Morale::NORMAL) return Morale::POOR;
    else
    if(result < Morale::GOOD)   return Morale::NORMAL;
    else
    if(result < Morale::GREAT)  return Morale::GOOD;
    else
    if(result < Morale::BLOOD)  return Morale::GREAT;

    return Morale::BLOOD;
}

Luck::luck_t Captain::GetLuck(void) const
{
    s8 result = Luck::NORMAL;

    // check castle morale modificators
    result += home.GetLuckWithModificators();

    // check from army morale modificators
    result += Army::GetLuckWithModificators(home.GetArmy());

    // result
    if(result < Luck::AWFUL)    return Luck::CURSED;
    else
    if(result < Luck::BAD)      return Luck::AWFUL;
    else
    if(result < Luck::NORMAL)   return Luck::BAD;
    else
    if(result < Luck::GOOD)     return Luck::NORMAL;
    else
    if(result < Luck::GREAT)    return Luck::GOOD;
    else
    if(result < Luck::IRISH)    return Luck::GREAT;

    return Luck::IRISH;
}
