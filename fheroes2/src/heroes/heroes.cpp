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

#include <cmath>
#include <algorithm>
#include "artifact.h"
#include "world.h"
#include "castle.h"
#include "config.h"
#include "agg.h"
#include "monster.h"
#include "error.h"
#include "payment.h"
#include "cursor.h"
#include "display.h"
#include "sprite.h"
#include "heroes.h"

Heroes::Heroes(heroes_t ht, Race::race_t rc, const std::string & str) : Skill::Primary(), name(str), experience(0), magic_point(0),
    move_point(0), army(HEROESMAXARMY), heroes(ht), race(rc), army_spread(true), enable_move(false), shipmaster(false),
    save_maps_general(MP2::OBJ_ZERO), path(*this), direction(Direction::RIGHT)
{
    // hero is freeman
    color = Color::GRAY;

    switch(race)
    {
	case Race::KNGT:
            attack      = DEFAULT_KNGT_ATTACK;
            defence     = DEFAULT_KNGT_DEFENCE;
            power       = DEFAULT_KNGT_POWER;
            knowledge   = DEFAULT_KNGT_KNOWLEDGE;

	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::BALLISTICS, Skill::Level::BASIC);
	    break;
	    
	case Race::BARB:
            attack              = DEFAULT_BARB_ATTACK;
            defence             = DEFAULT_BARB_DEFENCE;
            power               = DEFAULT_BARB_POWER;
            knowledge           = DEFAULT_BARB_KNOWLEDGE;

	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::ADVANCED);
	    break;
	    
	case Race::SORC:
            attack              = DEFAULT_SORC_ATTACK;
            defence             = DEFAULT_SORC_DEFENCE;
            power               = DEFAULT_SORC_POWER;
            knowledge           = DEFAULT_SORC_KNOWLEDGE;

	    secondary_skills.Level(Skill::NAVIGATION, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            attack              = DEFAULT_WRLK_ATTACK;
            defence             = DEFAULT_WRLK_DEFENCE;
            power               = DEFAULT_WRLK_POWER;
            knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    secondary_skills.Level(Skill::SCOUTING, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            attack              = DEFAULT_WZRD_ATTACK;
            defence             = DEFAULT_WZRD_DEFENCE;
            power               = DEFAULT_WZRD_POWER;
            knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            attack              = DEFAULT_NECR_ATTACK;
            defence             = DEFAULT_NECR_DEFENCE;
            power               = DEFAULT_NECR_POWER;
            knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    secondary_skills.Level(Skill::NECROMANCY, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	default: Error::Warning("Heroes::Heroes: unknown race."); break;
    }
    
    // set default army
    const Monster::stats_t monster = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));

    army[0].Set(monster.monster, monster.grown);

    // set master primary skill to army
    std::vector<Army::Troops>::iterator it1 = army.begin();
    std::vector<Army::Troops>::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) (*it1).SetMasterSkill(this);

    // extra hero
    switch(heroes)
    {
        case ROLAND:
            attack    = 0;
            defence   = 1;
            power     = 4;
            knowledge = 5;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::ARCHERY, Skill::Level::BASIC);
    	    break;

        case CORLAGON:
            attack    = 5;
            defence   = 3;
            power     = 1;
            knowledge = 1;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::NECROMANCY, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::BALLISTICS, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::BASIC);
    	    break;
        
        case ELIZA:
            attack    = 0;
            defence   = 1;
            power     = 2;
            knowledge = 6;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::NAVIGATION, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::ARCHERY, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::LUCK, Skill::Level::BASIC);
    	    break;

    	case ARCHIBALD:
            attack    = 1;
            defence   = 1;
            power     = 4;
            knowledge = 4;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::SCOUTING, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);
    	    break;

    	case HALTON:
            attack    = 3;
            defence   = 3;
            power     = 3;
            knowledge = 2;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::BALLISTICS, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::DIPLOMACY, Skill::Level::BASIC);
    	    break;
    	
    	case BAX:
            attack    = 1;
            defence   = 1;
            power     = 4;
            knowledge = 3;

	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::EXPERT);
	    secondary_skills.Level(Skill::NECROMANCY, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::NAVIGATION, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::BASIC);
    	    break;

	case SOLMYR:
	case DRAKONIA:
	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    break;

	case DAINWIN:
	case ELDERIAN:
	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::SCOUTING, Skill::Level::BASIC);
	    break;

	case MOG:
	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::NECROMANCY, Skill::Level::ADVANCED);
	    break;
	
	case UNCLEIVAN:
	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    break;
	
	case JOSEPH:
	    secondary_skills.Reset();
	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::SCOUTING, Skill::Level::BASIC);
	    break;
	
	case GALLAVANT:
	    break;
	
	case CEALLACH:
	    break;
	
	case MARTINE:
	    break;

	case JARKONAS:
	    break;

    	case SANDYSANDY:
	    army[0].Set(Monster::BLACK_DRAGON, 2);
    	    army[1].Set(Monster::RED_DRAGON, 3);

	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::LOGISTICS, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::MYSTICISM, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::NAVIGATION, Skill::Level::BASIC);
	    //secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    //secondary_skills.Level(Skill::LUCK, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MEDAL_VALOR);
	    artifacts.push_back(Artifact::STEALTH_SHIELD);
	    artifacts.push_back(Artifact::DRAGON_SWORD);
	    artifacts.push_back(Artifact::RABBIT_FOOT);
	    artifacts.push_back(Artifact::ENDLESS_BAG_GOLD);
	    
	    experience = 777;
	    break;
	
	default: break;
    }

    magic_point = GetMaxSpellPoints();
    move_point = GetMaxMovePoints();
}

