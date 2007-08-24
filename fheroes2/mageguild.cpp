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

#include <string>
#include "agg.h"
#include "localevent.h"
#include "dialog.h"
#include "button.h"
#include "cursor.h"
#include "background.h"
#include "text.h"
#include "mageguild.h"

MageGuild::MageGuild() : level(0), upgrade(false)
{
}

void MageGuild::BuildNextLevel(void)
{
    if(5 < level) return;

    ++level;
    
    switch(level)
    {
	case 1:
	{
	    Spell::spell_t spell = Spell::NONE;
	    const u8 count = upgrade ? 4 : 3;

	    for(u8 ii = 0; ii < count; ++ii)
	    {
		while(!Spell::Uniq(spells, spell))  spell = Spell::Rand1();

		spells.push_back(spell);
	    }
	}
	break;

	case 2:
	{
	    Spell::spell_t spell = Spell::NONE;
	    const u8 count = upgrade ? 4 : 3;

	    for(u8 ii = 0; ii < count; ++ii)
	    {
		while(!Spell::Uniq(spells, spell))  spell = Spell::Rand2();

		spells.push_back(spell);
	    }
	}
	break;

	case 3:
	{
	    Spell::spell_t spell = Spell::NONE;
	    const u8 count = upgrade ? 3 : 2;

	    for(u8 ii = 0; ii < count; ++ii)
	    {
		while(!Spell::Uniq(spells, spell))  spell = Spell::Rand3();

		spells.push_back(spell);
	    }
	}
	break;

	case 4:
	{
	    Spell::spell_t spell = Spell::NONE;
	    const u8 count = upgrade ? 3 : 2;

	    for(u8 ii = 0; ii < count; ++ii)
	    {
		while(!Spell::Uniq(spells, spell))  spell = Spell::Rand4();

		spells.push_back(spell);
	    }
	}
	break;

	case 5:
	{
	    Spell::spell_t spell = Spell::NONE;
	    const u8 count = upgrade ? 2 : 1;

	    for(u8 ii = 0; ii < count; ++ii)
	    {
		while(!Spell::Uniq(spells, spell))  spell = Spell::Rand5();

		spells.push_back(spell);
	    }
	}
	break;

	default: break;
    }
}

void MageGuild::UpgradeExt(void)
{
    if(upgrade) return;

    upgrade = true;

    Spell::spell_t spell = Spell::NONE;

    if(0 < level)
    {
	while(!Spell::Uniq(spells, spell))  spell = Spell::Rand1();
	spells.push_back(spell);
    }

    if(1 < level)
    {
	while(!Spell::Uniq(spells, spell))  spell = Spell::Rand2();
	spells.push_back(spell);
    }

    if(2 < level)
    {
	while(!Spell::Uniq(spells, spell))  spell = Spell::Rand3();
	spells.push_back(spell);
    }

    if(3 < level)
    {
	while(!Spell::Uniq(spells, spell))  spell = Spell::Rand4();
	spells.push_back(spell);
    }

    if(4 < level)
    {
	while(!Spell::Uniq(spells, spell))  spell = Spell::Rand5();
	spells.push_back(spell);
    }
}

/* return level spell from orders */
Spell::spell_t MageGuild::GetSpell(u8 level, u8 index) const
{
    if(0 == level || 5 < level || index >= spells.size()) return Spell::NONE;

    std::vector<Spell::spell_t>::const_iterator it1 = spells.begin();
    std::vector<Spell::spell_t>::const_iterator it2 = spells.end();

    u8 order = 0;

    for(; it1 != it2; ++it1)
    {
	if(level == Spell::Level(*it1)) ++order;
	
	if(order - 1 == index) return *it1;
    }

    return Spell::NONE;
}
