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

#include "rand.h"
#include "race.h"

const std::string & Race::String(Race::race_t race)
{
    static const std::string kngt("Knight");
    static const std::string barb("Barbarian");
    static const std::string sorc("Sorceress");
    static const std::string wrlk("Warlock");
    static const std::string wzrd("Wizard");
    static const std::string necr("Necromancer");
    static const std::string mult("Multi");
    static const std::string random("Randomize");
    static const std::string bomg("Bomg");

    switch(race){
        case Race::KNGT: return kngt;
        case Race::BARB: return barb;
        case Race::SORC: return sorc;
        case Race::WRLK: return wrlk;
        case Race::WZRD: return wzrd;
        case Race::NECR: return necr;
        case Race::MULT: return mult;
        case Race::RAND: return random;
        default: break;
    }

    return bomg;
}

Race::race_t Race::Rand(void)
{
    switch(Rand::Get(1, 6)){
        case 1: return Race::KNGT;
        case 2: return Race::BARB;
        case 3: return Race::SORC;
        case 4: return Race::WRLK;
        case 5: return Race::WZRD;
	default: break;
    }

    return Race::NECR;
}