void Heroes::LoadFromMP2(u16 map_index, const void *ptr, const Color::color_t cl)
{
    mp.x = map_index % world.w();
    mp.y = map_index / world.h();

    color = cl;

    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // unknown
    ++ptr8;

    // custom troops
    bool custom_troops = *ptr8;
    if(custom_troops)
    {
        ++ptr8;

        // monster1
        army[0].SetMonster(Monster::Monster(*ptr8));
        ++ptr8;

        // monster2
        army[1].SetMonster(Monster::Monster(*ptr8));
        ++ptr8;

        // monster3
        army[2].SetMonster(Monster::Monster(*ptr8));
        ++ptr8;

        // monster4
        army[3].SetMonster(Monster::Monster(*ptr8));
        ++ptr8;

        // monster5
        army[4].SetMonster(Monster::Monster(*ptr8));
        ++ptr8;

        // count1
        LOAD16(ptr8, byte16);
        army[0].SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count2
        LOAD16(ptr8, byte16);
        army[1].SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count3
        LOAD16(ptr8, byte16);
        army[2].SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count4
        LOAD16(ptr8, byte16);
        army[3].SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count5
        LOAD16(ptr8, byte16);
        army[4].SetCount(byte16);
        ++ptr8;
        ++ptr8;
    }
    else
    {
        ptr8 += 16;
    }

    // custom portrate
    ++ptr8;

    // index sprite portrate
    ++ptr8;

    // artifacts
    Artifact::artifact_t artifact = Artifact::UNKNOWN;

    // artifact 1
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*ptr8))) artifacts.push_back(artifact);
    ++ptr8;

    // artifact 2
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*ptr8))) artifacts.push_back(artifact);
    ++ptr8;

    // artifact 3
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*ptr8))) artifacts.push_back(artifact);
    ++ptr8;

    // unknown byte
    ++ptr8;

    // experience
    LOAD32(ptr8, byte32);
    experience = byte32;
    ptr8 += 4;


    // custom skill
    if(*ptr8)
    {
	++ptr8;

	secondary_skills.Reset();

	// skills
	for(u8 ii = 0; ii < 8; ++ii)
	{
	    const Skill::secondary_t skill = Skill::Secondary::FromMP2(*(ptr8 + ii));
	    const Skill::Level::type_t lvl = Skill::Level::FromMP2(*(ptr8 + ii + 8));

	    if(Skill::UNKNOWN != skill && Skill::Level::NONE != lvl) secondary_skills.Level(skill, lvl);
	}
	ptr8 += 15;
    }
    else
    {
	++ptr8;

        ptr8 += 16;
    }
    
    // unknown
    ++ptr8;

    // custom name
    ++ptr8;

    //name hero
    if(*ptr8) name = std::string(reinterpret_cast<const char *>(ptr8));
    
    ptr8 += 13;
    
    // patrol
    ++ptr8;

    // count square
    ++ptr8;

    // end

    // save general object
    save_maps_general = MP2::OBJ_ZERO;

    // other param
    magic_point = GetMaxSpellPoints();
    move_point = GetMaxMovePoints();

    if(H2Config::Debug()) Error::Verbose("add heroes: " + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

bool Heroes::operator== (const Heroes & h) const
{
    return heroes == h.heroes;
}

u8 Heroes::GetMobilityIndexSprite(void) const
{
    // valid range (0 - 25)
    const double floor = std::floor(move_point / 100);

    return 25 >= floor ? static_cast<u8>(floor) : 25;
}

u8 Heroes::GetManaIndexSprite(void) const
{
    // valid range (0 - 25)

    u8 r = magic_point / 5;

    return 25 >= r ? r : 25;
}

u8 Heroes::GetAttack(void) const
{
    u8 result = attack;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
	    case Artifact::SPIKED_HELM:
	    case Artifact::THUNDER_MACE:
	    case Artifact::GIANT_FLAIL:
		result += 1;
		break;

            case Artifact::SPIKED_SHIELD:
            case Artifact::POWER_AXE:
        	result += 2;
		break;
	    
	    case Artifact::DRAGON_SWORD:
	        result += 3;
	        break;
	
	    case Artifact::ULTIMATE_CROWN:
	        result += 4;
	        break;
	
	    case Artifact::ULTIMATE_SHIELD:
	        result += 6;
	        break;
	
	    case Artifact::ULTIMATE_SWORD:
	        result += 12;
	        break;
	
	    default:
	        break;
	}

    return result;
}

