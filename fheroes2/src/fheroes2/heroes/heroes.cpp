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
#include "engine.h"
#include "payment.h"
#include "cursor.h"
#include "sprite.h"
#include "rand.h"
#include "gamearea.h"
#include "tools.h"
#include "ai.h"
#include "heroes.h"

Heroes::Heroes(heroes_t ht, Race::race_t rc, const std::string & str) : Skill::Primary(), spellCasted(false), name(str), experience(0), magic_point(0),
    move_point(0), army(HEROESMAXARMY), spell_book(*this), heroes(ht), race(rc), army_spread(true), enable_move(false), shipmaster(false),
    save_maps_general(MP2::OBJ_ZERO), path(*this), direction(Direction::RIGHT), sprite_index(18)
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

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::BALLISTICS, Skill::Level::BASIC));
	    break;
	    
	case Race::BARB:
            attack              = DEFAULT_BARB_ATTACK;
            defence             = DEFAULT_BARB_DEFENCE;
            power               = DEFAULT_BARB_POWER;
            knowledge           = DEFAULT_BARB_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::ADVANCED));
	    break;
	    
	case Race::SORC:
            attack              = DEFAULT_SORC_ATTACK;
            defence             = DEFAULT_SORC_DEFENCE;
            power               = DEFAULT_SORC_POWER;
            knowledge           = DEFAULT_SORC_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NAVIGATION, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));

	    spell_book.Activate();
	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            attack              = DEFAULT_WRLK_ATTACK;
            defence             = DEFAULT_WRLK_DEFENCE;
            power               = DEFAULT_WRLK_POWER;
            knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));

	    spell_book.Activate();
	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            attack              = DEFAULT_WZRD_ATTACK;
            defence             = DEFAULT_WZRD_DEFENCE;
            power               = DEFAULT_WZRD_POWER;
            knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));

	    spell_book.Activate();
	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            attack              = DEFAULT_NECR_ATTACK;
            defence             = DEFAULT_NECR_DEFENCE;
            power               = DEFAULT_NECR_POWER;
            knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));

	    spell_book.Activate();
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

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::ARCHERY, Skill::Level::BASIC));
    	    break;

        case CORLAGON:
            attack    = 5;
            defence   = 3;
            power     = 1;
            knowledge = 1;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::BALLISTICS, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::BASIC));
    	    break;
        
        case ELIZA:
            attack    = 0;
            defence   = 1;
            power     = 2;
            knowledge = 6;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NAVIGATION, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::ARCHERY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LUCK, Skill::Level::BASIC));
    	    break;

    	case ARCHIBALD:
            attack    = 1;
            defence   = 1;
            power     = 4;
            knowledge = 4;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
    	    break;

    	case HALTON:
            attack    = 3;
            defence   = 3;
            power     = 3;
            knowledge = 2;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::BALLISTICS, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::DIPLOMACY, Skill::Level::BASIC));
    	    break;
    	
    	case BAX:
            attack    = 1;
            defence   = 1;
            power     = 4;
            knowledge = 3;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NAVIGATION, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::BASIC));
    	    break;

	case SOLMYR:
	case DRAKONIA:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    break;

	case DAINWIN:
	case ELDERIAN:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));
	    break;

	case MOG:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::ADVANCED));
	    break;
	
	case UNCLEIVAN:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    break;
	
	case JOSEPH:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));
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

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LOGISTICS, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::MYSTICISM, Skill::Level::BASIC));

	    artifacts.push_back(Artifact::MEDAL_VALOR);
	    artifacts.push_back(Artifact::STEALTH_SHIELD);
	    artifacts.push_back(Artifact::DRAGON_SWORD);
	    artifacts.push_back(Artifact::RABBIT_FOOT);
	    artifacts.push_back(Artifact::ENDLESS_BAG_GOLD);

	    experience = 777;

	    for(Spell::spell_t spell=(Spell::spell_t)(Spell::NONE+1); spell < Spell::STONE; spell = (Spell::spell_t)(spell+1)) 
		spell_book.Append(spell, Skill::Level::EXPERT);
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

	secondary_skills.clear();

	// skills
	for(u8 ii = 0; ii < 8; ++ii)
	{
	    const Skill::Secondary::skill_t skill = Skill::Secondary::FromMP2(*(ptr8 + ii));
	    const Skill::Level::type_t lvl = Skill::Level::FromMP2(*(ptr8 + ii + 8));

	    if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != lvl) secondary_skills.push_back(Skill::Secondary(skill, lvl));
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

