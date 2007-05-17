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

#include "uniq.h"
#include "world.h"
#include "error.h"
#include "heroes.h"

void Skill::SetSkill(u8 skill)
{
    ESTATES >= skill ? pair.first = static_cast<skill_t>(skill) : pair.first = NONE;
}

void Skill::SetLevel(u8 level)
{
    pair.second = static_cast<level_t>((level - 1) % 3);
}

Heroes::Heroes(u32 gid, u16 map_index, const void *ptr, u8 index_name)
    : skills(HEROESMAXSKILL), artifacts(HEROESMAXARTIFACT, Artifact::UNKNOWN), army(HEROESMAXARMY), uniq(gid), mp(map_index % world.w(), map_index / world.h())
{
    // calculate color
    if( 7 > index_name)
	color = Color::BLUE;
    else
    if(14 > index_name)
	color = Color::GREEN;
    else
    if(21 > index_name)
	color = Color::RED;
    else
    if(28 > index_name)
	color = Color::BLUE;
    else
    if(35 > index_name)
	color = Color::YELLOW;
    else
    if(42 > index_name)
	color = Color::ORANGE;
    else
	color = Color::PURPLE;

    // calculate race
    switch(index_name % 7)
    {
	case 0: race = Race::KNGT; break;
	case 1: race = Race::BARB; break;
	case 2: race = Race::SORC; break;
	case 3: race = Race::WRLK; break;
	case 4: race = Race::WZRD; break;
	case 5: race = Race::NECR; break;
	default: race = Race::Rand(); break;
    }

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
    if(*byte8)
    {
	++byte8;

	// male female
	if(BRAX > *byte8) heroes = static_cast<heroes_t>(*byte8);
    }
    else
    {
	heroes = Uniq::GetHeroes(race);
	++byte8;
    }
    ++byte8;

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
    SetDefaultParameters(heroes);

    Error::Verbose("add heroes: " + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

void Heroes::SetDefaultParameters(Heroes::heroes_t heroes)
{
    switch(heroes)
    {
	case LORDKILBURN:	SetRacesValues(Race::KNGT); if(name.empty()) name = "Lord Kilburn"; break;
	case SIRGALLANTH:	SetRacesValues(Race::KNGT); if(name.empty()) name = "Sir Gallanth"; break;
	case ECTOR:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Ector"; break;
	case GVENNETH:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Gvenneth"; break;
	case TYRO:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Tyro"; break;
	case AMBROSE:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Ambrose"; break;
	case RUBY:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Ruby"; break;
	case MAXIMUS:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Maximus"; break;
	case DIMITRY:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Dimitry"; break;
	
	case THUNDAX:		SetRacesValues(Race::BARB); if(name.empty()) name = "Thundax"; break;
	case FINEOUS:		SetRacesValues(Race::BARB); if(name.empty()) name = "Fineous"; break;
	case JOJOSH:		SetRacesValues(Race::BARB); if(name.empty()) name = "Jojosh"; break;
	case CRAGHACK:		SetRacesValues(Race::BARB); if(name.empty()) name = "Crag Hack"; break;
	case JEZEBEL:		SetRacesValues(Race::BARB); if(name.empty()) name = "Jezebel"; break;
	case JACLYN:		SetRacesValues(Race::BARB); if(name.empty()) name = "Jaclyn"; break;
	case ERGON:		SetRacesValues(Race::BARB); if(name.empty()) name = "Ergon"; break;
	case TSABU:		SetRacesValues(Race::BARB); if(name.empty()) name = "Tsabu"; break;
	case ATLAS:		SetRacesValues(Race::BARB); if(name.empty()) name = "Atlas"; break;

        case ASTRA:		SetRacesValues(Race::SORC); if(name.empty()) name = "Astra"; break;
	case NATASHA:		SetRacesValues(Race::SORC); if(name.empty()) name = "Natasha"; break;
	case TROYAN:		SetRacesValues(Race::SORC); if(name.empty()) name = "Troyan"; break;
	case VATAWNA:		SetRacesValues(Race::SORC); if(name.empty()) name = "Vatawna"; break;
	case REBECCA:		SetRacesValues(Race::SORC); if(name.empty()) name = "Rebecca"; break;
	case GEM:		SetRacesValues(Race::SORC); if(name.empty()) name = "Gem"; break;
	case ARIEL:		SetRacesValues(Race::SORC); if(name.empty()) name = "Ariel"; break;
	case CARLAWN:		SetRacesValues(Race::SORC); if(name.empty()) name = "Carlawn"; break;
	case LUNA:		SetRacesValues(Race::SORC); if(name.empty()) name = "Luna"; break;

        case ARIE:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Arie"; break;
	case ALAMAR:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Alamar"; break;
	case VESPER:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Vesper"; break;
	case CRODO:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Crodo"; break;
	case BAROK:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Barok"; break;
	case KASTORE:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Kastore"; break;
	case AGAR:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Agar"; break;
	case FALAGAR:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Falagar"; break;
	case WRATHMONT:		SetRacesValues(Race::WRLK); if(name.empty()) name = "Wrathmont"; break;

        case MYRA:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Myra"; break;
	case FLINT:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Flint"; break;
	case DAWN:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Dawn"; break;
	case HALON:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Halon"; break;
	case MYRINI:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Myrini"; break;
	case WILFREY:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Wilfrey"; break;
	case SARAKIN:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Sarakin"; break;
	case KALINDRA:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Kalindra"; break;
	case MANDIGAL:		SetRacesValues(Race::WZRD); if(name.empty()) name = "Mandigal"; break;

        case ZOM:		SetRacesValues(Race::NECR); if(name.empty()) name = "Zom"; break;
	case DARLANA:		SetRacesValues(Race::NECR); if(name.empty()) name = "Darlana"; break;
	case ZAM:		SetRacesValues(Race::NECR); if(name.empty()) name = "Zan"; break;
	case RANLOO:		SetRacesValues(Race::NECR); if(name.empty()) name = "Ranloo"; break;
	case CHARITY:		SetRacesValues(Race::NECR); if(name.empty()) name = "Charity"; break;
	case RIALDO:		SetRacesValues(Race::NECR); if(name.empty()) name = "Rialdo"; break;
	case ROXANA:		SetRacesValues(Race::NECR); if(name.empty()) name = "Roxana"; break;
	case SANDRO:		SetRacesValues(Race::NECR); if(name.empty()) name = "Sandro"; break;
	case CELIA:		SetRacesValues(Race::NECR); if(name.empty()) name = "Celia"; break;

        case ROLAND:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Roland"; break;
	case UNKNOWN1:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Unknown Person 1"; break;
	case UNKNOWN2:		SetRacesValues(Race::SORC); if(name.empty()) name = "Unknown Person 2"; break;
	case ARCHIBALD:		SetRacesValues(Race::KNGT); if(name.empty()) name = "Archibald"; break;
	case SANDYSANDY:	SetRacesValues(Race::WRLK); if(name.empty()) name = "SandySandy"; break;
	case BRAX:		SetRacesValues(Race::NECR); if(name.empty()) name = "Brax"; break;

	default: Error::Warning("Heroes::SetDefaultParameters: unknown heroes."); break;
    }
}

void Heroes::SetRacesValues(Race::race_t race)
{
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
	    
	default: Error::Warning("Heroes::SetRacesValues: unknown race."); break;
    }
}