u8 Heroes::GetDefense(void) const
{
    u8 result = defence;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
            case Artifact::SPIKED_HELM:
            case Artifact::ARMORED_GAUNTLETS:
            case Artifact::DEFENDER_HELM:
                result += 1;
                break;

            case Artifact::SPIKED_SHIELD:
            case Artifact::STEALTH_SHIELD:
                result += 2;
                break;

            case Artifact::DIVINE_BREASTPLATE:
                result += 3;
                break;

            case Artifact::ULTIMATE_CROWN:
                result += 4;
                break;

            case Artifact::ULTIMATE_SHIELD:
                result += 6;
                break;

            case Artifact::ULTIMATE_CLOAK:
                result += 12;
                break;

            default:
                break;
	}

    return result;
}

u8 Heroes::GetPower(void) const
{
    u8 result = power;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
            case Artifact::WHITE_PEARL:
                result += 1;
                break;

            case Artifact::BLACK_PEARL:
            case Artifact::CASTER_BRACELET:
            case Artifact::MAGE_RING:
                result += 2;
                break;

            case Artifact::WITCHES_BROACH:
                result += 3;
                break;

            case Artifact::ULTIMATE_CROWN:
            case Artifact::ARCANE_NECKLACE:
                result += 4;
                break;

            case Artifact::ULTIMATE_STAFF:
                result += 6;
                break;

            case Artifact::ULTIMATE_WAND:
                result += 12;
                break;

            default:
                break;
	}

    return result;
}

u8 Heroes::GetKnowledge(void) const
{
    u8 result = knowledge;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
            case Artifact::WHITE_PEARL:
                result += 1;
                break;

            case Artifact::BLACK_PEARL:
            case Artifact::MINOR_SCROLL:
                result += 2;
                break;

            case Artifact::MAJOR_SCROLL:
                result += 3;
                break;

            case Artifact::ULTIMATE_CROWN:
            case Artifact::SUPERIOR_SCROLL:
                result += 4;
                break;

            case Artifact::FOREMOST_SCROLL:
                result += 5;
                break;

            case Artifact::ULTIMATE_STAFF:
                result += 6;
                break;

            case Artifact::ULTIMATE_BOOK:
                result += 12;
                break;

            default:
                break;
	}

    return result;
}

u32 Heroes::GetExperience(void) const
{
    return experience;
}

u16 Heroes::GetSpellPoints(void) const
{
    return magic_point;
}

u16 Heroes::GetMovePoints(void) const
{
    return move_point;
}

