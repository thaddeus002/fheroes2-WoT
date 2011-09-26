/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include <functional>
#include "agg.h"
#include "settings.h"
#include "payment.h"
#include "world.h"
#include "kingdom.h"
#include "maps_tiles.h"
#include "text.h"
#include "luck.h"
#include "morale.h"
#include "speed.h"
#include "castle.h"
#include "heroes.h"
#include "race.h"
#include "battle_stats.h"
#include "tools.h"
#include "army.h"

enum armysize_t
{
    ARMY_FEW     = 1,
    ARMY_SEVERAL = 5,
    ARMY_PACK    = 10,
    ARMY_LOTS    = 20,
    ARMY_HORDE   = 50,
    ARMY_THRONG  = 100,
    ARMY_SWARM   = 250,
    ARMY_ZOUNDS  = 500,
    ARMY_LEGION  = 1000
};

armysize_t ArmyGetSize(u32 count)
{
    if(ARMY_LEGION <= count)	return ARMY_LEGION;
    else
    if(ARMY_ZOUNDS <= count)	return ARMY_ZOUNDS;
    else
    if(ARMY_SWARM <= count)	return ARMY_SWARM;
    else
    if(ARMY_THRONG <= count)	return ARMY_THRONG;
    else
    if(ARMY_HORDE <= count)	return ARMY_HORDE;
    else
    if(ARMY_LOTS <= count)	return ARMY_LOTS;
    else
    if(ARMY_PACK <= count)	return ARMY_PACK;
    else
    if(ARMY_SEVERAL <= count)	return ARMY_SEVERAL;

    return ARMY_FEW;
}

std::string Army::TroopSizeString(const Troop & troop)
{
    std::string str;

    switch(ArmyGetSize(troop.GetCount()))
    {
        default:		str = _("A few\n%{monster}"); break;
        case ARMY_SEVERAL:	str = _("Several\n%{monster}"); break;
        case ARMY_PACK:		str = _("A pack of\n%{monster}"); break;
        case ARMY_LOTS:		str = _("Lots of\n%{monster}"); break;
        case ARMY_HORDE:	str = _("A horde of\n%{monster}"); break;
        case ARMY_THRONG:	str = _("A throng of\n%{monster}"); break;
        case ARMY_SWARM:	str = _("A swarm of\n%{monster}"); break;
        case ARMY_ZOUNDS:	str = _("Zounds of\n%{monster}"); break;
        case ARMY_LEGION:	str = _("A legion of\n%{monster}"); break;
    }

    String::Replace(str, "%{monster}", String::Lower(troop.GetMultiName()));
    return str;
}

std::string Army::SizeString(u32 size)
{
    const char* str_size[] = { _("army|Few"), _("army|Several"), _("army|Pack"), _("army|Lots"),
		    _("army|Horde"), _("army|Throng"), _("army|Swarm"), _("army|Zounds"), _("army|Legion") };

    switch(ArmyGetSize(size))
    {
        default: break;
        case ARMY_SEVERAL:	return str_size[1];
        case ARMY_PACK:		return str_size[2];
        case ARMY_LOTS:		return str_size[3];
        case ARMY_HORDE:	return str_size[4];
        case ARMY_THRONG:	return str_size[5];
        case ARMY_SWARM:	return str_size[6];
        case ARMY_ZOUNDS:	return str_size[7];
        case ARMY_LEGION:	return str_size[8];
    }

    return str_size[0];
}

Army::army_t::army_t(HeroBase* s) : troops(ARMYMAXTROOPS), commander(s), combat_format(FORMAT_SPREAD), color(Color::NONE)
{
    for(Troops::iterator
	it = troops.begin(); it != troops.end(); ++it)
	(*it).SetArmy(this);
}

Army::army_t::army_t(const army_t & a) : troops(ARMYMAXTROOPS), commander(NULL), combat_format(FORMAT_SPREAD), color(Color::NONE)
{
    for(Troops::iterator
	it = troops.begin(); it != troops.end(); ++it)
	(*it).SetArmy(this);

    Import(a.troops);
}

