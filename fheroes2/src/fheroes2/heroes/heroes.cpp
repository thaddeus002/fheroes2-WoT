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
#include "settings.h"
#include "agg.h"
#include "speed.h"
#include "luck.h"
#include "morale.h"
#include "monster.h"
#include "payment.h"
#include "cursor.h"
#include "kingdom.h"
#include "battle.h"
#include "visit.h"
#include "heroes.h"

extern u16 DialogWithArtifact(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 buttons = Dialog::OK);
extern void PlayPickupSound(void);

const char* HeroesName(Heroes::heroes_t id)
{
    static const char* names[] = {
	// knight
	_("Lord Kilburn"), _("Sir Gallanth"), _("Ector"), _("Gvenneth"), _("Tyro"), _("Ambrose"), _("Ruby"), _("Maximus"), _("Dimitry"),
	// barbarian
	_("Thundax"), _("Fineous"), _("Jojosh"), _("Crag Hack"), _("Jezebel"), _("Jaclyn"), _("Ergon"), _("Tsabu"), _("Atlas"),
	// sorceress
	_("Astra"), _("Natasha"), _("Troyan"), _("Vatawna"), _("Rebecca"), _("Gem"), _("Ariel"), _("Carlawn"), _("Luna"),
	// warlock
	_("Arie"), _("Alamar"), _("Vesper"), _("Crodo"), _("Barok"), _("Kastore"), _("Agar"), _("Falagar"), _("Wrathmont"),
	// wizard
	_("Myra"), _("Flint"), _("Dawn"), _("Halon"), _("Myrini"), _("Wilfrey"), _("Sarakin"), _("Kalindra"), _("Mandigal"),
	// necromant
	_("Zom"), _("Darlana"), _("Zam"), _("Ranloo"), _("Charity"), _("Rialdo"), _("Roxana"), _("Sandro"), _("Celia"),
	// campains
	_("Roland"), _("Lord Corlagon"), _("Sister Eliza"), _("Archibald"), _("Lord Halton"), _("Brother Bax"),
	// loyalty version
	_("Solmyr"), _("Dainwin"), _("Mog"), _("Uncle Ivan"), _("Joseph"), _("Gallavant"), _("Elderian"), _("Ceallach"), _("Drakonia"), _("Martine"), _("Jarkonas"),
	// debug
	"SandySandy", "Unknown" };

    return names[id];
}

Heroes::heroes_t Heroes::ConvertID(u8 index)
{
    return index > UNKNOWN ? UNKNOWN : static_cast<heroes_t>(index);
}

Heroes::Heroes(heroes_t ht, Race::race_t rc) : killer_color(Color::GRAY), experience(0), magic_point(0), move_point(0),
    artifacts(HEROESMAXARTIFACT, Artifact::UNKNOWN), army(this), spell_book(this), portrait(ht), race(rc),
    save_maps_general(MP2::OBJ_ZERO), path(*this), direction(Direction::RIGHT), sprite_index(18)
{
    name = _(HeroesName(ht));

    SetModes(ARMYSPREAD);

    secondary_skills.reserve(HEROESMAXSKILL);

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
	    spell_book.Append(Spell::BLESS, Skill::Level::BASIC);
	    PickupArtifact(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WRLK:
            attack              = DEFAULT_WRLK_ATTACK;
            defence             = DEFAULT_WRLK_DEFENCE;
            power               = DEFAULT_WRLK_POWER;
            knowledge           = DEFAULT_WRLK_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));

	    spell_book.Activate();
	    spell_book.Append(Spell::CURSE, Skill::Level::BASIC);
	    PickupArtifact(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::WZRD:
            attack              = DEFAULT_WZRD_ATTACK;
            defence             = DEFAULT_WZRD_DEFENCE;
            power               = DEFAULT_WZRD_POWER;
            knowledge           = DEFAULT_WZRD_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));

	    spell_book.Activate();
	    spell_book.Append(Spell::STONESKIN, Skill::Level::ADVANCED);
	    PickupArtifact(Artifact::MAGIC_BOOK);
	    break;
	    
	case Race::NECR:
            attack              = DEFAULT_NECR_ATTACK;
            defence             = DEFAULT_NECR_DEFENCE;
            power               = DEFAULT_NECR_POWER;
            knowledge           = DEFAULT_NECR_KNOWLEDGE;

	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));

	    spell_book.Activate();
	    spell_book.Append(Spell::HASTE, Skill::Level::BASIC);
	    PickupArtifact(Artifact::MAGIC_BOOK);
	    break;
	    
	default: Error::Warning("Heroes::Heroes: unknown race."); break;
    }
    
    // set default army
    army.Reset(true);

    // extra hero
    switch(portrait)
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
	    army.Clear();
	    army.JoinTroop(Monster::BLACK_DRAGON, 2);
    	    army.JoinTroop(Monster::RED_DRAGON, 3);

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LOGISTICS, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));

	    PickupArtifact(Artifact::STEALTH_SHIELD);
	    PickupArtifact(Artifact::DRAGON_SWORD);
	    PickupArtifact(Artifact::NOMAD_BOOTS_MOBILITY);
	    PickupArtifact(Artifact::TRAVELER_BOOTS_MOBILITY);
	    PickupArtifact(Artifact::TRUE_COMPASS_MOBILITY);

	    experience = 777;

	    // all spell in magic book
	    for(u8 spell = Spell::FIREBALL; spell < Spell::STONE; ++spell) spell_book.Append(Spell::FromInt(spell), Skill::Level::EXPERT);
	    break;

	default: break;
    }

    magic_point = GetMaxSpellPoints();
    move_point = GetMaxMovePoints();
}