u16 Heroes::GetMaxSpellPoints(void) const
{
    return 10 * GetKnowledge();
}

u16 Heroes::GetMaxMovePoints(void) const
{
    u16 point = DEFAULT_MOVE_POINTS * 100;

    // skill logistics
    switch(GetLevelSkill(Skill::LOGISTICS))
    {
	// bonus: 10%
	case Skill::Level::BASIC:       point += (point * 10) / 100; break;
        // bonus: 20%
        case Skill::Level::ADVANCED:    point += (point * 20) / 100; break;
        // bonus: 30%
        case Skill::Level::EXPERT:      point += (point * 30) / 100; break;

        default: break;
    }

    if(isShipMaster())
    {
	switch(GetLevelSkill(Skill::NAVIGATION))
	{
	    // bonus: 1/3
	    case Skill::Level::BASIC:       point += point / 3; break;
    	    // bonus: 2/3
    	    case Skill::Level::ADVANCED:    point += (point * 2) / 3; break;
    	    // bonus: double
    	    case Skill::Level::EXPERT:      point *= 2; break;

    	    default: break;
	}

        // artifact bonus: FIXME: SAILORS_ASTROLABE_MOBILITY 20% ?
        if(HasArtifact(Artifact::SAILORS_ASTROLABE_MOBILITY)) point += (point * 20) / 100;
    }
    else
    {
        // artifact bonus: FIXME: NOMAD_BOOTS_MOBILITY 20% ?
	if(HasArtifact(Artifact::NOMAD_BOOTS_MOBILITY)) point += (point * 20) / 100;

        // artifact bonus: FIXME: TRAVELER_BOOTS_MOBILITY 20% ?
	if(HasArtifact(Artifact::TRAVELER_BOOTS_MOBILITY)) point += (point * 20) / 100;
    }

    // artifact bonus: FIXME: TRUE_COMPASS_MOBILITY 20% ?
    if(HasArtifact(Artifact::TRUE_COMPASS_MOBILITY)) point += (point * 20) / 100;

    return point;
}

Morale::morale_t Heroes::GetMorale(void) const
{
    s8 result = morale;

    // bonus artifact
    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)
	switch(*it)
	{
            case Artifact::MEDAL_VALOR:
            case Artifact::MEDAL_COURAGE:
            case Artifact::MEDAL_HONOR:
            case Artifact::MEDAL_DISTINCTION:
                result += 1;
                break;

            case Artifact::FIZBIN_MISFORTUNE:
                result -= 2;
                break;

            default:
		break;
    	}

    // bonus leadership
    switch(GetLevelSkill(Skill::LEADERSHIP))
    {
        case Skill::Level::EXPERT:
            result += 3;
            break;

        case Skill::Level::ADVANCED:
            result += 2;
            break;

        case Skill::Level::BASIC:
            result += 1;
            break;

        default:
            break;
    }
    
    const Castle* castle = inCastle();

    // bonus in castle
    if(castle)
    {
	// and tavern
	if(castle->isBuild(Castle::BUILD_TAVERN)) result += 1;

	// and barbarian coliseum
        if(Race::BARB == castle->GetRace() && castle->isBuild(Castle::BUILD_SPEC)) result += 2;
    }

    // object visited
    if(isVisited(MP2::OBJ_BUOY)) ++result;
    if(isVisited(MP2::OBJ_OASIS)) ++result;
    if(isVisited(MP2::OBJ_TEMPLE)) result += 2;
    if(isVisited(MP2::OBJ_GRAVEYARD)) --result;
    if(isVisited(MP2::OBJ_SHIPWRECK)) --result;
    if(isVisited(MP2::OBJ_DERELICTSHIP)) --result;

    // different race penalty
    std::vector<Army::Troops>::const_iterator it1_army = army.begin();
    std::vector<Army::Troops>::const_iterator it2_army = army.end();
    u8 count = 0;
    u8 count_kngt = 0;
    u8 count_barb = 0;
    u8 count_sorc = 0;
    u8 count_wrlk = 0;
    u8 count_wzrd = 0;
    u8 count_necr = 0;
    u8 count_bomg = 0;
    for(; it1_army != it2_army; ++it1_army) if(Monster::UNKNOWN != (*it1_army).Monster())
	switch(Monster::GetRace((*it1_army).Monster()))
	{
	    case Race::KNGT: ++count_kngt; break;
	    case Race::BARB: ++count_barb; break;
	    case Race::SORC: ++count_sorc; break;
	    case Race::WRLK: ++count_wrlk; break;
	    case Race::WZRD: ++count_wzrd; break;
	    case Race::NECR: ++count_necr; break;
	    case Race::BOMG: ++count_bomg; break;
	    default: break;
	}
    if(count_kngt) ++count;
    if(count_barb) ++count;
    if(count_sorc) ++count;
    if(count_wrlk) ++count;
    if(count_wzrd) ++count;
    if(count_necr) ++count;
    if(count_bomg) ++count;

    switch(count)
    {
	case 0: break;
	case 1: result += (count_necr ? 0 : 1); break;
	case 3: result -= 1; break;
	case 4: result -= 2; break;
	// over 4 different race
	default: result -=3; break;
    }

    // undead in life group
    if(count_necr && (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg)) --result;

    if(result < Morale::AWFUL)	return Morale::TREASON;
    else
    if(result < Morale::POOR)	return Morale::AWFUL;
    else
    if(result < Morale::NORMAL)	return Morale::POOR;
    else
    if(result < Morale::GOOD)	return Morale::NORMAL;
    else
    if(result < Morale::GREAT)	return Morale::GOOD;
    else
    if(result < Morale::BLOOD)	return Morale::GREAT;

    return Morale::BLOOD;
}

