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

#include "mp2.h"
#include "world.h"
#include "config.h"
#include "castle.h"
#include "error.h"
#include "monster.h"
#include "heroes.h"

void Heroes::Action(const u16 dst_index, const MP2::object_t object)
{
    switch(object)
    {
	case MP2::OBJ_ZERO:	return;

	case MP2::OBJ_MONSTER:	ActionToMonster(dst_index); break;

        case MP2::OBJ_CASTLE:	ActionToCastle(dst_index); break;
        case MP2::OBJ_HEROES:	ActionToHeroes(dst_index); break;

        case MP2::OBJ_BOAT:
    	    if(! isShipMaster())
    	    {
    		shipmaster = true;
    		move_point = 0;
    	    }
    	break;

	case MP2::OBJ_COAST:
	    if(isShipMaster())
	    {
		shipmaster = false;
		move_point = 0;
	    }
	break;

        // resource
        case MP2::OBJ_ARTIFACT:
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_ANCIENTLAMP:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_BOTTLE:

        // object
        case MP2::OBJ_ALCHEMYTOWER:
        case MP2::OBJ_SIGN:
        case MP2::OBJ_SKELETON:
        case MP2::OBJ_DAEMONCAVE:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_GAZEBO:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUNT:
        case MP2::OBJ_DWARFCOTT:
	case MP2::OBJ_PEASANTHUNT:
        case MP2::OBJ_PEASANTHUNT2:
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_LIGHTHOUSE:
        case MP2::OBJ_WATERMILL:
        case MP2::OBJ_MINES:
        case MP2::OBJ_OBELISK:
        case MP2::OBJ_OASIS:
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_ORACLE:
	case MP2::OBJ_DERELICTSHIP:
	case MP2::OBJ_DESERTTENT:
	case MP2::OBJ_STONELIGHTS:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_RUINS:
        case MP2::OBJ_FORT:
        case MP2::OBJ_TRADINGPOST:
        case MP2::OBJ_ABANDONEDMINE:
        case MP2::OBJ_STANDINGSTONES:
        case MP2::OBJ_IDOL:
        case MP2::OBJ_TREEKNOWLEDGE:
        case MP2::OBJ_DOCTORHUNT:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_CRAKEDLAKE:
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:
	case MP2::OBJ_PIRAMID:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_SPHINX:
        case MP2::OBJ_WAGON:
        case MP2::OBJ_ARTESIANSPRING:
        case MP2::OBJ_TROLLBRIDGE:
        case MP2::OBJ_WITCHHUNT:
        case MP2::OBJ_XANADU:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_MAGICWELL:
        case MP2::OBJ_MAGICGARDEN:
	case MP2::OBJ_OBSERVATIONTOWER:
        case MP2::OBJ_FREEMANFOUNDRY:

	default:
	    if(H2Config::Debug()) Error::Verbose("Heroes::Action: " + std::string(MP2::StringObject(object)));
	    break;
    }
}

void Heroes::ActionToMonster(const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const Monster::monster_t monster = Monster::Monster(tile);
    //const u16 count = Monster::GetSize(tile);

    if(H2Config::Debug()) Error::Verbose("Heroes::ActionToMonster: " + GetName() + " attack monster " + Monster::String(monster));

    if(H2Config::Debug()) Error::Verbose("Heroes::ActionToMonster: FIXME: attack monster");
}

void Heroes::ActionToHeroes(const u16 dst_index)
{
    const Heroes *other_hero = world.GetHeroes(dst_index);

    if(! other_hero) return;

    if(color == other_hero->GetColor())
    {
	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToHeroes: " + GetName() + " meeting " + other_hero->GetName());

	MeetingDialog(const_cast<Heroes &>(*other_hero));
    }
    else
    {
	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToHeroes: " + GetName() + " attack enemy hero " + other_hero->GetName());

	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToHeroes: FIXME: attack enemy hero");
    }
}

void Heroes::ActionToCastle(const u16 dst_index)
{
    const Castle *castle = world.GetCastle(dst_index);

    if(! castle) return;
    
    if(color == castle->GetColor())
    {
	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToCastle: " + GetName() + " goto castle " + castle->GetName());

	const_cast<Castle *>(castle)->OpenDialog();
    }
    else
    {
	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToCastle: " + GetName() + " attack enemy castle " + castle->GetName());

	if(H2Config::Debug()) Error::Verbose("Heroes::ActiontoCastle: FIXME: attack enemy castle");
    }
}
