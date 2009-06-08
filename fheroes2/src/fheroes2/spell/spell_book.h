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

class HeroBase;
class Heroes;

class SpellBook : public SpellStorage
{
public:
	enum filter_t
	{
	    ADVN = 0x01,
	    CMBT = 0x02,
	    ALL  = ADVN | CMBT,
	};

	SpellBook(const HeroBase *p);

	Spell::spell_t Open(filter_t filt, bool canselect) const;
	void Activate(void) { active = true; };
	bool isActive(void) const { return active; };

private:
	friend void Game::SaveXMLDoc(TiXmlDocument &);
	friend bool Game::LoadXMLDoc(const TiXmlDocument &);

	void SetFilter(std::vector<Spell::spell_t> & spells, filter_t filter) const;

	const HeroBase *hero;
	bool active;
};

#endif
