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

#include "artifact.h"
#include "world.h"
#include "castle.h"
#include "config.h"
#include "error.h"
#include "heroes.h"

Heroes::Heroes(heroes_t ht, Race::race_t rc, const std::string & str) : name(str), experience(0), magic_point(0),
    morale(Morale::NORMAL), luck(Luck::NORMAL), skills(HEROESMAXSKILL),
    army(HEROESMAXARMY), heroes(ht), race(rc), army_spread(true), save_maps_general(MP2::OBJ_HEROES)
{
    // hero is freeman
    color = Color::GRAY;

    switch(race)
    {
	case Race::KNGT:
            attack              = DEFAULT_KNGT_ATTACK;
            defence             = DEFAULT_KNGT_DEFENCE;
            power               = DEFAULT_KNGT_POWER;
            knowledge           = DEFAULT_KNGT_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::LEADERSHIP);
	    skills.at(0).SetLevel(Skill::BASIC);
	    skills.at(1).SetSkill(Skill::BALLISTICS);
	    skills.at(1).SetLevel(Skill::BASIC);

	    break;
	    
	case Race::BARB:
            attack              = DEFAULT_BARB_ATTACK;
            defence             = DEFAULT_BARB_DEFENCE;
            power               = DEFAULT_BARB_POWER;
            knowledge           = DEFAULT_BARB_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::PATHFINDING);
	    skills.at(0).SetLevel(Skill::ADVANCED);
	    break;
	    
	case Race::SORC:
            attack              = DEFAULT_SORC_ATTACK;
            defence             = DEFAULT_SORC_DEFENCE;
            power               = DEFAULT_SORC_POWER;
            knowledge           = DEFAULT_SORC_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::NAVIGATION);
	    skills.at(0).SetLevel(Skill::ADVANCED);
	    skills.at(1).SetSkill(Skill::WISDOM);
	    skills.at(1).SetLevel(Skill::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            attack              = DEFAULT_WRLK_ATTACK;
            defence             = DEFAULT_WRLK_DEFENCE;
            power               = DEFAULT_WRLK_POWER;
            knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::SCOUTING);
	    skills.at(0).SetLevel(Skill::ADVANCED);
	    skills.at(1).SetSkill(Skill::WISDOM);
	    skills.at(1).SetLevel(Skill::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            attack              = DEFAULT_WZRD_ATTACK;
            defence             = DEFAULT_WZRD_DEFENCE;
            power               = DEFAULT_WZRD_POWER;
            knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::WISDOM);
	    skills.at(0).SetLevel(Skill::ADVANCED);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            attack              = DEFAULT_NECR_ATTACK;
            defence             = DEFAULT_NECR_DEFENCE;
            power               = DEFAULT_NECR_POWER;
            knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    skills.at(0).SetSkill(Skill::NECROMANCY);
	    skills.at(0).SetLevel(Skill::BASIC);
	    skills.at(1).SetSkill(Skill::WISDOM);
	    skills.at(1).SetLevel(Skill::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	default: Error::Warning("Heroes::Heroes: unknown race."); break;
    }
    
    // set default army
    const Monster::stats_t monster = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));

    army[0].SetMonster(monster.monster);
    army[0].SetCount(monster.grown);

    // set debug param
    if(H2Config::Debug() && SANDYSANDY == heroes)
    {
	army[0].SetMonster(Monster::BLACK_DRAGON);
	army[0].SetCount(2);

        army[1].SetMonster(Monster::RED_DRAGON);
        army[1].SetCount(3);

	skills.at(2).SetSkill(Skill::PATHFINDING);
	skills.at(2).SetLevel(Skill::BASIC);

	skills.at(3).SetSkill(Skill::LOGISTICS);
	skills.at(3).SetLevel(Skill::BASIC);
	
	artifacts.push_back(Artifact::MEDAL_VALOR);
	artifacts.push_back(Artifact::STEALTH_SHIELD);
	artifacts.push_back(Artifact::DRAGON_SWORD);
	artifacts.push_back(Artifact::RABBIT_FOOT);
	artifacts.push_back(Artifact::ENDLESS_BAG_GOLD);
    }
    
    magic_point = GetMaxSpellPoints();
}

