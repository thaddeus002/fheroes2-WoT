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

#include "engine.h"
#include "gamedefs.h"
#include "week.h"

const std::string & Week::GetString(type_t type)
{
    static const std::string str_name[] = { "Unnamed", _("PLAGUE"),
		_("Ant"), _("Grasshopper"), _("Dragonfly"), _("Spider"), _("Butterfly"), _("Bumblebee"),
		_("Locust"), _("Earthworm"), _("Hornet"), _("Beetle"), _("Squirrel"), _("Rabbit"),
		_("Gopher"), _("Badger"), _("Eagle"), _("Weasel"), _("Raven"), _("Mongoose"), _("Aardvark"),
		_("Lizard"), _("Tortoise"), _("Hedgehog"), _("Condor"), "Monsters HII" };

    switch(type)
    {
	case PLAGUE:		return str_name[1];
	case ANT:		return str_name[2];
	case GRASSHOPPER:	return str_name[3];
	case DRAGONFLY:		return str_name[4];
	case SPIDER:		return str_name[5];
	case BUTTERFLY:		return str_name[6];
	case BUMBLEBEE:		return str_name[7];
	case LOCUST:		return str_name[8];
	case EARTHWORM:		return str_name[9];
	case HORNET:		return str_name[10];
	case BEETLE:		return str_name[11];
	case SQUIRREL:		return str_name[12];
	case RABBIT:		return str_name[13];
	case GOPHER:		return str_name[14];
	case BADGER:		return str_name[15];
	case EAGLE:		return str_name[16];
	case WEASEL:		return str_name[17];
	case RAVEN:		return str_name[18];
	case MONGOOSE:		return str_name[19];
	case AARDVARK:		return str_name[20];
	case LIZARD:		return str_name[21];
	case TORTOISE:		return str_name[22];
	case HEDGEHOG:		return str_name[23];
	case CONDOR:		return str_name[24];
	case MONSTERS:		return str_name[25];
	default: break;
    }

    return str_name[0];
}

Week::type_t Week::WeekRand(void)
{
    switch(Rand::Get(2, 24))
    {
	case 2:		return ANT;
	case 3:		return GRASSHOPPER;
	case 4:		return DRAGONFLY;
	case 5:		return SPIDER;
	case 6:		return BUTTERFLY;
	case 7:		return BUMBLEBEE;
	case 8:		return LOCUST;
	case 9:		return EARTHWORM;
	case 10:	return HORNET;
	case 11:	return BEETLE;
	case 12:	return SQUIRREL;
	case 13:	return RABBIT;
	case 14:	return GOPHER;
	case 15:	return BADGER;
	case 16:	return EAGLE;
	case 17:	return WEASEL;
	case 18:	return RAVEN;
	case 19:	return MONGOOSE;
	case 20:	return AARDVARK;
	case 21:	return LIZARD;
	case 22:	return TORTOISE;
	case 23:	return HEDGEHOG;
	case 24:	return CONDOR;
	default: break;
    }

    return UNNAMED;
}

Week::type_t Week::MonthRand(void)
{
    switch(Rand::Get(1, 24))
    {
	case 1:		return PLAGUE;
	case 2:		return ANT;
	case 3:		return GRASSHOPPER;
	case 4:		return DRAGONFLY;
	case 5:		return SPIDER;
	case 6:		return BUTTERFLY;
	case 7:		return BUMBLEBEE;
	case 8:		return LOCUST;
	case 9:		return EARTHWORM;
	case 10:	return HORNET;
	case 11:	return BEETLE;
	case 12:	return SQUIRREL;
	case 13:	return RABBIT;
	case 14:	return GOPHER;
	case 15:	return BADGER;
	case 16:	return EAGLE;
	case 17:	return WEASEL;
	case 18:	return RAVEN;
	case 19:	return MONGOOSE;
	case 20:	return AARDVARK;
	case 21:	return LIZARD;
	case 22:	return TORTOISE;
	case 23:	return HEDGEHOG;
	case 24:	return CONDOR;
	default: break;
    }

    return UNNAMED;
}
