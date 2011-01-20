/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <functional>                                                                                                         
#include <algorithm>                                                                                                          

#include "settings.h"
#include "kingdom.h"
#include "castle.h"
#include "army.h"
#include "battle2.h"
#include "luck.h"
#include "morale.h"
#include "world.h"
#include "payment.h"
#include "gameevent.h"
#include "heroes.h"
#include "cursor.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "maps_tiles.h"
#include "ai_simple.h"

#define HERO_MAX_SHEDULED_TASK 7

void MoveHero2Dest(Heroes & hero, s32 dst_index, MP2::object_t from_obj, MP2::object_t under_obj);

AIHeroes & AIHeroes::Get(void)
{
    static AIHeroes ai_heroes;
    return ai_heroes;
}

AIHero & AIHeroes::Get(const Heroes & ht)
{
    return AIHeroes::Get().at(ht.GetID());
}

void AIHeroes::Reset(void)
{
    AIHeroes & ai = AIHeroes::Get();
#ifdef ANDROID
    for(AIHeroes::iterator
	it = ai.begin(); it != ai.end(); ++it) it->Reset();
#else
    std::for_each(ai.begin(), ai.end(), std::mem_fun_ref(&AIHero::Reset));
#endif
}

void AIHero::Reset(void)
{
    primary_target = -1;
    sheduled_visit.clear();
    fix_loop = 0;
}

