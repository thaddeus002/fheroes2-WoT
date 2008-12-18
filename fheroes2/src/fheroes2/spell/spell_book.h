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

#ifndef H2SPELLBOOK_H
#define H2SPELLBOOK_H

#include "gamedefs.h"
#include "spell_storage.h"

class Heroes;
namespace Skill { class Primary; };

namespace Spell
{
    class Book : public Storage
    {
    public:
	Book(const Skill::Primary *p);

        void Appends(const Storage & st, const u8 wisdom);
        void Append(const Spell::spell_t sp, const u8 wisdom);

	typedef enum
	{
	    ALL = 0,
	    ADVN,
	    CMBT
	} filter_t;

	Spell::spell_t Open(filter_t filt = ALL, bool canselect=false) const;
	void Activate(void) { active = true; };
	bool isActive(void) const { return active; };

    private:
	void RedrawLists(const std::vector<Spell::spell_t> & spells, const size_t cur, const Point & pt) const;
	static Spell::spell_t GetSelected(const std::vector<Spell::spell_t> & spells, const size_t cur, const Point & pt);
	void Filter(std::vector<Spell::spell_t> & spells, bool adv_mode) const;

	const Skill::Primary *hero;
	bool active;
	u8 list_count;
    };
};

#endif