void Heroes::IncreasePrimarySkill(const Skill::Primary::skill_t skill)
{
    switch(skill)
    {
	case Skill::Primary::ATTACK:	++attack; break;
	case Skill::Primary::DEFENCE:	++defence; break;
	case Skill::Primary::POWER:	++power; break;
	case Skill::Primary::KNOWLEDGE:	++knowledge; break;
	default: break;
    }
}

u32 Heroes::GetExperience(void) const
{
    return experience;
}

u16 Heroes::GetSpellPoints(void) const
{
    return magic_point;
}

void Heroes::IncreaseMovePoints(const u16 point)
{
    move_point += point;
}

u16 Heroes::GetMovePoints(void) const
{
    return move_point;
}

void Heroes::SetSpellPoints(const u16 point)
{
    magic_point = point;
}

u16 Heroes::GetMaxSpellPoints(void) const
{
    return 10 * GetKnowledge();
}

u16 Heroes::GetMaxMovePoints(void) const
{
    u16 point = 0;

    // start point
    if(isShipMaster())
    {
	point = 1500;

	switch(GetLevelSkill(Skill::Secondary::NAVIGATION))
	{
	    case Skill::Level::BASIC:       point += 450; break;
    	    case Skill::Level::ADVANCED:    point += 950; break;
    	    case Skill::Level::EXPERT:      point += 1500; break;

    	    default: break;
	}

	// artifact bonus
        if(HasArtifact(Artifact::SAILORS_ASTROLABE_MOBILITY)) point += 1000;

        // visited object
        if(isVisited(MP2::OBJ_LIGHTHOUSE)) point += 500;
    }
    else
    {
    	switch(Monster::GetStats(GetSlowestArmyMonster()).speed)
	{
	    case Speed::CRAWLING:
	    case Speed::VERYSLOW:	point = 1000; break;
	    case Speed::SLOW:		point = 1100; break;
	    case Speed::AVERAGE:	point = 1200; break;
	    case Speed::FAST:		point = 1300; break;
	    case Speed::VERYFAST:	point = 1400; break;
	    case Speed::ULTRAFAST:
	    case Speed::BLAZING:
	    case Speed::INSTANT:	point = 1500; break;
	}

	// skill logistics
	switch(GetLevelSkill(Skill::Secondary::LOGISTICS))
	{
	    // bonus: 10%
	    case Skill::Level::BASIC:       point += point * 10 / 100; break;
    	    // bonus: 20%
    	    case Skill::Level::ADVANCED:    point += point * 20 / 100; break;
    	    // bonus: 29%
    	    case Skill::Level::EXPERT:      point += point * 29 / 100; break;

	    default: break;
	}

	// artifact bonus
	if(HasArtifact(Artifact::NOMAD_BOOTS_MOBILITY)) point += 600;
	if(HasArtifact(Artifact::TRAVELER_BOOTS_MOBILITY)) point += 300;

        // visited object
        if(isVisited(MP2::OBJ_STABLES)) point += 500;
    }

    if(HasArtifact(Artifact::TRUE_COMPASS_MOBILITY)) point += 500;

    return point;
}

Morale::morale_t Heroes::GetMorale(void) const
{
    return GetMoraleWithModificators(NULL);
}

