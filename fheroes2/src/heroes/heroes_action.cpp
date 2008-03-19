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

#include "audio.h"
#include "mp2.h"
#include "world.h"
#include "config.h"
#include "castle.h"
#include "error.h"
#include "monster.h"
#include "heroes.h"

// action to next cell
void Heroes::Action(void)
{
    if(path.empty()) return;

    const u16 dst_index = path.front().to_index;
    const MP2::object_t object = world.GetTiles(dst_index).GetObject();

    switch(object)
    {
	case MP2::OBJ_ZERO:	MoveNext(); return;

	case MP2::OBJ_MONSTER:	ActionToMonster(dst_index); break;

        case MP2::OBJ_CASTLE:	ActionToCastle(dst_index); break;
        case MP2::OBJ_HEROES:	ActionToHeroes(dst_index); break;

        case MP2::OBJ_BOAT:	ActionToBoat(); break;
	case MP2::OBJ_COAST:	ActionToCoast(); break;

        // resource
        case MP2::OBJ_RESOURCE:	ActionToResource(dst_index); break;
        case MP2::OBJ_ARTIFACT:
        case MP2::OBJ_ANCIENTLAMP:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_BOTTLE:
    	    path.Hide();
    	    path.Reset();
	    if(H2Config::Debug()) Error::Verbose("Heroes::Action: " + std::string(MP2::StringObject(object)));
	    break;

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
    	    MoveNext();
	    if(H2Config::Debug()) Error::Verbose("Heroes::Action: " + std::string(MP2::StringObject(object)));
	    break;

	default:
	    if(H2Config::Debug()) Error::Verbose("Heroes::Action: unknown object: " + std::string(MP2::StringObject(object)));
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
	MoveNext();

	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToCastle: " + GetName() + " goto castle " + castle->GetName());

	Audio::Mixer::Get().Reduce();

	const_cast<Castle *>(castle)->OpenDialog();

	Audio::Mixer::Get().Enhance();
    }
    else
    {
	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToCastle: " + GetName() + " attack enemy castle " + castle->GetName());

	if(H2Config::Debug()) Error::Verbose("Heroes::ActiontoCastle: FIXME: attack enemy castle");
    }
}

void Heroes::ActionToBoat(void)
{
    if(isShipMaster()) return;

    MoveNext();
    SetShipMaster(true);
    move_point = 0;

    if(H2Config::Debug()) Error::Verbose("Heroes::ActionToBoat: " + GetName() + " to boat");
}

void Heroes::ActionToCoast(void)
{
    if(! isShipMaster()) return;

    MoveNext();
    SetShipMaster(false);
    move_point = 0;

    if(H2Config::Debug()) Error::Verbose("Heroes::ActionToCoast: " + GetName() + " to coast");
}

void Heroes::ActionToResource(const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Maps::TilesAddon *addon = tile.FindResource();

    path.Hide();
    path.Reset();

    if(addon)
    {
	Resource::funds_t resource;
	u16 count = 0;
	const u32 uniq = addon->uniq;

	switch(addon->index)
        {
    	    case 1:	count = Resource::RandCount(Resource::WOOD); resource.wood += count;  break;
    	    case 3:	count = Resource::RandCount(Resource::MERCURY); resource.mercury += count; break;
    	    case 5:	count = Resource::RandCount(Resource::ORE); resource.ore += count; break;
    	    case 7:	count = Resource::RandCount(Resource::SULFUR); resource.sulfur += count; break;
    	    case 9:	count = Resource::RandCount(Resource::CRYSTAL); resource.crystal += count; break;
    	    case 11:	count = Resource::RandCount(Resource::GEMS); resource.gems += count; break;
    	    case 13:	count = Resource::RandCount(Resource::GOLD); resource.gold += count; break;

	    default:
		if(H2Config::Debug()) Error::Warning("Heroes::ActionToResource: unknown resource, from tile: ", dst_index);
		return;
        }

	PlayPickupSound();

	world.GetKingdom(GetColor()).AddFundsResource(resource);

	tile.Remove(uniq);
	tile.SetObject(MP2::OBJ_ZERO);

	// remove shadow from left cell
	if(Maps::isValidDirection(dst_index, Direction::LEFT))
	{
	    Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT));

	    left_tile.Remove(uniq);
	    left_tile.Redraw();
        }

	tile.Redraw();

	if(H2Config::Debug()) Error::Verbose("Heroes::ActionToResource: " + GetName() + " pickup small resource");
    }
}
