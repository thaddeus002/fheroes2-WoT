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
#include "objxloc.h"
#include "objtown.h"
#include "objwatr.h"
#include "objlava.h"
#include "objmult.h"
#include "objdsrt.h"
#include "objdirt.h"
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
		case ICN::OBJNDIRT:	if(! ObjDirt::isPassable(icn, addon.index)) return false; break;
		case ICN::OBJNDSRT:	if(! ObjDesert::isPassable(icn, addon.index)) return false; break;
		case ICN::OBJNMUL2:
		case ICN::OBJNMULT:	if(! ObjMulti::isPassable(icn, addon.index)) return false; break;
		case ICN::OBJNLAVA:
		case ICN::OBJNLAV3:
		case ICN::OBJNLAV2:	if(! ObjLava::isPassable(icn, addon.index)) return false; break;
		case ICN::OBJNWAT2:
		case ICN::OBJNWATR:	if(! ObjWater::isPassable(icn, addon.index)) return false; break;

		case ICN::MONS32:
		case ICN::MINIMON:	return false;

		case ICN::OBJNRSRC:	if(addon.index % 2) return false; break;

		case ICN::OBJNTWBA:
		case ICN::OBJNTOWN:	if(! ObjTown::isPassable(icn, addon.index)) return false; break;

		case ICN::X_LOC1:
		case ICN::X_LOC2:
		case ICN::X_LOC3:	if(! ObjLoyalty::isPassable(icn, addon.index)) return false; break;

		default: break;
	    }
        }
    }

    return true;
}