Morale::morale_t Heroes::GetMoraleWithModificators(std::list<std::string> *list) const
{
    s8 result = morale;

    const std::string p1(" +1");
    const std::string p2(" +2");
    const std::string m1(" -1");
    const std::string m2(" -2");

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
		if(list) list->push_back(Artifact::String(*it) + p1);
                break;

            case Artifact::FIZBIN_MISFORTUNE:
                result -= 2;
		if(list) list->push_back(Artifact::String(*it) + m2);
                break;

            default:
		break;
    	}

    // bonus leadership
    switch(GetLevelSkill(Skill::Secondary::LEADERSHIP))
    {
        case Skill::Level::EXPERT:
            result += 3;
	    if(list) list->push_back("Expert Leadership +3");
            break;

        case Skill::Level::ADVANCED:
            result += 2;
	    if(list) list->push_back("Advanced Leadership +2");
            break;

        case Skill::Level::BASIC:
            result += 1;
	    if(list) list->push_back("Basic Leadership +1");
            break;

        default:
            break;
    }
    
    const Castle* castle = inCastle();

    // bonus in castle
    if(castle)
    {
	// and tavern
	if(Race::NECR != castle->GetRace() && castle->isBuild(Castle::BUILD_TAVERN))
	{
	    result += 1;
	    if(list) list->push_back(Castle::GetStringBuilding(Castle::BUILD_TAVERN, castle->GetRace()) + p1);
	}

	// and barbarian coliseum
        if(Race::BARB == castle->GetRace() && castle->isBuild(Castle::BUILD_SPEC))
	{
	    result += 2;
	    if(list) list->push_back(Castle::GetStringBuilding(Castle::BUILD_SPEC, castle->GetRace()) + p2);
	}
    }

    // object visited
    if(isVisited(MP2::OBJ_BUOY))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_BUOY) + p1);
    }
    if(isVisited(MP2::OBJ_OASIS))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_OASIS) + p1);
    }
    if(isVisited(MP2::OBJ_WATERINGHOLE))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_WATERINGHOLE) + p1);
    }
    if(isVisited(MP2::OBJ_TEMPLE))
    {
	result += 2;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_TEMPLE) + p2);
    }
    if(isVisited(MP2::OBJ_GRAVEYARD))
    {
	--result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_GRAVEYARD) + m1);
    }
    if(isVisited(MP2::OBJ_SHIPWRECK))
    {
	--result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_SHIPWRECK) + m1);
    }
    if(isVisited(MP2::OBJ_DERELICTSHIP))
    {
	--result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_DERELICTSHIP) + m1);
    }

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
	case 1:
	    if(0 == count_necr)
	    {
		++result;
		if(list) list->push_back("All " + Race::String(GetRace()) + " in groups +1");
	    }
	    break;
	case 3:
	    --result;
	    if(list) list->push_back("Troops of 3 alignments -1");
	    break;
	case 4:
	    result -= 2;
	    --result;
	    if(list) list->push_back("Troops of 4 alignments -2");
	    break;
	default:
	    result -=3;
	    if(list) list->push_back("Troops of 5 alignments -3");
	    break;
    }

    // undead in life group
    if(count_necr && (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg))
    {
	--result;
	if(list) list->push_back("Some undead in groups -1");
    }

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
    return GetLuckWithModificators(NULL);
}

Luck::luck_t Heroes::GetLuckWithModificators(std::list<std::string> *list) const
{
    s8 result = luck;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    const std::string p1(" +1");
    const std::string p2(" +2");
    const std::string m2(" -2");

    // bonus artifact
    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
    	    case Artifact::RABBIT_FOOT:
            case Artifact::GOLDEN_HORSESHOE:
            case Artifact::GAMBLER_LUCKY_COIN:
            case Artifact::FOUR_LEAF_CLOVER:
	    	++result;
		if(list) list->push_back(Artifact::String(*it) + p1);
		break;

            default:
		break;
    	}

    // bonus luck
    switch(GetLevelSkill(Skill::Secondary::LUCK))
    {
        case Skill::Level::EXPERT:
            result += 3;
	    if(list) list->push_back("Expert Luck +3");
            break;

        case Skill::Level::ADVANCED:
            result += 2;
	    if(list) list->push_back("Advanced Luck +2");
            break;

        case Skill::Level::BASIC:
            result += 1;
	    if(list) list->push_back("Basic Luck +1");
            break;

        default:
            break;
    }

    // object visited
    if(isVisited(MP2::OBJ_FAERIERING))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_FAERIERING) + p1);
    }
    if(isVisited(MP2::OBJ_FOUNTAIN))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_FOUNTAIN) + p1);
    }
    if(isVisited(MP2::OBJ_IDOL))
    {
	++result;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_IDOL) + p1);
    }
    if(isVisited(MP2::OBJ_PYRAMID))
    {
	 result -= 2;
	if(list) list->push_back(MP2::StringObject(MP2::OBJ_PYRAMID) + m2);
    }

    // bonus in castle and sorceress rainbow
    const Castle* castle = inCastle();
    if(castle && Race::SORC == castle->GetRace() && castle->isBuild(Castle::BUILD_SPEC))
    {
	result += 2;
	if(list) list->push_back(Castle::GetStringBuilding(Castle::BUILD_SPEC, castle->GetRace()) + p2);
    }

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
    // increase resource
    Resource::funds_t resource;

    if(HasArtifact(Artifact::ENDLESS_SACK_GOLD))      resource.gold += INCOME_ENDLESS_SACK_GOLD;
    if(HasArtifact(Artifact::ENDLESS_BAG_GOLD))       resource.gold += INCOME_ENDLESS_BAG_GOLD;
    if(HasArtifact(Artifact::ENDLESS_PURSE_GOLD))     resource.gold += INCOME_ENDLESS_PURSE_GOLD;
    if(HasArtifact(Artifact::ENDLESS_POUCH_SULFUR))   resource.sulfur += INCOME_ENDLESS_POUCH_SULFUR;
    if(HasArtifact(Artifact::ENDLESS_VIAL_MERCURY))   resource.mercury += INCOME_ENDLESS_VIAL_MERCURY;
    if(HasArtifact(Artifact::ENDLESS_POUCH_GEMS))     resource.gems += INCOME_ENDLESS_POUCH_GEMS;
    if(HasArtifact(Artifact::ENDLESS_CORD_WOOD))      resource.wood += INCOME_ENDLESS_CORD_WOOD;
    if(HasArtifact(Artifact::ENDLESS_CART_ORE))       resource.ore += INCOME_ENDLESS_CART_ORE;
    if(HasArtifact(Artifact::ENDLESS_POUCH_CRYSTAL))  resource.crystal += INCOME_ENDLESS_POUCH_CRYSTAL;

    // estates skill bonus
    switch(GetLevelSkill(Skill::Secondary::ESTATES))
    {
        case Skill::Level::BASIC:       resource.gold += 100; break;
        case Skill::Level::ADVANCED:    resource.gold += 250; break;
        case Skill::Level::EXPERT:      resource.gold += 500; break;

        default: break;
    }

    if(resource.GetValidItems()) world.GetKingdom(GetColor()).AddFundsResource(resource);

    if(HasArtifact(Artifact::TAX_LIEN))
    {
	resource.Reset();
	resource.gold = 250;
	world.GetKingdom(GetColor()).OddFundsResource(resource);
    }

    // recovery move points
    move_point = GetMaxMovePoints();
    path.Rescan();

    // recovery spell points
    if(spell_book.Active())
    {
	// possible visit arteian spring 2 * max
	u16 prev = magic_point;

	// everyday
	++magic_point;

	if(HasArtifact(Artifact::POWER_RING)) magic_point += 2;

	// secondary skill
	switch(GetLevelSkill(Skill::Secondary::MYSTICISM))
	{
	    case Skill::Level::BASIC:	magic_point += 1; break;
	    case Skill::Level::ADVANCED:magic_point += 2; break;
	    case Skill::Level::EXPERT:	magic_point += 3; break;

	    default: break;
	}

	if((magic_point > GetMaxSpellPoints()) && (magic_point > prev)) magic_point = prev;
    }

    // remove day visit object
    visit_object.remove_if(Visit::IndexObject::isDayLife);
}