void Heroes::LoadFromMP2(u16 map_index, const void *ptr, const Color::color_t cl)
{
    // reset modes
    modes = 0;

    mp.x = map_index % world.w();
    mp.y = map_index / world.h();

    color = cl;
    killer_color = Color::GRAY;

    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // unknown
    ++ptr8;

    // custom troops
    if(*ptr8)
    {
        ++ptr8;

        // monster1
        army.At(0).SetMonster(Monster::FromInt(*ptr8 + 1));
        ++ptr8;

        // monster2
        army.At(1).SetMonster(Monster::FromInt(*ptr8 + 1));
        ++ptr8;

        // monster3
        army.At(2).SetMonster(Monster::FromInt(*ptr8 + 1));
        ++ptr8;

        // monster4
        army.At(3).SetMonster(Monster::FromInt(*ptr8 + 1));
        ++ptr8;

        // monster5
        army.At(4).SetMonster(Monster::FromInt(*ptr8 + 1));
        ++ptr8;

        // count1
        byte16 = ReadLE16(ptr8);
        army.At(0).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count2
        byte16 = ReadLE16(ptr8);
        army.At(1).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count3
        byte16 = ReadLE16(ptr8);
        army.At(2).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count4
        byte16 = ReadLE16(ptr8);
        army.At(3).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count5
        byte16 = ReadLE16(ptr8);
        army.At(4).SetCount(byte16);
        ++ptr8;
        ++ptr8;
    }
    else
    {
        ptr8 += 16;
    }

    // custom portrate
    if(*ptr8)
    {
	++ptr8;

	// index sprite portrate
	if(Heroes::SANDYSANDY > *ptr8)
	{
	    portrait = static_cast<heroes_t>(*ptr8);
	}

	++ptr8;
    }
    else
	ptr8 += 2;

    // artifacts
    Artifact::artifact_t artifact = Artifact::UNKNOWN;

    // artifact 1
    if(Artifact::UNKNOWN != (artifact = Artifact::FromInt(*ptr8))) PickupArtifact(artifact);
    ++ptr8;

    // artifact 2
    if(Artifact::UNKNOWN != (artifact = Artifact::FromInt(*ptr8))) PickupArtifact(artifact);
    ++ptr8;

    // artifact 3
    if(Artifact::UNKNOWN != (artifact = Artifact::FromInt(*ptr8))) PickupArtifact(artifact);
    ++ptr8;

    // unknown byte
    ++ptr8;

    // experience
    byte32 = ReadLE32(ptr8);
    experience = byte32;
    ptr8 += 4;

    bool skip_levelup_skill = false;
    // custom skill
    if(*ptr8)
    {
	skip_levelup_skill = true;
	++ptr8;

	secondary_skills.clear();

	// skills
	for(u8 ii = 0; ii < 8; ++ii)
	{
	    const Skill::Secondary::skill_t skill = Skill::Secondary::FromMP2(*(ptr8 + ii));
	    const Skill::Level::type_t lvl = Skill::Level::FromMP2(*(ptr8 + ii + 8));

	    if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != lvl) secondary_skills.push_back(Skill::Secondary(skill, lvl));
	}
	ptr8 += 16;
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

    // fix zero army
    if(!army.isValid()) army.Reset(true);

    // level up
    u8 level = GetLevel();
    while(1 < level--)
    {
	Skill::Primary::skill_t primary = LevelUpPrimarySkill();
	if(!skip_levelup_skill) LevelUpSecondarySkill(primary, true);
    }

    // other param
    magic_point = GetMaxSpellPoints();
    move_point = GetMaxMovePoints();

    if(Settings::Get().Debug()) Error::Verbose("add heroes: " + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

Heroes::heroes_t Heroes::GetID(void) const
{
    return portrait;
}

const Point & Heroes::GetCenter(void) const
{ return mp; }

u16 Heroes::GetIndex(void) const
{ return Maps::GetIndexFromAbsPoint(mp); }

u8 Heroes::GetMobilityIndexSprite(void) const
{
    // valid range (0 - 25)
    const u8 index = move_point  < Maps::Ground::GetPenalty(GetIndex(), Direction::CENTER, GetLevelSkill(Skill::Secondary::PATHFINDING)) ? 0 : move_point / 100;
    return 25 >= index ? index : 25;
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

    BagArtifacts::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch((*it).GetID())
	{
	    case Artifact::SPIKED_HELM:
	    case Artifact::THUNDER_MACE:
	    case Artifact::GIANT_FLAIL:
	    case Artifact::SWORD_BREAKER:
		result += 1;
		break;

            case Artifact::SPIKED_SHIELD:
            case Artifact::POWER_AXE:
            case Artifact::LEGENDARY_SCEPTER:
        	result += 2;
		break;
	    
	    case Artifact::DRAGON_SWORD:
	        result += 3;
	        break;
	
	    case Artifact::ULTIMATE_CROWN:
	        result += 4;
	        break;

	    case Artifact::BATTLE_GARB:
	    case Artifact::SWORD_ANDURAN:
	    case Artifact::HOLY_HAMMER:
	        result += 5;
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

    BagArtifacts::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch((*it).GetID())
	{
            case Artifact::SPIKED_HELM:
            case Artifact::ARMORED_GAUNTLETS:
            case Artifact::DEFENDER_HELM:
                result += 1;
                break;

            case Artifact::SPIKED_SHIELD:
            case Artifact::STEALTH_SHIELD:
            case Artifact::LEGENDARY_SCEPTER:
                result += 2;
                break;

            case Artifact::DIVINE_BREASTPLATE:
                result += 3;
                break;

            case Artifact::ULTIMATE_CROWN:
	    case Artifact::SWORD_BREAKER:
                result += 4;
                break;

            case Artifact::BREASTPLATE_ANDURAN:
	    case Artifact::BATTLE_GARB:
                result += 5;
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
    s8 result = power;

    BagArtifacts::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch((*it).GetID())
	{
            case Artifact::BROACH_SHIELDING:
                result += -2;
                break;

            case Artifact::WHITE_PEARL:
                result += 1;
                break;

            case Artifact::BLACK_PEARL:
            case Artifact::CASTER_BRACELET:
            case Artifact::MAGE_RING:
            case Artifact::LEGENDARY_SCEPTER:
                result += 2;
                break;

            case Artifact::WITCHES_BROACH:
            case Artifact::ARM_MARTYR:
                result += 3;
                break;

            case Artifact::ULTIMATE_CROWN:
            case Artifact::ARCANE_NECKLACE:
                result += 4;
                break;

	    case Artifact::BATTLE_GARB:
	    case Artifact::STAFF_WIZARDRY:
	    case Artifact::HELMET_ANDURAN:
	        result += 5;
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

    return result < 0 ? 0 : result;
}

u8 Heroes::GetKnowledge(void) const
{
    u8 result = knowledge;

    BagArtifacts::const_iterator it = artifacts.begin();

    for(; it != artifacts.end(); ++it)

	switch((*it).GetID())
	{
            case Artifact::WHITE_PEARL:
                result += 1;
                break;

            case Artifact::BLACK_PEARL:
            case Artifact::MINOR_SCROLL:
            case Artifact::LEGENDARY_SCEPTER:
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
    	switch(army.GetSlowestTroop().GetSpeed())
	{
	    default: break;
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

s8 Heroes::GetMorale(void) const
{
    return GetMoraleWithModificators(NULL);
}

const std::string & StringModifiers(s8 mod)
{
    static const std::string mods[] = { " 0", " -3", " -2", " -1", " +1", " +2", " +3" };

    switch(mod)
    {
	case -3: return mods[1];
	case -2: return mods[2];
	case -1: return mods[3];
	case  1: return mods[4];
	case  2: return mods[5];
	case  3: return mods[6];

	default: break;
    }

    return mods[0];
}

s8 Heroes::GetMoraleWithModificators(std::string *strs) const
{
    s8 result = Morale::NORMAL;

    std::vector<std::pair<int, s8> > modifiers;
    modifiers.reserve(10);

    modifiers.push_back(std::make_pair(Artifact::MEDAL_VALOR, 1));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_COURAGE, 1));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_HONOR, 1));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_DISTINCTION, 1));
    modifiers.push_back(std::make_pair(Artifact::FIZBIN_MISFORTUNE, -2));
    modifiers.push_back(std::make_pair(Artifact::BATTLE_GARB, 10));
    if(isShipMaster()) modifiers.push_back(std::make_pair(Artifact::MASTHEAD, 1));
    
    // bonus artifact
    BagArtifacts::const_iterator it = artifacts.begin();

    for(u16 i = 0; i < modifiers.size(); i++)
    {
        Artifact::artifact_t art = Artifact::FromInt(modifiers[i].first);
	if(!HasArtifact(art))
	    continue;

        result += modifiers[i].second;
        if(strs)
	{
	    strs->append(Artifact::GetName(art));
	    strs->append(StringModifiers(modifiers[i].second));
	    strs->append("\n");
	}
    }

    modifiers.clear();
    modifiers.push_back(std::make_pair(Skill::Level::BASIC, 1));
    modifiers.push_back(std::make_pair(Skill::Level::ADVANCED, 2));
    modifiers.push_back(std::make_pair(Skill::Level::EXPERT, 3));

    // bonus leadership
    for(u16 i = 0; i < modifiers.size(); i++)
    {
        Skill::Level::type_t level = (Skill::Level::type_t)modifiers[i].first;
        if(GetLevelSkill(Skill::Secondary::LEADERSHIP) != level)
            continue;

        result += modifiers[i].second;
        if(strs)
            strs->append(Skill::Level::String(level) + " " + Skill::Secondary::String(Skill::Secondary::LEADERSHIP) + StringModifiers(modifiers[i].second) + "\n");
        break;
    }

    modifiers.clear();
    modifiers.push_back(std::make_pair(MP2::OBJ_BUOY, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_OASIS, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_WATERINGHOLE, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_TEMPLE, 2));
    modifiers.push_back(std::make_pair(MP2::OBJ_GRAVEYARD, -1));
    modifiers.push_back(std::make_pair(MP2::OBJ_DERELICTSHIP, -1));
    modifiers.push_back(std::make_pair(MP2::OBJ_SHIPWRECK, -1));

    // object visited
    for(u16 i = 0; i < modifiers.size(); i++)
    {
        MP2::object_t obj = (MP2::object_t)modifiers[i].first;
        if(!isVisited(obj))
            continue;

        result += modifiers[i].second;
        if(strs)
            strs->append(MP2::StringObject(obj) + StringModifiers(modifiers[i].second) + "\n");
    }
    
    const Castle* castle = inCastle();
    // check castle morale modificators
    if(castle) result += castle->GetMoraleWithModificators(strs);

    // check from army morale
    result += army.GetMoraleWithModificators(strs);

    // result
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

s8 Heroes::GetLuck(void) const
{
    return GetLuckWithModificators(NULL);
}

s8 Heroes::GetLuckWithModificators(std::string *strs) const
{
    s8 result = Luck::NORMAL;

    BagArtifacts::const_iterator it = artifacts.begin();

    std::vector<std::pair<int, s8> > modifiers;
    modifiers.reserve(5);

    modifiers.push_back(std::make_pair(Artifact::RABBIT_FOOT, 1));
    modifiers.push_back(std::make_pair(Artifact::GOLDEN_HORSESHOE, 1));
    modifiers.push_back(std::make_pair(Artifact::GAMBLER_LUCKY_COIN, 1));
    modifiers.push_back(std::make_pair(Artifact::FOUR_LEAF_CLOVER, 1));
    modifiers.push_back(std::make_pair(Artifact::BATTLE_GARB, 10));
    if(isShipMaster()) modifiers.push_back(std::make_pair(Artifact::MASTHEAD, 1));

    // bonus artifact
    for(u16 i = 0; i < modifiers.size(); i++)
    {
        Artifact::artifact_t art = Artifact::FromInt(modifiers[i].first);
        if(!HasArtifact(art))
            continue;

        result += modifiers[i].second;
        if(strs)
	{
	    strs->append(Artifact::GetName(art));
	    strs->append(StringModifiers(modifiers[i].second));
	    strs->append("\n");
	}
    }

    modifiers.clear();
    modifiers.push_back(std::make_pair(Skill::Level::BASIC, 1));
    modifiers.push_back(std::make_pair(Skill::Level::ADVANCED, 2));
    modifiers.push_back(std::make_pair(Skill::Level::EXPERT, 3));

    // bonus luck
    for(u16 i = 0; i < modifiers.size(); i++)
    {
        Skill::Level::type_t level = (Skill::Level::type_t)modifiers[i].first;
        if(GetLevelSkill(Skill::Secondary::LUCK) != level)
            continue;

        result += modifiers[i].second;
        if(strs)
            strs->append(Skill::Level::String(level) + " " + Skill::Secondary::String(Skill::Secondary::LUCK) + StringModifiers(modifiers[i].second) + "\n");
    }

    modifiers.clear();
    modifiers.push_back(std::make_pair(MP2::OBJ_MERMAID, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_FAERIERING, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_FOUNTAIN, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_IDOL, 1));
    modifiers.push_back(std::make_pair(MP2::OBJ_PYRAMID, -2));
    
    // object visited
    for(u16 i = 0; i < modifiers.size(); i++)
    {
        MP2::object_t obj = (MP2::object_t)modifiers[i].first;
        if(!isVisited(obj))
            continue;

        result += modifiers[i].second;
        if(strs)
            strs->append(MP2::StringObject(obj) + StringModifiers(modifiers[i].second) + "\n");
    }
    
    const Castle* castle = inCastle();
    // check castle morale modificators
    if(castle) result += castle->GetLuckWithModificators(strs);

    // check from army morale
    result += army.GetLuckWithModificators(strs);

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

/* recrut hero */
void Heroes::Recruit(const Color::color_t & cl, const Point & pt)
{
    if(color != Color::GRAY) Error::Warning("Heroes::Recrut: hero not freeman!");

    color = cl;
    killer_color = Color::GRAY;

    mp = pt;

    Maps::Tiles & tiles = world.GetTiles(mp);

    if(!army.isValid()) army.Reset(false);

    // save general object
    save_maps_general = tiles.GetObject();
    tiles.SetObject(MP2::OBJ_HEROES);
}

void Heroes::Recruit(const Castle & castle)
{
    Recruit(castle.GetColor(), castle.GetCenter());

    // learn spell
    if(castle.GetLevelMageGuild()) AppendSpellsToBook(castle.GetMageGuild());
}

void Heroes::ActionNewDay(void)
{
    // increase resource
    Resource::funds_t resource;

    // skip incomes for first day
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
    // stables visited?
    if(isVisited(MP2::OBJ_STABLES)) move_point += 400;

    // recovery spell points
    if(spell_book.isActive())
    {
	// possible visit arteian spring 2 * max
	u16 prev = magic_point;
	const Castle* castle = inCastle();

	// in castle?
	if(castle && castle->GetLevelMageGuild())
	{
	    //restore from mage guild
	    if(prev < GetMaxSpellPoints()) magic_point = GetMaxSpellPoints();
	}
	else
	{
	    // everyday
	    ++magic_point;

	    if(HasArtifact(Artifact::POWER_RING)) magic_point += 2;

	    // secondary skill
	    switch(GetLevelSkill(Skill::Secondary::MYSTICISM))
	    {
		case Skill::Level::BASIC:	magic_point += 1; break;
		case Skill::Level::ADVANCED:	magic_point += 2; break;
		case Skill::Level::EXPERT:	magic_point += 3; break;

		default: break;
	    }

	    if((magic_point > GetMaxSpellPoints()) && (magic_point > prev)) magic_point = prev;
	}
    }

    // remove day visit object
    visit_object.remove_if(Visit::isDayLife);
}

void Heroes::ActionNewWeek(void)
{
    // remove week visit object
    visit_object.remove_if(Visit::isWeekLife);
    
    // fix artesian spring effect
    if(magic_point > GetMaxSpellPoints()) magic_point = GetMaxSpellPoints();
}

void Heroes::ActionNewMonth(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::isMonthLife);
}

void Heroes::ActionAfterBattle(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::isBattleLife);
    //
    SetModes(ACTION);
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

Castle* Heroes::inCastle(void)
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
    const u16 & index = tile.GetIndex();
    const MP2::object_t object = (tile.GetObject() == MP2::OBJ_HEROES ? GetUnderObject() : tile.GetObject());

    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(index, object);

    return visit_object.end() != std::find(visit_object.begin(), visit_object.end(), IndexObject(index, object));
}

/* return true if object visited */
bool Heroes::isVisited(const u8 object, const Visit::type_t type) const
{
    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(object);

    return visit_object.end() != std::find_if(visit_object.begin(), visit_object.end(), std::bind2nd(std::mem_fun_ref(&IndexObject::isObject), object));
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
    if(MP2::OBJ_ZERO != object) visit_object.push_front(IndexObject(index, object));
}

void Heroes::TakeArtifacts(Heroes & hero2)
{
    for(u8 ii = 0; ii < HEROESMAXARTIFACT; ++ii)
    {
        const Artifact & art = hero2.artifacts[ii];
        if(art != Artifact::UNKNOWN && art != Artifact::MAGIC_BOOK)
        {
            if(PickupArtifact(art()))
            hero2.artifacts[ii].Set(Artifact::UNKNOWN);
        }
    }
}

u8 Heroes::GetCountArtifacts(void) const
{
    return std::count_if(artifacts.begin(), artifacts.end(), std::mem_fun_ref(&Artifact::isValid));
}

/* return true if artifact present */
bool Heroes::HasArtifact(const Artifact::artifact_t art) const
{
    return artifacts.end() != std::find(artifacts.begin(), artifacts.end(), art);
}

bool Heroes::HasUltimateArtifact(void) const
{
    return artifacts.end() != std::find_if(artifacts.begin(), artifacts.end(), std::mem_fun_ref(&Artifact::isUltimate));
}

bool Heroes::PickupArtifact(const Artifact::artifact_t art)
{
    BagArtifacts::iterator it = std::find(artifacts.begin(), artifacts.end(), Artifact::UNKNOWN);

    if(artifacts.end() == it)
    {
	if(Settings::Get().MyColor() == color)
	{
	    art == Artifact::MAGIC_BOOK ?
	    Dialog::Message(_("You must purchase a spell book to use the mage guild, but you currently have no room for a spell book."), _("Try giving one of your artifacts to another hero."), Font::BIG, Dialog::OK) :
	    Dialog::Message(Artifact::GetName(art), _("You have no room to carry another artifact!"), Font::BIG, Dialog::OK);
	}
	return false;
    }

    *it = art;

    return true;
}

BagArtifacts & Heroes::GetBagArtifacts(void)
{
    return artifacts;
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

	default:        break;
    }

    const u32 l1 = GetExperienceFromLevel(lvl - 1);
    return (l1 + static_cast<u32>(round((l1 - GetExperienceFromLevel(lvl - 2)) * 1.2 / 100) * 100));
}

/* buy book */
bool Heroes::BuySpellBook(const Castle & castle)
{
    if(spell_book.isActive() || Color::GRAY == color) return false;

    Resource::funds_t payment(Resource::GOLD, BUY_SPELL_BOOK_GOLD);
    Kingdom & kingdom = world.GetKingdom(color);

    std::string header = _("To cast spells, you must first buy a spell book for %{gold} gold");
    String::Replace(header, "%{gold}", BUY_SPELL_BOOK_GOLD);

    if( ! kingdom.AllowPayment(payment))
    {
	if(Settings::Get().MyColor() == color)
	Dialog::Message(header, _("Unfortunately, you seem to be a little short of cash at the moment."), Font::BIG, Dialog::OK);
	return false;
    }

    if(Settings::Get().MyColor() == color)
    {
	const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
	Surface sprite(border.w(), border.h());

	sprite.Blit(border);
	sprite.Blit(AGG::GetICN(ICN::ARTIFACT, Artifact::IndexSprite64(Artifact::MAGIC_BOOK)), 5, 5);

	if(Dialog::NO == Dialog::SpriteInfo(header, _("Do you wish to buy one?"), sprite, Dialog::YES | Dialog::NO)) return false;
    }

    if(PickupArtifact(Artifact::MAGIC_BOOK))
    {
	kingdom.OddFundsResource(payment);
	spell_book.Activate();
	// add all spell to book
	AppendSpellsToBook(castle.GetMageGuild());
	return true;
    }

    return false;
}

/* add new spell to book from storage */
void Heroes::AppendSpellsToBook(const SpellStorage & spells)
{
    if(spell_book.isActive())
    spell_book.Appends(spells, GetLevelSkill(Skill::Secondary::WISDOM));
}

void Heroes::AppendSpellToBook(const Spell::spell_t spell)
{
    spell_book.Append(spell, GetLevelSkill(Skill::Secondary::WISDOM));
}

/* return true is move enable */
bool Heroes::isEnableMove(void) const
{
    return Modes(ENABLEMOVE) && path.isValid() && path.GetFrontPenalty() <= move_point;
}

/* set enable move */
void Heroes::SetMove(bool f)
{
    if(f)
	SetModes(ENABLEMOVE);
    else
    {
	ResetModes(ENABLEMOVE);

	// reset sprite position
	switch(direction)
        {
            case Direction::TOP:            sprite_index = 0; break;
            case Direction::BOTTOM:         sprite_index = 36; break;
            case Direction::TOP_RIGHT:
            case Direction::TOP_LEFT:       sprite_index = 9; break;
            case Direction::BOTTOM_RIGHT:
            case Direction::BOTTOM_LEFT:    sprite_index = 27; break;
            case Direction::RIGHT:
            case Direction::LEFT:           sprite_index = 18; break;
            default: break;
	}
    }
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
    return Modes(SHIPMASTER);
}

void Heroes::SetShipMaster(bool f)
{
    f ? SetModes(SHIPMASTER) : ResetModes(SHIPMASTER);
}

const std::vector<Skill::Secondary> & Heroes::GetSecondarySkills(void) const
{
    return secondary_skills;
}

bool Heroes::HasSecondarySkill(const Skill::Secondary::skill_t skill) const
{
    return Skill::Level::NONE != GetLevelSkill(skill);
}

u8 Heroes::CountSecondarySkill(void) const
{
    return secondary_skills.size();
}

u8 Heroes::GetLevelSkill(const Skill::Secondary::skill_t skill) const
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
    Maps::ClearFog(GetIndex(), GetScoute(), color);
}