void Army::army_t::FromGuardian(const Maps::Tiles & t)
{
    Reset();

    if(MP2::isCaptureObject(t.GetObject()))
	color = world.ColorCapturedObject(t.GetIndex());

    switch(t.GetObject(false))
    {
	case MP2::OBJ_PYRAMID:
            troops[0].Set(Monster::ROYAL_MUMMY, 10);
            troops[1].Set(Monster::VAMPIRE_LORD, 10);
            troops[2].Set(Monster::ROYAL_MUMMY, 10);
            troops[3].Set(Monster::VAMPIRE_LORD, 10);
            troops[4].Set(Monster::ROYAL_MUMMY, 10);
	    break;

	case MP2::OBJ_GRAVEYARD:
	    troops[0].Set(Monster::MUTANT_ZOMBIE, 100);
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_SHIPWRECK:
	    troops[0].Set(Monster::GHOST, t.GetQuantity2());
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_DERELICTSHIP:
	    troops[0].Set(Monster::SKELETON, 200);
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_ARTIFACT:
	    switch(t.GetQuantity2())
	    {
		case 6:	troops[0].Set(Monster::ROGUE, 50); break;	
		case 7:	troops[0].Set(Monster::GENIE, 1); break;	
		case 8:	troops[0].Set(Monster::PALADIN, 1); break;	
		case 9:	troops[0].Set(Monster::CYCLOPS, 1); break;	
		case 10:troops[0].Set(Monster::PHOENIX, 1); break;	
		case 11:troops[0].Set(Monster::GREEN_DRAGON, 1); break;	
		case 12:troops[0].Set(Monster::TITAN, 1); break;	
		case 13:troops[0].Set(Monster::BONE_DRAGON, 1); break;
		default: break;	
	    }
	    ArrangeForBattle();
	    break;

	//case MP2::OBJ_ABANDONEDMINE:
	//    troops[0] = Troop(t);
	//    ArrangeForBattle();
	//    break;

	case MP2::OBJ_CITYDEAD:
            troops[0].Set(Monster::ZOMBIE, 20);
            troops[1].Set(Monster::VAMPIRE_LORD, 5);
            troops[2].Set(Monster::POWER_LICH, 5);
            troops[3].Set(Monster::VAMPIRE_LORD, 5);
            troops[4].Set(Monster::ZOMBIE, 20);
	    break;

	case MP2::OBJ_TROLLBRIDGE:
            troops[0].Set(Monster::TROLL, 4);
            troops[1].Set(Monster::WAR_TROLL, 4);
            troops[2].Set(Monster::TROLL, 4);
            troops[3].Set(Monster::WAR_TROLL, 4);
            troops[4].Set(Monster::TROLL, 4);
	    break;

	case MP2::OBJ_DRAGONCITY:
            troops[0].Set(Monster::GREEN_DRAGON, 3);
            troops[1].Set(Monster::RED_DRAGON, 2);
            troops[2].Set(Monster::BLACK_DRAGON, 1);
	    break;

	case MP2::OBJ_DAEMONCAVE:
            troops[0].Set(Monster::EARTH_ELEMENT, 2);
            troops[1].Set(Monster::EARTH_ELEMENT, 2);
            troops[2].Set(Monster::EARTH_ELEMENT, 2);
            troops[3].Set(Monster::EARTH_ELEMENT, 2);
	    break;

	default:
	    troops[0] = Troop(t);
	    ArrangeForBattle();
	    break;
    }
}

Army::army_t & Army::army_t::operator= (const army_t & a)
{
    commander = NULL;

    Import(a.troops);
    combat_format = a.combat_format;
    color = a.color;

    return *this;
}

void Army::army_t::SetCombatFormat(format_t f)
{
    combat_format = f;
}

u8 Army::army_t::GetCombatFormat(void) const
{
    return combat_format;
}

void Army::army_t::Import(const Troops & v)
{
    for(u8 ii = 0; ii < Size(); ++ii)
    {
	if(ii < v.size())
	    troops[ii] = v[ii];
	else
	    troops[ii].Reset();
    }
}