void Heroes::ActionNewWeek(void)
{
    // remove week visit object
    visit_object.remove_if(Visit::IndexObject::isWeekLife);
}

void Heroes::ActionNewMonth(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::IndexObject::isMonthLife);
}

void Heroes::ActionAfterBattle(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::IndexObject::isBattleLife);
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
bool Heroes::isVisited(const Maps::Tiles & tile, const Visit::type_t type) const
{
    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(tile);

    std::list<Visit::IndexObject>::const_iterator it1 = visit_object.begin();
    std::list<Visit::IndexObject>::const_iterator it2 = visit_object.end();

    const u16 & index = tile.GetIndex();
    const MP2::object_t object = (tile.GetObject() == MP2::OBJ_HEROES ? GetUnderObject() : tile.GetObject());
    //const MP2::object_t & object = tile.GetObject();

    for(; it1 != it2; ++it1) if(index == (*it1).first && object == (*it1).second) return true;

    return false;
}

/* return true if object visited */
bool Heroes::isVisited(const MP2::object_t & object, const Visit::type_t type) const
{
    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(object);

    std::list<Visit::IndexObject>::const_iterator it1 = visit_object.begin();
    std::list<Visit::IndexObject>::const_iterator it2 = visit_object.end();

    for(; it1 != it2; ++it1) if((*it1).second == object) return true;

    return false;
}

/* set visited cell */
void Heroes::SetVisited(const u16 index, const Visit::type_t type)
{
    const Maps::Tiles & tile = world.GetTiles(index);

    const MP2::object_t object = (tile.GetObject() == MP2::OBJ_HEROES ? GetUnderObject() : tile.GetObject());

    if(Visit::GLOBAL == type)
	world.GetKingdom(color).SetVisited(index, object);
    else
    if(isVisited(tile))
	return;
    else
    if(MP2::OBJ_ZERO != object) visit_object.push_front(Visit::IndexObject(index, object));
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

void Heroes::IncreaseExperience(const u32 exp)
{
    const u8 level_old = GetLevelFromExperience(experience);
    const u8 level_new = GetLevelFromExperience(experience + exp);

    for(u8 ii = 0; ii < level_new - level_old; ++ii) LevelUp();

    experience += exp;
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
	case 11:	return 18500;
	case 12:	return 22100;
	case 13:	return 26400;
	case 14:	return 31600;
	case 15:	return 37800;
	case 16:	return 45300;
	case 17:	return 54200;
	case 18:	return 65000;


	// FIXME:	calculate alghoritm

	default:	return MAXU16;
    }

    return 0;
}

