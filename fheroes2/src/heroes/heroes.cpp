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
    move_point(0), army(HEROESMAXARMY), heroes(ht), race(rc), army_spread(true), save_maps_general(MP2::OBJ_ZERO), path(*this)
{
    // hero is freeman
    color = Color::GRAY;

    switch(race)
    {
	case Race::KNGT:
            primary_skills.attack      = DEFAULT_KNGT_ATTACK;
            primary_skills.defence     = DEFAULT_KNGT_DEFENCE;
            primary_skills.power       = DEFAULT_KNGT_POWER;
            primary_skills.knowledge   = DEFAULT_KNGT_KNOWLEDGE;

	    secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::BALLISTICS, Skill::Level::BASIC);
	    break;
	    
	case Race::BARB:
            primary_skills.attack              = DEFAULT_BARB_ATTACK;
            primary_skills.defence             = DEFAULT_BARB_DEFENCE;
            primary_skills.power               = DEFAULT_BARB_POWER;
            primary_skills.knowledge           = DEFAULT_BARB_KNOWLEDGE;

	    secondary_skills.Level(Skill::PATHFINDING, Skill::Level::ADVANCED);
	    break;
	    
	case Race::SORC:
            primary_skills.attack              = DEFAULT_SORC_ATTACK;
            primary_skills.defence             = DEFAULT_SORC_DEFENCE;
            primary_skills.power               = DEFAULT_SORC_POWER;
            primary_skills.knowledge           = DEFAULT_SORC_KNOWLEDGE;

	    secondary_skills.Level(Skill::NAVIGATION, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            primary_skills.attack              = DEFAULT_WRLK_ATTACK;
            primary_skills.defence             = DEFAULT_WRLK_DEFENCE;
            primary_skills.power               = DEFAULT_WRLK_POWER;
            primary_skills.knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    secondary_skills.Level(Skill::SCOUTING, Skill::Level::ADVANCED);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            primary_skills.attack              = DEFAULT_WZRD_ATTACK;
            primary_skills.defence             = DEFAULT_WZRD_DEFENCE;
            primary_skills.power               = DEFAULT_WZRD_POWER;
            primary_skills.knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    secondary_skills.Level(Skill::WISDOM, Skill::Level::ADVANCED);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            primary_skills.attack              = DEFAULT_NECR_ATTACK;
            primary_skills.defence             = DEFAULT_NECR_DEFENCE;
            primary_skills.power               = DEFAULT_NECR_POWER;
            primary_skills.knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    secondary_skills.Level(Skill::NECROMANCY, Skill::Level::BASIC);
	    secondary_skills.Level(Skill::WISDOM, Skill::Level::BASIC);

	    artifacts.push_back(Artifact::MAGIC_BOOK);
	    break;
	    
	default: Error::Warning("Heroes::Heroes: unknown race."); break;
    }
    
    // set default army
    const Monster::stats_t monster = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));

    army[0].Set(monster.monster, monster.grown);

    // set debug param
    if(H2Config::Debug() && SANDYSANDY == heroes)
    {
	army[0].Set(Monster::BLACK_DRAGON, 2);

        army[1].Set(Monster::RED_DRAGON, 3);

	// path finding audit
	//secondary_skills.Level(Skill::PATHFINDING, Skill::Level::BASIC);
	secondary_skills.Level(Skill::LOGISTICS, Skill::Level::BASIC);
	secondary_skills.Level(Skill::MYSTICISM, Skill::Level::BASIC);
	secondary_skills.Level(Skill::NAVIGATION, Skill::Level::BASIC);
	secondary_skills.Level(Skill::LEADERSHIP, Skill::Level::BASIC);
	secondary_skills.Level(Skill::LUCK, Skill::Level::BASIC);

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
        army[0].monster = Monster::Monster(*ptr8);
        ++ptr8;

        // monster2
        army[1].monster = Monster::Monster(*ptr8);
        ++ptr8;

        // monster3
        army[2].monster = Monster::Monster(*ptr8);
        ++ptr8;

        // monster4
        army[3].monster = Monster::Monster(*ptr8);
        ++ptr8;

        // monster5
        army[4].monster = Monster::Monster(*ptr8);
        ++ptr8;

        // count1
        LOAD16(ptr8, byte16);
        army[0].count = byte16;
        ++ptr8;
        ++ptr8;

        // count2
        LOAD16(ptr8, byte16);
        army[1].count = byte16;
        ++ptr8;
        ++ptr8;

        // count3
        LOAD16(ptr8, byte16);
        army[2].count = byte16;
        ++ptr8;
        ++ptr8;

        // count4
        LOAD16(ptr8, byte16);
        army[3].count = byte16;
        ++ptr8;
        ++ptr8;

        // count5
        LOAD16(ptr8, byte16);
        army[4].count = byte16;
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

	// skills
	for(u8 ii = 0; ii < 8; ++ii)
	    secondary_skills.Level(Skill::Secondary::FromMP2(*ptr8 + ii), Skill::Level::FromMP2(*(ptr8 + ii + 8)));

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
    u8 result = primary_skills.attack;

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
    u8 result = primary_skills.defence;

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
    u8 result = primary_skills.power;

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
    u8 result = primary_skills.knowledge;

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
    // use Logistics

    return 10;		// FIXME: calculate move points
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

    Morale::morale_t result = primary_skills.morale;

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
        case Skill::Level::EXPERT:
            ++result;
            ++result;
            ++result;
            break;

        case Skill::Level::ADVANCED:
            ++result;
            ++result;
            break;

        case Skill::Level::BASIC:
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
    for(; it1_army != it2_army; ++it1_army) if(Monster::UNKNOWN != (*it1_army).monster)
	switch(Monster::GetRace((*it1_army).monster))
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
    Luck::luck_t result = primary_skills.luck;

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

    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii) if(Army::isValid(army[ii])) ++result;

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

void Heroes::ActionNewDay(void)
{
    move_point = GetMaxMovePoints();
    path.Reset();
}

void Heroes::ActionNewWeek(void)
{
}

void Heroes::ActionNewMonth(void)
{
}

u16 Heroes::FindPath(u16 dst_index)
{
    return path.Calculate(dst_index);
}

void Heroes::Goto(u16 dst_index)
{
    if(MP2::OBJ_HEROES != save_maps_general)
    {
        Maps::Tiles & tiles_old = world.GetTiles(mp.x, mp.y);

	// restore general object
	tiles_old.SetObject(save_maps_general);
	
	// redraw old tile
	tiles_old.Redraw();
    }


    // redraw sprite move hero
    // center area maps

    Maps::Tiles & tiles_new = world.GetTiles(dst_index);

    // save general object
    save_maps_general = tiles_new.GetObject();
    tiles_new.SetObject(MP2::OBJ_HEROES);

    mp.x = dst_index % world.w();
    mp.y = dst_index / world.w();

    //Game::globalfocus.center = mp;

    // redraw new tile
    tiles_new.Redraw();

    if(H2Config::Debug()) Error::Verbose("Heroes::Goto: ", dst_index);
}

void Heroes::Action(u16 dst_index)
{
    Maps::Tiles & tiles_new = world.GetTiles(dst_index);

    if(H2Config::Debug()) Error::Verbose("Heroes::Action: " + std::string(MP2::StringObject(tiles_new.GetObject())));
}
