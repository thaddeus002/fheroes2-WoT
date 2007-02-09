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
#include "mp2.h"
#include "maps.h"
#include "maps_tiles.h"

/* move on tile */
bool MapsTiles::Move(void)
{
    switch(object){
	case MP2::OBJ_STONES:
	case MP2::OBJ_OILLAKE:
	case MP2::OBJ_BIGCRACK:
	case MP2::OBJ_MOUNTS:
	case MP2::OBJ_TREES:
	case MP2::OBJ_FIRTREES:
        case MP2::OBJN_WAGONCAMP:
        case MP2::OBJN_SAWMILL:
        case MP2::OBJN_MINES:
        case MP2::OBJ_WATERLAKE:
        case MP2::OBJN_ALCHEMYTOWER:
        case MP2::OBJN_EXCAVATION:
        case MP2::OBJN_FORT:
        case MP2::OBJN_DRAGONCITY:
    	    return false;

        default:
            break;
        }

    return true;
}

u16 Maps::GetTypeGrounds(const MP2::tile_t &info, const std::vector<MP2::addons_t> &addon)
{
    u16 index = info.tileIndex;

    // сканируем дорогу ROAD
    if(0x7A == info.objectName1) return Maps::ROAD;

    u16 indexAddon = info.indexAddon;

    while(indexAddon){

        if(0x7A == addon[indexAddon].objectNameN1 * 2) return Maps::ROAD;

        indexAddon = addon[indexAddon].indexAddon;
    }

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
