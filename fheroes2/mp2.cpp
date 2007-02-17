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

#include <string>
#include "config.h"
#include "error.h"
#include "mp2.h"

/* sort two element */
bool MP2::VectorAddonSort(const MP2::addon_t & lhs, const MP2::addon_t & rhs){ return ((lhs.quantity1 % 4) > (rhs.quantity1 % 4)); }

/* return name icn object */
const char * MP2::GetICNObject(u8 type)
{
    switch(type){

	// artifact
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
	    return "OBJNARTI.ICN";
	    
	// monster
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	    return "MINIMON.ICM";
	    
	// castle flags
	case 0x38:
	    break;
	    
	// heroes
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	    break;

	// relief: snow
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	    return "MTNSNOW.ICN";
	    
	// relief: swamp
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
	    return "MTNSWMP.ICN";
	    
	// relief: lava
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	    return "MTNLAVA.ICN";
	    
	// relief: desert
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	    return "MTNDSRT.ICN";

	// relief: dirt
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	    return "MTNDIRT.ICN";
	    
	// relief: others
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	    return "MTNMULT.ICN";

	// mines
	case 0x74:
	    return "EXTRAOVR.ICN";
	    
	// road
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	    return "ROAD.ICN";
	    
	// relief: crck
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:
	    return "MTNCRCK.ICN";

	// relief: gras
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	    return "MTNGRAS.ICN";
	    
	// trees jungle
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	    return "TREJNGL.ICN";

	// trees evil
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	    return "TREEVIL.ICN";

	// castle and tower
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:
	    return "OBJNTOWN.ICN";

	// castle lands
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	    return "OBJNTWBA.ICN";
	    
	// castle shadow
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	    return "OBJNTWSH.ICN";

	// random castle
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	    break;

	// water object
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	    return "OBJNWAT2.ICN";

	// object other
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:
	    return "OBJNMUL2.ICN";

	// trees snow
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	    return "TRESNOW.ICN";

	// trees trefir
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	    return "TREFIR.ICN";

	// trees
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	    return "TREFALL.ICN";
	    
	// river
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
	    return "STREAM.ICN";
	    
	// resource
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	    return "OBJNRSRC.ICN";
	    
	// gras object
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	    return "OBJNGRA2.ICN";
	    
	// trees tredeci
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:
	    return "TREDECI.ICN";
	    
	// sea object
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	    return "OBJNWATR.ICN";
	    
	// vegetation gras
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	    return "OBJNGRAS.ICN";
	    
	// object on snow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	    return "OBJNSNOW.ICN";
	    
	// object on swamp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	    return "OBJNSWMP.ICN";
	    
	// object on lava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	    return "OBJNLAVA.ICN";
	    
	// object on desert
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
	    return "OBJNDSRT.ICN";
	    
	// object on dirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	    return "OBJNDIRT.ICN";
	    
	// object on crck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:
	    return "OBJNCRCK.ICN";
	    
	// object on lava
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	    return "OBJNLAV3.ICN";
	    
	// object on earth
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	    return "OBJNMULT.ICN";
	    
	//  object on lava
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	    return "OBJNLAV2.ICN";
	    
	default:
	    if(H2Config::Debug()) Error::Warning("GetStringObject: unknown type.");
	    break;
    }

    return NULL;
}

bool MP2::StaticObject(u8 type, u8 index)
{
    switch(type){

	// artifact
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
	// monster
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	// castle flags
	case 0x38:
	// heroes (skip)
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	// castle and tower
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:
	// castle lands
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	// castle shadow
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	// random castle
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	// objnrsrc
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	    return false;

	// objnwat2
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	    // sprite boat
	    if(23 == index) break;
	    return true;
	// objnmul2
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:
	    // sprite event
	    if(163 == index) break;
	    return true;
	// obnsnow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	    // sprite campfire
	    if(4 == index) break;
	    return true;
	// objndsrt
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
	    // sprite campfire
	    if(61 == index) break;
	    return true;
	// objnmul
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	    // sprite campfire
	    if(131 == index) break;
	    return true;

	// relief: snow
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	// relief: swamp
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
	// relief: lava
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	// relief: desert
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	// relief: dirt
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	// relief: others
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	// resource mines
	case 0x74:
	// road
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	// relief: crck
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:
	// relief: gras
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	// trees jungle
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	// trees evil
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	// trees trefir
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	// trees
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	// trees tredeci
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:
	// trees snow
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	// river
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
	// objngra2
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	// objnwatr
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	// objngras
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	// objnswmp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	// objnlava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	// objndirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	// objncrck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:
	// objnlav3
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	//  objnlav2
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	    return true;
	    
	default:
	    break;
    }

    return false;
}

/* Maps::ground from mp2 tile */
/*
u16 MP2::GetTypeGrounds(const MP2::tile_t &info)
{
    // сканируем дорогу ROAD
    std::vector<addon_t>::const_iterator it = info.addons.begin();
    std::vector<addon_t>::const_iterator it_end = info.addons.end();

    for(;it != it_end; ++it) if(0x7A == (*it).object1) return Maps::ROAD;

    u16 index = info.tile;
    // list grouns from GROUND32.TIL
    if(30 > index)
        return Maps::WATER;

    else if(92 > index)
        return Maps::GRASS;

    else if(146 > index)
        return Maps::SNOW;

    else if(208 > index)
        return Maps::SWAMP;

    else if(262 > index)
        return Maps::LAVA;

    else if(321 > index)
        return Maps::DESERT;

    else if(361 > index)
        return Maps::DIRT;

    else if(415 > index)
        return Maps::WASTELAND;

    //else if(432 > index)
    return Maps::BEACH;
}
*/
