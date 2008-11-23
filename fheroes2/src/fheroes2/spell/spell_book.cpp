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
#include "agg.h"
#include "cursor.h"
#include "spell_book.h"
#include "dialog.h"
#include "heroes.h"

#define SPELL_PER_PAGE	6

Spell::Book::Book(const Heroes &h) : hero(&h), active(false), list_count(0)
{
}

void Spell::Book::Appends(const Storage & st, const Skill::Level::type_t & wisdom)
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

    if(st.Size3() && Skill::Level::BASIC <= wisdom)
    {
	spells_level3.insert(spells_level3.begin(), st.Spells3().begin(), st.Spells3().end());
	spells_level3.sort();
	spells_level3.unique();
    }

    if(st.Size4() && Skill::Level::ADVANCED <= wisdom)
    {
	spells_level4.insert(spells_level4.begin(), st.Spells4().begin(), st.Spells4().end());
	spells_level4.sort();
	spells_level4.unique();
    }

    if(st.Size5() && Skill::Level::EXPERT == wisdom)
    {
	spells_level5.insert(spells_level5.begin(), st.Spells5().begin(), st.Spells5().end());
	spells_level5.sort();
	spells_level5.unique();
    }
}

void Spell::Book::Append(const Spell::spell_t sp, const Skill::Level::type_t & wisdom)
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
	    if(Skill::Level::BASIC <= wisdom &&
		spells_level3.end() == std::find(spells_level3.begin(), spells_level3.end(), sp))
	    {
		spells_level3.push_back(sp);
		spells_level3.sort();
	    }
	break;

	case 4:
	    if(Skill::Level::ADVANCED <= wisdom &&
		spells_level4.end() == std::find(spells_level4.begin(), spells_level4.end(), sp))
	    {
		spells_level4.push_back(sp);
		spells_level4.sort();
	    }
	break;

	case 5:
	    if(Skill::Level::EXPERT == wisdom &&
		spells_level5.end() == std::find(spells_level5.begin(), spells_level5.end(), sp))
	    {
		spells_level5.push_back(sp);
		spells_level5.sort();
	    }
	break;
	
	default: break;
    }
}

Spell::spell_t Spell::Book::Open(filter_t filt, bool canselect) const
{
    if(!active) return Spell::NONE;

    Display & display = Display::Get();

    Cursor & cursor = Cursor::Get();
    const Cursor::themes_t oldcursor = cursor.Themes();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    std::vector<Spell::spell_t> spells;

    if(filt == CMBT)
	Filter(spells, false);
    else
	Filter(spells, true);

    size_t current_index = 0;

    const Sprite & r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite & l_list = AGG::GetICN(ICN::BOOK, 0, true);
    const Sprite & bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite & bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite & bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite & bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect pos((display.w() - (r_list.w() + l_list.w())) / 2, (display.h() - r_list.h()) / 2, r_list.w() + l_list.w(), r_list.h() + 70);
    Background back(pos);
    back.Save();

    const Rect prev_list(pos.x + 30, pos.y + 8 ,30, 25);
    const Rect next_list(pos.x + 410, pos.y + 8, 30, 25);

    const Rect info_rt(pos.x + 125, pos.y + 275, bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pos.x + 270, pos.y + 270, bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pos.x + 304, pos.y + 278, bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pos.x + 420, pos.y + 284, bookmark_clos.w(), bookmark_clos.h());

    Spell::spell_t curspell = Spell::NONE;

    RedrawLists(spells, current_index, pos);

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
	if(le.MouseClickLeft(prev_list) && current_index)
	{
	    current_index -= 2 * SPELL_PER_PAGE;

	    cursor.Hide();
	    RedrawLists(spells, current_index, pos);
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(next_list) && spells.size() > (current_index + 2 * SPELL_PER_PAGE))
	{
	    current_index += 2 * SPELL_PER_PAGE;

	    cursor.Hide();
	    RedrawLists(spells, current_index, pos);
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(info_rt)) {
	    std::string str = "Your hero has ";
	    String::AddInt(str, hero->GetSpellPoints());
	    str += " spell points remaining";
	    Dialog::Message("", str, Font::BIG, Dialog::OK);
	}

	if(le.MousePressRight(info_rt)) {
	    std::string str = "Your hero has ";
	    String::AddInt(str, hero->GetSpellPoints());
	    str += " spell points remaining";
	    Dialog::Message("", str, Font::BIG);
	}

	if(le.MouseClickLeft(advn_rt) && filt != CMBT)
	{
	    Filter(spells, true);

	    cursor.Hide();
	    RedrawLists(spells, current_index, pos);
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(cmbt_rt) && filt != ADVN)
	{
	    Filter(spells, false);

	    cursor.Hide();
	    RedrawLists(spells, current_index, pos);
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(clos_rt) || le.KeyPress(KEY_ESCAPE)) break;

	if(le.MouseClickLeft(pos))
	{
	    Spell::spell_t spell = GetSelected(spells, current_index, pos);
	    if(canselect) {
		if(spell != Spell::NONE) {
		    if(hero->GetSpellPoints() >= Spell::Mana(spell)) {
			curspell = spell;
			break;
		    } else {
			std::string str = "That spell costs ";
			String::AddInt(str, Spell::Mana(spell));
			str += " mana. You only have ";
			String::AddInt(str, hero->GetSpellPoints());
			str += " mana, so you can't cast the spell.";
			Dialog::Message("", str, Font::BIG, Dialog::OK);
		    }
		}
	    } else if(spell != Spell::NONE) {
		cursor.Hide();
		Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, true);
		cursor.Show();
		display.Flip();
	    }
	}

	if(le.MousePressRight(pos)) {
	    Spell::spell_t spell = GetSelected(spells, current_index, pos);
	    if(spell != Spell::NONE) {
		cursor.Hide();
		Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
		cursor.Show();
		display.Flip();
	    }
	}
    }

    cursor.Hide();
    back.Restore();
    cursor.SetThemes(oldcursor);
    cursor.Show();
    display.Flip();

    return curspell;
}

