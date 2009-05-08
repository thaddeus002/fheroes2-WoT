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

#ifndef H2SPELLSTORAGE_H
#define H2SPELLSTORAGE_H

#include <list>
#include "spell.h"
#include "game_io.h"
#include "gamedefs.h"

class SpellStorage
{
public:
	SpellStorage();

	u8 Size(const u8 lvl) const;
	u8 Size1(void) const;
	u8 Size2(void) const;
	u8 Size3(void) const;
	u8 Size4(void) const;
	u8 Size5(void) const;

	const std::list<Spell::spell_t> & Spells(const u8 lvl) const;
	const std::list<Spell::spell_t> & Spells1(void) const;
	const std::list<Spell::spell_t> & Spells2(void) const;
	const std::list<Spell::spell_t> & Spells3(void) const;
	const std::list<Spell::spell_t> & Spells4(void) const;
	const std::list<Spell::spell_t> & Spells5(void) const;

	void Appends(const SpellStorage & st, const u8 wisdom);
	void Append(const Spell::spell_t sp, const u8 wisdom);

protected:
	friend void Game::SaveXML(const std::string &);
	friend void Game::LoadXML(const std::string &);

	std::list<Spell::spell_t>	spells_level1;
	std::list<Spell::spell_t>	spells_level2;
	std::list<Spell::spell_t>	spells_level3;
	std::list<Spell::spell_t>	spells_level4;
	std::list<Spell::spell_t>	spells_level5;
};

#endif