/* buy book */
bool Heroes::BuySpellBook(void)
{
    if(spell_book.Active() || Color::GRAY == color) return false;

    Resource::funds_t payment(Resource::GOLD, BUY_SPELL_BOOK_GOLD);
    Kingdom & kingdom = world.GetKingdom(color);

    std::string header = "To cast spells, you must first buy a spell book for ";
    String::AddInt(header, BUY_SPELL_BOOK_GOLD);
    header += " gold.";

    if( ! kingdom.AllowPayment(payment))
    {
	if(Settings::Get().MyColor() == color)
	Dialog::Message(header, "Unfortunately, you seem to be a little short of cash at the moment.", Font::BIG, Dialog::OK);
	return false;
    }

    if(artifacts.size() >= HEROESMAXARTIFACT)
    {
	if(Settings::Get().MyColor() == color)
	Dialog::Message("You must purchase a spell book to use the mage guild, but you currently have no room for a spell book.", "Try giving one of your artifacts to another hero.", Font::BIG, Dialog::OK);
	return false;
    }

    if(Settings::Get().MyColor() == color)
    {
	const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
	Surface sprite(border.w(), border.h());

	sprite.Blit(border);
	sprite.Blit(AGG::GetICN(ICN::ARTIFACT, Artifact::MAGIC_BOOK + 1), 5, 5);

	if(Dialog::NO == Dialog::SpriteInfo(header, "Do you wish to buy one?", sprite, Dialog::YES | Dialog::NO)) return false;
    }

    kingdom.OddFundsResource(payment);

    spell_book.Activate();
    artifacts.push_back(Artifact::MAGIC_BOOK);
    
    return true;
}

/* add new spell to book from storage */
void Heroes::AppendSpellsToBook(const Spell::Storage & spells)
{
    spell_book.Appends(spells, GetLevelSkill(Skill::Secondary::WISDOM));
}

void Heroes::AppendSpellToBook(const Spell::spell_t spell)
{
    spell_book.Append(spell, GetLevelSkill(Skill::Secondary::WISDOM));
}

/* return true is move enable */
bool Heroes::isEnableMove(void) const
{
    return enable_move && path.EnableMove();
}

/* return true isn allow move to dst tile */
bool Heroes::isAllowMove(const u16 dst_index)
{
    return path.Calculate(dst_index);
}

/* set enable move */
void Heroes::SetMove(bool f)
{
    enable_move = f;
}

void Heroes::SaveUnderObject(MP2::object_t obj)
{
    save_maps_general = obj;
}

MP2::object_t Heroes::GetUnderObject(void) const
{
    return save_maps_general;
}

bool Heroes::isShipMaster(void) const
{
    return shipmaster;
}

void Heroes::SetShipMaster(bool f)
{
    shipmaster = f;
}

bool Heroes::HasSecondarySkill(const Skill::Secondary::skill_t skill) const
{
    return Skill::Level::NONE != GetLevelSkill(skill);
}

u8 Heroes::CountSecondarySkill(void) const
{
    return secondary_skills.size();
}

Skill::Level::type_t Heroes::GetLevelSkill(const Skill::Secondary::skill_t skill) const
{
    std::vector<Skill::Secondary>::const_iterator it1 = secondary_skills.begin();
    std::vector<Skill::Secondary>::const_iterator it2 = secondary_skills.end();
    
    for(; it1 != it2; ++it1)
	if((*it1).Skill() == skill) return (*it1).Level();

    return Skill::Level::NONE;
}

void Heroes::LearnBasicSkill(const Skill::Secondary::skill_t skill)
{
    std::vector<Skill::Secondary>::iterator it1 = secondary_skills.begin();
    std::vector<Skill::Secondary>::const_iterator it2 = secondary_skills.end();

    // find_if
    for(; it1 != it2; ++it1) if((*it1).Skill() == skill) break;

    if(it1 != it2)
	(*it1).SetLevel(Skill::Level::BASIC);
    else
	secondary_skills.push_back(Skill::Secondary(skill, Skill::Level::BASIC));
}