void Heroes::LoadFromMP2(u16 map_index, const void *ptr, const Color::color_t cl)
{
    mp.x = map_index % world.w();
    mp.y = map_index / world.h();

    color = cl;

    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    const u32 *byte32 = NULL;
    u16 swap16 = 0;

    // unknown
    ++byte8;

    // custom troops
    bool custom_troops = *byte8;
    if(custom_troops)
    {
        ++byte8;

        // monster1
        army[0].SetMonster(Monster::Monster(*byte8));
        ++byte8;

        // monster2
        army[1].SetMonster(Monster::Monster(*byte8));
        ++byte8;

        // monster3
        army[2].SetMonster(Monster::Monster(*byte8));
        ++byte8;

        // monster4
        army[3].SetMonster(Monster::Monster(*byte8));
        ++byte8;

        // monster5
        army[4].SetMonster(Monster::Monster(*byte8));
        ++byte8;

        // count1
        byte16 = reinterpret_cast<const u16 *>(byte8);
        swap16 = *byte16;
        SWAP16(swap16);
        army[0].SetCount(swap16);
        ++byte16;

        // count2
        swap16 = *byte16;
        SWAP16(swap16);
        army[1].SetCount(swap16);
        ++byte16;

        // count3
        swap16 = *byte16;
        SWAP16(swap16);
        army[2].SetCount(swap16);
        ++byte16;

        // count4
        swap16 = *byte16;
        SWAP16(swap16);
        army[3].SetCount(swap16);
        ++byte16;

        // count5
        swap16 = *byte16;
        SWAP16(swap16);
        army[4].SetCount(swap16);
        ++byte16;

        byte8 = reinterpret_cast<const u8 *>(byte16);
    }
    else
    {
        byte8 += 16;
    }

    // custom portrate
    ++byte8;

    // index sprite portrate
    ++byte8;

    // artifacts
    Artifact::artifact_t artifact = Artifact::UNKNOWN;

    // artifact 1
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*byte8))) artifacts.push_back(artifact);
    ++byte8;

    // artifact 2
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*byte8))) artifacts.push_back(artifact);
    ++byte8;

    // artifact 3
    if(Artifact::UNKNOWN != (artifact = Artifact::Artifact(*byte8))) artifacts.push_back(artifact);
    ++byte8;

    // unknown byte
    ++byte8;

    // experienc
    byte32 = reinterpret_cast<const u32 *>(byte8);
    experience = *byte32;
    SWAP32(experience);
    ++byte32;


    // custom skill
    byte8 = reinterpret_cast<const u8 *>(byte32);
    if(*byte8)
    {
	++byte8;

	// skill 1
	skills.at(0).SetSkill(*byte8);
	++byte8;

	// skill 2
	skills.at(1).SetSkill(*byte8);
	++byte8;

	// skill 3
	skills.at(2).SetSkill(*byte8);
	++byte8;

	// skill 4
	skills.at(3).SetSkill(*byte8);
	++byte8;

	// skill 5
	skills.at(4).SetSkill(*byte8);
	++byte8;

	// skill 6
	skills.at(5).SetSkill(*byte8);
	++byte8;

	// skill 7
	skills.at(6).SetSkill(*byte8);
	++byte8;

	// skill 8
	skills.at(7).SetSkill(*byte8);
	++byte8;
	
	// level skill 1
	skills.at(0).SetLevel(*byte8);
	++byte8;
	
	// level skill 2
	skills.at(1).SetLevel(*byte8);
	++byte8;
	
	// level skill 3
	skills.at(2).SetLevel(*byte8);
	++byte8;
	
	// level skill 4
	skills.at(3).SetLevel(*byte8);
	++byte8;
	
	// level skill 5
	skills.at(4).SetLevel(*byte8);
	++byte8;
	
	// level skill 6
	skills.at(5).SetLevel(*byte8);
	++byte8;
	
	// level skill 7
	skills.at(6).SetLevel(*byte8);
	++byte8;
	
	// level skill 8
	skills.at(7).SetLevel(*byte8);
	++byte8;
    }
    else
    {
	++byte8;

        byte8 += 16;
    }
    
    // unknown
    ++byte8;

    // custom name
    ++byte8;

    //name hero
    if(*byte8) name = std::string(reinterpret_cast<const char *>(byte8));
    
    byte8 += 13;
    
    // patrol
    ++byte8;

    // count square
    ++byte8;

    // end

    Error::Verbose("add heroes: " + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

u8 Heroes::GetMobilityIndexSprite(void) const
{
    // valid range (0 - 25)

    return 25 >= move_point ? static_cast<u8>(move_point) : 25;
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

u16 Heroes::GetMaxSpellPoints(void) const
{
    return 10 * GetKnowledge();
}

Morale::morale_t Heroes::GetMorale(void) const
{
    return morale;
}

Luck::luck_t Heroes::GetLuck(void) const
{
    Luck::luck_t result = luck;

    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
    	    case Artifact::RABBIT_FOOT:		++result; break;
            case Artifact::GOLDEN_HORSESHOE:	++result; break;
            case Artifact::GAMBLER_LUCKY_COIN:	++result; break;
            case Artifact::FOUR_LEAF_CLOVER:	++result; break;

            default: break;
    	}

    return result;
}

// return valid count heroes army
u8 Heroes::GetCountArmy(void) const
{
    u8 result = 0;

    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii) if(army[ii].Valid()) ++result;

    return result;
}

void Heroes::Recrut(const Castle & castle)
{
    mp = castle.GetCenter();
    color = castle.GetColor();

    Maps::Tiles & tiles = world.GetTiles(mp.x, mp.y);

    // save general object
    save_maps_general = tiles.GetObject();
    tiles.SetObject(MP2::OBJ_HEROES);
}

void Heroes::Move(u16 ax, u16 ay)
{
    if(MP2::OBJ_HEROES != save_maps_general)
    {
        Maps::Tiles & tiles_old = world.GetTiles(mp.x, mp.y);

	// restore general object
	tiles_old.SetObject(save_maps_general);
    }

    Maps::Tiles & tiles_new = world.GetTiles(ax, ay);

    // save general object
    save_maps_general = tiles_new.GetObject();
    tiles_new.SetObject(MP2::OBJ_HEROES);
    
    mp.x = ax;
    mp.y = ay;
}

u32 Heroes::GetNextLevelExperience(u8 level) const
{
    switch(level)
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
	
	default: return 0;
    }
}
