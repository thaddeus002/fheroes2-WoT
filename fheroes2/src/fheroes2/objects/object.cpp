/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

Object::Object(MP2::object_t obj, u16 sicn, u32 uid)
    : object(obj), icn(sicn), id(uid ? uid : World::GetUniq())
{
}

Object::~Object()
{
}

bool Object::AllowDirect(u8 general, u16 direct)
{
    switch(general)
    {
	case MP2::OBJ_SHIPWRECK:
	    return direct & (Direction::CENTER | Direction::LEFT | DIRECTION_BOTTOM_ROW);

	case MP2::OBJ_DERELICTSHIP:
	    return direct & (Direction::CENTER | Direction::LEFT | Direction::BOTTOM_LEFT | Direction::BOTTOM);

	case MP2::OBJ_ARCHERHOUSE:
	case MP2::OBJ_DOCTORHUT:
	case MP2::OBJ_DWARFCOTT:
	case MP2::OBJ_THATCHEDHUT:
	case MP2::OBJ_FOUNTAIN:
	case MP2::OBJ_IDOL:
	case MP2::OBJ_LIGHTHOUSE:
	case MP2::OBJ_OBELISK:
	case MP2::OBJ_SIGN:
	case MP2::OBJ_WATCHTOWER:
	case MP2::OBJ_WITCHSHUT:
	case MP2::OBJ_GAZEBO:
	case MP2::OBJ_MAGICWELL:
	case MP2::OBJ_OBSERVATIONTOWER:
	case MP2::OBJ_PEASANTHUT:
	case MP2::OBJ_STONELIGHTS:
	case MP2::OBJ_STANDINGSTONES:
	case MP2::OBJ_GOBLINHUT:
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_ARTESIANSPRING:
	case MP2::OBJ_SKELETON:
	case MP2::OBJ_TREEKNOWLEDGE:
	case MP2::OBJ_ORACLE:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_LEANTO:
	case MP2::OBJ_MAGICGARDEN:
	case MP2::OBJ_WAGON:
	//case MP2::OBJ_BARRIER: // because Barrier used with any direction
	case MP2::OBJ_TRAVELLERTENT:
	case MP2::OBJ_JAIL:
	case MP2::OBJ_ALCHEMYTOWER:
        case MP2::OBJ_HUTMAGI:
        case MP2::OBJ_EYEMAGI:
	case MP2::OBJ_MERCENARYCAMP:
	case MP2::OBJ_WINDMILL:
	case MP2::OBJ_WATERINGHOLE:
	case MP2::OBJ_TROLLBRIDGE:
	case MP2::OBJ_TRADINGPOST:
	case MP2::OBJ_EXCAVATION:
	case MP2::OBJ_DESERTTENT:
	case MP2::OBJ_DAEMONCAVE:
	case MP2::OBJ_PYRAMID:
	case MP2::OBJ_FORT:
	case MP2::OBJ_RUINS:
	case MP2::OBJ_HILLFORT:
	case MP2::OBJ_FREEMANFOUNDRY:
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_TREECITY:
	case MP2::OBJ_SPHINX:
	case MP2::OBJ_TEMPLE:
	case MP2::OBJ_FAERIERING:
	case MP2::OBJ_BARROWMOUNDS:
	case MP2::OBJ_STABLES:
	case MP2::OBJ_ABANDONEDMINE:
	case MP2::OBJ_MINES:
	case MP2::OBJ_ALCHEMYLAB:
	case MP2::OBJ_CAVE:
	case MP2::OBJ_CITYDEAD:
	case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DRAGONCITY:
	case MP2::OBJ_XANADU:
	case MP2::OBJ_HALFLINGHOLE:
	case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_WATERALTAR:
	case MP2::OBJ_AIRALTAR:
	case MP2::OBJ_FIREALTAR:
	case MP2::OBJ_EARTHALTAR:
	case MP2::OBJ_ARENA:
	case MP2::OBJ_SIRENS:
	case MP2::OBJ_MERMAID:
	    return direct & (DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW); 

	case MP2::OBJ_WATERWHEEL:
	    return direct & (Direction::CENTER | Direction::RIGHT | DIRECTION_BOTTOM_ROW);

	case MP2::OBJ_MAGELLANMAPS:
	    return direct & (Direction::CENTER | Direction::LEFT | DIRECTION_BOTTOM_ROW);

	case MP2::OBJ_CASTLE:
	    return direct & (Direction::CENTER | Direction::BOTTOM);

	default: break;
    }

    return true;
}
