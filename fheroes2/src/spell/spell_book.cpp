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

#include "spell_book.h"

Spell::Book::Book() : list_count(0)
{
}

void Spell::Book::Appends(const Storage & st)
{
    if(st.Size1())
    {
	spells_level1.insert(spells_level1.begin(), st.Spells1().begin(), st.Spells1().end());
	spells_level1.sort();
	spells_level1.unique();
    }

    if(st.Size2())
    {
	spells_level2.insert(spells_level2.begin(), st.Spells2().begin(), st.Spells2().end());
	spells_level2.sort();
	spells_level2.unique();
    }

    if(st.Size3())
    {
	spells_level3.insert(spells_level3.begin(), st.Spells3().begin(), st.Spells3().end());
	spells_level3.sort();
	spells_level3.unique();
    }

    if(st.Size4())
    {
	spells_level4.insert(spells_level4.begin(), st.Spells4().begin(), st.Spells4().end());
	spells_level4.sort();
	spells_level4.unique();
    }

    if(st.Size5())
    {
	spells_level5.insert(spells_level5.begin(), st.Spells5().begin(), st.Spells5().end());
	spells_level5.sort();
	spells_level5.unique();
    }
}

void Spell::Book::Append(const Spell::spell_t sp)
{
    switch(Spell::Level(sp))
    {
	case 1:
	    if(spells_level1.end() == std::find(spells_level1.begin(), spells_level1.end(), sp))
	    {
		spells_level1.push_back(sp);
		spells_level1.sort();
	    }
	break;
	
	case 2:
	    if(spells_level2.end() == std::find(spells_level2.begin(), spells_level2.end(), sp))
	    {
		spells_level2.push_back(sp);
		spells_level2.sort();
	    }
	break;
	
	case 3:
	    if(spells_level3.end() == std::find(spells_level3.begin(), spells_level3.end(), sp))
	    {
		spells_level3.push_back(sp);
		spells_level3.sort();
	    }
	break;

	case 4:
	    if(spells_level4.end() == std::find(spells_level4.begin(), spells_level4.end(), sp))
	    {
		spells_level4.push_back(sp);
		spells_level4.sort();
	    }
	break;

	case 5:
	    if(spells_level5.end() == std::find(spells_level5.begin(), spells_level5.end(), sp))
	    {
		spells_level5.push_back(sp);
		spells_level5.sort();
	    }
	break;
	
	default: break;
    }
}