void Army::army_t::UpgradeMonsters(const Monster & m)
{
    for(u8 ii = 0; ii < Size(); ++ii) if(troops[ii].isValid() && troops[ii] == m) troops[ii].Upgrade();
}

u8 Army::army_t::Size(void) const
{
    return troops.size() > ARMYMAXTROOPS ? troops.size() : ARMYMAXTROOPS;
}

Army::Troop & Army::army_t::FirstValid(void)
{
    Troops::iterator it = std::find_if(troops.begin(), troops.end(), Army::isValidTroop);

    if(it == troops.end())
    {
	DEBUG(DBG_GAME, DBG_WARN, "not found" << ", return first..");
	it = troops.begin();
    }

    return *it;
}

s8 Army::army_t::GetTroopIndex(const Troop & t) const
{
    for(u8 ii = 0; ii < Size(); ++ii) if(&troops[ii] == &t) return ii;
    return -1;
}

Army::Troop & Army::army_t::At(u8 index)
{
    return index < Size() ? troops[index] : troops[Size() - 1];
}

const Army::Troop & Army::army_t::At(u8 index) const
{
    return index < Size() ? troops[index] : troops[Size() - 1];
}

u8 Army::army_t::GetCount(void) const
{
    return std::count_if(troops.begin(), troops.end(), Army::isValidTroop);
}

u8 Army::army_t::GetUniqCount(void) const
{
    std::vector<Monster> troops2;
    troops2.reserve(troops.size());

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()) troops2.push_back(*it);

    std::sort(troops2.begin(), troops2.end());
    troops2.resize(std::unique(troops2.begin(), troops2.end()) - troops2.begin());

    return troops2.size();
}

u32 Army::army_t::GetCountMonsters(const Monster & m) const
{
    u32 c = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid() && (*it) == m) c += (*it).GetCount();

    return c;
}

bool Army::army_t::JoinTroop(const Monster & mon, const u32 count)
{
    return JoinTroop(Troop(mon, count));
}

bool Army::army_t::JoinTroop(const Troop & troop)
{
    if(!troop.isValid()) return false;

    Troops::iterator it = std::find(troops.begin(), troops.end(), static_cast<Monster>(troop));
    if(it == troops.end()) it = std::find_if(troops.begin(), troops.end(), std::not1(std::mem_fun_ref(&Troop::isValid)));

    if(it != troops.end())
    {
	if((*it).isValid())
	    (*it).SetCount((*it).GetCount() + troop.GetCount());
	else
	    (*it).Set(troop, troop.GetCount());
	DEBUG(DBG_GAME, DBG_INFO, std::dec << troop.GetCount() << " " << troop.GetName() << ", commander: " << (commander ? commander->GetName() : "unknown"));
	return true;
    }

    return false;
}

bool Army::army_t::CanJoinTroop(const Monster & mon) const
{
    Troops::const_iterator it = std::find(troops.begin(), troops.end(), mon);
    if(it == troops.end()) it = std::find_if(troops.begin(), troops.end(), std::not1(std::mem_fun_ref(&Troop::isValid)));

    return it != troops.end();
}

bool Army::army_t::CanJoinArmy(const Army::army_t & army2) const
{
    Army::army_t army1(*this);

    for(Troops::const_iterator
	it = army2.troops.begin(); it != army2.troops.end(); ++it)
	    if((*it).isValid() && ! army1.JoinTroop(*it)) return false;

    return true;
}

bool Army::army_t::JoinArmy(Army::army_t & army2)
{
    for(Troops::iterator
	it = army2.troops.begin(); it != army2.troops.end(); ++it) if((*it).isValid())
    {
	if(JoinTroop(*it))
	    (*it).Reset();
	else
	    return false;
    }

    return true;
}

bool Army::army_t::isValid(void) const
{
    return troops.end() != std::find_if(troops.begin(),troops.end(), Army::isValidTroop);
}

bool Army::army_t::HasMonster(const Monster & mons) const
{
    return troops.end() != std::find(troops.begin(), troops.end(), mons);
}

