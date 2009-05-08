/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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
#include "skill.h"
#include "spell_storage.h"

SpellStorage::SpellStorage()
{
}

u8 SpellStorage::Size(const u8 lvl) const
{
    switch(lvl)
    {
	case 1:	return Size1();
	case 2:	return Size2();
	case 3:	return Size3();
	case 4:	return Size4();
	case 5:	return Size5();

	default: break;
    }

    Error::Warning("SpellStorage::Size: unknown level: ", lvl);

    return Size1();
}

u8 SpellStorage::Size1(void) const
{
    return spells_level1.size();
}

u8 SpellStorage::Size2(void) const
{
    return spells_level2.size();
}

u8 SpellStorage::Size3(void) const
{
    return spells_level3.size();
}

u8 SpellStorage::Size4(void) const
{
    return spells_level4.size();
}

u8 SpellStorage::Size5(void) const
{
    return spells_level5.size();
}

const std::list<Spell::spell_t> & SpellStorage::Spells(const u8 lvl) const
{
    switch(lvl)
    {
	case 1:	return Spells1();
	case 2:	return Spells2();
	case 3:	return Spells3();
	case 4:	return Spells4();
	case 5:	return Spells5();
	
	default: break;
    }
    
    Error::Warning("SpellStorage::Spells: unknown level: ", lvl);
    
    return Spells1();
}

const std::list<Spell::spell_t> & SpellStorage::Spells1(void) const
{
    return spells_level1;
}

const std::list<Spell::spell_t> & SpellStorage::Spells2(void) const
{
    return spells_level2;
}

const std::list<Spell::spell_t> & SpellStorage::Spells3(void) const
{
    return spells_level3;
}

const std::list<Spell::spell_t> & SpellStorage::Spells4(void) const
{
    return spells_level4;
}

const std::list<Spell::spell_t> & SpellStorage::Spells5(void) const
{
    return spells_level5;
}

void SpellStorage::Appends(const SpellStorage & st, const u8 wisdom)
{
    if(st.spells_level1.size())
    {
        spells_level1.insert(spells_level1.begin(), st.spells_level1.begin(), st.spells_level1.end());
        spells_level1.sort();
        spells_level1.unique();
    }

    if(st.spells_level2.size())
    {
        spells_level2.insert(spells_level2.begin(), st.spells_level2.begin(), st.spells_level2.end());
        spells_level2.sort();
        spells_level2.unique();
    }

    if(st.spells_level3.size() && Skill::Level::BASIC <= wisdom)
    {
        spells_level3.insert(spells_level3.begin(), st.spells_level3.begin(), st.spells_level3.end());
        spells_level3.sort();
        spells_level3.unique();
    }

    if(st.spells_level4.size() && Skill::Level::ADVANCED <= wisdom)
    {
        spells_level4.insert(spells_level4.begin(), st.spells_level4.begin(), st.spells_level4.end());
        spells_level4.sort();
        spells_level4.unique();
    }

    if(st.spells_level5.size() && Skill::Level::EXPERT == wisdom)
    {
        spells_level5.insert(spells_level5.begin(), st.spells_level5.begin(), st.spells_level5.end());
        spells_level5.sort();
        spells_level5.unique();
    }
}

void SpellStorage::Append(const Spell::spell_t sp, const u8 wisdom)
{
    std::list<Spell::spell_t> *spells = NULL;

    switch(Spell::Level(sp))
    {
        case 1:	spells = &spells_level1; break;
        case 2:	spells = &spells_level2; break;
        case 3:	if(Skill::Level::BASIC <= wisdom)    spells = &spells_level3; break;
        case 4:	if(Skill::Level::ADVANCED <= wisdom) spells = &spells_level4; break;
        case 5:	if(Skill::Level::EXPERT == wisdom)   spells = &spells_level5; break;
	default: break;
    }

    if(spells)
    {
	if(spells->end() == std::find(spells->begin(), spells->end(), sp))
        {
            spells->push_back(sp);
            spells->sort();
        }
    }
}