Luck::luck_t Heroes::GetLuck(void) const
{
    s8 result = luck;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    // bonus artifact
    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
    	    case Artifact::RABBIT_FOOT:
            case Artifact::GOLDEN_HORSESHOE:
            case Artifact::GAMBLER_LUCKY_COIN:
            case Artifact::FOUR_LEAF_CLOVER:
	    	++result;
		break;

            default:
		break;
    	}

    // bonus luck
    switch(GetLevelSkill(Skill::LUCK))
    {
        case Skill::Level::EXPERT:
            result += 3;
            break;

        case Skill::Level::ADVANCED:
            result += 2;
            break;

        case Skill::Level::BASIC:
            result += 1;
            break;

        default:
            break;
    }

    // object visited
    if(isVisited(MP2::OBJ_FAERIERING)) ++result;
    if(isVisited(MP2::OBJ_FOUNTAIN)) ++result;

    // bonus in castle and sorceress rainbow
    const Castle* castle = inCastle();
    if(castle && Race::SORC == castle->GetRace() && castle->isBuild(Castle::BUILD_SPEC)) result += 2;


    if(result < Luck::AWFUL)	return Luck::CURSED;
    else
    if(result < Luck::BAD)	return Luck::AWFUL;
    else
    if(result < Luck::NORMAL)	return Luck::BAD;
    else
    if(result < Luck::GOOD)	return Luck::NORMAL;
    else
    if(result < Luck::GREAT)	return Luck::GOOD;
    else
    if(result < Luck::IRISH)	return Luck::GREAT;

    return Luck::IRISH;
}

// return valid count heroes army
u8 Heroes::GetCountArmy(void) const
{
    u8 result = 0;

    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii) if(Army::isValid(army[ii])) ++result;

    return result;
}

/* recrut hero */
void Heroes::Recruit(const Color::color_t & cl, const Point & pt)
{
    color = cl;
    mp = pt;

    Maps::Tiles & tiles = world.GetTiles(mp);

    // save general object
    save_maps_general = tiles.GetObject();
    tiles.SetObject(MP2::OBJ_HEROES);
}

void Heroes::Recruit(const Castle & castle)
{
    Recruit(castle.GetColor(), castle.GetCenter());
}

void Heroes::ActionNewDay(void)
{
    move_point = GetMaxMovePoints();

    // remove day visit object
    std::remove_if(visit_object.begin(), visit_object.end(), Maps::VisitIndexObject::isDayLife);
}

void Heroes::ActionNewWeek(void)
{
    // remove week visit object
    std::remove_if(visit_object.begin(), visit_object.end(), Maps::VisitIndexObject::isWeekLife);
}

void Heroes::ActionNewMonth(void)
{
    // remove month visit object
    std::remove_if(visit_object.begin(), visit_object.end(), Maps::VisitIndexObject::isMonthLife);
}


