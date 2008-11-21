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

#include <algorithm>
#include "error.h"
#include "rand.h"
#include "mageguild.h"
#include "spell.h"

MageGuild::MageGuild() : level(0), upgrade(false)
{
}

void MageGuild::SetRace(const Race::race_t rc)
{
    race = rc;
}

void MageGuild::BuildNextLevel(void)
{
    if(5 < level) return;

    ++level;
    
    PackSpells(level);
}

void MageGuild::UpgradeExt(void)
{
    if(upgrade) return;

    upgrade = true;

    if(0 < level){ AddExtSpells(1); spells_level1.sort(); }
    if(1 < level){ AddExtSpells(2); spells_level2.sort(); }
    if(2 < level){ AddExtSpells(3); spells_level3.sort(); }
    if(3 < level){ AddExtSpells(4); spells_level4.sort(); }
    if(4 < level){ AddExtSpells(5); spells_level5.sort(); }
}

/* return level spell from orders */
Spell::spell_t MageGuild::GetSpell(u8 level, u8 index) const
{
    if(0 == level || 5 < level) return Spell::NONE;

    std::list<Spell::spell_t>::const_iterator it1;
    std::list<Spell::spell_t>::const_iterator it2;

    switch(level)
    {
	case 1:
	    it1 = spells_level1.begin();
	    it2 = spells_level1.end();
	    break;

	case 2:
	    it1 = spells_level2.begin();
	    it2 = spells_level2.end();
	    break;

	case 3:
	    it1 = spells_level3.begin();
	    it2 = spells_level3.end();
	    break;

	case 4:
	    it1 = spells_level4.begin();
	    it2 = spells_level4.end();
	    break;

	case 5:
	    it1 = spells_level5.begin();
	    it2 = spells_level5.end();
	    break;

	default: return Spell::NONE;
    }

    u8 order = 0;

    for(; it1 != it2; ++it1) if(index == order++) return *it1;

    return Spell::NONE;
}

u8 MageGuild::GetLevel(void) const
{
    return level;
}

void MageGuild::PackSpells(const u8 lvl)
{
    if(1 > lvl || 5 < lvl) return;

    std::list<Spell::spell_t> & spells = (4 < lvl ? spells_level5 : (3 < lvl ? spells_level4 : (2 < lvl ? spells_level3 : (1 < lvl ? spells_level2 : spells_level1))));

    Spell::spell_t spell = Spell::NONE;

    if(spells.size()) return;

    // level 5
    if(4 < lvl)
    {
	spell = GetSpellRace(race, lvl);
    	spells.push_back(spell);
    
        if(upgrade) AddExtSpells(lvl);
    }
    else
    // level 3, 4
    if(2 < lvl)
    {
	spell = GetSpellRace(race, lvl);
        spells.push_back(spell);
    
        if(upgrade) AddExtSpells(lvl);

	spells.push_back(Spell::RandAdventure(lvl));
    }
    else
    // level 1, 2
    {
	spell = GetSpellRace(race, lvl);
        spells.push_back(spell);

        spell = GetSpellRace(race, lvl);

        while(spells.end() != std::find(spells.begin(), spells.end(), spell)) spell = GetSpellRace(race, lvl);
    	spells.push_back(spell);

        if(upgrade) AddExtSpells(lvl);

	spells.push_back(Spell::RandAdventure(lvl));
    }
}

void MageGuild::AddExtSpells(const u8 lvl)
{
    if(1 > lvl || 5 < lvl) return;

    std::list<Spell::spell_t> & spells = (4 < lvl ? spells_level5 : (3 < lvl ? spells_level4 : (2 < lvl ? spells_level3 : (1 < lvl ? spells_level2 : spells_level1))));

    Spell::spell_t spell = Spell::NONE;

    if(spells.empty()) return;

    spell = GetSpellRace(race, lvl);

    while(spells.end() != std::find(spells.begin(), spells.end(), spell)) spell = GetSpellRace(race, lvl);
    spells.push_back(spell);
}

Spell::spell_t MageGuild::GetSpellRace(const Race::race_t race, const u8 lvl)
{
    Spell::spell_t spell = Spell::NONE;

    if(5 == lvl)
    	return 7 > Rand::Get(1, 10) ? Spell::RandCombat(lvl) : Spell::RandAdventure(lvl);
    else
    if(4 == lvl)
	switch(race)
	{
	    default:
		spell = Spell::RandCombat(lvl);
		break;

    	    case Race::NECR:
        	spell = Spell::RandCombat(lvl);
        	while(Spell::MASSCURE == spell || Spell::HOLYSHOUT == spell) spell = Spell::RandCombat(lvl);
        	break;
	}
    else
    if(3 == lvl)
	switch(race)
	{
	    default:
        	spell = Spell::RandCombat(lvl);
        	while(Spell::DEATHWAVE == spell || Spell::ANIMATEDEAD == spell) spell = Spell::RandCombat(lvl);
        	break;

    	    case Race::NECR:
        	spell = Spell::RandCombat(lvl);
        	while(Spell::HOLYWORD == spell || Spell::MASSBLESS == spell) spell = Spell::RandCombat(lvl);
        	break;
	}
    else
    if(2 == lvl)
	switch(race)
	{
	    default:
        	spell = Spell::RandCombat(lvl);
        	while(Spell::DEATHRIPPLE == spell) spell = Spell::RandCombat(lvl);
        	break;

	    case Race::NECR:
        	spell = Spell::RandCombat(lvl);
        	break;
	}
    else
    if(1 == lvl)
	switch(race)
	{
	    default:
        	spell = Spell::RandCombat(lvl);
        	break;

	    case Race::NECR:
        	spell = Spell::RandCombat(lvl);
        	while(Spell::BLESS == spell) spell = Spell::RandCombat(lvl);
        	break;
	}

    return spell;
}