u8 Heroes::GetScoute(void) const
{
    return (HasArtifact(Artifact::TELESCOPE) ? 1 : 0) + SCOUTINGBASE + GetLevelSkill(Skill::Secondary::SCOUTING);
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
    const u16 limit = max * 5 / 100; // limit ~5 day

    // approximate distance, this restriction calculation
    if((4 * max / 100) < Maps::GetApproximateDistance(GetIndex(), dst))
    {
	if(Settings::Get().Debug()) Error::Warning("Heroes::GetRangeRouteDays: distance limit");
	return 0;
    }

    Route::Path test(*this);
    // approximate limit, this restriction path finding algorithm
    if(test.Calculate(dst, limit))
    {
	u32 total = test.TotalPenalty();
	if(move_point >= total) return 1;

	total -= move_point;
	if(max >= total) return 2;

	total -= move_point;
	if(max >= total) return 3;

	return 4;
    }
    else
    if(Settings::Get().Debug()) Error::Warning("Heroes::GetRangeRouteDays: iteration limit: ", limit);

    return 0;
}

/* select secondary skills for level up */
void Heroes::FindSkillsForLevelUp(Skill::Secondary & sec1, Skill::Secondary & sec2) const
{
    std::vector<Skill::Secondary::skill_t> exclude_skills;
    exclude_skills.reserve(MAXSECONDARYSKILL);

    std::vector<Skill::Secondary>::const_iterator it1 = secondary_skills.begin();
    std::vector<Skill::Secondary>::const_iterator it2 = secondary_skills.end();
    for(; it1 != it2; ++it1) if((*it1).Level() == Skill::Level::EXPERT) exclude_skills.push_back((*it1).Skill());

    // is full, add other.
    if(HEROESMAXSKILL <= secondary_skills.size())
    {
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::PATHFINDING)) exclude_skills.push_back(Skill::Secondary::PATHFINDING);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::ARCHERY)) exclude_skills.push_back(Skill::Secondary::ARCHERY);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::LOGISTICS)) exclude_skills.push_back(Skill::Secondary::LOGISTICS);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::SCOUTING)) exclude_skills.push_back(Skill::Secondary::SCOUTING);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::DIPLOMACY)) exclude_skills.push_back(Skill::Secondary::DIPLOMACY);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::NAVIGATION)) exclude_skills.push_back(Skill::Secondary::NAVIGATION);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::LEADERSHIP)) exclude_skills.push_back(Skill::Secondary::LEADERSHIP);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::WISDOM)) exclude_skills.push_back(Skill::Secondary::WISDOM);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::MYSTICISM)) exclude_skills.push_back(Skill::Secondary::MYSTICISM);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::LUCK)) exclude_skills.push_back(Skill::Secondary::LUCK);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::BALLISTICS)) exclude_skills.push_back(Skill::Secondary::BALLISTICS);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::EAGLEEYE)) exclude_skills.push_back(Skill::Secondary::EAGLEEYE);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::NECROMANCY)) exclude_skills.push_back(Skill::Secondary::NECROMANCY);
	if(Skill::Level::NONE == GetLevelSkill(Skill::Secondary::ESTATES)) exclude_skills.push_back(Skill::Secondary::ESTATES);
    }

    sec1.SetSkill(Skill::Secondary::PriorityFromRace(GetRace(), exclude_skills));
    exclude_skills.push_back(sec1.Skill());
    sec2.SetSkill(Skill::Secondary::PriorityFromRace(GetRace(), exclude_skills));

    sec1.SetLevel(GetLevelSkill(sec1.Skill()));
    sec2.SetLevel(GetLevelSkill(sec2.Skill()));

    sec1.NextLevel();
    sec2.NextLevel();
}

