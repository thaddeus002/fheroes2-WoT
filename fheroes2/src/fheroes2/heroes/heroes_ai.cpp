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

#include "settings.h"
#include "kingdom.h"
#include "army.h"
#include "battle.h"
#include "luck.h"
#include "morale.h"
#include "world.h"
#include "gameevent.h"
#include "heroes.h"

void AIToMonster(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToPickupResource(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToTreasureChest(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToResource(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToWagon(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToSkeleton(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToCaptureObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToFlotSam(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToObservationTower(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToMagellanMaps(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToTeleports(Heroes &hero, const u16 dst_index);
void AIToWhirlpools(Heroes &hero, const u16 dst_index);
void AIToPrimarySkillObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToExperienceObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToWitchsHut(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToShrine(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToGoodLuckObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToGoodMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToMagicWell(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToArtesianSpring(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToXanadu(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToEvent(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToUpgradeArmyObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToPoorMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToPoorLuckObject(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToObelisk(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToTreeKnowledge(Heroes &hero, const u8 obj, const u16 dst_index);
void AIToDaemonCave(Heroes &hero, const u8 obj, const u16 dst_index);

void Heroes::AIUpdateRoles(void)
{
    SetModes(SCOUTER);
    SetModes(HUNTER);
}

void AIBattleLose(Heroes &hero, const u8 reason)
{
    world.GetKingdom(hero.GetColor()).RemoveHeroes(&hero);
    hero.SetFreeman(reason);
}

void Heroes::AIAction(const u16 dst_index)
{
    const MP2::object_t object = (dst_index == GetIndex() ? GetUnderObject() : world.GetTiles(dst_index).GetObject());

    switch(object)
    {
	case MP2::OBJ_MONSTER:		AIToMonster(*this, object, dst_index); break;

        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:         AIToPickupResource(*this, object, dst_index); break;

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:	AIToTreasureChest(*this, object, dst_index); break;

        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATERWHEEL:	AIToResource(*this, object, dst_index); break;

        case MP2::OBJ_WAGON:		AIToWagon(*this, object, dst_index); break;
        case MP2::OBJ_SKELETON:		AIToSkeleton(*this, object, dst_index); break;
        case MP2::OBJ_FLOTSAM:		AIToFlotSam(*this, object, dst_index); break;

	case MP2::OBJ_ALCHEMYLAB:
        case MP2::OBJ_MINES:
	case MP2::OBJ_SAWMILL:
        case MP2::OBJ_LIGHTHOUSE:	AIToCaptureObject(*this, object, dst_index); break;

	// event
	case MP2::OBJ_EVENT:		AIToEvent(*this, object, dst_index); break;

	// increase view
	case MP2::OBJ_OBSERVATIONTOWER:	AIToObservationTower(*this, object, dst_index); break;
        case MP2::OBJ_MAGELLANMAPS:	AIToMagellanMaps(*this, object, dst_index); break;

        // teleports
	case MP2::OBJ_STONELIGHTS:	AIToTeleports(*this, dst_index); break;
	case MP2::OBJ_WHIRLPOOL:	AIToWhirlpools(*this, dst_index); break;

        // primary skill modification
        case MP2::OBJ_FORT:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_DOCTORHUT:
        case MP2::OBJ_STANDINGSTONES:	AIToPrimarySkillObject(*this, object, dst_index); break;

	// experience modification
        case MP2::OBJ_GAZEBO:           AIToExperienceObject(*this, object, dst_index); break;

        // witchs hut
        case MP2::OBJ_WITCHSHUT: 	AIToWitchsHut(*this, object, dst_index); break;

        // shrine circle
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:		AIToShrine(*this, object, dst_index); break;

        // luck modification
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_IDOL:		AIToGoodLuckObject(*this, object, dst_index); break;

        // morale modification
        case MP2::OBJ_OASIS:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_WATERINGHOLE:
        case MP2::OBJ_BUOY:		AIToGoodMoraleObject(*this, object, dst_index); break;

	case MP2::OBJ_OBELISK:		AIToObelisk(*this, object, dst_index); break;

        // magic point
	case MP2::OBJ_ARTESIANSPRING:	AIToArtesianSpring(*this, object, dst_index); break;
        case MP2::OBJ_MAGICWELL: 	AIToMagicWell(*this, object, dst_index); break;

        // increase skill
	case MP2::OBJ_XANADU:		AIToXanadu(*this, object, dst_index); break;

        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_FREEMANFOUNDRY:	AIToUpgradeArmyObject(*this, object, dst_index); break;

	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:	AIToPoorMoraleObject(*this, object, dst_index); break;

	case MP2::OBJ_PYRAMID:		AIToPoorLuckObject(*this, object, dst_index); break;
	case MP2::OBJ_DAEMONCAVE:	AIToDaemonCave(*this, object, dst_index); break;

        case MP2::OBJ_TREEKNOWLEDGE:	AIToTreeKnowledge(*this, object, dst_index); break;


	default:
	    Error::Verbose("AI::Action: Hero " + GetName() + " say: I'm stupid, help my please..");
	    break;
    }
}

void AIToMonster(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Monster monster(tile);
    Army::army_t army;
    army.JoinTroop(monster, tile.GetCountMonster());
    army.ArrangeForBattle();

    if(Settings::Get().Debug()) Error::Verbose("AIToMonster: " + hero.GetName() + " attack monster " + monster.GetName());

    const u32 exp = army.CalculateExperience();
    const Army::battle_t b = Army::Battle(hero, army, tile);

    switch(b)
    {
	case Army::WIN:
	{
	    hero.IncreaseExperience(exp);
	    Maps::TilesAddon *addon = tile.FindMonster();
	    if(addon)
	    {
		const u32 uniq = addon->uniq;
		tile.Remove(uniq);
		tile.SetObject(MP2::OBJ_ZERO);

		// remove shadow from left cell
		if(Maps::isValidDirection(dst_index, Direction::LEFT))
		    world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT)).Remove(uniq);
	    }
	    hero.ActionAfterBattle();
	    break;
	}

	case Army::RETREAT:
	case Army::SURRENDER:
	case Army::LOSE:
	    AIBattleLose(hero, b);
	    if(!Settings::Get().Original())
	    {
	        tile.SetCountMonster(army.GetCountMonsters(monster));
	    }
	    break;
        
        default: break;
    }
}

void AIToPickupResource(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    const u8 count = tile.GetQuantity2();

    switch(tile.GetQuantity1())
    {
        case Resource::WOOD: resource.wood += count; break;
        case Resource::MERCURY: resource.mercury += count; break;
        case Resource::ORE: resource.ore += count; break;
        case Resource::SULFUR: resource.sulfur += count; break;
        case Resource::CRYSTAL: resource.crystal += count; break;
        case Resource::GEMS: resource.gems += count; break;
        case Resource::GOLD: resource.gold += 100 * count; break;

        default: break;
    }

    switch(obj)
    {
        case MP2::OBJ_CAMPFIRE:
	    resource.gold += 100 * count;
    	    break;

        default: break;
    }

    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    tile.RemoveObjectSprite();

    hero.GetPath().Reset();

    tile.SetObject(MP2::OBJ_ZERO);
    if(Settings::Get().Debug()) Error::Verbose("AIToPickupResource: " + hero.GetName() + " pickup small resource");
}

void AIToTreasureChest(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    resource.gold = tile.GetQuantity2() * 100;

    if(Maps::Ground::WATER == tile.GetGround())
    {
	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	    if(!hero.PickupArtifact(art))
		    resource.gold = 1500;	// it is from FAQ
	}
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    }
    else
    {
	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	
	    if(!hero.PickupArtifact(art))
		    resource.gold = 1000;	// it is from FAQ
	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	}
	else
	if(resource.gold >= 500)
	{
	    const u16 expr = resource.gold - 500;

	    // select gold or exp
	    if(Rand::Get(1))
		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    else
		hero.IncreaseExperience(expr);
	}
    }

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("AIToTreasureChest: " + hero.GetName() + " pickup chest");
}

void AIToResource(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u8 count = tile.GetQuantity2();
    Resource::funds_t resource;

    switch(tile.GetQuantity1())
    {
	case Resource::WOOD: resource.wood += count; break;
    	case Resource::MERCURY: resource.mercury += count; break;
    	case Resource::ORE: resource.ore += count; break;
    	case Resource::SULFUR: resource.sulfur += count; break;
    	case Resource::CRYSTAL: resource.crystal += count; break;
    	case Resource::GEMS: resource.gems += count; break;
    	case Resource::GOLD: resource.gold += 100 * count; break;

	default: break;
    }

    if(resource.GetValidItems())
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

    tile.SetQuantity1(0);
    tile.SetQuantity2(0);

    if(Settings::Get().Debug()) Error::Verbose("AIToResource: " + hero.GetName() + " pickup small resource");
}

void AIToSkeleton(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

	if(hero.PickupArtifact(art))
	{
	    tile.SetQuantity1(0);
	    tile.SetQuantity2(0);
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToSkeleton: " + hero.GetName());
}

void AIToWagon(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	if(hero.PickupArtifact(art))
	    tile.SetQuantity1(0);
    }
    else
    if(tile.GetQuantity1() && tile.GetQuantity2())
    {
	const u8 count = tile.GetQuantity2();
	Resource::funds_t resource;

	switch(tile.GetQuantity1())
	{
	    case Resource::WOOD:	resource.wood += count; break;
    	    case Resource::MERCURY:	resource.mercury += count; break;
    	    case Resource::ORE:		resource.ore += count; break;
    	    case Resource::SULFUR:	resource.sulfur += count; break;
    	    case Resource::CRYSTAL:	resource.crystal += count; break;
    	    case Resource::GEMS:	resource.gems += count; break;
    	    case Resource::GOLD:	resource.gold += 100 * count; break;

	    default: break;
	}

	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToWagon: " + hero.GetName());
}

void AIToCaptureObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Resource::resource_t res = Resource::UNKNOWN;

    switch(obj)
    {
	case MP2::OBJ_ALCHEMYLAB:	res = Resource::MERCURY; break;
	case MP2::OBJ_SAWMILL:		res = Resource::WOOD; break;
        case MP2::OBJ_MINES:
    	{
    	    const Maps::TilesAddon * taddon = world.GetTiles(dst_index).FindMines();
            // ore
            if(0 == taddon->index)	res = Resource::ORE;
            else
            // sulfur
            if(1 == taddon->index)	res = Resource::SULFUR;
            else
            // crystal
            if(2 == taddon->index)	res = Resource::CRYSTAL;
            else
            // gems
            if(3 == taddon->index)	res = Resource::GEMS;
            else
            // gold
            if(4 == taddon->index)	res = Resource::GOLD;
    	}
    	break;
        default: break;
    }

    // capture object
    if(hero.GetColor() != world.ColorCapturedObject(dst_index))
	world.CaptureObject(dst_index, hero.GetColor());
    if(Settings::Get().Debug()) Error::Verbose("AIToCaptureObject: " + hero.GetName() + " captured: " + std::string(MP2::StringObject(obj)));
}

void AIToFlotSam(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;

    resource.gold += 100 * tile.GetQuantity1();
    resource.wood += tile.GetQuantity2();

    if(resource.GetValidItems()) world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("AIToFlotSam: " + hero.GetName() + " pickup small resource");
}

void AIToObservationTower(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::ClearFog(dst_index, OBSERVATIONTOWERSCOUTE, hero.GetColor());
    hero.SetVisited(dst_index, Visit::GLOBAL);
}

void AIToMagellanMaps(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(1000 <= world.GetKingdom(hero.GetColor()).GetFundsGold())
    {
	hero.SetVisited(dst_index, Visit::GLOBAL);
	world.ActionForMagellanMaps(hero.GetColor());
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToMagellanMaps: " + hero.GetName());
}

void AIToTeleports(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextTeleport(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	Error::Warning("AIToTeleports: action unsuccessfully...");
	return;
    }

    hero.SetCenter(index_to);
    hero.Scoute();

    world.GetTiles(index_from).SetObject(MP2::OBJ_STONELIGHTS);
    world.GetTiles(index_to).SetObject(MP2::OBJ_HEROES);

    if(Settings::Get().Debug()) Error::Verbose("AIToStoneLights: " + hero.GetName());
}

void AIToWhirlpools(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextWhirlpool(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	Error::Warning("AIToWhirlpools: action unsuccessfully...");
	return;
    }

    hero.SetCenter(index_to);
    hero.Scoute();

    world.GetTiles(index_from).SetObject(MP2::OBJ_WHIRLPOOL);
    world.GetTiles(index_to).SetObject(MP2::OBJ_HEROES);

    if(Rand::Get(1))
    {
	Army::Troop & troops = hero.GetArmy().GetWeakestTroop();
	const u16 c = troops.Count() / 2;
	troops.SetCount(c ? c : 1);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToWhirlpools: " + hero.GetName());
}

void AIToPrimarySkillObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    Skill::Primary::skill_t skill = Skill::Primary::UNKNOWN;

    switch(obj)
    {
        case MP2::OBJ_FORT:		skill = Skill::Primary::DEFENCE; break;
        case MP2::OBJ_MERCENARYCAMP:	skill = Skill::Primary::ATTACK; break;
        case MP2::OBJ_DOCTORHUT:	skill = Skill::Primary::KNOWLEDGE; break;
        case MP2::OBJ_STANDINGSTONES:	skill = Skill::Primary::POWER; break;

    	default: break;
    }

    if(!hero.isVisited(tile))
    {
	// increase skill
	hero.IncreasePrimarySkill(skill);
	hero.SetVisited(dst_index);

        // fix double action tile
        if(obj == MP2::OBJ_STANDINGSTONES)
        {
            if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
            if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
        }
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToPrimarySkillObject: " + hero.GetName());
}

void AIToExperienceObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    u32 exp = 0;

    switch(obj)
    {
        case MP2::OBJ_GAZEBO: exp = 1000; break;
    	default: break;
    }

    // check already visited
    if(! hero.isVisited(tile) && exp)
    {
	hero.SetVisited(dst_index);
	hero.IncreaseExperience(exp);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToExperienceObject: " + hero.GetName());
}

void AIToWitchsHut(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Skill::Secondary::skill_t skill = Skill::Secondary::Skill(world.GetTiles(dst_index).GetQuantity1());

    // check full
    if(HEROESMAXSKILL > hero.CountSecondarySkill() && !hero.HasSecondarySkill(skill))
	hero.LearnBasicSkill(skill);

    hero.SetVisited(dst_index);
    if(Settings::Get().Debug()) Error::Verbose("AIToWitchsHut: " + hero.GetName());
}

void AIToShrine(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Spell::spell_t spell = Spell::Spell(world.GetTiles(dst_index).GetQuantity1());
    const u8 spell_level = Spell::Level(spell);

    // check spell book
    if(hero.HasArtifact(Artifact::MAGIC_BOOK) &&
    // check valid level spell and wisdom skill
      !(3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM)))
    {
	hero.AppendSpellToBook(spell);
	hero.SetVisited(dst_index);
    }
    if(Settings::Get().Debug()) Error::Verbose("AIToShrine: " + hero.GetName());
}

void AIToGoodLuckObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    // check already visited
    if(!hero.isVisited(obj)) hero.SetVisited(dst_index);
    if(Settings::Get().Debug()) Error::Verbose("AIToGoodLuckObject: " + hero.GetName());
}

void AIToGoodMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    u16 move = 0;

    switch(obj)
    {
        case MP2::OBJ_OASIS:		move = 800; break;
        case MP2::OBJ_WATERINGHOLE:	move = 400; break;
    	default: break;
    }

    // check already visited
    if(!hero.isVisited(obj))
    {
	// modify morale
	hero.SetVisited(dst_index);
        hero.IncreaseMovePoints(move);

        // fix double action tile
        if(obj == MP2::OBJ_OASIS)
        {
            if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
            if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
        }
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToGoodMoraleObject: " + hero.GetName());
}

void AIToMagicWell(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(hero.GetSpellPoints() != max &&
	// check already visited
	!hero.isVisited(MP2::OBJ_MAGICWELL))
    {
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToMagicWell: " + hero.GetName());
}

void AIToArtesianSpring(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(!hero.isVisited(MP2::OBJ_ARTESIANSPRING) &&
       hero.GetSpellPoints() < max * 2)
    {
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max * 2);

        // fix double action tile
        {
	    const Maps::Tiles & tile = world.GetTiles(dst_index);

            if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
            if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
                tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
        }
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToArtesianSpring: " + hero.GetName());
}

void AIToXanadu(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const u8 level1 = hero.GetLevelSkill(Skill::Secondary::DIPLOMACY);
    const u8 level2 = hero.GetLevel();

    if(!hero.isVisited(tile) &&
      ((level1 == Skill::Level::BASIC && 7 < level2) ||
       (level1 == Skill::Level::ADVANCED && 5 < level2) ||
       (level1 == Skill::Level::EXPERT && 3 < level2) ||
       (9 < level2)))
    {
	hero.IncreasePrimarySkill(Skill::Primary::ATTACK);
	hero.IncreasePrimarySkill(Skill::Primary::DEFENCE);
	hero.IncreasePrimarySkill(Skill::Primary::KNOWLEDGE);
	hero.IncreasePrimarySkill(Skill::Primary::POWER);
	hero.SetVisited(dst_index);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToXanadu: " + hero.GetName());
}

void AIToEvent(Heroes &hero, const u8 obj, const u16 dst_index)
{
    // check event maps
    const GameEvent::Coord* event_maps = world.GetEventMaps(hero.GetColor(), dst_index);
    if(event_maps)
    {
        if(event_maps->GetResource().GetValidItems())
    	    world.GetKingdom(hero.GetColor()).AddFundsResource(event_maps->GetResource());
	if(Artifact::UNKNOWN != event_maps->GetArtifact())
	    hero.PickupArtifact(event_maps->GetArtifact());
    }
    world.GetTiles(dst_index).SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("AIToEvent: " + hero.GetName());
}

void AIToUpgradeArmyObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    switch(obj)
    {
	case MP2::OBJ_HILLFORT:
	    if(hero.GetArmy().HasMonster(Monster::DWARF))
		hero.GetArmy().UpgradeMonsters(Monster::DWARF);
	    if(hero.GetArmy().HasMonster(Monster::ORC))
		hero.GetArmy().UpgradeMonsters(Monster::ORC);
	    if(hero.GetArmy().HasMonster(Monster::OGRE))
		hero.GetArmy().UpgradeMonsters(Monster::OGRE);
	    break;

	case MP2::OBJ_FREEMANFOUNDRY:
	    if(hero.GetArmy().HasMonster(Monster::PIKEMAN))
		hero.GetArmy().UpgradeMonsters(Monster::PIKEMAN);
	    if(hero.GetArmy().HasMonster(Monster::SWORDSMAN))
		hero.GetArmy().UpgradeMonsters(Monster::SWORDSMAN);
	    if(hero.GetArmy().HasMonster(Monster::IRON_GOLEM))
		hero.GetArmy().UpgradeMonsters(Monster::IRON_GOLEM);
	    break;

	default: break;
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToUpgradeArmyObject: " + hero.GetName());
}

void AIToPoorMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    const bool battle = (tile.GetQuantity1() || tile.GetQuantity2());
    bool complete = false;
    
    switch(obj)
    {
        case MP2::OBJ_GRAVEYARD:
	if(battle)
	{
	    Army::army_t army;
	    army.FromGuardian(tile);

	    // battle
	    const u32 exp = army.CalculateExperience();
	    const Army::battle_t b = Army::Battle(hero, army, tile);
	    switch(b)
	    {
		case Army::WIN:
		{
	    	    hero.IncreaseExperience(exp);
	    	    complete = true;
	    	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	    	    Resource::funds_t resource;
	    	    resource.gold = tile.GetQuantity2() * 100;
	    	    hero.PickupArtifact(art);
	    	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    	    hero.ActionAfterBattle();
	    	    break;
		}
		case Army::RETREAT:
		case Army::SURRENDER:
		case Army::LOSE:
	    	    AIBattleLose(hero, b);
	    	    break;
        
		default: break;
	    }
	}
	break;

        case MP2::OBJ_SHIPWRECK:
	if(battle)
	{
		Army::army_t army;
		army.FromGuardian(tile);
		Resource::funds_t resource;
		Artifact::artifact_t art = Artifact::UNKNOWN;
                switch(tile.GetQuantity2())
                {
            	    case 10: resource.gold = 1000; break;
            	    case 15: resource.gold = 2000; break;
            	    case 25: resource.gold = 5000; break;
            	    case 50: resource.gold = 2000; art = Artifact::Artifact(tile.GetQuantity1()); break;
            	    default: Error::Warning("ActionToPoorMoraleObject: unknown variant for ShipWreck, index: ", dst_index); break;
                }

		// battle
		const u32 exp = army.CalculateExperience();
		const Army::battle_t b = Army::Battle(hero, army, tile);
		switch(b)
		{
		    case Army::WIN:
		    {
	    		hero.IncreaseExperience(exp);
	    		complete = true;
			hero.PickupArtifact(art);
	    		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    		hero.ActionAfterBattle();
	    		break;
		    }
		    case Army::RETREAT:
		    case Army::SURRENDER:
		    case Army::LOSE:
	    		AIBattleLose(hero, b);
	    		break;
        
		    default: break;
		}
	    }
	    break;

        case MP2::OBJ_DERELICTSHIP:
	if(battle)
	{
	    Army::army_t army;
	    army.FromGuardian(tile);

	    // battle
	    const u32 exp = army.CalculateExperience();
	    const Army::battle_t b = Army::Battle(hero, army, tile);
	    switch(b)
	    {
		case Army::WIN:
		{
	    	    hero.IncreaseExperience(exp);
	    	    complete = true;
	    	    Resource::funds_t resource;
	    	    resource.gold = tile.GetQuantity2() * 100;
	    	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    	    hero.ActionAfterBattle();
	    	    break;
		}
		case Army::RETREAT:
		case Army::SURRENDER:
		case Army::LOSE:
	    	    AIBattleLose(hero, b);
	    	    break;
        
		default: break;
	    }
	}
	break;

	default: break;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify morale
	hero.SetVisited(dst_index);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToPoorMoraleObject: " + hero.GetName());
}

void AIToPoorLuckObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const bool battle = tile.GetQuantity1();
    bool complete = false;

    switch(obj)
    {
        case MP2::OBJ_PYRAMID:
    	if(battle)
    	{
            // battle
            Army::army_t army;
            army.FromGuardian(tile);

	    // battle
	    const u32 exp = army.CalculateExperience();
	    const Army::battle_t b = Army::Battle(hero, army, tile);
	    switch(b)
	    {
		case Army::WIN:
		{
		    hero.IncreaseExperience(exp);
		    complete = true;
		    const Spell::spell_t spell(static_cast<Spell::spell_t>(tile.GetQuantity1()));
		    // check magick book
		    if(hero.HasArtifact(Artifact::MAGIC_BOOK) &&
		    // check skill level for wisdom
			Skill::Level::EXPERT == hero.GetLevelSkill(Skill::Secondary::WISDOM))
		    {
			hero.AppendSpellToBook(spell);
		    }
		    hero.ActionAfterBattle();
		    break;
		}
		case Army::RETREAT:
		case Army::SURRENDER:
		case Army::LOSE:
		    AIBattleLose(hero, b);
		    break;
        
    		default: break;
	    }
	}
    	break;

    	default: break;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify luck
        hero.SetVisited(dst_index);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToPoorLuckObject: " + hero.GetName());
}

void AIToObelisk(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(!hero.isVisited(obj, Visit::GLOBAL))
    {
        hero.SetVisited(dst_index, Visit::GLOBAL);
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToObelisk: " + hero.GetName());
}

void AIToTreeKnowledge(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    if(!hero.isVisited(tile))
    {
	Resource::funds_t payment;
	switch(tile.GetQuantity2())
	{
	    case 10:	payment.gems = 10; break;
	    case 20:	payment.gold = 2000; break;
	    default:	break;
	}

    	if(!payment.GetValidItems() || world.GetKingdom(hero.GetColor()).AllowPayment(payment))
    	{
	    if(payment.GetValidItems()) world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
	    hero.SetVisited(dst_index);
	    Error::Verbose("level: ", hero.GetLevel());
	    hero.IncreaseExperience(hero.GetExperienceFromLevel(hero.GetLevel()) - hero.GetExperience());
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToTreeKnowledge: " + hero.GetName());
}


void AIToDaemonCave(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    if(tile.GetQuantity2())
    {
	Resource::funds_t resource;

	// check variants
	switch(tile.GetQuantity2())
	{
	    case 1:
    		hero.IncreaseExperience(1000);
		tile.SetQuantity2(0);
		break;
	    case 2:
	    {
		const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
		if(Artifact::UNKNOWN != art) hero.PickupArtifact(art);
    		hero.IncreaseExperience(1000);
		tile.SetQuantity1(Artifact::UNKNOWN);
		tile.SetQuantity2(0);
		break;
	    }
	    case 3:
		resource.gold = 2500;
    		hero.IncreaseExperience(1000);
		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
		tile.SetQuantity2(0);
		break;
	    default:
		break;
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("AIToDaemonCave: " + hero.GetName());
}
















bool Heroes::AIValidObject(const u8 obj, const u16 index)
{
    switch(obj)
    {
	// water object
	case MP2::OBJ_SHIPWRECKSURVIROR:
	case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_FLOTSAM:
	case MP2::OBJ_BOTTLE:
	case MP2::OBJ_BUOY:

	case MP2::OBJ_MAGELLANMAPS:
	case MP2::OBJ_MERMAID:
	case MP2::OBJ_SIRENS:
	case MP2::OBJ_WHIRLPOOL:
	case MP2::OBJ_COAST:
	    if(isShipMaster()) return true;
            break;

	// capture objects
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_MINES:
	case MP2::OBJ_ALCHEMYLAB:
	    if(world.ColorCapturedObject(index) != GetColor()) return true;
	    break;

	// pickup object
	case MP2::OBJ_WAGON:
	case MP2::OBJ_WATERWHEEL:
	case MP2::OBJ_WINDMILL:
	case MP2::OBJ_LEANTO:
	case MP2::OBJ_MAGICGARDEN:
	case MP2::OBJ_SKELETON:
	    if(world.GetTiles(index).ValidQuantity()) return true;
	    break;

	// pickup resource
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_CAMPFIRE:
	case MP2::OBJ_TREASURECHEST:
	    return true;

	// increase view
	case MP2::OBJ_OBSERVATIONTOWER:
	// obelisk
	case MP2::OBJ_OBELISK:
	    if(! isVisited(world.GetTiles(index), Visit::GLOBAL)) return true;
	    break;

	// new spell
        case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	    if(! isVisited(world.GetTiles(index)) &&
                // check spell book
		HasArtifact(Artifact::MAGIC_BOOK) &&
                // check valid level spell and wisdom skill
                !(3 == Spell::Level(Spell::Spell(world.GetTiles(index).GetQuantity1())) &&
                Skill::Level::NONE == GetLevelSkill(Skill::Secondary::WISDOM))) return true;
	    break;

    	// primary skill
	case MP2::OBJ_FORT:
    	case MP2::OBJ_MERCENARYCAMP:
    	case MP2::OBJ_DOCTORHUT:
    	case MP2::OBJ_STANDINGSTONES:
	// sec skill
	case MP2::OBJ_WITCHSHUT:
	// exp
	case MP2::OBJ_GAZEBO:
	    if(! isVisited(world.GetTiles(index))) return true;
	    break;

	case MP2::OBJ_TREEKNOWLEDGE:
	    if(! isVisited(world.GetTiles(index)))
	    {
		Resource::funds_t payment;
		switch(world.GetTiles(index).GetQuantity2())
		{
		    case 10:	payment.gems = 10; break;
		    case 20:	payment.gold = 2000; break;
		    default:	break;
		}

    		if(!payment.GetValidItems() || world.GetKingdom(GetColor()).AllowPayment(payment))
		    return true;
	    }
	    break;

    	// good luck
        case MP2::OBJ_FOUNTAIN:
    	case MP2::OBJ_FAERIERING:
    	case MP2::OBJ_IDOL:
	    if(! isVisited(obj) &&
		Luck::IRISH > GetLuck()) return true;
	    break;

	// good morale
	case MP2::OBJ_OASIS:
    	case MP2::OBJ_TEMPLE:
    	case MP2::OBJ_WATERINGHOLE:
	    if(! isVisited(obj) &&
		Morale::BLOOD > GetMorale()) return true;
	    break;

	case MP2::OBJ_MAGICWELL:
	    if(! isVisited(world.GetTiles(index)) &&
		GetMaxSpellPoints() != GetSpellPoints()) return true;
	    break;

	case MP2::OBJ_ARTESIANSPRING:
	    if(! isVisited(world.GetTiles(index)) &&
		2 * GetMaxSpellPoints() > GetSpellPoints()) return true;
	    break;

	case MP2::OBJ_XANADU:
	{
	    const u8 level1 = GetLevelSkill(Skill::Secondary::DIPLOMACY);
	    const u8 level2 = GetLevel();

	    if(!isVisited(world.GetTiles(index)) &&
		((level1 == Skill::Level::BASIC && 7 < level2) ||
		(level1 == Skill::Level::ADVANCED && 5 < level2) ||
		(level1 == Skill::Level::EXPERT && 3 < level2) || (9 < level2))) return true;
	    break;
	}

	// upgrade army
	case MP2::OBJ_HILLFORT:
            if(army.HasMonster(Monster::DWARF) ||
               army.HasMonster(Monster::ORC) ||
               army.HasMonster(Monster::OGRE)) return true;
            break;

	// upgrade army
        case MP2::OBJ_FREEMANFOUNDRY:
            if(army.HasMonster(Monster::PIKEMAN) ||
               army.HasMonster(Monster::SWORDSMAN) ||
               army.HasMonster(Monster::IRON_GOLEM)) return true;
            break;

	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:
        {
	    const Maps::Tiles & tile = world.GetTiles(index);
            Army::army_t enemy;
	    enemy.FromGuardian(tile);

            // can we will win battle
            if(enemy.isValid() && GetArmy().StrongerEnemyArmy(enemy)) return true;
	    break;
	}

	//case MP2::OBJ_PYRAMID:

	case MP2::OBJ_DAEMONCAVE:
	    if(world.GetTiles(index).GetQuantity2() && 4 != world.GetTiles(index).GetQuantity2()) return true;
	    break;

	case MP2::OBJ_MONSTER:
	{
            const Maps::Tiles & tile = world.GetTiles(index);
            Army::army_t enemy;
            enemy.JoinTroop(Monster(tile), tile.GetCountMonster());

            // can we will win battle
            if(enemy.isValid() && GetArmy().StrongerEnemyArmy(enemy)) return true;
	    break;
	}

	case MP2::OBJ_CASTLE:
	case MP2::OBJ_BOAT:
	case MP2::OBJ_STONELIGHTS:
	    // check later
	    return true;

	/*
	    or add later
	*/

	default: break;
    }

    return false;
}

void Heroes::AIRescueWhereMove(u16 & to)
{
    u8 scoute = GetScoute();

    switch(Settings::Get().GameDifficulty())
    {
        case Difficulty::HARD:      scoute += 2; break;
        case Difficulty::EXPERT:    scoute += 4; break;
        case Difficulty::IMPOSSIBLE:scoute += 6; break;
        default: break;
    }

    scoute += 1;

    // find unchartered territory
    for(u16 ii = 1; ii <= scoute; ++ii)
    {
        const s16 tx = mp.x - ii;
        const s16 ty = mp.y - ii;

        const s16 mx = tx + 2 * ii;
        const s16 my = ty + 2 * ii;

        for(s16 iy = ty; iy <= my; ++iy)
            for(s16 ix = tx; ix <= mx; ++ix)
        {
            if(ty < iy && iy < my && tx < ix && ix < mx) continue;

            const u16 res = Maps::GetIndexFromAbsPoint(ix, iy);

            if(Maps::isValidAbsPoint(ix, iy) &&
                world.GetTiles(res).isFog(color) &&
                world.GetTiles(res).isPassable(this, true) &&
		GetPath().Calculate(res))
            {
                to = res;
                return;
            }
        }
    }

    if(MP2::OBJ_STONELIGHTS == save_maps_general ||
	MP2::OBJ_WHIRLPOOL == save_maps_general)
    {
	AIAction(GetIndex());
    }
}
