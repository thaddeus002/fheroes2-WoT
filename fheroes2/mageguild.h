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
#ifndef H2MAGEGUILD_H
#define H2MAGEGUILD_H

#include <vector>
#include "gamedefs.h"
#include "spell.h"

class MageGuild
{
    public:
	MageGuild();

	u8 GetLevel(void) const;
	//const std::vector<Spell::spell_t> & GetSpells(u8 lvl);

	void Upgrade(void);

    private:
	u8 level;

	std::vector<Spell::spell_t>	level1;
	std::vector<Spell::spell_t>	level2;
	std::vector<Spell::spell_t>	level3;
	std::vector<Spell::spell_t>	level4;
	std::vector<Spell::spell_t>	level5;
	std::vector<Spell::spell_t>	level6;
};

#endif
