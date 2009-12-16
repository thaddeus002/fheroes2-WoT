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
#include "mageguild.h"

MageGuild::MageGuild(const Race::race_t & rc) : race(rc), level(0), upgrade(false)
{
}

void MageGuild::BuildNextLevel(void)
{
    if(5 < level) return;

    ++level;
    
    PackSpells(level);
}

bool MageGuild::AllowUpgrade(void) const
{
    return race == Race::WZRD && !upgrade;
}

void MageGuild::UpgradeExt(void)
{
    if(!AllowUpgrade()) return;

    upgrade = true;

    if(0 < level){ AddExtSpells(1); }
    if(1 < level){ AddExtSpells(2); }
    if(2 < level){ AddExtSpells(3); }
    if(3 < level){ AddExtSpells(4); }
    if(4 < level){ AddExtSpells(5); }
}

u8 MageGuild::GetLevel(void) const
{
    return level;
}

void MageGuild::PackSpells(const u8 lvl)
{
    if(1 > lvl || 5 < lvl) return;

    // level 5
    if(4 < lvl)
    {
	Spell::spell_t spell = GetSpellRace(race, lvl);
    	spells.push_back(spell);
    
        if(upgrade) AddExtSpells(lvl);
    }
    else
    // level 3, 4
    if(2 < lvl)
    {
	Spell::spell_t spell = GetSpellRace(race, lvl);
        spells.push_back(spell);
    
        if(upgrade) AddExtSpells(lvl);

	spells.push_back(Spell::RandAdventure(lvl));
    }
    else
    // level 1, 2
    {
	Spell::spell_t spell = GetSpellRace(race, lvl);
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

    Spell::spell_t spell = GetSpellRace(race, lvl);
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
