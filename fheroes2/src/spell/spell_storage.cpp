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
#include "error.h"
#include "spell_storage.h"

Spell::Storage::Storage()
{
}

u8 Spell::Storage::Size(const u8 lvl) const
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

    Error::Warning("Spell::Storage::Size: unknown level: ", lvl);

    return Size1();
}

u8 Spell::Storage::Size1(void) const
{
    return spells_level1.size();
}

u8 Spell::Storage::Size2(void) const
{
    return spells_level2.size();
}

u8 Spell::Storage::Size3(void) const
{
    return spells_level3.size();
}

u8 Spell::Storage::Size4(void) const
{
    return spells_level4.size();
}

u8 Spell::Storage::Size5(void) const
{
    return spells_level5.size();
}

const std::list<Spell::spell_t> & Spell::Storage::Spells(const u8 lvl) const
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
    
    Error::Warning("Spell::Storage::Spells: unknown level: ", lvl);
    
    return Spells1();
}

const std::list<Spell::spell_t> & Spell::Storage::Spells1(void) const
{
    return spells_level1;
}

const std::list<Spell::spell_t> & Spell::Storage::Spells2(void) const
{
    return spells_level2;
}

const std::list<Spell::spell_t> & Spell::Storage::Spells3(void) const
{
    return spells_level3;
}

const std::list<Spell::spell_t> & Spell::Storage::Spells4(void) const
{
    return spells_level4;
}

const std::list<Spell::spell_t> & Spell::Storage::Spells5(void) const
{
    return spells_level5;
}