/* up level */
void Heroes::LevelUp(bool autoselect)
{
    LevelUpSecondarySkill(LevelUpPrimarySkill(), autoselect);
}

Skill::Primary::skill_t Heroes::LevelUpPrimarySkill(void)
{
    const Skill::Primary::skill_t primary1 = Skill::Primary::FromLevelUp(race, GetLevel());

    // upgrade primary
    switch(primary1)
    {
	case Skill::Primary::ATTACK:	++attack; break;
	case Skill::Primary::DEFENCE:	++defence; break;
	case Skill::Primary::POWER:	++power; break;
	case Skill::Primary::KNOWLEDGE:	++knowledge; break;
	default: break;
    }
    if(Settings::Get().Debug()) Error::Verbose("Heroes::LevelUpPrimarySkill: for " + GetName());

    return primary1;
}

void Heroes::LevelUpSecondarySkill(const Skill::Primary::skill_t primary1, bool autoselect)
{
    Skill::Secondary sec1;
    Skill::Secondary sec2;

    FindSkillsForLevelUp(sec1, sec2);
    if(1 < Settings::Get().Debug()) Error::Verbose("Heroes::LevelUp: " + Skill::Secondary::String(sec1.Skill()) + " or " + Skill::Secondary::String(sec2.Skill()));

    std::string header;
    std::string message;

    if(Skill::Secondary::UNKNOWN == sec1.Skill() && Skill::Secondary::UNKNOWN == sec2.Skill())
    {
	if(!autoselect && GetColor() == Settings::Get().MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = _("%{name} has gained a level.");
	    String::Replace(header, "%{name}", name);
	    message = _("%{skill} Skill +1");
	    String::Replace(message, "%{skill}", Skill::Primary::String(primary1));
	    Dialog::Message(header, message, Font::BIG, Dialog::OK);
	}
    }
    else
    if(Skill::Secondary::UNKNOWN == sec1.Skill())
    {
	if(!autoselect && GetColor() == Settings::Get().MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = _("%{name} has gained a level. %{skill} Skill +1");
	    String::Replace(header, "%{name}", name);
	    String::Replace(header, "%{skill}", Skill::Primary::String(primary1));
    	    message = _("You have learned %{level} %{skill}.");
	    String::Replace(message, "%{level}", Skill::Level::String(sec2.Level()));
	    String::Replace(message, "%{skill}", Skill::Secondary::String(sec2.Skill()));

	    const Sprite & sprite_frame = AGG::GetICN(ICN::SECSKILL, 15);
    	    Surface sf(sprite_frame.w(), sprite_frame.h());
    	    sf.Blit(sprite_frame);
	    // sprite
	    const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec2.Skill()));
	    sf.Blit(sprite_skill, 3, 3);
	    // text
	    const std::string &name_skill = Skill::Secondary::String(sec2.Skill());
	    Text text_skill(name_skill, Font::SMALL);
	    text_skill.Blit(3 + (sprite_skill.w() - text_skill.w()) / 2, 6, sf);
	    const std::string &name_level = Skill::Level::String(sec2.Level());
	    Text text_level(name_level, Font::SMALL);
	    text_level.Blit(3 + (sprite_skill.w() - text_level.w()) / 2, sprite_skill.h() - 12, sf);

	    Dialog::SpriteInfo(header, message, sf);
	}
	LevelUpSkill(sec2.Skill());
    }
    else
    if(Skill::Secondary::UNKNOWN == sec2.Skill())
    {
	if(!autoselect && GetColor() == Settings::Get().MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = _("%{name} has gained a level. %{skill} Skill +1");
	    String::Replace(header, "%{name}", name);
	    String::Replace(header, "%{skill}", Skill::Primary::String(primary1));
    	    message = _("You have learned %{level} %{skill}.");
	    String::Replace(message, "%{level}", Skill::Level::String(sec1.Level()));
	    String::Replace(message, "%{skill}", Skill::Secondary::String(sec1.Skill()));

	    const Sprite & sprite_frame = AGG::GetICN(ICN::SECSKILL, 15);
    	    Surface sf(sprite_frame.w(), sprite_frame.h());
    	    sf.Blit(sprite_frame);
	    // sprite
	    const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec1.Skill()));
	    sf.Blit(sprite_skill, 3, 3);
	    // text
	    const std::string &name_skill = Skill::Secondary::String(sec1.Skill());
	    Text text_skill(name_skill, Font::SMALL);
	    text_skill.Blit(3 + (sprite_skill.w() - text_skill.w()) / 2, 6, sf);
	    const std::string &name_level = Skill::Level::String(sec1.Level());
	    Text text_level(name_level, Font::SMALL);
	    text_level.Blit(3 + (sprite_skill.w() - text_level.w()) / 2, sprite_skill.h() - 12, sf);

	    Dialog::SpriteInfo(header, message, sf);
	}
	LevelUpSkill(sec1.Skill());
    }
    else
    {
	Skill::Secondary::skill_t skill_select(Skill::Secondary::UNKNOWN);

	if(!autoselect && GetColor() == Settings::Get().MyColor())
	{
	    AGG::PlaySound(M82::NWHEROLV);
	    header = _("%{name} has gained a level. %{skill} Skill +1");
	    String::Replace(header, "%{name}", name);
	    String::Replace(header, "%{skill}", Skill::Primary::String(primary1));
    	    skill_select = Dialog::LevelUpSelectSkill(header, sec1, sec2);
	}
	// AI select
	else
	{
    	    skill_select = (Rand::Get(0, 1) ? sec1.Skill() : sec2.Skill());
	}

	LevelUpSkill(skill_select);
    }

    if(Settings::Get().Debug()) Error::Verbose("Heroes::LevelUpSecondarySkill: for " + GetName());
}