Color::color_t Army::army_t::GetColor(void) const
{
    return commander ? commander->GetColor() : color;
}

void Army::army_t::SetColor(Color::color_t cl)
{
    color = cl;
}

u8 Army::army_t::GetRace(void) const
{
    std::vector<u8> races;
    races.reserve(ARMYMAXTROOPS);

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()) races.push_back((*it).GetRace());

    std::sort(races.begin(), races.end());
    races.resize(std::unique(races.begin(), races.end()) - races.begin());

    if(races.empty())
    {
        DEBUG(DBG_GAME, DBG_WARN, "empty");
        return Race::MULT;
    }

    return 1 < races.size() ? Race::MULT : races.at(0);
}

s8 Army::army_t::GetLuck(void) const
{
    return commander ? commander->GetLuck() : GetLuckModificator(NULL);
}

s8 Army::army_t::GetLuckModificator(std::string *strs) const
{
    return Luck::NORMAL;
}

bool Army::army_t::AllTroopsIsRace(u8 race) const
{
    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid() && (*it).GetRace() != race) return false;

    return true;
}

s8 Army::army_t::GetMorale(void) const
{
    return commander ? commander->GetMorale() : GetMoraleModificator(NULL);
}

// TODO:: need optimize
s8 Army::army_t::GetMoraleModificator(std::string *strs) const
{
    s8 result(Morale::NORMAL);

    // different race penalty
    u8 count = 0;
    u8 count_kngt = 0;
    u8 count_barb = 0;
    u8 count_sorc = 0;
    u8 count_wrlk = 0;
    u8 count_wzrd = 0;
    u8 count_necr = 0;
    u8 count_bomg = 0;
    bool ghost_present = false;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it) if((*it).isValid())
    {
        switch((*it).GetRace())
	{
            case Race::KNGT: ++count_kngt; break;
            case Race::BARB: ++count_barb; break;
            case Race::SORC: ++count_sorc; break;
            case Race::WRLK: ++count_wrlk; break;
            case Race::WZRD: ++count_wzrd; break;
            case Race::NECR: ++count_necr; break;
            case Race::NONE: ++count_bomg; break;
            default: break;
	}
        if(*it == Monster::GHOST) ghost_present = true;
    }

    u8 r = Race::MULT;
    if(count_kngt){ ++count; r = Race::KNGT; }
    if(count_barb){ ++count; r = Race::BARB; }
    if(count_sorc){ ++count; r = Race::SORC; }
    if(count_wrlk){ ++count; r = Race::WRLK; }
    if(count_wzrd){ ++count; r = Race::WZRD; }
    if(count_necr){ ++count; r = Race::NECR; }
    if(count_bomg){ ++count; r = Race::NONE; }
    const u8 uniq_count = GetUniqCount();

    switch(count)
    {
        case 2:
        case 0: break;
        case 1:
    	    if(0 == count_necr && !ghost_present)
            {
		if(1 < uniq_count)
                {
		    ++result;
            	    if(strs)
            	    {
            		std::string str = _("All %{race} troops +1");
            		String::Replace(str, "%{race}", Race::String(r));
            		strs->append(str);
            		strs->append("\n");
            	    }
		}
            }
	    else
            {
	        if(strs)
                {
            	    strs->append(_("Entire unit is undead, so morale does not apply."));
            	    strs->append("\n");
            	}
		return 0;
	    }
            break;
        case 3:
            result -= 1;
            if(strs)
            {
        	strs->append(_("Troops of 3 alignments -1"));
        	strs->append("\n");
    	    }
            break;
        case 4:
    	    result -= 2;
            if(strs)
            {
        	strs->append(_("Troops of 4 alignments -2"));
        	strs->append("\n");
    	    }
            break;
        default:
            result -= 3;
            if(strs)
            {
        	strs->append(_("Troops of 5 alignments -3"));
        	strs->append("\n");
    	    }
            break;
    }

    // undead in life group
    if((1 < uniq_count && (count_necr || ghost_present) && (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg)) ||
    // or artifact Arm Martyr
	(commander && commander->HasArtifact(Artifact::ARM_MARTYR)))
    {
        result -= 1;
        if(strs)
        {
    	    strs->append(_("Some undead in groups -1"));
    	    strs->append("\n");
    	}
    }

    return result;
}

