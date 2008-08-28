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

#include "world.h"
#include "mounts.h"
#include "trees.h"
#include "objsnow.h"
#include "objswmp.h"
#include "objgras.h"
#include "objcrck.h"
#include "object.h"

Object::Object(const MP2::object_t obj, const u16 sicn, const u32 uid)
    : object(obj), icn(sicn), id(uid ? uid : World::GetUniq())
{
}

Object::~Object()
{
}

bool Object::isPassable(const u8 general, const std::list<Maps::TilesAddon> & bottoms)
{
    if(bottoms.size())
    {
        std::list<Maps::TilesAddon>::const_iterator it1 = bottoms.begin();
        std::list<Maps::TilesAddon>::const_iterator it2 = bottoms.end();

        for(; it1 != it2; ++it1)
        {
            const Maps::TilesAddon & addon = *it1;
	    const ICN::icn_t icn = MP2::GetICNObject(addon.object);

            switch(icn)
            {
		case ICN::MTNCRCK:
		case ICN::MTNSNOW:
		case ICN::MTNSWMP:
	    	case ICN::MTNLAVA:
	    	case ICN::MTNDSRT:
	    	case ICN::MTNDIRT:
	    	case ICN::MTNMULT:
	    	case ICN::MTNGRAS:	if(! Mounts::isPassable(icn, addon.index)) return false; break;

		case ICN::TREJNGL:
		case ICN::TREEVIL:
		case ICN::TRESNOW:
		case ICN::TREFIR:
		case ICN::TREFALL:
		case ICN::TREDECI:	if(! Trees::isPassable(icn, addon.index)) return false; break;

		case ICN::OBJNSNOW:	if(! ObjSnow::isPassable(icn, addon.index)) return false; break;
		case ICN::OBJNSWMP:	if(! ObjSwamp::isPassable(icn, addon.index)) return false; break;

		case ICN::OBJNGRAS:
		case ICN::OBJNGRA2:	if(! ObjGrass::isPassable(icn, addon.index)) return false; break;

		case ICN::OBJNCRCK:	if(! ObjWasteLand::isPassable(icn, addon.index)) return false; break;
		default: break;
	    }
        }
    }

    return true;
}
/*
	    return ICN::MONS32;
	    return ICN::OBJNRSRC;
	    return ICN::EXTRAOVR;

	    return ICN::OBJNTOWN;
	    return ICN::OBJNTWBA;
	    return ICN::OBJNTWSH;
	    return ICN::OBJNTWRD;

	    return ICN::OBJNWAT2;
	    return ICN::OBJNWATR;

	    return ICN::OBJNMUL2;
	    return ICN::OBJNMULT;


	    return ICN::OBJNLAVA;
	    return ICN::OBJNLAV3;
	    return ICN::OBJNLAV2;

	    return ICN::OBJNDSRT;

	    return ICN::OBJNDIRT;

	    return ICN::OBJNCRCK;

	    return ICN::X_LOC1;
	    return ICN::X_LOC2;
	    return ICN::X_LOC3;

	    return ICN::UNKNOWN;
*/