/* apply penalty */
bool Heroes::ApplyPenaltyMovement(void)
{
    const u16 center = Maps::GetIndexFromAbsPoint(mp);

    const u16 penalty = path.isValid() ?
	    path.GetFrontPenalty() :
	    Maps::Ground::GetPenalty(center, Direction::CENTER, GetLevelSkill(Skill::Secondary::PATHFINDING));

    if(move_point >= penalty) move_point -= penalty;
    else return false;

    return true;
}

bool Heroes::MayStillMove(void) const
{
    if(Modes(STUPID) || isFreeman()) return false;

    if(path.isValid()) return move_point >= path.GetFrontPenalty();
    return move_point >= Maps::Ground::GetPenalty(Maps::GetIndexFromAbsPoint(mp), Direction::CENTER, GetLevelSkill(Skill::Secondary::PATHFINDING));
}

bool Heroes::isFreeman(void) const
{
    return Color::GRAY == color && !Modes(JAIL);
}

void Heroes::SetFreeman(const u8 reason)
{
    if(isFreeman()) return;

    if(Army::RETREAT == reason || Army::SURRENDER == reason) world.GetKingdom(color).GetRecruits().SetHero2(this);

    if(!army.isValid() || Army::LOSE == reason || Army::RETREAT == reason) army.Reset(false);
    else
    if(Army::LOSE == reason) army.Reset(true);

    color = Color::GRAY;
    world.GetTiles(mp).SetObject(save_maps_general);
    modes = 0;
    mp.x = -1;
    mp.y = -1;
    path.Reset();
    SetMove(false);
    SetModes(ACTION);
}