void Heroes::ActionAfterBattle(void)
{
    // remove month visit object
    std::remove_if(visit_object.begin(), visit_object.end(), Maps::VisitIndexObject::isBattleLife);
}

u16 Heroes::FindPath(u16 dst_index)
{
    return path.Calculate(dst_index);
}

/* if hero in castle */
const Castle* Heroes::inCastle(void) const
{
    if(Color::GRAY == color) return false;

    const std::vector<Castle *> & castles = world.GetKingdom(color).GetCastles();
    
    std::vector<Castle *>::const_iterator it1 = castles.begin();
    std::vector<Castle *>::const_iterator it2 = castles.end();

    for(; it1 != it2; ++it1) if((**it1).GetCenter() == mp) return *it1;

    return NULL;
}

/* is visited cell */
bool Heroes::isVisited(const Maps::Tiles & tile) const
{
    std::list<Maps::VisitIndexObject>::const_iterator end = visit_object.end();

    return end != std::find(visit_object.begin(), end, Maps::VisitIndexObject(tile));

    return false;
}

/* return true if object visited */
bool Heroes::isVisited(const MP2::object_t & object) const
{
    std::list<Maps::VisitIndexObject>::const_iterator it1 = visit_object.begin();
    std::list<Maps::VisitIndexObject>::const_iterator it2 = visit_object.end();

    for(; it1 != it2; ++it1) if((*it1).second == object) return true;

    return false;
}

/* set visited cell */
void Heroes::SetVisited(const u32 index)
{
    const Maps::Tiles & tile = world.GetTiles(index);

    if(isVisited(tile)) return;

    const MP2::object_t object = tile.GetObject();

    // valid
    if(Maps::Object::isDayLife(object) ||
	Maps::Object::isWeekLife(object) ||
	Maps::Object::isMonthLife(object) ||
	Maps::Object::isBattleLife(object))
	    visit_object.push_front(Maps::VisitIndexObject(index, object));
}

/* return true if artifact present */
bool Heroes::HasArtifact(const Artifact::artifact_t & art) const
{
    return artifacts.end() != std::find(artifacts.begin(), artifacts.end(), art);
}

/* return level hero */
u8 Heroes::GetLevel(void) const
{
    return GetLevelFromExperience(experience);
}

/* calc level from exp */
u8 Heroes::GetLevelFromExperience(u32 exp)
{
    for(u8 lvl = 1; lvl < 255; ++ lvl) if(exp < GetExperienceFromLevel(lvl)) return lvl;

    return 0;
}

/* calc exp from level */
u32 Heroes::GetExperienceFromLevel(u8 lvl)
{
    switch(lvl)
    {
	case 0:		return 0;
	case 1:		return 1000;
	case 2:		return 2000;
	case 3:		return 3200;
	case 4:		return 4500;
	case 5:		return 6000;
	case 6:		return 7700;
	case 7:		return 9000;
	case 8: 	return 11000;
	case 9:		return 13200;
	case 10:	return 15500;

	// FIXME:	calculate alghoritm

	default:	return MAXU16;
    }

    return 0;
}

/* buy book */
bool Heroes::BuySpellBook(void)
{
    if(HasArtifact(Artifact::MAGIC_BOOK) || Color::GRAY == color) return false;

    PaymentConditions::BuySpellBook payment;
    Kingdom & kingdom = world.GetKingdom(color);

    if( ! kingdom.AllowPayment(payment)) return false;

    kingdom.OddFundsResource(payment);

    artifacts.push_back(Artifact::MAGIC_BOOK);
    
    return true;
}

/* add new spell to book from storage */
void Heroes::AppendSpellsToBook(const Spell::Storage & spells)
{
    spell_book.Appends(spells);
}

/* return true is move enable */
bool Heroes::isEnableMove(void) const
{
    return enable_move;
}

/* set enable move */
void Heroes::StopMove(void)
{
    enable_move = false;
}

void Heroes::StartMove(void)
{
    enable_move = true;
}

/* return true is need move */
bool Heroes::isNeedMove(void) const
{
    return path.size() && path.GetDestinationIndex() != Maps::GetIndexFromAbsPoint(mp);
}