Army::Troop & Army::army_t::GetWeakestTroop(void)
{
    Troops::iterator first, last, lowest;

    first = troops.begin();
    last  = troops.end();

    while(first != last) if(isValidTroop(*first)) break; else ++first;

    lowest = first;

    if(first != last)
    while(++first != last) if(isValidTroop(*first) && WeakestTroop(*first, *lowest)) lowest = first;

    return *lowest;
}

const Army::Troop & Army::army_t::GetSlowestTroop(void) const
{
    Troops::const_iterator first, last, lowest;

    first = troops.begin();
    last  = troops.end();

    while(first != last) if(isValidTroop(*first)) break; else ++first;

    lowest = first;

    if(first != last)
    while(++first != last) if(isValidTroop(*first) && SlowestTroop(*first, *lowest)) lowest = first;

    return *lowest;
}

/* draw MONS32 sprite in line, first valid = 0, count = 0 */
void Army::DrawMons32Line(const army_t & army, s16 cx, s16 cy, u16 width, u8 first, u8 count)
{
    DrawMons32LineWithScoute(army, cx, cy, width, first, count, Skill::Level::EXPERT);
}

void Army::DrawMons32LineWithScoute(const army_t & army, s16 cx, s16 cy, u16 width, u8 first, u8 count, u8 scoute)
{
    if(!army.isValid()) return;

    if(0 == count) count = army.GetCount();
    else
    if(army.Size() < count) count = army.Size();

    const u16 chunk = width / count;
    cx += chunk / 2;

    std::string str;
    Text text;
    text.Set(Font::SMALL);

    for(u8 ii = 0; ii < army.Size(); ++ii)
    {
	const Army::Troop & troop = army.At(ii);

    	if(troop.isValid())
	{
	    if(0 == first && count)
    	    {
		const Sprite & monster = AGG::GetICN(ICN::MONS32, troop.GetSpriteIndex());

    		monster.Blit(cx - monster.w() / 2, cy + 30 - monster.h());
		text.Set(Game::CountScoute(troop.GetCount(), scoute));
		text.Blit(cx - text.w() / 2, cy + 28);

		cx += chunk;
		--count;
	    }
	    else
		--first;
	}
    }
}

std::vector<Army::Troop> Army::army_t::Optimize(void) const
{
    Troops optimize;
    optimize.reserve(ARMYMAXTROOPS);

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it) if((*it).isValid())
    {
	const Troop & troop = *it;

	Troops::iterator it2 = std::find(optimize.begin(), optimize.end(),
								static_cast<Monster>(troop));
	if(it2 == optimize.end())
	    optimize.push_back(troop);
	else
	    (*it2).SetCount((*it2).GetCount() + troop.GetCount());
    }
    return optimize;
}

void Army::army_t::ArrangeForBattle(void)
{
    if(GetControl() & CONTROL_AI)
    {
	Troops priority = Optimize();
	Clear();

	switch(priority.size())
	{
	    case 1:
	    {
		const Monster & m = priority.back();
		const u32 count = priority.back().GetCount();
		if(49 < count)
		{
		    const u32 c = count / 5;
		    troops[0].Set(m, c);
		    troops[1].Set(m, c);
		    troops[2].Set(m, c + count - (c * 5));
		    troops[3].Set(m, c);
		    troops[4].Set(m, c);
		}
		else
		if(20 < count)
		{
		    const u32 c = count / 3;
		    troops[1].Set(m, c);
		    troops[2].Set(m, c + count - (c * 3));
		    troops[3].Set(m, c);
		}
		else
	    	    troops[2].Set(m, count);
		break;
	    }
	    case 2:
	    {
		// TODO: need modify army for 2 troops
		Import(priority);
		break;
	    }
	    case 3:
	    {
		// TODO: need modify army for 3 troops
		Import(priority);
		break;
	    }
	    case 4:
	    {
		// TODO: need modify army for 4 troops
		Import(priority);
		break;
	    }
	    case 5:
	    {
		// possible change orders monster
		// store
		Import(priority);
		break;
	    }
	    default: break;
	}
    }
}

