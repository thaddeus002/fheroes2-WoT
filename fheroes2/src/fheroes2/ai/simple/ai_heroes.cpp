/********************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>               *
 *   All rights reserved.                                                       *
 *                                                                              *
 *   Part of the Free Heroes2 Engine:                                           *
 *   http://sourceforge.net/projects/fheroes2                                   *
 *                                                                              *
 *   Redistribution and use in source and binary forms, with or without         *
 *   modification, are permitted provided that the following conditions         *
 *   are met:                                                                   *
 *   - Redistributions may not be sold, nor may they be used in a               *
 *     commercial product or activity.                                          *
 *   - Redistributions of source code and/or in binary form must reproduce      *
 *     the above copyright notice, this list of conditions and the              *
 *     following disclaimer in the documentation and/or other materials         *
 *     provided with the distribution.                                          *
 *                                                                              *
 * THIS SOFTWARE IS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,   *
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,        *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,     *
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE         *
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,            *
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           *
 *******************************************************************************/

#include <functional>
#include <algorithm>

#include "settings.h"
#include "kingdom.h"
#include "castle.h"
#include "army.h"
#include "battle.h"
#include "luck.h"
#include "morale.h"
#include "race.h"
#include "difficulty.h"
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
    std::for_each(ai.begin(), ai.end(), std::mem_fun_ref(&AIHero::Reset));
}

void AIHero::Reset(void)
{
    primary_target = -1;
    sheduled_visit.clear();
    fix_loop = 0;
}

std::string AI::HeroesString(const Heroes & hero)
{
    std::ostringstream os;

    AIHero & ai_hero = AIHeroes::Get(hero);
    Queue & task = ai_hero.sheduled_visit;

    os << "ai primary target: " << ai_hero.primary_target << std::endl <<
          "ai sheduled visit: ";
    for(Queue::const_iterator
	it = task.begin(); it != task.end(); ++it)
	os << *it << "(" << MP2::StringObject(world.GetTiles(*it).GetObject()) << "), ";
    os << std::endl;

    return os.str();
}

void AI::HeroesLevelUp(Heroes & hero)
{
    if(4 < hero.GetLevel() && !hero.Modes(Heroes::HUNTER))
	hero.SetModes(Heroes::HUNTER);

    if(9 < hero.GetLevel() && hero.Modes(Heroes::SCOUTER))
	hero.ResetModes(Heroes::SCOUTER);
}

void AI::HeroesPreBattle(HeroBase & hero)
{
    Castle* castle = world.GetCastle(hero.GetIndex());
    if(castle && hero.GetType() != Skill::Primary::CAPTAIN)
	hero.GetArmy().JoinTroops(castle->GetArmy());
}

void AI::HeroesAfterBattle(HeroBase & hero)
{
}