void Heroes::LevelUpSkill(const Skill::Secondary::skill_t skill)
{
    std::vector<Skill::Secondary>::iterator it1 = secondary_skills.begin();
    std::vector<Skill::Secondary>::const_iterator it2 = secondary_skills.end();

    // find_if
    for(; it1 != it2; ++it1) if((*it1).Skill() == skill) break;

    if(it1 != it2)
	(*it1).NextLevel();
    else
	secondary_skills.push_back(Skill::Secondary(skill, Skill::Level::BASIC));
}

void Heroes::Scoute(void)
{
    Maps::ClearFog(mp, (HasArtifact(Artifact::TELESCOPE) ? 1 : 0) + SCOUTINGBASE + GetLevelSkill(Skill::Secondary::SCOUTING), color);
}

bool Heroes::PickupArtifact(const Artifact::artifact_t & art)
{
    //if(HasArtifact(art)) return false; TODO for Message

    if(MaxCountArtifact())
    {
	if(H2Config::MyColor() == color) Dialog::Message("Warning", "You have no room to carry another artifact!", Font::BIG, Dialog::OK);
	return false;
    }

    artifacts.push_back(art);
    
    return true;
}

bool Heroes::MaxCountArtifact(void) const
{
    return HEROESMAXARTIFACT <= artifacts.size();
}

/* set cente from index maps */
void Heroes::SetCenter(const u16 index)
{
    mp.x = index % world.w();
    mp.y = index / world.h();
}

/* return route range in days */
u8 Heroes::GetRangeRouteDays(const u16 dst) const
{
    const u32 max = GetMaxMovePoints();

    // 60 - approximate distance, this restriction calculation
    if(60 < std::abs(mp.x - dst % world.w()) + std::abs(mp.y - dst / world.w()))
    {
	if(Settings::Get().Debug()) Error::Warning("Heroes::GetRangeRouteDays: distance limit");

	return 4;
    }

    Route::Path test(*this);
    // 150 - approximate limit, this restriction path finding algorithm
    if(test.Calculate(dst, 150))
    {
	u32 total = test.TotalPenalty();

	if(move_point >= total) return 1;

	total -= move_point;

	if(max >= total) return 2;

	total -= move_point;

	if(max >= total) return 3;

	total -= move_point;

	if(max >= total) return 4;
    }

    return 4;
}

/* FIXME: algorithm for levelup select secondary skills */
void Heroes::FindSkillsForLevelUp(Skill::Secondary & sec1, Skill::Secondary & sec2) const
{
    // get skill basic or advanced
    std::list<Skill::Secondary::skill_t> noexpert_skills;
    std::vector<Skill::Secondary>::const_iterator it1 = secondary_skills.begin();
    std::vector<Skill::Secondary>::const_iterator it2 = secondary_skills.end();
    for(; it1 != it2; ++it1) if((*it1).Level() != Skill::Level::EXPERT) noexpert_skills.push_back((*it1).Skill());

    // get free skills
    std::list<Skill::Secondary::skill_t> free_skills;
    free_skills.push_back(Skill::Secondary::PATHFINDING);
    free_skills.push_back(Skill::Secondary::ARCHERY);
    free_skills.push_back(Skill::Secondary::LOGISTICS);
    free_skills.push_back(Skill::Secondary::SCOUTING);
    free_skills.push_back(Skill::Secondary::DIPLOMACY);
    free_skills.push_back(Skill::Secondary::NAVIGATION);
    free_skills.push_back(Skill::Secondary::LEADERSHIP);
    free_skills.push_back(Skill::Secondary::WISDOM);
    free_skills.push_back(Skill::Secondary::MYSTICISM);
    free_skills.push_back(Skill::Secondary::LUCK);
    free_skills.push_back(Skill::Secondary::BALLISTICS);
    free_skills.push_back(Skill::Secondary::EAGLEEYE);
    free_skills.push_back(Skill::Secondary::NECROMANCY);
    free_skills.push_back(Skill::Secondary::ESTATES);
    it1 = secondary_skills.begin();
    it2 = secondary_skills.end();
    for(; it1 != it2; ++it1) free_skills.remove((*it1).Skill());

    // get skills
    if((noexpert_skills.empty() && 2 <= free_skills.size()) ||
	secondary_skills.empty())
    {
	sec1.SetSkill(*Rand::Get(free_skills));
	sec1.SetLevel(Skill::Level::BASIC);
	free_skills.remove(sec1.Skill());
	sec2.SetSkill(*Rand::Get(free_skills));
	sec2.SetLevel(Skill::Level::BASIC);
    }
    else
    if(HEROESMAXSKILL == secondary_skills.size() && noexpert_skills.size())
    {
	sec1.SetSkill(*Rand::Get(noexpert_skills));
	sec1.SetLevel(GetLevelSkill(sec1.Skill()));
	sec1.NextLevel();
	if(2 <= noexpert_skills.size())
	{
	    sec2.SetSkill(*Rand::Get(noexpert_skills));
	    sec2.SetLevel(GetLevelSkill(sec2.Skill()));
	    sec2.NextLevel();
	}
    }
    else
    if(noexpert_skills.size() && free_skills.size())
    {
	sec1.SetSkill(*Rand::Get(noexpert_skills));
	sec1.SetLevel(GetLevelSkill(sec1.Skill()));
	sec1.NextLevel();
	sec2.SetSkill(*Rand::Get(free_skills));
	sec2.SetLevel(Skill::Level::BASIC);
    }
}