bool Heroes::isShow(u8 color)
{
    const u16 index_from = GetIndex();
    const Maps::Tiles & tile_from = world.GetTiles(index_from);

    if(path.isValid())
    {
        const u16 index_to = Maps::GetDirectionIndex(index_from, path.GetFrontDirection());
        const Maps::Tiles & tile_to = world.GetTiles(index_to);

        return !tile_from.isFog(color) && !tile_to.isFog(color);
    }

    return !tile_from.isFog(color);
}

const Surface & Heroes::GetPortrait30x22(void) const
{
    if(Heroes::SANDYSANDY > portrait) return AGG::GetICN(ICN::MINIPORT, portrait);
    else
    if(Heroes::SANDYSANDY == portrait) return AGG::GetICN(ICN::MINIPORT, BAX);

    return AGG::GetICN(ICN::MINIPORT, 0);
}

const Surface & Heroes::GetPortrait50x46(void) const
{
    if(Heroes::SANDYSANDY > portrait) return AGG::GetICN(ICN::PORTMEDI, portrait + 1);
    else
    if(Heroes::SANDYSANDY == portrait) return AGG::GetICN(ICN::PORTMEDI, BAX + 1);

    return AGG::GetICN(ICN::PORTMEDI, 0);
}

const Surface & Heroes::GetPortrait101x93(void) const
{
    ICN::icn_t icn = ICN::PORTxxxx(portrait);

    return AGG::GetICN(ICN::UNKNOWN != icn ? icn : ICN::PORT0000, 0);
}

