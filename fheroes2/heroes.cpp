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
#include "monster.h"
#include "error.h"
#include "heroes.h"

Heroes::Heroes(heroes_t ht, Race::race_t rc, const std::string & str) : name(str), experience(0), magic_point(0),
    move_point(0), morale(Morale::NORMAL), luck(Luck::NORMAL), skills(HEROESMAXSKILL),
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

	    skills[0] = Skill::Skill(Skill::LEADERSHIP, Skill::BASIC);
	    skills[1] = Skill::Skill(Skill::BALLISTICS, Skill::BASIC);

	    break;
	    
	case Race::BARB:
            attack              = DEFAULT_BARB_ATTACK;
            defence             = DEFAULT_BARB_DEFENCE;
            power               = DEFAULT_BARB_POWER;
            knowledge           = DEFAULT_BARB_KNOWLEDGE;

	    skills[0] = Skill::Skill(Skill::PATHFINDING, Skill::ADVANCED);
	    break;
	    
	case Race::SORC:
            attack              = DEFAULT_SORC_ATTACK;
            defence             = DEFAULT_SORC_DEFENCE;
            power               = DEFAULT_SORC_POWER;
            knowledge           = DEFAULT_SORC_KNOWLEDGE;

	    skills[0] = Skill::Skill(Skill::NAVIGATION, Skill::ADVANCED);
	    skills[1] = Skill::Skill(Skill::WISDOM, Skill::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            attack              = DEFAULT_WRLK_ATTACK;
            defence             = DEFAULT_WRLK_DEFENCE;
            power               = DEFAULT_WRLK_POWER;
            knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    skills[0] = Skill::Skill(Skill::SCOUTING, Skill::ADVANCED);
	    skills[1] = Skill::Skill(Skill::WISDOM, Skill::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            attack              = DEFAULT_WZRD_ATTACK;
            defence             = DEFAULT_WZRD_DEFENCE;
            power               = DEFAULT_WZRD_POWER;
            knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    skills[0] = Skill::Skill(Skill::WISDOM, Skill::ADVANCED);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            attack              = DEFAULT_NECR_ATTACK;
            defence             = DEFAULT_NECR_DEFENCE;
            power               = DEFAULT_NECR_POWER;
            knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    skills[0] = Skill::Skill(Skill::NECROMANCY, Skill::BASIC);
	    skills[1] = Skill::Skill(Skill::WISDOM, Skill::BASIC);

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

	skills[2] = Skill::Skill(Skill::PATHFINDING, Skill::BASIC);
	skills[3] = Skill::Skill(Skill::LOGISTICS, Skill::BASIC);
	skills[4] = Skill::Skill(Skill::MYSTICISM, Skill::BASIC);
	skills[5] = Skill::Skill(Skill::NAVIGATION, Skill::BASIC);
	skills[6] = Skill::Skill(Skill::LEADERSHIP, Skill::BASIC);
	skills[7] = Skill::Skill(Skill::LUCK, Skill::BASIC);

	artifacts.push_back(Artifact::MEDAL_VALOR);
	artifacts.push_back(Artifact::STEALTH_SHIELD);
	artifacts.push_back(Artifact::DRAGON_SWORD);
	artifacts.push_back(Artifact::RABBIT_FOOT);
	artifacts.push_back(Artifact::ENDLESS_BAG_GOLD);
    }

    magic_point = GetMaxSpellPoints();
    move_point = GetMaxMovePoints();
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

    // save general object
    save_maps_general = world.GetTiles(mp.x, mp.y).GetObject();

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

u16 Heroes::GetMaxMovePoints(void) const
{
    return 10;
}

Morale::morale_t Heroes::GetMorale(void) const
{
/* FIXME:
[-] Knight bonus +1 ???
[-] Buoy visited +1
[-] Oasis visited +1
[-] Temple visited +2
[-] Graveyard robber -1
[-] Shipwreck robber -1
[-] Watering hole visited +1
[-] Derelict ship robber -1
[-] Barbarian Coliseum +2
[-] Tavern +1
[+] All %s troops +1
[+] Troops of 3 alignments -1
[+] Troops of 4 alignments -2
[+] Troops of 5 alignments -3
[+] Medal of Valor +1
[+] Medal of Courage +1
[+] Medal of Honor +1
[+] Medal of Distinction +1
[+] Fizbin of Misfortune -2
[+] Some undead in group -1
[+] Basic Leadership +1
[+] Advanced Leadership +2
[+] Expert Leadership +3
*/

    Morale::morale_t result = morale;

    // bonus artifact
    std::vector<Artifact::artifact_t>::const_iterator it = artifacts.begin();
    for(; it != artifacts.end(); ++it)

	switch(*it)
	{
            case Artifact::MEDAL_VALOR:
            case Artifact::MEDAL_COURAGE:
            case Artifact::MEDAL_HONOR:
            case Artifact::MEDAL_DISTINCTION:
                ++result;
                break;

            case Artifact::FIZBIN_MISFORTUNE:
                --result;
                --result;
                break;

            default:
		break;
    	}

    // bonus leadership
    switch(GetLevelSkill(Skill::LEADERSHIP))
    {
        case Skill::EXPERT:
            ++result;
            ++result;
            ++result;
            break;

        case Skill::ADVANCED:
            ++result;
            ++result;
            break;

        case Skill::BASIC:
            ++result;
            break;

        default:
            break;
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
    for(; it1_army != it2_army; ++it1_army) if(Monster::UNKNOWN != (*it1_army).GetMonster())
	switch(Monster::GetRace((*it1_army).GetMonster()))
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
	case 1: ++result; break;
	case 3: --result; break;
	case 4: --result; --result; break;
	// over 4 different race
	default: --result; --result; --result; break;
    }

    // undead in life group
    if(count_necr && (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg)) --result;



    return result;
}

Luck::luck_t Heroes::GetLuck(void) const
{
    Luck::luck_t result = luck;

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

    // redraw old tile

    // redraw sprite move hero
    // center area maps

    Maps::Tiles & tiles_new = world.GetTiles(ax, ay);

    // save general object
    save_maps_general = tiles_new.GetObject();
    tiles_new.SetObject(MP2::OBJ_HEROES);

    mp.x = ax;
    mp.y = ay;

    // redraw new tile
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
	// FIXME:	calculate alghoritm

	default: return 0;
    }
}

/* get level skill */
Skill::level_t Heroes::GetLevelSkill(const Skill::skill_t & skill) const
{
    if(skills.size())
    {
	std::vector<Skill>::const_iterator it1 = skills.begin();
	std::vector<Skill>::const_iterator it2 = skills.end();
	
	for(; it1 != it2; ++it1) if((*it1).GetSkill() == skill) return (*it1).GetLevel();
    }

    return Skill::NEVER;;
}