/* draw move to next cell */
void Heroes::Move(void)
{
    if(path.empty() || !enable_move) return;

    const Route::Step & step = path.front();
    const u16 index_from = Maps::GetIndexFromAbsPoint(mp);
    const u16 index_to = step.to_index;
    const Direction::vector_t direction2 = Direction::Get(index_from, index_to);

    bool action = false;
    u16 action_tiles = MAXU16;
    MP2::object_t action_obj = MP2::OBJ_ZERO;

    if(Direction::UNKNOWN == direction || Direction::CENTER == direction) return;

    Maps::Tiles & tiles_from = world.GetTiles(index_from);
    Maps::Tiles & tiles_to = world.GetTiles(index_to);

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    // move point deficiency
    if(move_point < step.penalty)
    {
	enable_move = false;

	return;
    }

    cursor.Hide();

    // redraw top cell
    if(Maps::isValidDirection(index_from, Direction::TOP))
	world.GetTiles(Maps::GetDirectionIndex(index_from, Direction::TOP)).Redraw();
    // redraw top left cell (for flag)
    if(Maps::isValidDirection(index_from, Direction::TOP_LEFT))
	world.GetTiles(Maps::GetDirectionIndex(index_from, Direction::TOP_LEFT)).Redraw();
    // redraw top right cell (for flag)
    if(Maps::isValidDirection(index_from, Direction::TOP_RIGHT))
	world.GetTiles(Maps::GetDirectionIndex(index_from, Direction::TOP_RIGHT)).Redraw();

    // change through the circle
    if(direction != direction2)
    {
	Direction::vector_t to1 = direction;
	Direction::vector_t to2 = direction2;

	while(to1 != to2)
	{
	    Direction::ShortDistanceClockWise(to1, to2) ? ++to1 : --to1;
	    if(Direction::CENTER == to1) Direction::ShortDistanceClockWise(to1, to2) ? ++to1 : --to1;

	    RedrawRotate(Direction::ShortDistanceClockWise(to1, to2));

	    direction = to1;
	}

	// FIXME: rotate animation heroes
	//direction = direction2;

	tiles_from.Redraw();
    }
    else
    // next to last
    if(isNeedStopNextToLast() && path.GetDestinationIndex() == index_to)
    {
	enable_move = false;

	path.Hide();
	path.Reset();

	action = true;
	action_tiles = index_to;
	action_obj = tiles_to.GetObject();
    }
    else
    // goto next cell
    {
	// restore from cell
	if(MP2::OBJ_HEROES != save_maps_general)
	{
	    tiles_from.SetObject(save_maps_general);
	    tiles_from.Redraw();
	}

	save_maps_general = tiles_to.GetObject();
	tiles_to.SetObject(MP2::OBJ_HEROES);

	mp.x = index_to % world.w();
	mp.y = index_to / world.w();

	move_point -= step.penalty;

	path.Hide();
	path.pop_front();
	path.size() && path.GetDestinationIndex() != index_to ? path.Show() : path.Reset();

	tiles_to.Redraw();

	if(path.GetDestinationIndex() == index_to)
	{
	    enable_move = false;

	    action = true;
	    action_tiles = index_to;
	    action_obj = save_maps_general;
	}

	// fix boat
	if(!shipmaster && MP2::OBJ_BOAT == save_maps_general) save_maps_general = MP2::OBJ_ZERO;
	else
	if(shipmaster && MP2::OBJ_COAST == save_maps_general)
	{
	    tiles_from.SetObject(MP2::OBJ_BOAT);
	    tiles_from.Redraw();
	}

    }
    cursor.Show();
    display.Flip();

    if(action) Action(action_tiles, action_obj);
}

/* show path */
void Heroes::ShowPathOrStartMove(const u16 dst_index)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    // show path
    if(path.GetDestinationIndex() != dst_index)
    {
	enable_move = false;

	cursor.Hide();
		
	if(path.size()) path.Hide();

	path.Calculate(dst_index);
	path.Show();
	world.GetTiles(mp).Redraw();
		
	cursor.Show();
	display.Flip();
    }
    // start move
    else
    if(path.size() && move_point >= path.front().penalty)
    {
	enable_move = true;
    }
}