void AIToMonster(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToPickupResource(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToTreasureChest(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToArtifact(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToResource(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToWagon(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToSkeleton(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToCaptureObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToFlotSam(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToObservationTower(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToMagellanMaps(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToTeleports(Heroes &hero, const s32 dst_index);
void AIToWhirlpools(Heroes &hero, const s32 dst_index);
void AIToPrimarySkillObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToExperienceObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToWitchsHut(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToShrine(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToGoodLuckObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToGoodMoraleObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToMagicWell(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToArtesianSpring(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToXanadu(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToEvent(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToUpgradeArmyObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToPoorMoraleObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToPoorLuckObject(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToObelisk(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToTreeKnowledge(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToDaemonCave(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToCastle(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToSign(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToDwellingJoinMonster(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToHeroes(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToDwellingRecruitMonster(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToStables(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToAbandoneMine(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToBarrier(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToTravellersTent(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToShipwreckSurvivor(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToBoat(Heroes &hero, const u8 obj, const s32 dst_index);
void AIToCoast(Heroes &hero, const u8 obj, const s32 dst_index);
void AIMeeting(Heroes & hero1, Heroes & hero2);

Skill::Primary::skill_t AISelectPrimarySkill(Heroes &hero)
{
    switch(hero.GetRace())
    {
	case Race::KNGT:
	{
	    if(5 > hero.GetDefense())	return Skill::Primary::DEFENSE;
	    if(5 > hero.GetAttack())	return Skill::Primary::ATTACK;
	    if(3 > hero.GetKnowledge())	return Skill::Primary::KNOWLEDGE;
	    if(3 > hero.GetPower())	return Skill::Primary::POWER;
    	    break;
	}

	case Race::BARB:
	{
	    if(5 > hero.GetAttack())	return Skill::Primary::ATTACK;
	    if(5 > hero.GetDefense())	return Skill::Primary::DEFENSE;
	    if(3 > hero.GetPower())	return Skill::Primary::POWER;
	    if(3 > hero.GetKnowledge())	return Skill::Primary::KNOWLEDGE;
	    break;
	}

	case Race::SORC:
	case Race::WZRD:
	{
	    if(5 > hero.GetKnowledge())	return Skill::Primary::KNOWLEDGE;
	    if(5 > hero.GetPower())	return Skill::Primary::POWER;
	    if(3 > hero.GetDefense())	return Skill::Primary::DEFENSE;
	    if(3 > hero.GetAttack())	return Skill::Primary::ATTACK;
	    break;
	}

	case Race::WRLK:
	case Race::NECR:
	{
	    if(5 > hero.GetPower())	return Skill::Primary::POWER;
	    if(5 > hero.GetKnowledge())	return Skill::Primary::KNOWLEDGE;
	    if(3 > hero.GetAttack())	return Skill::Primary::ATTACK;
	    if(3 > hero.GetDefense())	return Skill::Primary::DEFENSE;
	    break;
	}
	
	default: break;
    }

    switch(Rand::Get(1,4))
    {
	case 1:	return Skill::Primary::ATTACK;
	case 2:	return Skill::Primary::DEFENSE;
	case 3:	return Skill::Primary::POWER;
	case 4:	return Skill::Primary::KNOWLEDGE;
	default: break;
    }

    return Skill::Primary::UNKNOWN;
}

void AIBattleLose(Heroes &hero, const Battle2::Result & res, bool attacker, Color::color_t color = Color::GRAY)
{
    u8 reason = attacker ? res.AttackerResult() : res.DefenderResult();

    if(Settings::Get().ExtHeroSurrenderingGiveExp() &&
	Battle2::RESULT_SURRENDER == reason)
    {
        const u32 & exp = attacker ? res.GetExperienceAttacker() : res.GetExperienceDefender();

        if(Settings::Get().MyColor() == hero.GetColor())
        {
            std::string msg = _("Hero %{name} also got a %{count} experience.");
            String::Replace(msg, "%{name}", hero.GetName());
            String::Replace(msg, "%{count}", exp);
            Dialog::Message("", msg, Font::BIG, Dialog::OK);
        }
        hero.IncreaseExperience(exp);
    }

    hero.SetKillerColor(color);
    hero.SetFreeman(reason);
}

void AI::HeroesAction(Heroes & hero, s32 dst_index)
{
    const MP2::object_t object = (dst_index == hero.GetIndex() ?
				    hero.GetUnderObject() : world.GetTiles(dst_index).GetObject());

    if(MP2::isActionObject(object, hero.isShipMaster())) hero.SetModes(Heroes::ACTION);

    switch(object)
    {
        case MP2::OBJ_BOAT:		AIToBoat(hero, object, dst_index); break;
        case MP2::OBJ_COAST:		AIToCoast(hero, object, dst_index); break;

    	case MP2::OBJ_MONSTER:		AIToMonster(hero, object, dst_index); break;
	case MP2::OBJ_HEROES:		AIToHeroes(hero, object, dst_index); break;
	case MP2::OBJ_CASTLE:		AIToCastle(hero, object, dst_index); break;

        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:         AIToPickupResource(hero, object, dst_index); break;

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:	AIToTreasureChest(hero, object, dst_index); break;
	case MP2::OBJ_ARTIFACT:		AIToArtifact(hero, object, dst_index); break;

        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATERWHEEL:	AIToResource(hero, object, dst_index); break;

        case MP2::OBJ_WAGON:		AIToWagon(hero, object, dst_index); break;
        case MP2::OBJ_SKELETON:		AIToSkeleton(hero, object, dst_index); break;
        case MP2::OBJ_FLOTSAM:		AIToFlotSam(hero, object, dst_index); break;

	case MP2::OBJ_ALCHEMYLAB:
        case MP2::OBJ_MINES:
	case MP2::OBJ_SAWMILL:
        case MP2::OBJ_LIGHTHOUSE:	AIToCaptureObject(hero, object, dst_index); break;
        case MP2::OBJ_ABANDONEDMINE:    AIToAbandoneMine(hero, object, dst_index); break;

	case MP2::OBJ_SHIPWRECKSURVIROR:AIToShipwreckSurvivor(hero, object, dst_index); break;

	// event
	case MP2::OBJ_EVENT:		AIToEvent(hero, object, dst_index); break;

	case MP2::OBJ_SIGN:		AIToSign(hero, object, dst_index); break;

	// increase view
	case MP2::OBJ_OBSERVATIONTOWER:	AIToObservationTower(hero, object, dst_index); break;
        case MP2::OBJ_MAGELLANMAPS:	AIToMagellanMaps(hero, object, dst_index); break;

        // teleports
	case MP2::OBJ_STONELIGHTS:	AIToTeleports(hero, dst_index); break;
	case MP2::OBJ_WHIRLPOOL:	AIToWhirlpools(hero, dst_index); break;

        // primary skill modification
        case MP2::OBJ_FORT:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_DOCTORHUT:
        case MP2::OBJ_STANDINGSTONES:	AIToPrimarySkillObject(hero, object, dst_index); break;

	// experience modification
        case MP2::OBJ_GAZEBO:           AIToExperienceObject(hero, object, dst_index); break;

        // witchs hut
        case MP2::OBJ_WITCHSHUT: 	AIToWitchsHut(hero, object, dst_index); break;

        // shrine circle
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:		AIToShrine(hero, object, dst_index); break;

        // luck modification
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_IDOL:		AIToGoodLuckObject(hero, object, dst_index); break;

        // morale modification
        case MP2::OBJ_OASIS:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_WATERINGHOLE:
        case MP2::OBJ_BUOY:		AIToGoodMoraleObject(hero, object, dst_index); break;

	case MP2::OBJ_OBELISK:		AIToObelisk(hero, object, dst_index); break;

        // magic point
	case MP2::OBJ_ARTESIANSPRING:	AIToArtesianSpring(hero, object, dst_index); break;
        case MP2::OBJ_MAGICWELL: 	AIToMagicWell(hero, object, dst_index); break;

        // increase skill
	case MP2::OBJ_XANADU:		AIToXanadu(hero, object, dst_index); break;

        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_FREEMANFOUNDRY:	AIToUpgradeArmyObject(hero, object, dst_index); break;

	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:	AIToPoorMoraleObject(hero, object, dst_index); break;

	case MP2::OBJ_PYRAMID:		AIToPoorLuckObject(hero, object, dst_index); break;
	case MP2::OBJ_DAEMONCAVE:	AIToDaemonCave(hero, object, dst_index); break;

        case MP2::OBJ_TREEKNOWLEDGE:	AIToTreeKnowledge(hero, object, dst_index); break;

        // accept army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
	case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:      AIToDwellingJoinMonster(hero, object, dst_index); break;

        // recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_DESERTTENT:
        // loyalty ver
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:     AIToDwellingRecruitMonster(hero, object, dst_index); break;

        // recruit army (battle)
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:	AIToDwellingRecruitMonster(hero, object, dst_index); break;

	// recruit genie
	case MP2::OBJ_ANCIENTLAMP:	AIToDwellingRecruitMonster(hero, object, dst_index); break;

	case MP2::OBJ_STABLES:		AIToStables(hero, object, dst_index); break;
	case MP2::OBJ_ARENA:		AIToPrimarySkillObject(hero, object, dst_index); break;

        case MP2::OBJ_BARRIER:          AIToBarrier(hero, object, dst_index); break;
        case MP2::OBJ_TRAVELLERTENT:    AIToTravellersTent(hero, object, dst_index); break;


    	default: break;
    }
}

void AIToHeroes(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Settings & conf = Settings::Get();
    Heroes *other_hero = world.GetHeroes(dst_index);
    if(! other_hero) return;

    if(hero.GetColor() == other_hero->GetColor() ||
        (conf.ExtUnionsAllowHeroesMeetings() && conf.IsUnions(hero.GetColor(), other_hero->GetColor())))
    {
        DEBUG(DBG_AI, DBG_INFO, "AIToHeroes: " << hero.GetName() << " meeting " << other_hero->GetName());
        AIMeeting(hero, *other_hero);
    }
    else
    if(! hero.AllowBattle(true))
    {
        DEBUG(DBG_AI, DBG_INFO, "AIToHeroes: " << hero.GetName() << " currently can not allow battle");
    }
    else
    if(! other_hero->AllowBattle(false))
    {
        DEBUG(DBG_AI, DBG_INFO, "AIToHeroes: " << other_hero->GetName() << " currently can not allow battle");
    }
    else
    {
        if(other_hero->GetUnderObject() == MP2::OBJ_CASTLE)
        {
            Castle *castle = world.GetCastle(dst_index);
            if(castle && &hero == castle->GetHeroes().GuardFirst())
            {
                AIToCastle(hero, MP2::OBJ_CASTLE, dst_index);
                return;
            }
        }

	bool disable_auto_move = hero.isShipMaster() || other_hero->isShipMaster() || other_hero->inCastle();
        DEBUG(DBG_AI , DBG_INFO, "AIToHeroes: " << hero.GetName() << " attack enemy hero " << other_hero->GetName());

            // new battle2
            Battle2::Result res = Battle2::Loader(hero.GetArmy(), other_hero->GetArmy(), dst_index);

            // loss defender
            if(!res.DefenderWins())
               AIBattleLose(*other_hero, res, false, hero.GetColor());

            // loss attacker
            if(!res.AttackerWins())
                AIBattleLose(hero, res, true, other_hero->GetColor());

            // wins attacker
            if(res.AttackerWins())
            {
                hero.IncreaseExperience(res.GetExperienceAttacker());

        	// auto move hero
        	if(conf.ExtHeroAutoMove2BattleTarget() && !disable_auto_move)
        	{
            	    MoveHero2Dest(hero, dst_index, hero.GetUnderObject(), world.GetTiles(dst_index).GetObject());
        	}
            }
            else
            // wins defender
            if(res.DefenderWins())
            {
                other_hero->IncreaseExperience(res.GetExperienceDefender());
            }
    }
}

void AIToCastle(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Settings & conf = Settings::Get();
    Castle *castle = world.GetCastle(dst_index);

    if(! castle) return;

    if(hero.GetColor() == castle->GetColor() ||
	(conf.ExtUnionsAllowCastleVisiting() && conf.IsUnions(hero.GetColor(), castle->GetColor())))
    {
        DEBUG(DBG_AI , DBG_INFO, "AIToCastle: " << hero.GetName() << " goto castle " << castle->GetName());
	castle->GetMageGuild().EducateHero(hero);
	hero.SetVisited(dst_index);
    }
    else
    {
        DEBUG(DBG_AI , DBG_INFO, "AIToCastle: " << hero.GetName() << " attack enemy castle " << castle->GetName());

        CastleHeroes heroes = castle->GetHeroes();

        // first attack to guest hero
        if(heroes.FullHouse())
        {
            AIToHeroes(hero, MP2::OBJ_HEROES, dst_index);
            return;
        }

        Army::army_t & army = castle->GetActualArmy();
	bool allow_enter = false;

	if(army.isValid())
	{
	    Heroes* defender = heroes.GuardFirst();
	    castle->ActionPreBattle();

            // new battle2
            Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

	    castle->ActionAfterBattle(res.AttackerWins());

            // loss defender
            if(!res.DefenderWins() && defender)
               AIBattleLose(*defender, res, false, hero.GetColor());

            // loss attacker
            if(!res.AttackerWins())
                AIBattleLose(hero, res, true, castle->GetColor());

            // wins attacker
            if(res.AttackerWins())
            {
                world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
                world.GetKingdom(hero.GetColor()).AddCastle(castle);
                world.CaptureObject(dst_index, hero.GetColor());
    		castle->Scoute();

                hero.IncreaseExperience(res.GetExperienceAttacker());
		allow_enter = true;
            }
            else
            // wins defender
            if(res.DefenderWins() && defender)
            {
                defender->IncreaseExperience(res.GetExperienceDefender());
            }
	}
	else
	{
    	    world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
	    world.GetKingdom(hero.GetColor()).AddCastle(castle);
    	    world.CaptureObject(dst_index, hero.GetColor());
    	    castle->Scoute();
	    allow_enter = true;
	}

        // auto move hero to castle
        if(conf.ExtHeroAutoMove2BattleTarget() && allow_enter)
        {
            MoveHero2Dest(hero, dst_index, hero.GetUnderObject(), MP2::OBJ_CASTLE);
            AIToCastle(hero, MP2::OBJ_CASTLE, dst_index);
        }
    }
}

void AIToMonster(Heroes &hero, const u8 obj, const s32 dst_index)
{
    bool destroy = false;
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Army::Troop troop(tile);
    //const Settings & conf = Settings::Get();

    u32 join = 0;
    Resource::funds_t cost;

    u8 reason = Army::GetJoinSolution(hero, tile, join, cost.gold);

    // free join
    if(1 == reason)
    {
    	// join if ranged or flying monsters present
    	if(hero.GetArmy().HasMonster(troop()) || troop.isArchers() || troop.isFly())
    	{
    	    DEBUG(DBG_AI , DBG_INFO, "AIToMonster: " << hero.GetName() << " join monster " << troop.GetName());
    	    hero.GetArmy().JoinTroop(troop);
	    destroy = true;
    	}
	else
	    reason = 0;
    }
    else
    // join with cost
    if(2 == reason)
    {
    	// join if archers or fly or present
    	if(hero.GetArmy().HasMonster(troop()) || troop.isArchers() || troop.isFly())
    	{
    	    DEBUG(DBG_AI , DBG_INFO, "AIToMonster: " << hero.GetName() << " join monster " << troop.GetName() << ", count: " << join << ", cost: " << cost.gold);
    	    hero.GetArmy().JoinTroop(troop(), join);
	    world.GetKingdom(hero.GetColor()).OddFundsResource(cost);
	    destroy = true;
    	}
	else
	    reason = 0;
    }

    bool allow_move = false;

    // fight
    if(0 == reason)
    {
	DEBUG(DBG_AI , DBG_INFO, "AIToMonster: " << hero.GetName() << " attack monster " << troop.GetName());

	Army::army_t army;
	army.JoinTroop(troop);
	army.ArrangeForBattle();

    	Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    	if(res.AttackerWins())
    	{
    	    hero.IncreaseExperience(res.GetExperienceAttacker());
    	    destroy = true;
	    allow_move = true;
    	}
    	else
    	{
    	    AIBattleLose(hero, res, true);
    	    if(Settings::Get().ExtSaveMonsterBattle())
    	    {
            	tile.SetCountMonster(army.GetCountMonsters(troop()));
            	if(2 == tile.GetQuantity4()) tile.SetQuantity4(1);
    	    }
    	}
    }
    // unknown
    else
	destroy = true;


    if(destroy)
    {
        Maps::TilesAddon *addon = tile.FindObject(MP2::OBJ_MONSTER);
        if(addon)
        {
            const u32 uniq = addon->uniq;
            tile.Remove(uniq);
            tile.SetObject(MP2::OBJ_ZERO);
    	    tile.ResetQuantity();

            // remove shadow from left cell
            if(Maps::isValidDirection(dst_index, Direction::LEFT))
                world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT)).Remove(uniq);
        }

        // auto move hero
	// disable: https://sourceforge.net/tracker/index.php?func=detail&aid=3155230&group_id=96859&atid=616180
	/*
        if(conf.ExtHeroAutoMove2BattleTarget() && allow_move)
        {
            MoveHero2Dest(hero, dst_index, hero.GetUnderObject(), tile.GetObject());
        }
	*/
    }
}

void AIToPickupResource(Heroes &hero, const u8 obj, const s32 dst_index)
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
    DEBUG(DBG_AI , DBG_INFO, "AIToPickupResource: " << hero.GetName() << " pickup small resource");
}

void AIToTreasureChest(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    resource.gold = tile.GetQuantity2() * 100;

    if(Maps::Ground::WATER == tile.GetGround())
    {
	if(tile.GetQuantity1())
	{
	    const Artifact art(Artifact::FromInt(tile.GetQuantity1()));
	    if(!hero.PickupArtifact(art()))
		    resource.gold = 1500;	// it is from FAQ
	}
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    }
    else
    {
	if(tile.GetQuantity1())
	{
	    const Artifact art(Artifact::FromInt(tile.GetQuantity1()));
	
	    if(!hero.PickupArtifact(art()))
	    {
		resource.gold = 1000;	// it is from FAQ
		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    }
	}
	else
	if(resource.gold >= 1000)
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

    DEBUG(DBG_AI , DBG_INFO, "AIToTreasureChest: " << hero.GetName());
}

void AIToResource(Heroes &hero, const u8 obj, const s32 dst_index)
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

    DEBUG(DBG_AI , DBG_INFO, "AIToResource: " << hero.GetName());
}

void AIToSkeleton(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact art(Artifact::FromInt(tile.GetQuantity1()));

	if(hero.PickupArtifact(art()))
	{
	    tile.SetQuantity1(0);
	    tile.SetQuantity2(0);
	}
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToSkeleton: " << hero.GetName());
}

void AIToWagon(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact art(Artifact::FromInt(tile.GetQuantity1()));
	if(hero.PickupArtifact(art()))
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

    DEBUG(DBG_AI , DBG_INFO, "AIToWagon: " << hero.GetName());
}

void AIToCaptureObject(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::resource_t res = Resource::UNKNOWN;

    switch(obj)
    {
	case MP2::OBJ_ALCHEMYLAB:	res = Resource::MERCURY; break;
	case MP2::OBJ_SAWMILL:		res = Resource::WOOD; break;
        case MP2::OBJ_MINES:		res = static_cast<Resource::resource_t>(tile.GetMinesType()); break;
        default: break;
    }

    // capture object
    if(hero.GetColor() != world.ColorCapturedObject(dst_index))
    {
	bool capture = true;

	if(tile.CaptureObjectIsProtection(hero.GetColor()))
	{
	    const Army::Troop troop(tile);
	    Army::army_t army;
	    army.JoinTroop(troop);
	    army.SetColor(world.ColorCapturedObject(dst_index));

	    Battle2::Result result = Battle2::Loader(hero.GetArmy(), army, dst_index);

	    if(result.AttackerWins())
	    {
		hero.IncreaseExperience(result.GetExperienceAttacker());
		tile.ResetQuantity();
	    }
	    else
	    {
		capture = false;
	        AIBattleLose(hero, result, true);
		if(Settings::Get().ExtSaveMonsterBattle())
		    tile.SetCountMonster(army.GetCountMonsters(troop()));
	    }
	}

	if(capture) world.CaptureObject(dst_index, hero.GetColor());
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToCaptureObject: " << hero.GetName() << " captured: " << MP2::StringObject(obj));
}

void AIToFlotSam(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;

    resource.gold += 100 * tile.GetQuantity1();
    resource.wood += tile.GetQuantity2();

    if(resource.GetValidItems()) world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    DEBUG(DBG_AI , DBG_INFO, "AIToFlotSam: " << hero.GetName());
}

void AIToSign(Heroes &hero, const u8 obj, const s32 dst_index)
{
    hero.SetVisited(dst_index, Visit::LOCAL);
    DEBUG(DBG_AI , DBG_INFO, "AIToSign: " << hero.GetName());
}

void AIToObservationTower(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::ClearFog(dst_index, Game::GetViewDistance(Game::VIEW_OBSERVATION_TOWER), hero.GetColor());
    hero.SetVisited(dst_index, Visit::GLOBAL);
    DEBUG(DBG_AI , DBG_INFO, "AIToObservationTower: " << hero.GetName());
}

void AIToMagellanMaps(Heroes &hero, const u8 obj, const s32 dst_index)
{
    if(1000 <= world.GetKingdom(hero.GetColor()).GetFundsGold())
    {
	hero.SetVisited(dst_index, Visit::GLOBAL);
	world.ActionForMagellanMaps(hero.GetColor());
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToMagellanMaps: " << hero.GetName());
}

void AIToTeleports(Heroes &hero, const s32 index_from)
{
    u16 index_to = world.NextTeleport(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	DEBUG(DBG_AI , DBG_WARN, "AIToTeleports: action unsuccessfully...");
	return;
    }

    MoveHero2Dest(hero, index_to, MP2::OBJ_STONELIGHTS, MP2::OBJ_STONELIGHTS);

    hero.GetPath().Reset();
    hero.ActionNewPosition();

    DEBUG(DBG_AI , DBG_INFO, "AIToStoneLights: " << hero.GetName());
}

void AIToWhirlpools(Heroes &hero, const s32 index_from)
{
    const u16 index_to = world.NextWhirlpool(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	DEBUG(DBG_AI , DBG_WARN, "AIToWhirlpools: action unsuccessfully...");
	return;
    }

    MoveHero2Dest(hero, index_to, MP2::OBJ_WHIRLPOOL, MP2::OBJ_WHIRLPOOL);

    Army::Troop & troops = hero.GetArmy().GetWeakestTroop();

    if(Rand::Get(1) && 1 < troops.GetCount())
	troops.SetCount(Monster::GetCountFromHitPoints(troops(), troops.GetHitPoints() - troops.GetHitPoints() * Game::GetWhirlpoolPercent() / 100));

    hero.GetPath().Reset();
    hero.ActionNewPosition();

    DEBUG(DBG_AI , DBG_INFO, "AIToWhirlpools: " << hero.GetName());
}

void AIToPrimarySkillObject(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    Skill::Primary::skill_t skill = Skill::Primary::UNKNOWN;

    switch(obj)
    {
        case MP2::OBJ_FORT:		skill = Skill::Primary::DEFENSE; break;
        case MP2::OBJ_MERCENARYCAMP:	skill = Skill::Primary::ATTACK; break;
        case MP2::OBJ_DOCTORHUT:	skill = Skill::Primary::KNOWLEDGE; break;
        case MP2::OBJ_STANDINGSTONES:	skill = Skill::Primary::POWER; break;
        case MP2::OBJ_ARENA:		skill = AISelectPrimarySkill(hero); break;

    	default: break;
    }

    if(!hero.isVisited(tile))
    {
	// increase skill
	hero.IncreasePrimarySkill(skill);
	hero.SetVisited(dst_index, (MP2::OBJ_ARENA == obj ? Visit::GLOBAL : Visit::LOCAL));

        // fix double action tile
	hero.SetVisitedWideTile(dst_index, obj);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToPrimarySkillObject: " << hero.GetName());
}

void AIToExperienceObject(Heroes &hero, const u8 obj, const s32 dst_index)
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

    DEBUG(DBG_AI , DBG_INFO, "AIToExperienceObject: " << hero.GetName());
}

void AIToWitchsHut(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Skill::Secondary::skill_t skill = Skill::Secondary::Skill(world.GetTiles(dst_index).GetQuantity1());

    // check full
    if(!hero.HasMaxSecondarySkill() && !hero.HasSecondarySkill(skill))
	hero.LearnBasicSkill(skill);

    hero.SetVisited(dst_index);
    DEBUG(DBG_AI , DBG_INFO, "AIToWitchsHut: " << hero.GetName());
}

void AIToShrine(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Spell::spell_t spell = Spell::FromInt(world.GetTiles(dst_index).GetQuantity1());
    const u8 spell_level = Spell::Level(spell);

    // check spell book
    if(hero.HasArtifact(Artifact::MAGIC_BOOK) &&
      !hero.HaveSpell(spell) &&
    // check valid level spell and wisdom skill
      !(3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM)))
    {
	hero.AppendSpellToBook(spell);
	hero.SetVisited(dst_index);
    }
    DEBUG(DBG_AI , DBG_INFO, "AIToShrine: " << hero.GetName());
}

void AIToGoodLuckObject(Heroes &hero, const u8 obj, const s32 dst_index)
{
    // check already visited
    if(!hero.isVisited(obj)) hero.SetVisited(dst_index);
    DEBUG(DBG_AI , DBG_INFO, "AIToGoodLuckObject: " << hero.GetName());
}

void AIToGoodMoraleObject(Heroes &hero, const u8 obj, const s32 dst_index)
{
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
	hero.SetVisitedWideTile(dst_index, obj);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToGoodMoraleObject: " << hero.GetName());
}

void AIToMagicWell(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(hero.GetSpellPoints() != max &&
	// check already visited
	!hero.isVisited(MP2::OBJ_MAGICWELL))
    {
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToMagicWell: " << hero.GetName());
}

void AIToArtesianSpring(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(!hero.isVisited(MP2::OBJ_ARTESIANSPRING) &&
       hero.GetSpellPoints() < max * 2)
    {
	hero.SetSpellPoints(max * 2);

        // fix double action tile
	hero.SetVisitedWideTile(dst_index, obj);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToArtesianSpring: " << hero.GetName());
}

void AIToXanadu(Heroes &hero, const u8 obj, const s32 dst_index)
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
	hero.IncreasePrimarySkill(Skill::Primary::DEFENSE);
	hero.IncreasePrimarySkill(Skill::Primary::KNOWLEDGE);
	hero.IncreasePrimarySkill(Skill::Primary::POWER);
	hero.SetVisited(dst_index);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToXanadu: " << hero.GetName());
}

void AIToEvent(Heroes &hero, const u8 obj, const s32 dst_index)
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

    hero.SaveUnderObject(MP2::OBJ_ZERO);

    DEBUG(DBG_AI , DBG_INFO, "AIToEvent: " << hero.GetName());
}

void AIToUpgradeArmyObject(Heroes &hero, const u8 obj, const s32 dst_index)
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

    DEBUG(DBG_AI , DBG_INFO, "AIToUpgradeArmyObject: " << hero.GetName());
}

void AIToPoorMoraleObject(Heroes &hero, const u8 obj, const s32 dst_index)
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

    		// new battle2
    		Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    		if(res.AttackerWins())
		{
        	    hero.IncreaseExperience(res.GetExperienceAttacker());
	    	    complete = true;
	    	    const Artifact art(Artifact::FromInt(tile.GetQuantity1()));
	    	    Resource::funds_t resource;
	    	    resource.gold = tile.GetQuantity2() * 100;
	    	    hero.PickupArtifact(art());
	    	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
		}
		else
		{
	    	    AIBattleLose(hero, res, true);
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
            	    case 50: resource.gold = 2000; art = Artifact::FromInt(tile.GetQuantity1()); break;
            	    default: DEBUG(DBG_AI , DBG_WARN, "ActionToPoorMoraleObject: unknown variant for ShipWreck, index: " << dst_index); break;
                }

    		    // new battle2
    		    Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    		    if(res.AttackerWins())
		    {
        		hero.IncreaseExperience(res.GetExperienceAttacker());
	    		complete = true;
			hero.PickupArtifact(art);
	    		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
		    }
		    else
		    {
	    		AIBattleLose(hero, res, true);
		    }
	    }
	    break;

        case MP2::OBJ_DERELICTSHIP:
	if(battle)
	{
	    Army::army_t army;
	    army.FromGuardian(tile);

    		// new battle2
    		Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    		if(res.AttackerWins())
		{
        	    hero.IncreaseExperience(res.GetExperienceAttacker());
	    	    complete = true;
	    	    Resource::funds_t resource;
	    	    resource.gold = tile.GetQuantity2() * 100;
	    	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
		}
		else
		{
	    	    AIBattleLose(hero, res, true);
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

    DEBUG(DBG_AI , DBG_INFO, "AIToPoorMoraleObject: " << hero.GetName());
}

void AIToPoorLuckObject(Heroes &hero, const u8 obj, const s32 dst_index)
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

    		// new battle2
    		Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    		if(res.AttackerWins())
		{
        	    hero.IncreaseExperience(res.GetExperienceAttacker());
		    complete = true;
		    const Spell::spell_t spell(static_cast<Spell::spell_t>(tile.GetQuantity1()));
		    // check magick book
		    if(hero.HasArtifact(Artifact::MAGIC_BOOK) &&
		    // check skill level for wisdom
			Skill::Level::EXPERT == hero.GetLevelSkill(Skill::Secondary::WISDOM))
		    {
			hero.AppendSpellToBook(spell);
		    }
		}
		else
		{
		    AIBattleLose(hero, res, true);
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

    DEBUG(DBG_AI , DBG_INFO, "AIToPoorLuckObject: " << hero.GetName());
}

void AIToObelisk(Heroes &hero, const u8 obj, const s32 dst_index)
{
    if(!hero.isVisited(obj, Visit::GLOBAL))
    {
        hero.SetVisited(dst_index, Visit::GLOBAL);
        Kingdom & kingdom = world.GetKingdom(hero.GetColor());
        kingdom.PuzzleMaps().Update(kingdom.CountVisitedObjects(MP2::OBJ_OBELISK), world.CountObeliskOnMaps());
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToObelisk: " << hero.GetName());
}

void AIToTreeKnowledge(Heroes &hero, const u8 obj, const s32 dst_index)
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
	    hero.IncreaseExperience(hero.GetExperienceFromLevel(hero.GetLevel()) - hero.GetExperience());
	}
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToTreeKnowledge: " << hero.GetName());
}


void AIToDaemonCave(Heroes &hero, const u8 obj, const s32 dst_index)
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
		const Artifact::artifact_t art = Artifact::FromInt(tile.GetQuantity1());
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

    DEBUG(DBG_AI , DBG_INFO, "AIToDaemonCave: " << hero.GetName());
}

void AIToDwellingJoinMonster(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u32 count = tile.GetCountMonster();

    if(count && hero.GetArmy().JoinTroop(Monster(Monster::FromObject(obj)), count)) tile.SetCountMonster(0);

    DEBUG(DBG_AI , DBG_INFO, "AIToDwellingJoinMonster: " << hero.GetName());
}

void AIToDwellingRecruitMonster(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u32 count = tile.GetCountMonster();

    if(count)
    {
        Kingdom & kingdom = world.GetKingdom(hero.GetColor());
        const Monster monster(Monster::FromObject(obj));
	const payment_t paymentCosts(PaymentConditions::BuyMonster(monster()) * count);
	const Resource::funds_t & kingdomResource = kingdom.GetFundsResource();

        if(paymentCosts <= kingdomResource && hero.GetArmy().JoinTroop(monster, count))
        {
    	    tile.SetCountMonster(0);
	    kingdom.OddFundsResource(paymentCosts);

	    // remove ancient lamp sprite
	    if(MP2::OBJ_ANCIENTLAMP == obj)
	    {
	        tile.RemoveObjectSprite();
	        tile.SetObject(MP2::OBJ_ZERO);
	    }
	}
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToDwellingJoinMonster: " << hero.GetName());
}

void AIToStables(Heroes &hero, const u8 obj, const s32 dst_index)
{
    // check already visited
    if(!hero.isVisited(obj))
    {
        hero.SetVisited(dst_index);
        hero.IncreaseMovePoints(400);
    }

    if(hero.GetArmy().HasMonster(Monster::CAVALRY)) hero.GetArmy().UpgradeMonsters(Monster::CAVALRY);
                                                                
    DEBUG(DBG_AI , DBG_INFO, "AIToStables: " << hero.GetName());
}

void AIToAbandoneMine(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    Army::army_t army;
    army.FromGuardian(tile);

    	// new battle2
    	Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    	if(res.AttackerWins())
	{
            hero.IncreaseExperience(res.GetExperienceAttacker());
            tile.SetQuantity1(0);
            tile.UpdateAbandoneMineSprite();
            world.CaptureObject(dst_index, hero.GetColor());
            hero.SaveUnderObject(MP2::OBJ_MINES);
        }
	else
	{
	    AIBattleLose(hero, res, true);
	}

    DEBUG(DBG_AI , DBG_INFO, "AIToAbandoneMine: " << hero.GetName());
}

void AIToBarrier(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Kingdom & kingdom = world.GetKingdom(hero.GetColor());

    if(kingdom.IsVisitTravelersTent(tile.GetQuantity1()))
    {
        tile.RemoveObjectSprite();
        tile.SetObject(MP2::OBJ_ZERO);
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToBarrier: " << hero.GetName());
}

void AIToTravellersTent(Heroes &hero, const u8 obj, const s32 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    Kingdom & kingdom = world.GetKingdom(hero.GetColor());

    kingdom.SetVisitTravelersTent(tile.GetQuantity1());

    DEBUG(DBG_AI , DBG_INFO, "AIToTravellersTent: " << hero.GetName());
}

void AIToShipwreckSurvivor(Heroes &hero, const u8 obj, const s32 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Artifact art(Artifact::FromInt(tile.GetQuantity1()));

    if(!hero.PickupArtifact(art()))
    {
        Resource::funds_t prize(Resource::GOLD, 1500);
	world.GetKingdom(hero.GetColor()).OddFundsResource(prize);
    }

    tile.RemoveObjectSprite();
    tile.SetQuantity1(0);
    tile.SetObject(MP2::OBJ_ZERO);

    DEBUG(DBG_AI , DBG_INFO, "AIToShipwreckSurvivor: " << hero.GetName());
}

void AIToArtifact(Heroes &hero, const u8 obj, const s32 dst_index)
{
    if(hero.IsFullBagArtifacts()) return;

    Maps::Tiles & tile = world.GetTiles(dst_index);
    Artifact art(Artifact::FromInt(tile.GetQuantity1()));

    // update scroll artifact
    if(art.GetID() == Artifact::SPELL_SCROLL)
	art.SetExt(tile.GetQuantity3());

    bool conditions = false;

    switch(tile.GetQuantity2())
    {
	// 1,2,3 - 2000g, 2500g+3res,3000g+5res
	case 1:
	case 2:
	case 3:
	{
	    Resource::funds_t payment;

	    if(1 == tile.GetQuantity2())
		payment += Resource::funds_t(Resource::GOLD, 2000);
	    else
	    if(2 == tile.GetQuantity2())
	    {
		payment += Resource::funds_t(Resource::GOLD, 2500);
		payment += Resource::funds_t(tile.GetQuantity4(), 3);
	    }
	    else
	    {
		payment += Resource::funds_t(Resource::GOLD, 3000);
		payment += Resource::funds_t(tile.GetQuantity4(), 5);
	    }

	    if(world.GetKingdom(hero.GetColor()).AllowPayment(payment))
	    {
		conditions = true;
		world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
	    }
	    break;
	}

	// 4,5 - need have skill wisard or leadership,
	case 4:
	case 5:
	{
	    conditions = hero.HasSecondarySkill(4 == tile.GetQuantity2() ? Skill::Secondary::WISDOM : Skill::Secondary::LEADERSHIP);
	    break;
	}

	// 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	{
	    Army::army_t army;
	    army.FromGuardian(tile);

	    // new battle2
	    Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
    	    if(res.AttackerWins())
    	    {
		hero.IncreaseExperience(res.GetExperienceAttacker());
		conditions = true;
	    }
	    else
	    {
		AIBattleLose(hero, res, true);
	    }
	    break;
	}

    	default:
	    conditions = true;
	    break;
    }

    if(conditions && hero.PickupArtifact(art))
    {
	tile.RemoveObjectSprite();
	tile.SetObject(MP2::OBJ_ZERO);
	tile.ResetQuantity();
    }

    DEBUG(DBG_AI , DBG_INFO, "AIToArtifact: " << hero.GetName());
}

void AIToBoat(Heroes &hero, const u8 obj, const s32 dst_index)
{
    if(hero.isShipMaster()) return;

    const s32 from_index = hero.GetIndex();

    // disabled nearest coasts (on week MP2::isWeekLife)
    std::vector<s32> coasts;
    Maps::ScanDistanceObject(from_index, MP2::OBJ_COAST, 4, coasts);
    coasts.push_back(from_index);
    for(std::vector<s32>::const_iterator
	it = coasts.begin(); it != coasts.end(); ++it) hero.SetVisited(*it);

    hero.ResetMovePoints();
    MoveHero2Dest(hero, dst_index, hero.GetUnderObject(), MP2::OBJ_ZERO);
    hero.SetShipMaster(true);

    AIHeroes::Get(hero).ClearTasks();

    hero.GetPath().Reset();
    hero.ActionNewPosition();

    DEBUG(DBG_AI, DBG_INFO, "AIToBoat: " << hero.GetName());
}

void AIToCoast(Heroes &hero, const u8 obj, const s32 dst_index)
{
    if(! hero.isShipMaster()) return;

    hero.ResetMovePoints();
    MoveHero2Dest(hero, dst_index, MP2::OBJ_BOAT, MP2::OBJ_ZERO);
    hero.SetShipMaster(false);

    AIHeroes::Get(hero).ClearTasks();

    hero.GetPath().Reset();
    hero.ActionNewPosition();

    DEBUG(DBG_AI, DBG_INFO, "AIToCoast: " << hero.GetName());
}

void AIMeeting(Heroes & hero1, Heroes & hero2)
{
    if(Settings::Get().ExtEyeEagleAsScholar())
        Heroes::ScholarAction(hero1, hero2);

    if(hero1.Modes(Heroes::HUNTER))
	hero1.GetArmy().JoinStrongestFromArmy(hero2.GetArmy());
    else
    if(hero2.Modes(Heroes::HUNTER))
	hero2.GetArmy().JoinStrongestFromArmy(hero1.GetArmy());
    else
    if(hero1.Modes(Heroes::SCOUTER))
	hero1.GetArmy().KeepOnlyWeakestTroops(hero2.GetArmy());
    else
    if(hero2.Modes(Heroes::SCOUTER))
	hero2.GetArmy().KeepOnlyWeakestTroops(hero1.GetArmy());

    // artifacts change
}





void AI::HeroesDumpInfo(const Heroes & hero)
{
    AIHero & ai_hero = AIHeroes::Get(hero);
    Queue & task = ai_hero.sheduled_visit;

    VERBOSE("ai primary target: " << ai_hero.primary_target);
    VERBOSN("ai sheduled visit: ");
    Queue::const_iterator it1 = task.begin();
    Queue::const_iterator it2 = task.end();
    for(; it1 != it2; ++it1) VERBOSN(*it1 << "(" << MP2::StringObject(world.GetTiles(*it1).GetObject()) << "), ");
    VERBOSE("");
}

void AI::HeroesLevelUp(Heroes & hero)
{
    if(4 < hero.GetLevel() && !hero.Modes(Heroes::HUNTER))
	hero.SetModes(Heroes::HUNTER);

    if(9 < hero.GetLevel() && hero.Modes(Heroes::SCOUTER))
	hero.ResetModes(Heroes::SCOUTER);
}

bool AIHeroesValidObject(const Heroes & hero, s32 index)
{
    Maps::Tiles & tile = world.GetTiles(index);
    const u8 obj = tile.GetObject();
    const Army::army_t & army = hero.GetArmy();
    const Kingdom & kingdom = world.GetKingdom(hero.GetColor());
    const Settings & conf = Settings::Get();

    // check other
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
	    if(hero.isShipMaster()) return true;
            break;

	// capture objects
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_MINES:
	case MP2::OBJ_ALCHEMYLAB:
	    if(! conf.IsUnions(hero.GetColor(), world.ColorCapturedObject(index)))
	    {
		if(tile.CaptureObjectIsProtection(hero.GetColor()))
		{
		    Army::army_t enemy;
		    enemy.FromGuardian(tile);
		    return !enemy.isValid() || army.StrongerEnemyArmy(enemy);
		}
		else
		return true;
	    }
	    break;

	// pickup object
	case MP2::OBJ_WAGON:
	case MP2::OBJ_WATERWHEEL:
	case MP2::OBJ_WINDMILL:
	case MP2::OBJ_LEANTO:
	case MP2::OBJ_MAGICGARDEN:
	case MP2::OBJ_SKELETON:
	    if(tile.ValidQuantity()) return true;
	    break;

	// pickup resource
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_CAMPFIRE:
	case MP2::OBJ_TREASURECHEST:
	    return true;

	case MP2::OBJ_ARTIFACT:
	{
	    if(hero.IsFullBagArtifacts()) return false;

	    // 1,2,3 - 2000g, 2500g+3res, 3000g+5res
	    if(1 <= tile.GetQuantity2() && 3 >= tile.GetQuantity2())
	    {
		Resource::funds_t payment;
		if(1 == tile.GetQuantity2())
		    payment += Resource::funds_t(Resource::GOLD, 2000);
		else
		if(2 == tile.GetQuantity2())
		{
		    payment += Resource::funds_t(Resource::GOLD, 2500);
		    payment += Resource::funds_t(tile.GetQuantity4(), 3);
		}
		else
		{
		    payment += Resource::funds_t(Resource::GOLD, 3000);
		    payment += Resource::funds_t(tile.GetQuantity4(), 5);
		}
		return kingdom.AllowPayment(payment);
	    }
	    else
	    // 4,5 - need have skill wisard or leadership,
	    if(3 < tile.GetQuantity2() && 6 > tile.GetQuantity2())
	    {
		return hero.HasSecondarySkill(4 == tile.GetQuantity2() ? Skill::Secondary::WISDOM : Skill::Secondary::LEADERSHIP);
	    }
	    else
	    // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
	    if(5 < tile.GetQuantity2() && 14 > tile.GetQuantity2())
	    {
		Army::army_t enemy;
		enemy.FromGuardian(tile);
		return !enemy.isValid() || army.StrongerEnemyArmy(enemy);
	    }
	    else
	    // other
	    return true;
	}
	break;

	// increase view
	case MP2::OBJ_OBSERVATIONTOWER:
	// obelisk
	case MP2::OBJ_OBELISK:
	    if(! hero.isVisited(tile, Visit::GLOBAL)) return true;
	    break;

        case MP2::OBJ_BARRIER:
	    if(kingdom.IsVisitTravelersTent(tile.GetQuantity1())) return true;
	    break;

        case MP2::OBJ_TRAVELLERTENT:
	    if(!kingdom.IsVisitTravelersTent(tile.GetQuantity1())) return true;
	    break;

	// new spell
        case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	    if( // check spell book
		hero.HasArtifact(Artifact::MAGIC_BOOK) &&
		!hero.HaveSpell(Spell::FromInt(tile.GetQuantity1())) &&
                // check valid level spell and wisdom skill
                !(3 == Spell::Level(Spell::FromInt(tile.GetQuantity1())) &&
                Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM))) return true;
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
	    if(! hero.isVisited(tile)) return true;
	    break;

	case MP2::OBJ_TREEKNOWLEDGE:
	    if(! hero.isVisited(tile))
	    {
		Resource::funds_t payment;
		switch(tile.GetQuantity2())
		{
		    case 10:	payment.gems = 10; break;
		    case 20:	payment.gold = 2000; break;
		    default:	break;
		}

    		if(!payment.GetValidItems() || kingdom.AllowPayment(payment))
		    return true;
	    }
	    break;

    	// good luck
        case MP2::OBJ_FOUNTAIN:
    	case MP2::OBJ_FAERIERING:
    	case MP2::OBJ_IDOL:
	    if(! hero.isVisited(obj) &&
		Luck::IRISH > hero.GetLuck()) return true;
	    break;

	// good morale
	case MP2::OBJ_OASIS:
    	case MP2::OBJ_TEMPLE:
    	case MP2::OBJ_WATERINGHOLE:
	    if(! hero.isVisited(obj) &&
		Morale::BLOOD > hero.GetMorale()) return true;
	    break;

	case MP2::OBJ_MAGICWELL:
	    if(! hero.isVisited(tile) &&
		hero.GetMaxSpellPoints() != hero.GetSpellPoints()) return true;
	    break;

	case MP2::OBJ_ARTESIANSPRING:
	    if(! hero.isVisited(tile) &&
		2 * hero.GetMaxSpellPoints() > hero.GetSpellPoints()) return true;
	    break;

	case MP2::OBJ_XANADU:
	{
	    const u8 level1 = hero.GetLevelSkill(Skill::Secondary::DIPLOMACY);
	    const u8 level2 = hero.GetLevel();

	    if(!hero.isVisited(tile) &&
		((level1 == Skill::Level::BASIC && 7 < level2) ||
		(level1 == Skill::Level::ADVANCED && 5 < level2) ||
		(level1 == Skill::Level::EXPERT && 3 < level2) || (9 < level2))) return true;
	    break;
	}

        // accept army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
	case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
        {
    	    Monster mons = Monster::FromObject(obj);
	    if(tile.GetCountMonster() &&
		(army.HasMonster(mons) ||
		(army.GetCount() < army.Size() && (mons.isArchers() || mons.isFly())))) return true;
	    break;
	}

        // recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_DESERTTENT:
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:
	{
	    const u32 count = tile.GetCountMonster();
    	    const Monster monster(Monster::FromObject(obj));
	    const payment_t paymentCosts(PaymentConditions::BuyMonster(monster()) * count);
	    const Resource::funds_t & kingdomResource = kingdom.GetFundsResource();

	    if(count && paymentCosts <= kingdomResource &&
		(army.HasMonster(monster) ||
		(army.GetCount() < army.Size() && (monster.isArchers() || monster.isFly())))) return true;
	    break;
	}

        // recruit army (battle)
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:
        {
    	    const bool battle = (Color::GRAY == world.ColorCapturedObject(index));
	    const u32 count = tile.GetCountMonster();
    	    const Monster monster(Monster::FromObject(obj));
	    const payment_t paymentCosts(PaymentConditions::BuyMonster(monster()) * count);
	    const Resource::funds_t & kingdomResource = kingdom.GetFundsResource();

	    if(!battle && count && paymentCosts <= kingdomResource &&
		(army.HasMonster(monster) ||
		(army.GetCount() < army.Size()))) return true;
	    break;
        }

	// recruit genie
	case MP2::OBJ_ANCIENTLAMP:
	{
	    const u32 count = tile.GetCountMonster();
	    const payment_t paymentCosts(PaymentConditions::BuyMonster(Monster::GENIE) * count);
	    const Resource::funds_t & kingdomResource = kingdom.GetFundsResource();

	    if(count && paymentCosts <= kingdomResource &&
		(army.HasMonster(Monster::GENIE) ||
		(army.GetCount() < army.Size()))) return true;
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

	// loyalty obj
	case MP2::OBJ_STABLES:
	    if(army.HasMonster(Monster::CAVALRY) ||
		! hero.isVisited(tile)) return true;
	    break;

	case MP2::OBJ_ARENA:
	    if(! hero.isVisited(tile)) return true;
	    break;

	// poor morale obj
	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:
	    if(! hero.isVisited(tile, Visit::GLOBAL))
	    {
		if(tile.OtherObjectsIsProtection())
		{
		    Army::army_t enemy;
		    enemy.FromGuardian(tile);
		    return enemy.isValid() && army.StrongerEnemyArmy(enemy);
		}
	    }
	    break;

	//case MP2::OBJ_PYRAMID:

	case MP2::OBJ_DAEMONCAVE:
	    if(tile.GetQuantity2() && 4 != tile.GetQuantity2()) return true;
	    break;

	case MP2::OBJ_MONSTER:
	{
	    Army::army_t enemy;
	    enemy.FromGuardian(tile);
	    return !enemy.isValid() || army.StrongerEnemyArmy(enemy);
	}
	break;

	// sign
	case MP2::OBJ_SIGN:
	    if(!hero.isVisited(tile)) return true;
	    break;

	case MP2::OBJ_CASTLE:
	{
	    const Castle *castle = world.GetCastle(index);
	    if(castle)
	    {
		if(hero.GetColor() == castle->GetColor())
		    return NULL == castle->GetHeroes().Guest() && ! hero.isVisited(tile);
		else
		// FIXME: AI skip visiting alliance
		if(conf.IsUnions(hero.GetColor(), castle->GetColor())) return false;
		else
		if(army.StrongerEnemyArmy(castle->GetActualArmy())) return true;
	    }
	    break;
	}

	case MP2::OBJ_HEROES:
	{
	    const Heroes *hero2 = world.GetHeroes(index);
	    if(hero2)
	    {
		if(hero.GetColor() == hero2->GetColor()) return true;
		// FIXME: AI skip visiting alliance
		else
		if(conf.IsUnions(hero.GetColor(), hero2->GetColor())) return false;
		else
		if(hero2->AllowBattle(false) &&
		    army.StrongerEnemyArmy(hero2->GetArmy())) return true;
	    }
	    break;
	}

	case MP2::OBJ_BOAT:
	case MP2::OBJ_STONELIGHTS:
	    // check later
	    return true;

	// or add later

	default: break;
    }

    return false;
}

bool AIHeroesValidObject2(const Heroes* hero, s32 index)
{
    const Heroes & hero2 = *hero;
    return AIHeroesValidObject(hero2, index);
}

// get priority object for AI independent of distance (1 day)
bool AIHeroesPriorityObject(const Heroes & hero, s32 index)
{
    const Settings & conf = Settings::Get();
    Maps::Tiles & tile = world.GetTiles(index);

    if(MP2::OBJ_CASTLE == tile.GetObject())
    {
	const Castle *castle = world.GetCastle(index);
	if(castle)
	{
	    if(hero.GetColor() == castle->GetColor())
	    {
		// maybe need join army
		return hero.Modes(Heroes::HUNTER) &&
		    castle->GetArmy().isValid() &&
		    ! hero.isVisited(world.GetTiles(castle->GetIndex()));
		    //castle->GetArmy().StrongerEnemyArmy(hero.GetArmy());
	    }
	    else
	    if(!conf.IsUnions(hero.GetColor(), castle->GetColor()))
		return AIHeroesValidObject(hero, index);
	}
    }
    else
    if(MP2::OBJ_HEROES == tile.GetObject())
    {
	// kill enemy hero
	const Heroes *hero2 = world.GetHeroes(index);
	return hero2 &&
		!conf.IsUnions(hero.GetColor(), hero2->GetColor()) &&
		AIHeroesValidObject(hero, index);
    }

    switch(tile.GetObject())
    {
	case MP2::OBJ_MONSTER:
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_MINES:
	case MP2::OBJ_ALCHEMYLAB:

	case MP2::OBJ_ARTIFACT:
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_CAMPFIRE:
	case MP2::OBJ_TREASURECHEST:

	return AIHeroesValidObject(hero, index);

	default: break;
    }

    return false;
}

void AIHeroesAddedRescueTask(Heroes & hero)
{
    AIHero & ai_hero = AIHeroes::Get(hero);
    Queue & task = ai_hero.sheduled_visit;

    u8 scoute = hero.GetScoute();

    switch(Settings::Get().GameDifficulty())
    {
        case Difficulty::NORMAL:    scoute += 2; break;
        case Difficulty::HARD:      scoute += 3; break;
        case Difficulty::EXPERT:    scoute += 4; break;
        case Difficulty::IMPOSSIBLE:scoute += 6; break;
        default: break;
    }

    scoute += 1;
    const Point & mp = hero.GetCenter();

    // find unchartered territory
    for(u8 ii = 1; ii <= scoute; ++ii)
    {
        const s32 tx = mp.x - ii;
        const s32 ty = mp.y - ii;

        const s32 mx = tx + 2 * ii;
        const s32 my = ty + 2 * ii;

        for(s32 iy = ty; iy <= my; ++iy)
            for(s32 ix = tx; ix <= mx; ++ix)
        {
            if(ty < iy && iy < my && tx < ix && ix < mx) continue;

            if(Maps::isValidAbsPoint(ix, iy))
	    {
        	const s32 res = Maps::GetIndexFromAbsPoint(ix, iy);

        	if(world.GetTiles(res).isFog(hero.GetColor()) &&
            	    world.GetTiles(res).isPassable(&hero, true) &&
		    hero.GetPath().Calculate(res))
        	{
            	    task.push_back(res);

		    DEBUG(DBG_AI , DBG_INFO, "Heroes::AIRescueMove: " << Color::String(hero.GetColor()) <<
                	", hero: " << hero.GetName() << ", added task: " << res);

            	    return;
        	}
	    }
        }
    }

    if(MP2::OBJ_STONELIGHTS == hero.GetUnderObject() ||
	MP2::OBJ_WHIRLPOOL == hero.GetUnderObject())
    {
	AI::HeroesAction(hero, hero.GetIndex());
    }
}

void AIHeroesAddedTask(Heroes & hero)
{
    if(hero.GetPath().isValid()) return;

    AIHero & ai_hero = AIHeroes::Get(hero);
    AIKingdom & ai_kingdom = AIKingdoms::Get(hero.GetColor());

    Queue & task = ai_hero.sheduled_visit;
    IndexObjectMap & ai_objects = ai_kingdom.scans;


    DEBUG(DBG_AI , DBG_INFO, "Kingdom::AIHeroesTask: " << Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << ", task prepare");

    // load minimal distance tasks
    std::vector<IndexDistance> objs;
    objs.reserve(ai_objects.size());

    for(std::map<s32, MP2::object_t>::const_iterator
	it = ai_objects.begin(); it != ai_objects.end(); ++it)
    {
	if(hero.isShipMaster())
	{
	    if(MP2::isGroundObject((*it).second)) continue;

	    // check previous positions
	    if(MP2::OBJ_COAST == (*it).second &&
		hero.isVisited(world.GetTiles((*it).first))) continue;
	}
	else
	{
	    if(MP2::isWaterObject((*it).second)) continue;
	}

	objs.push_back(IndexDistance((*it).first,
			    Maps::GetApproximateDistance(hero.GetIndex(), (*it).first)));
    }

    std::sort(objs.begin(), objs.end(), IndexDistance::Shortest);

    for(std::vector<IndexDistance>::const_iterator
	it = objs.begin(); it != objs.end(); ++it)
    {
	if(task.size() >= HERO_MAX_SHEDULED_TASK) break;

	if(AIHeroesValidObject(hero, (*it).first) &&
	    hero.GetPath().Calculate((*it).first))
	{
	    s32 pos = 0;

	    // path dangerous
	    if(u16 around = hero.GetPath().isUnderProtection(pos))
	    {
		bool skip = false;
    		for(Direction::vector_t
        	    dir = Direction::TOP_LEFT; dir < Direction::CENTER && !skip; ++dir) if(around & dir)
    		{
        	    const s32 dst = Maps::GetDirectionIndex(pos, dir);
        	    Army::army_t enemy;
        	    enemy.FromGuardian(world.GetTiles(dst));
        	    if(enemy.isValid() && enemy.StrongerEnemyArmy(hero.GetArmy())) skip = true;
    		}
		if(skip) continue;
	    }

	    DEBUG(DBG_AI , DBG_INFO, "Kingdom::AIHeroesTask: " << Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << ", added tasks: " << 
		    MP2::StringObject(ai_objects[(*it).first]) << ", index: " << (*it).first <<
		    ", distance: " << (*it).second);

	    task.push_back((*it).first);
	    ai_objects.erase((*it).first);
	}
	else
	{
	    DEBUG(DBG_AI , DBG_TRACE, "Kingdom::AIHeroesTask: " << Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << (!AIHeroesValidObject(hero, (*it).first) ? ", invalid: " : ", impossible: ") << 
		    MP2::StringObject(ai_objects[(*it).first]) << ", index: " << (*it).first <<
		    ", distance: " << (*it).second);
	}
    }

    if(task.empty())
	AIHeroesAddedRescueTask(hero);
}

bool ScanPickupObjects(const Heroes & hero, u8 radius, std::vector<s32> & results)
{
    return Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_ARTIFACT, radius, results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_RESOURCE, radius, results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_CAMPFIRE, radius, results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_TREASURECHEST, radius, results);
}

void AIHeroesGetTask(Heroes & hero)
{
    std::vector<s32> results;
    results.reserve(5);

    const Settings & conf = Settings::Get();
    AIHero & ai_hero = AIHeroes::Get(hero);
    AIKingdom & ai_kingdom = AIKingdoms::Get(hero.GetColor());

    Queue & task = ai_hero.sheduled_visit;
    IndexObjectMap & ai_objects = ai_kingdom.scans;

    Castle *castle = hero.inCastle();
    // if hero in castle
    if(castle)
    {
	castle->RecruitAllMonster();
	hero.GetArmy().UpgradeTroops(*castle);

	// recruit army
	if(hero.Modes(Heroes::HUNTER))
		hero.GetArmy().JoinStrongestFromArmy(castle->GetArmy());
	else
	if(hero.Modes(Heroes::SCOUTER))
		hero.GetArmy().KeepOnlyWeakestTroops(castle->GetArmy());
    }

    // patrol task
    if(hero.Modes(Heroes::PATROL))
    {
	// goto patrol center
	if(hero.GetCenterPatrol() != hero.GetCenter() &&
	   hero.GetPath().Calculate(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol())))
		return;

	// scan enemy hero
	if(hero.GetSquarePatrol() &&
	    Maps::ScanDistanceObject(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol()),
				    MP2::OBJ_HEROES, hero.GetSquarePatrol(), results))
	{
	    std::vector<s32>::const_iterator it = results.begin();
	    for(; it != results.end(); ++it)
	    {
		const Heroes* enemy = world.GetHeroes(*it);
		if(enemy && !Settings::Get().IsUnions(enemy->GetColor(), hero.GetColor()))
		{
		    if(hero.GetPath().Calculate(enemy->GetIndex()))
			return;
		}
	    }
	}

	// can pickup objects
	if(conf.ExtHeroPatrolAllowPickup() &&
	    ScanPickupObjects(hero, hero.GetSquarePatrol(), results))
	{
	    std::vector<s32>::const_iterator it = results.begin();
	    for(; it != results.end(); ++it)
    		if(AIHeroesValidObject(hero, *it) &&
		    hero.GetPath().Calculate(*it))
	    {
		ai_objects.erase(*it);

		DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
			", Patrol " << hero.GetName() << ": find object: " <<
			MP2::StringObject(world.GetTiles(*it).GetObject()) << "(" << *it << ")");
		return;
	    }
	}

	// random move
	/*
	// disable move: https://sourceforge.net/tracker/?func=detail&aid=3157397&group_id=96859&atid=616180
	{
	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_ZERO, hero.GetSquarePatrol(), results);
	    if(results.size())
	    {
		std::random_shuffle(results.begin(), results.end());
		std::vector<s32>::const_iterator it = results.begin();
		for(; it != results.end(); ++it)
		    if(world.GetTiles(*it).isPassable(&hero, true) &&
			hero.GetPath().Calculate(*it))
		{
		    DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
			", Patrol " << hero.GetName() << ": move: " << *it);
		    return;
		}
	    }
	}
	*/

	hero.SetModes(Heroes::STUPID);
	return;
    }

    if(ai_hero.fix_loop > 3)
    {
	DEBUG(DBG_AI , DBG_TRACE, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	    ", Hero " << hero.GetName() << ": loop");
	hero.SetModes(hero.Modes(Heroes::AIWAITING) ? Heroes::STUPID : Heroes::AIWAITING);
	return;
    }

    // primary target
    if(Maps::isValidAbsIndex(ai_hero.primary_target))
    {
	const Castle* castle = NULL;

	if(hero.GetIndex() == ai_hero.primary_target)
	    ai_hero.primary_target = -1;
	else
	if(NULL != (castle = world.GetCastle(ai_hero.primary_target)) &&
	    NULL != castle->GetHeroes().Guest())
	    hero.SetModes(Heroes::AIWAITING);
	else
	if(ai_hero.primary_target != hero.GetPath().GetDestinationIndex() &&
	    !hero.GetPath().Calculate(ai_hero.primary_target))
	{
	    DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	    ", Hero " << hero.GetName() << ": path unknown, primary target reset");

	    ai_hero.primary_target = -1;
	}

	if(hero.GetPath().isValid()) return;
    }

	// scan heroes and castle
	if(Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_CASTLE, hero.GetScoute(), results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_HEROES, hero.GetScoute(), results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_MONSTER, hero.GetScoute(), results))
	{
	    std::vector<s32>::const_iterator it = results.begin();

	    for(; it != results.end(); ++it)
		if(AIHeroesPriorityObject(hero, *it) &&
		    hero.GetPath().Calculate(*it))
	    {
		DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
		", Hero " << hero.GetName() << ": set primary target: " <<
		MP2::StringObject(world.GetTiles(*it).GetObject()) << "(" << *it << ")");

		ai_hero.primary_target = *it;
		return;
	    }
	}

    // check destination
    if(hero.GetPath().isValid())
    {
	if(! AIHeroesValidObject(hero, hero.GetPath().GetDestinationIndex()))
	    hero.GetPath().Reset();
	else
	if(hero.GetPath().size() < 5)
	{
	    DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
		", Hero " << hero.GetName() << ": continue" << " short");
	    ai_hero.fix_loop++;
	    return;
	}
    }

	// scan 2x2 pickup objects
	if(ScanPickupObjects(hero, 2, results) ||
    	    // scan 3x3 capture objects
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_SAWMILL, 3, results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_MINES, 3, results) ||
    	    Maps::ScanDistanceObject(hero.GetIndex(), MP2::OBJ_ALCHEMYLAB, 3, results))
	{
	    std::vector<s32>::const_iterator it = results.begin();
	    for(; it != results.end(); ++it)
    		if(AIHeroesValidObject(hero, *it) &&
		    hero.GetPath().Calculate(*it))
	    {
		ai_objects.erase(*it);

		DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
		", Hero " << hero.GetName() << ": find object: " <<
		MP2::StringObject(world.GetTiles(*it).GetObject()) << "(" << *it << ")");

		return;
	    }
	}

    if(hero.GetPath().isValid())
    {
	DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
		", Hero " << hero.GetName() << ": continue");
        ai_hero.fix_loop++;
	return;
    }

    // get task from kingdom
    if(task.empty()) AIHeroesAddedTask(hero);
    else
    // remove invalid task
    task.remove_if(std::not1(std::bind1st(std::ptr_fun(&AIHeroesValidObject2), &hero)));

    // random shuffle
    if(1 < task.size() && Rand::Get(1))
    {
	Queue::iterator it1, it2;
	it2 = it1 = task.begin();
	++it2;

    	std::swap(*it1, *it2);
    }

    // find passable index
    while(task.size())
    {
	const s32 & index = task.front();

	if(hero.GetPath().Calculate(index)) break;

	DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	", Hero " << hero.GetName() << " say: unable get object: " <<
	MP2::StringObject(world.GetTiles(index).GetObject()) << ", index: " <<
	index << ", remove task...");

	task.pop_front();
    }

    // success
    if(task.size())
    {
	const s32 & index = task.front();

	DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	", Hero " << hero.GetName() << " say: go to object: " <<
	MP2::StringObject(world.GetTiles(index).GetObject()) << ", index: " << index);

	ai_objects.erase(index);
	task.pop_front();

	if(IS_DEBUG(DBG_AI, DBG_TRACE)) hero.GetPath().Dump();
    }
    else
    if(hero.Modes(Heroes::AIWAITING))
    {
	hero.GetPath().Reset();
	DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	", Hero " << hero.GetName() << " say: unknown task., help my please..");

	hero.ResetModes(Heroes::AIWAITING);
	hero.SetModes(Heroes::STUPID);
    }
    else
    {
	DEBUG(DBG_AI , DBG_INFO, "AI::HeroesTask: " << Color::String(hero.GetColor()) <<
	", Hero " << hero.GetName() << " say: waiting...");

	hero.SetModes(Heroes::AIWAITING);
    }
}

void AIHeroesNoGUITurns(Heroes &hero)
{
    if(hero.GetPath().isValid()) hero.SetMove(true);
    else return;

    while(1)
    {
        if(hero.isFreeman() || !hero.isEnableMove()) break;

        hero.Move(true);

        DELAY(10);
    }
}

void AIHeroesTurns(Heroes &hero)
{
    if(hero.GetPath().isValid()) hero.SetMove(true);
    else return;

    const Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Interface::Basic & I = Interface::Basic::Get();

    cursor.Hide();

    if(0 != conf.AIMoveSpeed() && hero.isShow(Settings::Get().MyColor()))
    {
	    cursor.Hide();
	    I.gameArea.Center(hero.GetCenter());
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
    }

    bool hide_move = (0 == conf.AIMoveSpeed()) ||
	    (! IS_DEVEL() && !hero.isShow(Settings::Get().MyColor()));

    while(LocalEvent::Get().HandleEvents())
    {
	if(hero.isFreeman() || !hero.isEnableMove()) break;

	if(hide_move)
	{
	    hero.Move(true);
	}
	else
	if(Game::AnimateInfrequent(Game::CURRENT_AI_DELAY))
	{
	    cursor.Hide();
	    hero.Move();

	    I.gameArea.Center(hero.GetCenter());
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
	}

    	if(Game::AnimateInfrequent(Game::MAPS_DELAY))
	{
	    Maps::IncreaseAnimationTicket();
	    cursor.Hide();
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
	}
    }

    // 0.2 sec delay for show enemy hero position
    if(!hero.isFreeman() && !hide_move) DELAY(200);
}

void AIHeroesTurn(Heroes* hero)
{
    if(hero)
    {
	DEBUG(DBG_AI, DBG_TRACE, "AIHeroesTurn: " << hero->GetName());

	Interface::StatusWindow *status = Interface::NoGUI() ? NULL : &Interface::StatusWindow::Get();

        while(hero->MayStillMove() &&
	    !hero->Modes(Heroes::AIWAITING|Heroes::STUPID))
        {
            // turn indicator
            if(status) status->RedrawTurnProgress(3);
            //if(status) status->RedrawTurnProgress(4);

            // get task for heroes
            AIHeroesGetTask(*hero);

            // turn indicator
            if(status) status->RedrawTurnProgress(5);
            //if(status) status->RedrawTurnProgress(6);

            // heroes AI turn
            if(Interface::NoGUI())
                AIHeroesNoGUITurns(*hero);
            else
                AIHeroesTurns(*hero);

            // turn indicator
            if(status) status->RedrawTurnProgress(7);
            //if(status) status->RedrawTurnProgress(8);
        }
    }
}

bool AIHeroesScheduledVisit(const Kingdom & kingdom, s32 index)
{
    const std::vector<Heroes *> & heroes = kingdom.GetHeroes();
    for(std::vector<Heroes *>::const_iterator
	it = heroes.begin(); it != heroes.end(); ++it)
    {
	AIHero & ai_hero = AIHeroes::Get(**it);
	Queue & task = ai_hero.sheduled_visit;
	if(task.isPresent(index)) return true;
    }
    return false;
}

bool IsPriorityAndNotVisitAndNotPresent(const std::pair<s32, MP2::object_t> indexObj, const Heroes* hero)
{
    AIHero & ai_hero = AIHeroes::Get(*hero);
    Queue & task = ai_hero.sheduled_visit;

    return AIHeroesPriorityObject(*hero, indexObj.first) &&
	    ! AIHeroesScheduledVisit(world.GetKingdom(hero->GetColor()), indexObj.first) &&
	    ! task.isPresent(indexObj.first);
}

void AIHeroesEnd(Heroes* hero)
{
    if(hero)
    {
	AIHero & ai_hero = AIHeroes::Get(*hero);
	AIKingdom & ai_kingdom = AIKingdoms::Get(hero->GetColor());
	Queue & task = ai_hero.sheduled_visit;
	IndexObjectMap & ai_objects = ai_kingdom.scans;

	if(hero->Modes(Heroes::AIWAITING|Heroes::STUPID))
	{
	    ai_hero.Reset();
	    hero->ResetModes(Heroes::AIWAITING|Heroes::STUPID);
	}

	IndexObjectMap::iterator it;

	while(ai_objects.end() != (it = std::find_if(ai_objects.begin(), ai_objects.end(),
		    std::bind2nd(std::ptr_fun(&IsPriorityAndNotVisitAndNotPresent), hero))))
	{
	    DEBUG(DBG_AI, DBG_TRACE, "AIHeroesEnd: " << hero->GetName() << ", added priority object: " << MP2::StringObject((*it).second));
	    task.push_front((*it).first);
	    ai_objects.erase((*it).first);
	}
    }
}


void AIHeroesSetHunterWithTarget(Heroes* hero, s32 dst)
{
    if(hero)
    {
	AIHero & ai_hero = AIHeroes::Get(*hero);

	hero->SetModes(Heroes::HUNTER);

	if(0 > ai_hero.primary_target)
	{
	    ai_hero.primary_target = dst;
	}
    }
}

void AIHeroesCaptureNearestTown(Heroes* hero)
{
    if(hero)
    {
	AIHero & ai_hero = AIHeroes::Get(*hero);

	if(0 > ai_hero.primary_target)
	{
	    std::vector<IndexDistance> pos_castles;
	    pos_castles.reserve(10);

	    if(world.GetObjectPositions(hero->GetIndex(), MP2::OBJ_CASTLE, pos_castles, false))
	    {
		DEBUG(DBG_AI, DBG_TRACE, "AIHeroesCaptureNearestTown: " << hero->GetName());

    		std::sort(pos_castles.begin(), pos_castles.end(), IndexDistance::Shortest);

		for(std::vector<IndexDistance>::iterator
		    it = pos_castles.begin(); it != pos_castles.end(); ++it)
		{
		    const IndexDistance & id = *it;
		    const Castle* castle = world.GetCastle(id.first);

		    if(castle)
		    DEBUG(DBG_AI, DBG_TRACE, "AIHeroesCaptureNearestTown: " << hero->GetName() << ", to castle: " << castle->GetName());

		    if(castle &&
			hero->GetArmy().StrongerEnemyArmy(castle->GetArmy()))
		    {
			ai_hero.primary_target = id.first;

			DEBUG(DBG_AI , DBG_INFO, "AI::HeroesCaptureNearestTown: " << Color::String(hero->GetColor()) <<
			    ", Hero " << hero->GetName() << " set primary target: " << id.first);
			break;
		    }
		}
	    }
	}
    }
}