void Spell::Book::RedrawLists(const std::vector<Spell::spell_t> & spells, const size_t cur, const Point & pt) const
{
    Display & display = Display::Get();

    const Sprite & r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite & l_list = AGG::GetICN(ICN::BOOK, 0, true);
    const Sprite & bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite & bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite & bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite & bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect info_rt(pt.x + 125, pt.y + 275, bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pt.x + 270, pt.y + 270, bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pt.x + 304, pt.y + 278, bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pt.x + 420, pt.y + 284, bookmark_clos.w(), bookmark_clos.h());

    display.Blit(l_list, pt.x, pt.y);
    display.Blit(r_list, pt.x + l_list.w(), pt.y);
    display.Blit(bookmark_info, info_rt);
    display.Blit(bookmark_advn, advn_rt);
    display.Blit(bookmark_cmbt, cmbt_rt);
    display.Blit(bookmark_clos, clos_rt);

    Point tp = info_rt;
    tp += Point(9, 6);
    int mp = hero->GetSpellPoints();
    std::string mps;
    for(int i=100; i>=1; i/=10) {
	mps = " ";
	if(mp >= i) {
	    mps = "";
	    String::AddInt(mps, (mp%(i*10))/i);
	}
	Text(mps, Font::SMALL, tp);
	tp.y += Text::height(mps, Font::SMALL);
    }

    u16 ox = 0;
    u16 oy = 0;

    for(u8 ii = 0; ii < 2 * SPELL_PER_PAGE; ++ii)
    {
	if(spells.size() > cur + ii)
	{
	    if(!(ii % 3))
	    {
		oy = 50;
		ox += 80;
	    }

	    if(SPELL_PER_PAGE == ii) ox += 70;

	    const Spell::spell_t & spell = spells.at(ii + cur);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));

    	    display.Blit(icon, pt.x + ox - icon.w() / 2, pt.y + oy - icon.h() / 2);

    	    std::string str = Spell::String(spell) + " [";
	    String::AddInt(str, Spell::Mana(spell));
	    str += "]";
            size_t pos;
            if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
            {
                std::string str1 = str.substr(0, pos);
        	std::string str2 = str.substr(pos);

                Text(str1, Font::SMALL, pt.x + ox - Text::width(str1, Font::SMALL) / 2, pt.y + oy + 22);
                Text(str2, Font::SMALL, pt.x + ox - Text::width(str2, Font::SMALL) / 2, pt.y + oy + 31);
            }
            else
                Text(str, Font::SMALL, pt.x + ox - Text::width(str, Font::SMALL) / 2, pt.y + oy + 25);

    	    oy += 80;
	}
    }
}

Spell::spell_t Spell::Book::GetSelected(const std::vector<Spell::spell_t> & spells, const size_t cur, const Point & pt)
{
    u16 ox = 0;
    u16 oy = 0;

    for(u8 ii = 0; ii < 2 * SPELL_PER_PAGE; ++ii)
    {
	if(spells.size() > cur + ii)
	{
	    if(!(ii % 3))
	    {
		oy = 50;
		ox += 80;
	    }

	    if(SPELL_PER_PAGE == ii) ox += 70;

	    const Spell::spell_t & spell = spells.at(ii + cur);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));
	    Rect rt(pt.x + ox - icon.w() / 2, pt.y + oy - icon.h() / 2, icon.w(), icon.h());

	    LocalEvent & le = LocalEvent::GetLocalEvent();
	    if(rt & le.MouseCursor()) return spell;

    	    oy += 80;
	}
    }
    return Spell::NONE;
}

void Spell::Book::Filter(std::vector<Spell::spell_t> & spells, bool adv_mode) const
{
    if(spells.size()) spells.clear();
    
    std::list<Spell::spell_t>::const_iterator it1;
    std::list<Spell::spell_t>::const_iterator it2;

    if(spells_level1.size())
    {
	it1 = spells_level1.begin();
	it2 = spells_level1.end();
	
	for(; it1 != it2; ++it1)
	    if((adv_mode && !Spell::isCombat(*it1)) || (!adv_mode && Spell::isCombat(*it1)))
		spells.push_back(*it1);
    }

    if(spells_level2.size())
    {
	it1 = spells_level2.begin();
	it2 = spells_level2.end();
	
	for(; it1 != it2; ++it1)
	    if((adv_mode && !Spell::isCombat(*it1)) || (!adv_mode && Spell::isCombat(*it1)))
		spells.push_back(*it1);
    }

    if(spells_level3.size())
    {
	it1 = spells_level3.begin();
	it2 = spells_level3.end();
	
	for(; it1 != it2; ++it1)
	    if((adv_mode && !Spell::isCombat(*it1)) || (!adv_mode && Spell::isCombat(*it1)))
		spells.push_back(*it1);
    }

    if(spells_level4.size())
    {
	it1 = spells_level4.begin();
	it2 = spells_level4.end();
	
	for(; it1 != it2; ++it1)
	    if((adv_mode && !Spell::isCombat(*it1)) || (!adv_mode && Spell::isCombat(*it1)))
		spells.push_back(*it1);
    }

    if(spells_level5.size())
    {
	it1 = spells_level5.begin();
	it2 = spells_level5.end();
	
	for(; it1 != it2; ++it1)
	    if((adv_mode && !Spell::isCombat(*it1)) || (!adv_mode && Spell::isCombat(*it1)))
		spells.push_back(*it1);
    }
}