bool Heroes::isNeedStopNextToLast(void)
{
    if(path.empty()) return false;

    const u16 dst_index = path.GetDestinationIndex();
    const Castle *castle = NULL;

    switch(world.GetTiles(dst_index).GetObject())
    {
	case MP2::OBJ_MONSTER:
	case MP2::OBJ_HEROES:
	case MP2::OBJ_ARTIFACT:
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_ANCIENTLAMP:
	case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_BOTTLE:
            		return true;
	
	case MP2::OBJ_CASTLE:
	    if(NULL != (castle = world.GetCastle(dst_index)) && color != castle->GetColor()) return true;

	default: break;
    }

    return false;
}

MP2::object_t Heroes::GetUnderObject(void) const
{
    return save_maps_general;
}


void Heroes::RedrawRotate(bool clockwise)
{
    const Sprite *sprite1 = NULL;
    const Sprite *sprite2 = NULL;
    ICN::icn_t icn = ICN::UNKNOWN;

    switch(race)
    {
	case Race::KNGT:	icn = ICN::KNGT32; break;
	case Race::BARB:	icn = ICN::BARB32; break;
	case Race::SORC:	icn = ICN::SORC32; break;
	case Race::WRLK:	icn = ICN::WRLK32; break;
	case Race::WZRD:	icn = ICN::WZRD32; break;
	case Race::NECR:	icn = ICN::NECR32; break;
	default: return;
    }

    switch(direction)
    {
	case Direction::TOP:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 45 : 55);
	    sprite2 = &AGG::GetICN(icn, 46, !clockwise);
	    break;

	case Direction::TOP_RIGHT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 47 : 46);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 48 : 45);
	    break;

	case Direction::RIGHT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 49 : 48);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 50 : 47);
	    break;

	case Direction::BOTTOM_RIGHT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 51 : 50);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 52 : 49);
	    break;

	case Direction::BOTTOM:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 53 : 52);
	    sprite2 = &AGG::GetICN(icn, 51, !clockwise);
	    break;

	case Direction::BOTTOM_LEFT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 50 : 51, true);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 49 : 52, clockwise);
	    break;

	case Direction::LEFT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 48 : 49, true);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 47 : 50, true);
	    break;

	case Direction::TOP_LEFT:
	    sprite1 = &AGG::GetICN(icn, clockwise ? 46 : 47, true);
	    sprite2 = &AGG::GetICN(icn, clockwise ? 55 : 48, !clockwise);
	    break;

	default: return;
    }

/*
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    cursor.Hide();
    DELAY(ANIMATION_LOW);

    cursor.Show();
    display.Flip();    
*/
}

bool Heroes::isShipMaster(void) const
{
    return shipmaster;
}

void Heroes::SetShipMaster(bool f)
{
    shipmaster = f;
}

void Heroes::PlayWalkSound(void)
{
    M82::m82_t wav = M82::UNKNOWN;
    
    const u8 speed = 3;

    // play sound
    switch(world.GetTiles(mp).GetGround())
    {
        case Maps::Ground::WATER:	wav = (1 == speed ? M82::WSND00 : (2 == speed ? M82::WSND10 : M82::WSND20)); break;
        case Maps::Ground::GRASS:	wav = (1 == speed ? M82::WSND01 : (2 == speed ? M82::WSND11 : M82::WSND21)); break;
        case Maps::Ground::WASTELAND:	wav = (1 == speed ? M82::WSND02 : (2 == speed ? M82::WSND12 : M82::WSND22)); break;
        case Maps::Ground::SWAMP:
        case Maps::Ground::BEACH:	wav = (1 == speed ? M82::WSND03 : (2 == speed ? M82::WSND13 : M82::WSND23)); break;
        case Maps::Ground::LAVA:	wav = (1 == speed ? M82::WSND04 : (2 == speed ? M82::WSND14 : M82::WSND24)); break;
        case Maps::Ground::DESERT:
        case Maps::Ground::SNOW:	wav = (1 == speed ? M82::WSND05 : (2 == speed ? M82::WSND15 : M82::WSND25)); break;
        case Maps::Ground::DIRT:	wav = (1 == speed ? M82::WSND06 : (2 == speed ? M82::WSND16 : M82::WSND26)); break;

        default: return;
    }

    AGG::PlaySound(wav);
}