u32 Army::army_t::CalculateExperience(void) const
{
    u32 res = 0;

    for(u8 ii = 0; ii < Size(); ++ii)
	res += troops[ii].GetHitPoints();

    if(commander) res += 500;

    return res;

}

void Army::army_t::BattleInit(void)
{
    // reserve ARMYMAXTROOPS for summons and mirrors spell
    if(commander)
    {
	troops.reserve(ARMYMAXTROOPS * 2);
	commander->ActionPreBattle();
    }
}

void Army::army_t::BattleQuit(void)
{
    std::for_each(troops.begin(), troops.end(), std::mem_fun_ref(&Troop::BattleQuit));
    if(troops.size() > ARMYMAXTROOPS) troops.resize(ARMYMAXTROOPS);
    if(commander)
	commander->ActionAfterBattle();
}

u32 Army::army_t::BattleKilled(void) const
{
    u32 res = 0;
    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	res += (*it).BattleKilled();

    return res;
}

void Army::army_t::Clear(void)
{
    std::for_each(troops.begin(), troops.end(), std::mem_fun_ref(&Troop::Reset));
}

void Army::army_t::BattleExportKilled(army_t & a) const
{
    a.troops.resize(Size());

    for(u8 ii = 0; ii < Size(); ++ii)
    {
	a.troops[ii].SetMonster(troops[ii].GetID());
	a.troops[ii].SetCount(troops[ii].BattleKilled());
    }

    a.commander = commander;
}

void Army::army_t::Reset(bool soft)
{
    Clear();

    if(commander)
    {
    	const Monster mons1(commander->GetRace(), DWELLING_MONSTER1);

	if(soft)
	{
    	    const Monster mons2(commander->GetRace(), DWELLING_MONSTER2);

	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    JoinTroop(mons1, 3 * mons1.GetGrown());
		    break;
		case 2:
		    JoinTroop(mons2, static_cast<u8>(1.5 * mons2.GetGrown()));
		    break;
		default:
		    JoinTroop(mons1, 2 * mons1.GetGrown());
		    JoinTroop(mons2, mons2.GetGrown());
		    break;
	    }
	}
	else
	{
	    JoinTroop(mons1, 1);
	}
    }
}

void Army::army_t::JoinStrongestFromArmy(army_t & army2)
{
    bool save_last = army2.commander && Skill::Primary::HEROES == army2.commander->GetType();

    Troops priority;
    priority.reserve(ARMYMAXTROOPS * 2);
    priority = Optimize();
    Troops priority2 = army2.Optimize();
    priority.insert(priority.end(), priority2.begin(), priority2.end());

    Clear();
    army2.Clear();

    // sort: strongest
    std::sort(priority.begin(), priority.end(), StrongestTroop);

    // weakest to army2
    while(Size() < priority.size())
    {
	army2.JoinTroop(priority.back());
	priority.pop_back();
    }

    // save half weak of strongest to army2
    if(save_last && !army2.isValid())
    {
	Troop & last = priority.back();
	u32 count = last.GetCount() / 2;
	army2.JoinTroop(last, last.GetCount() - count);
	last.SetCount(count);
    }

    // strongest to army
    while(priority.size())
    {
	JoinTroop(priority.back());
	priority.pop_back();
    }
}

void Army::army_t::KeepOnlyWeakestTroops(army_t & army2)
{

    bool save_last = commander && Skill::Primary::HEROES == commander->GetType();

    Troops priority;
    priority.reserve(ARMYMAXTROOPS * 2);
    priority = Optimize();
    Troops priority2 = army2.Optimize();
    priority.insert(priority.end(), priority2.begin(), priority2.end());

    Clear();
    army2.Clear();

    // sort: strongest
    std::sort(priority.begin(), priority.end(), StrongestTroop);

    // weakest to army
    while(Size() < priority.size())
    {
	JoinTroop(priority.back());
	priority.pop_back();
    }

    // save half weak of strongest to army
    if(save_last && !isValid())
    {
	Troop & last = priority.back();
	u32 count = last.GetCount() / 2;
	JoinTroop(last, last.GetCount() - count);
	last.SetCount(count);
    }

    // strongest to army2
    while(priority.size())
    {
	army2.JoinTroop(priority.back());
	priority.pop_back();
    }
}