void Heroes::SetKillerColor(Color::color_t c)
{
    killer_color = c;
}

Color::color_t Heroes::GetKillerColor(void) const
{
    return killer_color;
}

void Heroes::Dump(void) const
{
    std::cout << "name            : " << name << std::endl;
    std::cout << "race            : " << Race::String(race) << std::endl;
    std::cout << "color           : " << Color::String(color) << std::endl;
    std::cout << "experience      : " << experience << std::endl;
    std::cout << "magic point     : " << magic_point << std::endl;
    std::cout << "position x      : " << mp.x << std::endl;
    std::cout << "position y      : " << mp.y << std::endl;
    std::cout << "move point      : " << move_point << std::endl;
    std::cout << "max magic point : " << GetMaxSpellPoints() << std::endl;
    std::cout << "max move point  : " << GetMaxMovePoints() << std::endl;
    std::cout << "direction       : " << Direction::String(direction) << std::endl;
    std::cout << "index sprite    : " << static_cast<u16>(sprite_index) << std::endl;
    std::cout << "flags           : " << (Modes(SHIPMASTER) ? "SHIPMATER," : ",") <<
                                         (Modes(SCOUTER) ? "SCOUTER," : ",") <<
                                         (Modes(HUNTER) ? "HUNTER," : ",") <<
                                         (Modes(STUPID) ? "STUPID," : ",") << std::endl;
}
