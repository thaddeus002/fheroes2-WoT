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

Spell::spell_t GetCombatSpellCompatibility(u8 race, u8 level);

MageGuild::MageGuild(const Race::race_t & rc) : race(rc), level(0), upgrade(false)
{
}

void MageGuild::BuildNextLevel(void)
{
    if(5 > level)
    {
	++level;
    	PackSpells(level);
    }
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
    	spells.push_back(7 > Rand::Get(1, 10) ? Spell::RandCombat(lvl) : Spell::RandAdventure(lvl));
    
        if(upgrade) AddExtSpells(lvl);
    }
    else
    // level 3, 4
    if(2 < lvl)
    {
	Spell::spell_t spell = GetCombatSpellCompatibility(race, lvl);
        spells.push_back(spell);
    
        if(upgrade) AddExtSpells(lvl);

	spells.push_back(Spell::RandAdventure(lvl));
    }
    else
    // level 1, 2
    {
	Spell::spell_t spell = GetCombatSpellCompatibility(race, lvl);
        spells.push_back(spell);

        spell = GetCombatSpellCompatibility(race, lvl);

        while(spells.end() != std::find(spells.begin(), spells.end(), spell)) spell = GetCombatSpellCompatibility(race, lvl);
    	spells.push_back(spell);

        if(upgrade) AddExtSpells(lvl);

	spells.push_back(Spell::RandAdventure(lvl));
    }
}

void MageGuild::AddExtSpells(const u8 lvl)
{
    if(1 > lvl || 5 < lvl) return;

    Spell::spell_t spell = GetCombatSpellCompatibility(race, lvl);
    while(spells.end() != std::find(spells.begin(), spells.end(), spell)) spell = GetCombatSpellCompatibility(race, lvl);
    spells.push_back(spell);
}

Spell::spell_t GetCombatSpellCompatibility(u8 race, u8 lvl)
{
    Spell::spell_t spell = Spell::RandCombat(lvl);
    while(!Spell::isRaceCompatible(spell, race)) spell = Spell::RandCombat(lvl);
    return spell;
}