/* up level */
void Heroes::LevelUp(void)
{
    const Skill::Primary::skill_t primary1 = Skill::Primary::FromLevelUp(race, GetLevel());

    // upgrade primary
    switch(primary1)
    {
	case Skill::Primary::ATTACK:	++attack;
	case Skill::Primary::DEFENCE:	++defence;
	case Skill::Primary::POWER:	++power;
	case Skill::Primary::KNOWLEDGE:	++knowledge;
	default: break;
    }

    Skill::Secondary sec1;
    Skill::Secondary sec2;

    FindSkillsForLevelUp(sec1, sec2);

    std::string header;
    std::string message;

    if(Skill::Secondary::UNKNOWN == sec1.Skill() && Skill::Secondary::UNKNOWN == sec2.Skill())
    {
	if(GetColor() == H2Config::MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = name + " has gained a level.";
	    message = Skill::Primary::String(primary1) + " Skill +1";
	    Dialog::Message(header, message, Font::BIG, Dialog::OK);
	}
    }
    else
    if(Skill::Secondary::UNKNOWN == sec1.Skill())
    {
	if(GetColor() == H2Config::MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = name + " has gained a level. " + Skill::Primary::String(primary1) + " Skill +1";
    	    message = "You have learned " + Skill::Level::String(sec2.Level()) + " " + Skill::Secondary::String(sec2.Skill()) + ".";

	    const Sprite & sprite_frame = AGG::GetICN(ICN::SECSKILL, 15);
    	    Surface sf(sprite_frame.w(), sprite_frame.h());
    	    sf.Blit(sprite_frame);
	    // sprite
	    const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec2.Skill()));
	    sf.Blit(sprite_skill, 3, 3);
	    // text
	    const std::string &name_skill = Skill::Secondary::String(sec2.Skill());
	    Text text_skill(name_skill, Font::SMALL);
	    text_skill.Blit(3 + (sprite_skill.w() - Text::width(name_skill, Font::SMALL)) / 2, 6, sf);
	    const std::string &name_level = Skill::Level::String(sec2.Level());
	    Text text_level(name_level, Font::SMALL);
	    text_level.Blit(3 + (sprite_skill.w() - Text::width(name_level, Font::SMALL)) / 2, sprite_skill.h() - 12, sf);

	    Dialog::SpriteInfo(header, message, sf);
	}
	LevelUpSkill(sec2.Skill());
    }
    else
    if(Skill::Secondary::UNKNOWN == sec2.Skill())
    {
	if(GetColor() == H2Config::MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = name + " has gained a level. " + Skill::Primary::String(primary1) + " Skill +1";
    	    message = "You have learned " + Skill::Level::String(sec1.Level()) + " " + Skill::Secondary::String(sec1.Skill()) + ".";

	    const Sprite & sprite_frame = AGG::GetICN(ICN::SECSKILL, 15);
    	    Surface sf(sprite_frame.w(), sprite_frame.h());
    	    sf.Blit(sprite_frame);
	    // sprite
	    const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec1.Skill()));
	    sf.Blit(sprite_skill, 3, 3);
	    // text
	    const std::string &name_skill = Skill::Secondary::String(sec1.Skill());
	    Text text_skill(name_skill, Font::SMALL);
	    text_skill.Blit(3 + (sprite_skill.w() - Text::width(name_skill, Font::SMALL)) / 2, 6, sf);
	    const std::string &name_level = Skill::Level::String(sec1.Level());
	    Text text_level(name_level, Font::SMALL);
	    text_level.Blit(3 + (sprite_skill.w() - Text::width(name_level, Font::SMALL)) / 2, sprite_skill.h() - 12, sf);

	    Dialog::SpriteInfo(header, message, sf);
	}
	LevelUpSkill(sec1.Skill());
    }
    else
    {
	Skill::Secondary::skill_t skill_select(Skill::Secondary::UNKNOWN);

	if(GetColor() == H2Config::MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = name + " has gained a level. " + Skill::Primary::String(primary1) + " Skill +1";
    	    skill_select = Dialog::LevelUpSelectSkill(header, sec1, sec2);
	}
	// AI select
	else
	{
    	    skill_select = AI::SelectSkill(*this, sec1, sec2);
	}

	LevelUpSkill(skill_select);
    }

    if(H2Config::Debug()) Error::Verbose("Heroes::LevelUp: for " + GetName());
}