void Army::army_t::UpgradeTroops(const Castle & castle)
{
    for(Troops::iterator
	it = troops.begin(); it != troops.end(); ++it) if((*it).isValid())
    {
	Troop & troop = *it;
        payment_t payment = troop.GetUpgradeCost();

	if(castle.GetRace() == troop.GetRace() &&
	   castle.isBuild(troop.GetUpgrade().GetDwelling()) &&
	   payment <= world.GetKingdom(castle.GetColor()).GetFunds())
	{
    	    world.GetKingdom(castle.GetColor()).OddFundsResource(payment);
            troop.Upgrade();
	}
    }
}

std::string Army::army_t::String(void) const
{
    std::ostringstream os;

    os << "color(" << Color::String(commander ? commander->GetColor() : color) << "), ";

    if(commander)
	os << "commander(" << commander->GetName() << "), ";

    os << ": ";

    for(Troops::const_iterator
	    it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid())
	    os << std::dec << (*it).GetCount() << " " << (*it).GetName() << ", ";

    return os.str();
}

u16 Army::army_t::GetAttack(bool hero) const
{
    u16 res = 0;
    u8 count = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()){ res += (*it).GetAttack(hero); ++count; }

    return count ? res / count : 0;
}

u16 Army::army_t::GetDefense(bool hero) const
{
    u16 res = 0;
    u8 count = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()){ res += (*it).GetDefense(hero); ++count; }

    return count ? res / count : 0;
}

u32 Army::army_t::GetHitPoints(void) const
{
    u32 res = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()) res += (*it).GetHitPoints();

    return res;
}

u32 Army::army_t::GetDamageMin(void) const
{
    u32 res = 0;
    u8 count = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()){ res += (*it).GetDamageMin(); ++count; }

    return count ? res / count : 0;
}

u32 Army::army_t::GetDamageMax(void) const
{
    u32 res = 0;
    u8 count = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()){ res += (*it).GetDamageMax(); ++count; }

    return count ? res / count : 0;
}

u32 Army::army_t::GetStrength(void) const
{
    u32 res = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
	if((*it).isValid()) res += (*it).GetStrength();

    return res;
}

bool Army::army_t::StrongerEnemyArmy(const army_t & army2) const
{
    if(! army2.isValid()) return true;

    const u16 a1 = GetAttack(false);
    const u16 d1 = GetDefense(false);
    float r1 = 0;

    const u16 a2 = army2.GetAttack(false);
    const u16 d2 = army2.GetDefense(false);
    float r2 = 0;

    if(a1 > d2)
        r1 = 1 + 0.1 * static_cast<float>(std::min(a1 - d2, 20));
    else
        r1 = 1 + 0.05 * static_cast<float>(std::min(d2 - a1, 14));

    if(a2 > d1)
        r2 = 1 + 0.1 * static_cast<float>(std::min(a2 - d1, 20));
    else
        r2 = 1 + 0.05 * static_cast<float>(std::min(d1 - a2, 14));

    const u32 s1 = GetStrength();
    const u32 s2 = army2.GetStrength();

    const float h1 = GetHitPoints();
    const float h2 = army2.GetHitPoints();

    DEBUG(DBG_AI, DBG_INFO, "r1: " << r1 << ", s1: " << s1 << ", h1: " << h1 \
			<< ", r2: " << r2 << ", s2: " << s2 << ", h2: " << h2);

    r1 *= s1 / h2;
    r2 *= s2 / h1;

    DEBUG(DBG_GAME, DBG_INFO, "army1: " << String() << ", army2: " << army2.String());

    return static_cast<s32>(r1) > static_cast<s32>(r2);
}

