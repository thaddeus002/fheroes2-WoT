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

#include "agg.h"
#include "error.h"
#include "race.h"
#include "heroes.h"
#include "tools.h"
#include "surface.h"
#include "portrait.h"

const Surface & Portrait::Captain(const Race::race_t rc, const Portrait::size_t sz)
{
    switch(sz)
    {
	case BIG:
	    switch(rc)
	    {
    		case Race::KNGT:        return AGG::GetICN("PORT0090.ICN", 0);
    		case Race::BARB:        return AGG::GetICN("PORT0091.ICN", 0);
    		case Race::SORC:        return AGG::GetICN("PORT0092.ICN", 0);
    		case Race::WRLK:        return AGG::GetICN("PORT0093.ICN", 0);
    		case Race::WZRD:        return AGG::GetICN("PORT0094.ICN", 0);
    		case Race::NECR:        return AGG::GetICN("PORT0095.ICN", 0);
    		default: break;
	    }
	    break;

	case MEDIUM:
	case SMALL:
	    switch(rc)
	    {
    		case Race::KNGT:        return AGG::GetICN("MINICAPT.ICN", 0);
    		case Race::BARB:        return AGG::GetICN("MINICAPT.ICN", 1);
    		case Race::SORC:        return AGG::GetICN("MINICAPT.ICN", 2);
    		case Race::WRLK:        return AGG::GetICN("MINICAPT.ICN", 3);
    		case Race::WZRD:        return AGG::GetICN("MINICAPT.ICN", 4);
    		case Race::NECR:        return AGG::GetICN("MINICAPT.ICN", 5);
    		default: break;
	    }
	    break;
    }

    Error::Warning("Portrait::Captain: unknown race.");

    return AGG::GetICN("PORT0090.ICN", 0);
}

const Surface & Portrait::Hero(const Heroes::heroes_t hero, const Portrait::size_t sz)
{
    switch(sz)
    {
	case BIG:
	{
	    std::string icn( hero < 10 ? "PORT000" : "PORT00");
	    String::AddInt(icn, Heroes::SANDYSANDY > hero ? hero : 59);
	    icn += ".ICN";
	    return AGG::GetICN(icn, 0);
	}

	case MEDIUM:
	    if(Heroes::UNKNOWN <= hero) return AGG::GetICN("PORTMEDI.ICN", 0);
	    else
	    if(Heroes::SANDYSANDY > hero) return AGG::GetICN("PORTMEDI.ICN", hero + 1);
	    else
	    return AGG::GetICN("PORTMEDI.ICN", 60);

	case SMALL:
	    return AGG::GetICN("MINIPORT.ICN", Heroes::SANDYSANDY > hero ? hero : 59);
    }

    return AGG::GetICN("PORTMEDI.ICN", 0);
}