/* apply penalty */
void Heroes::ApplyPenaltyMovement(void)
{
    if(path.isValid()) move_point -= path.GetFrontPenalty();
}

/*  true if present monster */
bool Heroes::HasMonster(const Monster::monster_t mon) const
{
    std::vector<Army::Troops>::const_iterator it1 = army.begin();
    std::vector<Army::Troops>::const_iterator it2 = army.end();
    
    for(; it1 != it2; it1++) if(mon == (*it1).Monster()) return true;

    return false;
}

bool Heroes::JoinTroops(const Monster::monster_t mon, const u16 count)
{
    std::vector<Army::Troops>::iterator it1 = army.begin();
    std::vector<Army::Troops>::const_iterator it2 = army.end();
    
    for(; it1 != it2; it1++) if(mon == (*it1).Monster())
    {
	(*it1).SetCount((*it1).Count() + count);
	if(H2Config::Debug()) Error::Verbose("Heroes::JoinTroops: monster: " + Monster::String(mon) + ", count: ", count);
	return true;
    }

    it1 = army.begin();
    if(HEROESMAXARMY > GetCountArmy())
    {
	for(; it1 != it2; it1++) if(Monster::UNKNOWN == (*it1).Monster())
	{
	    (*it1).SetMonster(mon);
	    (*it1).SetCount(count);
	    
	    if(H2Config::Debug()) Error::Verbose("Heroes::JoinTroops: monster: " + Monster::String(mon) + ", count: ", count);
	    return true;
	}
    }

    if(H2Config::MyColor() == GetColor())
	Dialog::Message(Monster::String(mon), "You are unable to recruit at this time, your ranks are full.", Font::BIG, Dialog::OK);

    return false;
}

Monster::monster_t Heroes::GetSlowestArmyMonster(void) const
{
    std::vector<Army::Troops>::const_iterator it = min_element(army.begin(), army.end(), Army::PredicateSlowestTroops);
    return it != army.end() ? (*it).Monster() : Monster::UNKNOWN;
}

Monster::monster_t Heroes::GetHighestArmyMonster(void) const
{
    std::vector<Army::Troops>::const_iterator it = min_element(army.begin(), army.end(), Army::PredicateHighestTroops);
    return it != army.end() ? (*it).Monster() : Monster::UNKNOWN;
}

Monster::monster_t Heroes::GetStrongestArmyMonster(void) const
{
    std::vector<Army::Troops>::const_iterator it = min_element(army.begin(), army.end(), Army::PredicateStrongestTroops);
    return it != army.end() ? (*it).Monster() : Monster::UNKNOWN;
}

Monster::monster_t Heroes::GetWeakestArmyMonster(void) const
{
    std::vector<Army::Troops>::const_iterator it = min_element(army.begin(), army.end(), Army::PredicateWeakestTroops);
    return it != army.end() ? (*it).Monster() : Monster::UNKNOWN;
}

bool Heroes::MayStillMove(void) const
{
    const u16 center = Maps::GetIndexFromAbsPoint(mp);

    return move_point >= Maps::Ground::GetPenalty(center, center, GetLevelSkill(Skill::Secondary::PATHFINDING));
}

void Heroes::SetFreeman(const u8 reason)
{
    // TODO reason: 0 - DISMISS, Army::LOSE, Army::RETREAT, Army::SURRENDER
    color = Color::GRAY;
    world.GetTiles(mp).SetObject(save_maps_general);
    mp.x = -1;
    mp.y = -1;
    path.Reset();
    SetMove(false);
}

void Heroes::Dump(void) const
{
    std::cout << "----------------I--------" << std::endl;
    std::cout << "name            : " << name << std::endl;
    std::cout << "race            : " << Race::String(race) << std::endl;
    std::cout << "color           : " << Color::String(color) << std::endl;
    std::cout << "experience      : " << experience << std::endl;
    std::cout << "magic point     : " << magic_point << std::endl;
    std::cout << "move point      : " << move_point << std::endl;
    std::cout << "direction       : " << Direction::String(direction) << std::endl;
    std::cout << "index sprite    : " << static_cast<u16>(sprite_index) << std::endl;
}