bool AIHeroesValidObject(const Heroes & hero, s32 index)
{
    Maps::Tiles & tile = world.GetTiles(index);
    const u8 obj = tile.GetObject();
    const Army & army = hero.GetArmy();
    const Kingdom & kingdom = world.GetKingdom(hero.GetColor());

    // check other
    switch(obj)
    {
	// water object
	case MP2::OBJ_SHIPWRECKSURVIROR:
	case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_FLOTSAM:
	case MP2::OBJ_BOTTLE:
	    if(hero.isShipMaster()) return true;
            break;

	case MP2::OBJ_BUOY:
	    if(! hero.isVisited(obj) &&
		Morale::BLOOD > hero.GetMorale()) return true;
	    break;

	case MP2::OBJ_MERMAID:
	    if(! hero.isVisited(obj) &&
		Luck::IRISH > hero.GetLuck()) return true;
	    break;

	case MP2::OBJ_SIRENS:
		return false;

	case MP2::OBJ_MAGELLANMAPS:
	case MP2::OBJ_WHIRLPOOL:
	case MP2::OBJ_COAST:
	    if(hero.isShipMaster()) return true;
            break;

	// capture objects
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_MINES:
	case MP2::OBJ_ALCHEMYLAB:
	    if(! Players::isFriends(hero.GetColor(), tile.QuantityColor()))
	    {
		if(tile.CaptureObjectIsProtection())
		{
		    Army enemy(tile);
		    return !enemy.isValid() || Army::TroopsStrongerEnemyTroops(army, enemy);
		}
		else
		return true;
	    }
	    break;

	// pickup object
	case MP2::OBJ_WAGON:
	case MP2::OBJ_LEANTO:
	case MP2::OBJ_MAGICGARDEN:
	case MP2::OBJ_SKELETON:
	    if(tile.QuantityIsValid()) return true;
	    break;

	case MP2::OBJ_WATERWHEEL:
	case MP2::OBJ_WINDMILL:
	    if(Settings::Get().ExtWorldExtObjectsCaptured())
	    {
		if(! Players::isFriends(hero.GetColor(), tile.QuantityColor()))
		{
		    if(tile.CaptureObjectIsProtection())
		    {
			Army enemy(tile);
			return !enemy.isValid() || Army::TroopsStrongerEnemyTroops(army, enemy);
		    }
		    else
		    return true;
		}
	    }
	    else
	    if(tile.QuantityIsValid()) return true;
	    break;

	// pickup resource
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_CAMPFIRE:
	case MP2::OBJ_TREASURECHEST:
	    if(! hero.isShipMaster()) return true;
	    break;

	case MP2::OBJ_ARTIFACT:
	{
	    const u8 variants = tile.QuantityVariant();

	    if(hero.IsFullBagArtifacts()) return false;

	    if(hero.isShipMaster()) return false;
	    else
	    // 1,2,3 - 2000g, 2500g+3res, 3000g+5res
	    if(1 <= variants && 3 >= variants)
	    {
		return kingdom.AllowPayment(tile.QuantityFunds());
	    }
	    else
	    // 4,5 - need have skill wisard or leadership,
	    if(3 < variants && 6 > variants)
	    {
		return hero.HasSecondarySkill(tile.QuantitySkill().Skill());
	    }
	    else
	    // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
	    if(5 < variants && 14 > variants)
	    {
                Army enemy(tile);
		return !enemy.isValid() || Army::TroopsStrongerEnemyTroops(army, enemy);
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
	    if(kingdom.IsVisitTravelersTent(tile.QuantityColor())) return true;
	    break;

        case MP2::OBJ_TRAVELLERTENT:
	    if(! kingdom.IsVisitTravelersTent(tile.QuantityColor())) return true;
	    break;

	// new spell
        case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	{
	    const Spell & spell = tile.QuantitySpell();
	    if(spell.isValid() &&
		 // check spell book
		hero.HaveSpellBook() &&
		!hero.HaveSpell(spell) &&
                // check valid level spell and wisdom skill
                !(3 == spell.Level() &&
                Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM))) return true;
	}
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
		const ResourceCount & rc = tile.QuantityResourceCount();
    		if(! rc.isValid() || kingdom.AllowPayment(Funds(rc)))
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
    	    const Troop & troop = tile.QuantityTroop();
	    if(troop.isValid() &&
		(army.HasMonster(troop()) ||
		(! army.isFullHouse() && (troop.isArchers() || troop.isFly())))) return true;
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
	    const Troop & troop = tile.QuantityTroop();
	    const payment_t paymentCosts = troop.GetCost();

	    if(troop.isValid() && kingdom.AllowPayment(paymentCosts) &&
		(army.HasMonster(troop()) ||
		(! army.isFullHouse() && (troop.isArchers() || troop.isFly())))) return true;
	    break;
	}

        // recruit army (battle)
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:
        {
    	    const bool battle = (Color::NONE == tile.QuantityColor());
	    if(!battle)
	    {
		const Troop & troop = tile.QuantityTroop();
		const payment_t paymentCosts = troop.GetCost();

		if(troop.isValid() && kingdom.AllowPayment(paymentCosts) &&
		(army.HasMonster(troop()) ||
		(! army.isFullHouse()))) return true;
	    }
	    break;
        }

	// recruit genie
	case MP2::OBJ_ANCIENTLAMP:
	{
	    const Troop & troop = tile.QuantityTroop();
	    const payment_t paymentCosts = troop.GetCost();

	    if(troop.isValid() && kingdom.AllowPayment(paymentCosts) &&
		(army.HasMonster(troop()) ||
		(! army.isFullHouse()))) return true;
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
	    if(! hero.isVisited(tile, Visit::GLOBAL) &&
		tile.QuantityIsValid())
	    {
		Army enemy(tile);
		return enemy.isValid() && Army::TroopsStrongerEnemyTroops(army, enemy);
	    }
	    break;

	//case MP2::OBJ_PYRAMID:

	case MP2::OBJ_DAEMONCAVE:
	    if(tile.QuantityIsValid() && 4 != tile.QuantityVariant()) return true;
	    break;

	case MP2::OBJ_MONSTER:
	{
	    Army enemy(tile);
	    return !enemy.isValid() || Army::TroopsStrongerEnemyTroops(army, enemy);
	}
	break;

	// sign
	case MP2::OBJ_SIGN:
	    if(!hero.isVisited(tile)) return true;
	    break;

	case MP2::OBJ_CASTLE:
	{
	    const Castle* castle = world.GetCastle(index);
	    if(castle)
	    {
		if(hero.GetColor() == castle->GetColor())
		    return NULL == castle->GetHeroes().Guest() && ! hero.isVisited(tile);
		else
		// FIXME: AI skip visiting alliance
		if(Players::isFriends(hero.GetColor(), castle->GetColor())) return false;
		else
		if(Army::TroopsStrongerEnemyTroops(army, castle->GetActualArmy())) return true;
	    }
	    break;
	}

	case MP2::OBJ_HEROES:
	{
	    const Heroes* hero2 = tile.GetHeroes();
	    if(hero2)
	    {
		if(hero.GetColor() == hero2->GetColor()) return true;
		// FIXME: AI skip visiting alliance
		else
		if(Players::isFriends(hero.GetColor(), hero2->GetColor())) return false;
		else
		if(hero2->AllowBattle(false) &&
		    Army::TroopsStrongerEnemyTroops(army, hero2->GetArmy())) return true;
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
    Maps::Tiles & tile = world.GetTiles(index);

    if(MP2::OBJ_CASTLE == tile.GetObject())
    {
	const Castle* castle = world.GetCastle(index);
	if(castle)
	{
	    if(hero.GetColor() == castle->GetColor())
	    {
		// maybe need join army
		return hero.Modes(Heroes::HUNTER) &&
		    castle->GetArmy().isValid() &&
		    ! hero.isVisited(world.GetTiles(castle->GetIndex()));
	    }
	    else
	    if(!Players::isFriends(hero.GetColor(), castle->GetColor()))
		return AIHeroesValidObject(hero, index);
	}
    }
    else
    if(MP2::OBJ_HEROES == tile.GetObject())
    {
	// kill enemy hero
	const Heroes* hero2 = tile.GetHeroes();
	return hero2 &&
		!Players::isFriends(hero.GetColor(), hero2->GetColor()) &&
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

s32  FindUncharteredTerritory(Heroes & hero, const u8 & scoute)
{
    MapsIndexes v = Maps::GetDistanceIndexes(hero.GetIndex(), scoute, true);
    MapsIndexes res;

    v.resize(std::distance(v.begin(),
	std::remove_if(v.begin(), v.end(), std::ptr_fun(&Maps::TileIsUnderProtection))));


#if (__GNUC__ == 3 && __GNUC_MINOR__ == 4)
    const MapsIndexes::const_reverse_iterator crend = v.rend();

    for(MapsIndexes::const_reverse_iterator
	it = v.rbegin(); it != crend && res.size() < 4; ++it)
#else
    for(MapsIndexes::const_reverse_iterator
	it = v.rbegin(); it != v.rend() && res.size() < 4; ++it)
#endif
    {
	// find fogs
	if(world.GetTiles(*it).isFog(hero.GetColor()) &&
    	    world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
	    hero.GetPath().Calculate(*it))
	    res.push_back(*it);
    }

    const s32 result = res.size() ? *Rand::Get(res) : -1;

    if(0 <= result)
    {
	DEBUG(DBG_AI, DBG_INFO, Color::String(hero.GetColor()) <<
                ", hero: " << hero.GetName() << ", added task: " << result);
    }

    return result;
}

s32  GetRandomHeroesPosition(Heroes & hero, const u8 & scoute)
{
    MapsIndexes v = Maps::GetDistanceIndexes(hero.GetIndex(), scoute, true);
    MapsIndexes res;

    v.resize(std::distance(v.begin(),
	std::remove_if(v.begin(), v.end(), std::ptr_fun(&Maps::TileIsUnderProtection))));

#if (__GNUC__ == 3 && __GNUC_MINOR__ == 4)
    const MapsIndexes::const_reverse_iterator crend = v.rend();

    for(MapsIndexes::const_reverse_iterator
	it = v.rbegin(); it != crend && res.size() < 4; ++it)
#else
    for(MapsIndexes::const_reverse_iterator
	it = v.rbegin(); it != v.rend() && res.size() < 4; ++it)
#endif
    {
        if(world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
	    hero.GetPath().Calculate(*it))
	    res.push_back(*it);
    }

    const s32 result = res.size() ? *Rand::Get(res) : -1;

    if(0 <= result)
    {
	DEBUG(DBG_AI, DBG_INFO, Color::String(hero.GetColor()) <<
                ", hero: " << hero.GetName() << ", added task: " << result);
    }

    return result;
}

void AIHeroesAddedRescueTask(Heroes & hero)
{
    AIHero & ai_hero = AIHeroes::Get(hero);
    Queue & task = ai_hero.sheduled_visit;

    DEBUG(DBG_AI, DBG_TRACE, hero.GetName());

    u8 scoute = hero.GetScoute();

    switch(Settings::Get().GameDifficulty())
    {
        case Difficulty::NORMAL:    scoute += 2; break;
        case Difficulty::HARD:      scoute += 3; break;
        case Difficulty::EXPERT:    scoute += 4; break;
        case Difficulty::IMPOSSIBLE:scoute += 6; break;
        default: break;
    }

    // find unchartered territory
    s32 index = FindUncharteredTerritory(hero, scoute);
    const Maps::Tiles & tile = world.GetTiles(hero.GetIndex());

    if(index < 0)
    {
	// check teleports
	if(MP2::OBJ_STONELIGHTS == tile.GetObject(false) ||
	    MP2::OBJ_WHIRLPOOL == tile.GetObject(false))
	{
	    AI::HeroesAction(hero, hero.GetIndex());
	}
	else
	{
	    // random
	    index = GetRandomHeroesPosition(hero, scoute);
	}
    }

    if(0 <= index) task.push_back(index);
}

void AIHeroesAddedTask(Heroes & hero)
{
    AIHero & ai_hero = AIHeroes::Get(hero);
    AIKingdom & ai_kingdom = AIKingdoms::Get(hero.GetColor());

    Queue & task = ai_hero.sheduled_visit;
    IndexObjectMap & ai_objects = ai_kingdom.scans;

    // load minimal distance tasks
    std::vector<IndexDistance> objs;
    objs.reserve(ai_objects.size());

    for(std::map<s32, MP2::object_t>::const_iterator
	it = ai_objects.begin(); it != ai_objects.end(); ++it)
    {
	const Maps::Tiles & tile = world.GetTiles((*it).first);

	if(hero.isShipMaster())
	{
	    if(! tile.isWater()) continue;

	    // check previous positions
	    if(MP2::OBJ_COAST == (*it).second &&
		hero.isVisited(world.GetTiles((*it).first))) continue;
	}
	else
	{
	    if(tile.isWater() && MP2::OBJ_BOAT != tile.GetObject()) continue;
	}

	objs.push_back(IndexDistance((*it).first,
			    Maps::GetApproximateDistance(hero.GetIndex(), (*it).first)));
    }

    DEBUG(DBG_AI, DBG_INFO, Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << ", task prepare: " << objs.size());

    std::sort(objs.begin(), objs.end(), IndexDistance::Shortest);

    for(std::vector<IndexDistance>::const_iterator
	it = objs.begin(); it != objs.end(); ++it)
    {
	if(task.size() >= HERO_MAX_SHEDULED_TASK) break;
	const bool validobj = AIHeroesValidObject(hero, (*it).first);

	if(validobj &&
	    hero.GetPath().Calculate((*it).first))
	{
	    DEBUG(DBG_AI, DBG_INFO, Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << ", added tasks: " <<
		    MP2::StringObject(ai_objects[(*it).first]) << ", index: " << (*it).first <<
		    ", distance: " << (*it).second);

	    task.push_back((*it).first);
	    ai_objects.erase((*it).first);
	}
	else
	{
	    DEBUG(DBG_AI, DBG_TRACE, Color::String(hero.GetColor()) <<
		    ", hero: " << hero.GetName() << (!validobj ? ", invalid: " : ", impossible: ") <<
		    MP2::StringObject(ai_objects[(*it).first]) << ", index: " << (*it).first <<
		    ", distance: " << (*it).second);
	}
    }

    if(task.empty())
	AIHeroesAddedRescueTask(hero);
}

void AI::HeroesActionNewPosition(Heroes & hero)
{
    AIHero & ai_hero = AIHeroes::Get(hero);
    //AIKingdom & ai_kingdom = AIKingdoms::Get(hero.GetColor());
    Queue & task = ai_hero.sheduled_visit;

    const u8 objs[] = { MP2::OBJ_ARTIFACT, MP2::OBJ_RESOURCE, MP2::OBJ_CAMPFIRE, MP2::OBJ_TREASURECHEST, 0 };
    MapsIndexes pickups = Maps::ScanAroundObjects(hero.GetIndex(), objs);

    if(pickups.size() && hero.GetPath().isValid() &&
	pickups.end() == std::find(pickups.begin(), pickups.end(), hero.GetPath().GetDestinationIndex()))
	hero.GetPath().Reset();

    for(MapsIndexes::const_iterator
	it = pickups.begin(); it != pickups.end(); ++it)
	if(*it != hero.GetPath().GetDestinationIndex())
	    task.push_front(*it);
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

    const u8 objs1[] = { MP2::OBJ_ARTIFACT, MP2::OBJ_RESOURCE, MP2::OBJ_CAMPFIRE, MP2::OBJ_TREASURECHEST, 0 };
    const u8 objs2[] = { MP2::OBJ_SAWMILL, MP2::OBJ_MINES, MP2::OBJ_ALCHEMYLAB, 0 };
    const u8 objs3[] = { MP2::OBJ_CASTLE, MP2::OBJ_HEROES, MP2::OBJ_MONSTER, 0 };

    // rescan path
    hero.RescanPath();

    Castle* castle = hero.inCastle();
    // if hero in castle
    if(castle)
    {
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", in castle");

	castle->RecruitAllMonster();
	hero.GetArmy().UpgradeTroops(*castle);

	// recruit army
	if(hero.Modes(Heroes::HUNTER))
		hero.GetArmy().JoinStrongestFromArmy(castle->GetArmy());
	else
	if(hero.Modes(Heroes::SCOUTER))
		hero.GetArmy().KeepOnlyWeakestTroops(castle->GetArmy());

	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", " << hero.GetArmy().String());
    }

    // patrol task
    if(hero.Modes(Heroes::PATROL))
    {
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", is patrol mode");

	// goto patrol center
	if(hero.GetCenterPatrol() != hero.GetCenter() &&
	   hero.GetPath().Calculate(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol())))
		return;

	// scan enemy hero
	if(hero.GetSquarePatrol())
	{
	    const MapsIndexes & results = Maps::ScanDistanceObject(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol()),
									MP2::OBJ_HEROES, hero.GetSquarePatrol());
	    for(MapsIndexes::const_iterator
		it = results.begin(); it != results.end(); ++it)
	    {
		const Heroes* enemy = world.GetTiles(*it).GetHeroes();
		if(enemy && !Players::isFriends(enemy->GetColor(), hero.GetColor()))
		{
		    if(hero.GetPath().Calculate(enemy->GetIndex()))
		    {
			DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", find enemy");
			return;
		    }
		}
	    }
	}

	// can pickup objects
	if(conf.ExtHeroPatrolAllowPickup())
	{
	    const MapsIndexes & results = Maps::ScanDistanceObjects(hero.GetIndex(),
								    objs1, hero.GetSquarePatrol());
	    for(MapsIndexes::const_iterator
		it = results.begin(); it != results.end(); ++it)
    		if(AIHeroesValidObject(hero, *it) &&
		    hero.GetPath().Calculate(*it))
	    {
		ai_objects.erase(*it);

		DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ": find object: " <<
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
		    if(world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
			hero.GetPath().Calculate(*it))
		{
		    DEBUG(DBG_AI, Color::String(hero.GetColor()) <<
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
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ": loop");
	hero.SetModes(hero.Modes(Heroes::AIWAITING) ? Heroes::STUPID : Heroes::AIWAITING);
	return;
    }

    // primary target
    if(Maps::isValidAbsIndex(ai_hero.primary_target))
    {
	const Castle* castle = NULL;

	if(hero.GetIndex() == ai_hero.primary_target)
	{
	    ai_hero.primary_target = -1;
	    hero.GetPath().Reset();
	    DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", reset path");
	}
	else
	{
	    DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", primary target: " <<
		    ai_hero.primary_target << ", " << MP2::StringObject(world.GetTiles(ai_hero.primary_target).GetObject()));

	    if(NULL != (castle = world.GetCastle(ai_hero.primary_target)) &&
		NULL != castle->GetHeroes().Guest() && Players::isFriends(castle->GetColor(), hero.GetColor()))
	    {
		hero.SetModes(Heroes::AIWAITING);
		DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", castle busy..");
	    }

	    // make path
	    if(ai_hero.primary_target != hero.GetPath().GetDestinationIndex() &&
		!hero.GetPath().Calculate(ai_hero.primary_target))
	    {
		DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", path unknown, primary target reset");
		ai_hero.primary_target = -1;
	    }
	}

	if(hero.GetPath().isValid()) return;
    }

    // scan heroes and castle
    const MapsIndexes & enemies = Maps::ScanDistanceObjects(hero.GetIndex(), objs3, hero.GetScoute());

    for(MapsIndexes::const_iterator
	it = enemies.begin(); it != enemies.end(); ++it)
	if(AIHeroesPriorityObject(hero, *it) &&
		hero.GetPath().Calculate(*it))
    {
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", set primary target: " <<
	MP2::StringObject(world.GetTiles(*it).GetObject()) << "(" << *it << ")");

	ai_hero.primary_target = *it;
	return;
    }

    // check destination
    if(hero.GetPath().isValid())
    {
	if(! AIHeroesValidObject(hero, hero.GetPath().GetDestinationIndex()))
	    hero.GetPath().Reset();
	else
	if(hero.GetPath().size() < 5)
	{
	    DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", continue short");
	    ai_hero.fix_loop++;
	    return;
	}
    }

    // scan 2x2 pickup objects
    MapsIndexes pickups = Maps::ScanDistanceObjects(hero.GetIndex(), objs1, 2);
    // scan 3x3 capture objects
    const MapsIndexes & captures = Maps::ScanDistanceObjects(hero.GetIndex(), objs2, 3);
    if(captures.size()) pickups.insert(pickups.end(), captures.begin(), captures.end());

    if(pickups.size())
    {
	hero.GetPath().Reset();

	for(MapsIndexes::const_iterator
	    it = pickups.begin(); it != pickups.end(); ++it)
    	    if(AIHeroesValidObject(hero, *it))
	{
	    task.push_front(*it);

	    DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", find object: " <<
		MP2::StringObject(world.GetTiles(*it).GetObject()) << "(" << *it << ")");
	}
    }

    if(hero.GetPath().isValid())
    {
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", continue");
        ai_hero.fix_loop++;
	return;
    }

    if(task.empty())
    {
	// get task from kingdom
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", empty task");
	AIHeroesAddedTask(hero);
    }
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

	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", look for: " << MP2::StringObject(world.GetTiles(index).GetObject()) << "(" << index << ")");
	if(hero.GetPath().Calculate(index)) break;

	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << " say: unable get object: " << index << ", remove task...");
	task.pop_front();
    }

    // success
    if(task.size())
    {
	const s32 & index = task.front();
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << " go to: " << index);

	ai_objects.erase(index);
	task.pop_front();

	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << ", route: " << hero.GetPath().String());
    }
    else
    if(hero.Modes(Heroes::AIWAITING))
    {
	hero.GetPath().Reset();
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << " say: unknown task., help my please..");

	hero.ResetModes(Heroes::AIWAITING);
	hero.SetModes(Heroes::STUPID);
    }
    else
    {
	DEBUG(DBG_AI, DBG_TRACE, hero.GetName() << " say: waiting...");
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

bool AIHeroesShowAnimation(const Heroes & hero)
{
    const Settings & conf = Settings::Get();

    // accumulate colors
    u8 colors = 0;

    if(conf.GameType() & Game::TYPE_HOTSEAT)
    {
	const Colors vcolors(Players::HumanColors());

        for(Colors::const_iterator
	    it = vcolors.begin(); it != vcolors.end(); ++it)
	{
    	    const Player* player = conf.GetPlayers().Get(*it);
    	    if(player) colors |= player->friends;
	}
    }
    else
    {
        const Player* player = conf.GetPlayers().Get(Players::HumanColors());
        if(player) colors = player->friends;
    }

    // get result
    const s32 index_from = hero.GetIndex();

    if(colors && Maps::isValidAbsIndex(index_from))
    {
	const Maps::Tiles & tile_from = world.GetTiles(index_from);

	if(hero.GetPath().isValid())
	{
    	    const s32 index_to = Maps::GetDirectionIndex(index_from, hero.GetPath().GetFrontDirection());
    	    const Maps::Tiles & tile_to = world.GetTiles(index_to);

    	    return !tile_from.isFog(colors) && !tile_to.isFog(colors);
	}

	return !tile_from.isFog(colors);
    }

    return false;
}

void AIHeroesTurns(Heroes & hero)
{
    if(hero.GetPath().isValid()) hero.SetMove(true);
    else return;

    const Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Interface::Basic & I = Interface::Basic::Get();

    cursor.Hide();

    if(0 != conf.AIMoveSpeed() && AIHeroesShowAnimation(hero))
    {
	    cursor.Hide();
	    I.gameArea.SetCenter(hero.GetCenter());
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
    }

    while(LocalEvent::Get().HandleEvents())
    {
	if(hero.isFreeman() || !hero.isEnableMove()) break;

	bool hide_move = (0 == conf.AIMoveSpeed()) ||
	    (! IS_DEVEL() && !AIHeroesShowAnimation(hero));

	if(hide_move)
	{
	    hero.Move(true);
	}
	else
	if(Game::AnimateInfrequent(Game::CURRENT_AI_DELAY))
	{
	    cursor.Hide();
	    hero.Move();

	    I.gameArea.SetCenter(hero.GetCenter());
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
	}

    	if(Game::AnimateInfrequent(Game::MAPS_DELAY))
	{
	    u32 & frame = Game::MapsAnimationFrame();
	    ++frame;
	    cursor.Hide();
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
	}
    }

    bool hide_move = (0 == conf.AIMoveSpeed()) ||
	    (! IS_DEVEL() && !AIHeroesShowAnimation(hero));

    // 0.2 sec delay for show enemy hero position
    if(!hero.isFreeman() && !hide_move) DELAY(200);
}

void AIHeroesTurn(Heroes* hero)
{
    if(hero)
    {
	DEBUG(DBG_AI, DBG_TRACE, hero->GetName() << ", start: " <<
	    (hero->Modes(Heroes::SHIPMASTER) ? "SHIPMASTER," : "") <<
            (hero->Modes(Heroes::SCOUTER) ? "SCOUTER," : "") <<
            (hero->Modes(Heroes::HUNTER) ? "HUNTER," : "") <<
            (hero->Modes(Heroes::PATROL) ? "PATROL," : "") <<
            (hero->Modes(Heroes::AIWAITING) ? "WAITING," : "") <<
            (hero->Modes(Heroes::STUPID) ? "STUPID," : ""));

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

	DEBUG(DBG_AI, DBG_TRACE, hero->GetName() << ", end");
    }
}

bool AIHeroesScheduledVisit(const Kingdom & kingdom, s32 index)
{
    for(KingdomHeroes::const_iterator
	it = kingdom.GetHeroes().begin(); it != kingdom.GetHeroes().end(); ++it)
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
	    DEBUG(DBG_AI, DBG_TRACE, hero->GetName() << ", added priority object: " <<
		MP2::StringObject((*it).second) << ", index: " << (*it).first);
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
	    const MapsIndexes & castles = Maps::GetObjectPositions(hero->GetIndex(), MP2::OBJ_CASTLE, true);

	    for(MapsIndexes::const_iterator
		it = castles.begin(); it != castles.end(); ++it)
	    {
		const Castle* castle = world.GetCastle(*it);

		if(castle)
		    DEBUG(DBG_AI, DBG_TRACE, hero->GetName() << ", to castle: " << castle->GetName());

		if(castle &&
		    Army::TroopsStrongerEnemyTroops(hero->GetArmy(), castle->GetArmy()))
		{
		    ai_hero.primary_target = *it;

		    DEBUG(DBG_AI, DBG_INFO, Color::String(hero->GetColor()) <<
			    ", Hero " << hero->GetName() << " set primary target: " << *it);
		    break;
		}
	    }
	}
    }
}