void Army::army_t::SetCommander(HeroBase* c)
{
    commander = c;
}

HeroBase* Army::army_t::GetCommander(void)
{
    return (!commander || (Skill::Primary::CAPTAIN == commander->GetType() && !commander->isValid()) ? NULL : commander);
}

const HeroBase* Army::army_t::GetCommander(void) const
{
    return (!commander || (Skill::Primary::CAPTAIN == commander->GetType() && !commander->isValid()) ? NULL : commander);
}

u32 Army::army_t::ActionToSirens(void)
{
    u32 res = 0;

    for(Troops::iterator
	it = troops.begin(); it != troops.end(); ++it)
    if((*it).isValid())
    {
	const u32 kill = (*it).GetCount() * 30 / 100;

	if(kill)
	{
	    (*it).SetCount((*it).GetCount() - kill);
	    res += kill * static_cast<Monster>(*it).GetHitPoints();
	}
    }

    return res;
}

u8 Army::army_t::GetControl(void) const
{
    return commander ? commander->GetControl() : (color == Color::NONE ? CONTROL_AI : world.GetKingdom(color).GetControl());
}

u32 Army::army_t::GetSurrenderCost(void) const
{
    float res = 0;

    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
    if((*it).isValid())
    {
	// FIXME: orig: 3 titan = 7500
	payment_t payment = (*it).GetCost();
	res += payment.gold;
    }

    if(commander)
    {
	switch(commander->GetLevelSkill(Skill::Secondary::DIPLOMACY))
	{
	    // 40%
	    case Skill::Level::BASIC: res = res * 40 / 100; break;
	    // 30%
	    case Skill::Level::ADVANCED: res = res * 30 / 100; break;
	    // 20%
	    case Skill::Level::EXPERT: res = res * 20 / 100; break;
	    // 50%
	    default: res = res * 50 / 100; break;
	}

	Artifact art(Artifact::STATESMAN_QUILL);
	u8 acount = commander->HasArtifact(art);
	if(acount)
	    res -= res * acount * art.ExtraValue() / 100;
    }

    // limit
    if(res < 100) res = 100.0;

    return static_cast<u32>(res);
}

u8 Army::GetJoinSolution(const Heroes & hero, const Maps::Tiles & tile, u32 & join, s32 & cost)
{
    const Army::Troop troop(tile);

    if(! troop.isValid()) return 0xFF;

    const u32  ratios = troop.isValid() ? hero.GetArmy().GetHitPoints() / troop.GetHitPoints() : 0;
    const bool check_free_stack = (hero.GetArmy().GetCount() < hero.GetArmy().Size() || hero.GetArmy().HasMonster(troop));
    const bool check_extra_condition = (!hero.HasArtifact(Artifact::HIDEOUS_MASK) && Morale::NORMAL <= hero.GetMorale());

    // force join for campain and others...
    const bool force_join = (5 == tile.GetQuantity4());

    if(tile.GetQuantity4() && check_free_stack && ((check_extra_condition && ratios >= 2) || force_join))
    {
        if(2 == tile.GetQuantity4() || force_join)
        {
	    join = troop.GetCount();
	    return 1;
	}
	else
        if(hero.HasSecondarySkill(Skill::Secondary::DIPLOMACY))
        {
            const Kingdom & kingdom = world.GetKingdom(hero.GetColor());
            payment_t payment = troop.GetCost();
            cost = payment.gold;
	    payment.Reset();
	    payment.gold = cost;

            // skill diplomacy
            const u32 to_join = Monster::GetCountFromHitPoints(troop,
			    troop.GetHitPoints() * hero.GetSecondaryValues(Skill::Secondary::DIPLOMACY) / 100);

            if(to_join && kingdom.AllowPayment(payment))
            {
		join = to_join;
		return 2;
	    }
	}
    }
    else
    if(ratios >= 5)
    {
	// ... surely flee before us

	if(hero.GetControl() & CONTROL_AI) return Rand::Get(0, 10) < 5 ? 0 : 3;

	return 3;
    }

    return 0;
}
