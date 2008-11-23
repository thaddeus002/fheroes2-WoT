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
#include "race.h"
#include "spell_storage.h"

class MageGuild : public Spell::Storage
{
    public:
	MageGuild(const Race::race_t & rc);

	u8 GetLevel(void) const;
	Spell::spell_t GetSpell(u8 level, u8 index) const;

	void BuildNextLevel(void);
	void UpgradeExt(void);
	
	bool isUpgrade(void) const{ return upgrade; };
	
    protected:
	void PackSpells(const u8 lvl);
	void AddExtSpells(const u8 lvl);

	static Spell::spell_t GetSpellRace(const Race::race_t rc, const u8 lvl);

    private:
	const Race::race_t &	race;
	u8			level;
	bool			upgrade;
};

#endif
