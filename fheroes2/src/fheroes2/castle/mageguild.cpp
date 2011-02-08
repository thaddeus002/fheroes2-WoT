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
#include "castle.h"
#include "heroes_base.h"
#include "race.h"
#include "settings.h"
#include "mageguild.h"

Spell GetUniqueCombatSpellCompatibility(const SpellStorage &, u8 race, u8 level);
Spell GetCombatSpellCompatibility(u8 race, u8 level);

MageGuild::MageGuild(const Castle & cstl) : home(cstl)
{
}

void MageGuild::Builds(void)
{
    general.clear();
    library.clear();

    // level 5
    general.Append(7 > Rand::Get(1, 10) ? Spell::RandCombat(5) : Spell::RandAdventure(5));

    // level 4
    general.Append(GetCombatSpellCompatibility(home.GetRace(), 4));
    general.Append(Spell::RandAdventure(4));

    // level 3
    general.Append(GetCombatSpellCompatibility(home.GetRace(), 3));
    general.Append(Spell::RandAdventure(3));

    // level 2
    general.Append(GetCombatSpellCompatibility(home.GetRace(), 2));
    general.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 2));
    general.Append(Spell::RandAdventure(2));

    // level 1
    general.Append(GetCombatSpellCompatibility(home.GetRace(), 1));
    general.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 1));
    general.Append(Spell::RandAdventure(1));

    if(HaveLibraryCapability())
    {
	library.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 1));
	library.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 2));
	library.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 3));
	library.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 4));
	library.Append(GetUniqueCombatSpellCompatibility(general, home.GetRace(), 5));
    }
}

bool MageGuild::isLibraryBuild(void) const
{
    return home.GetRace() == Race::WZRD && home.isBuild(BUILD_SPEC);
}

bool MageGuild::HaveLibraryCapability(void) const
{
    return home.GetRace() == Race::WZRD;
}

u8 MageGuild::GetLevel(void) const
{
    return home.GetLevelMageGuild();
}

SpellStorage MageGuild::GetSpells(u8 level) const
{
    SpellStorage result;

    if(home.GetLevelMageGuild() >= level)
    {
	result = general.GetSpells(level);
	if(isLibraryBuild()) result.Append(library.GetSpells(level));
    }

    return result;
}

void MageGuild::EducateHero(HeroBase & hero) const
{
    if(hero.HaveSpellBook() && GetLevel())
    {
	SpellStorage spells;

	for(u8 level = 1; level <= 5; ++level) if(level <= GetLevel())
	{
	    spells.Append(general.GetSpells(level));
	    if(isLibraryBuild()) spells.Append(library.GetSpells(level));
	}

	hero.AppendSpellsToBook(spells);
    }
}

Spell GetUniqueCombatSpellCompatibility(const SpellStorage & spells, u8 race, u8 lvl)
{
    Spell spell = GetCombatSpellCompatibility(race, lvl);
    while(spells.isPresentSpell(spell)) spell = GetCombatSpellCompatibility(race, lvl);
    return spell;
}

Spell GetCombatSpellCompatibility(u8 race, u8 lvl)
{
    Spell spell = Spell::RandCombat(lvl);
    while(!spell.isRaceCompatible(race)) spell = Spell::RandCombat(lvl);
    return spell;
}
