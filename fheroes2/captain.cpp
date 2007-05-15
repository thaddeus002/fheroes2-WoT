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

#include "error.h"
#include "captain.h"

const std::string & Captain::BigPortraitString(Race::race_t race)
{
    static const std::string big_portrate_kngt("PORT0089.ICN");
    static const std::string big_portrate_barb("PORT0090.ICN");
    static const std::string big_portrate_sorc("PORT0091.ICN");
    static const std::string big_portrate_wrlk("PORT0092.ICN");
    static const std::string big_portrate_wzrd("PORT0093.ICN");
    static const std::string big_portrate_necr("PORT0094.ICN");

    switch(race)
    {
	case Race::KNGT:	return big_portrate_kngt;
	case Race::BARB:	return big_portrate_barb;
	case Race::SORC:	return big_portrate_sorc;
	case Race::WRLK:	return big_portrate_wrlk;
	case Race::WZRD:	return big_portrate_wzrd;
	case Race::NECR:	return big_portrate_necr;
	default: break;
    }

    Error::Warning("Captain::BigPortraitString: unknown race.");

    return big_portrate_kngt;
}
