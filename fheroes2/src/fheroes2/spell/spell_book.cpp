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
#include <functional>
#include "agg.h"
#include "cursor.h"
#include "spell_book.h"
#include "dialog.h"
#include "heroes_base.h"
#include "skill.h"

#define SPELL_PER_PAGE	6

struct SpellFiltered : std::binary_function<Spell::spell_t, SpellBook::filter_t, bool>
{
    bool operator() (Spell::spell_t s, SpellBook::filter_t f) const { return ((SpellBook::ADVN & f) && Spell::isCombat(s)) || ((SpellBook::CMBT & f) && !Spell::isCombat(s)); };
};

void SpellBookRedrawLists(const std::vector<Spell::spell_t> & spells, std::vector<Rect> & coords, const size_t cur, const Point & pt, const HeroBase *hero);

SpellBook::SpellBook(const HeroBase *p) : hero(p), active(false)
{
}

Spell::spell_t SpellBook::Open(filter_t filt, bool canselect) const
{
    if(!active) return Spell::NONE;

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    const Cursor::themes_t oldcursor = cursor.Themes();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    std::vector<Spell::spell_t> spells;
    SetFilter(spells, filt);

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
    
    std::vector<Rect> coords;
    coords.reserve(SPELL_PER_PAGE * 2);

    SpellBookRedrawLists(spells, coords, current_index, pos, hero);

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
	if(le.MouseClickLeft(prev_list) && current_index)
	{
	    current_index -= 2 * SPELL_PER_PAGE;
	    SpellBookRedrawLists(spells, coords, current_index, pos, hero);
	}
	else
	if(le.MouseClickLeft(next_list) && spells.size() > (current_index + 2 * SPELL_PER_PAGE))
	{
	    current_index += 2 * SPELL_PER_PAGE;
	    SpellBookRedrawLists(spells, coords, current_index, pos, hero);
	}
	else
	if((le.MouseClickLeft(info_rt) && hero) ||
	   (le.MousePressRight(info_rt) && hero))
	{
	    std::string str = _("Your hero has %{point} spell points remaining");
	    String::Replace(str, "%{point}", hero->GetSpellPoints());
	    str += " spell points remaining";
	    cursor.Hide();
	    Dialog::Message("", str, Font::BIG, Dialog::OK);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(advn_rt) && filt != ADVN)
	{
	    filt = ADVN;
	    current_index = 0;
	    SetFilter(spells, filt);
	    SpellBookRedrawLists(spells, coords, current_index, pos, hero);
	}
	else
	if(le.MouseClickLeft(cmbt_rt) && filt != CMBT)
	{
	    filt = CMBT;
	    current_index = 0;
	    SetFilter(spells, filt);
	    SpellBookRedrawLists(spells, coords, current_index, pos, hero);
	}
	else
	if(le.MouseClickLeft(clos_rt) || le.KeyPress(KEY_ESCAPE)) break;
	else
	if(le.MouseClickLeft(pos))
	{
	    std::vector<Rect>::const_iterator it = std::find_if(coords.begin(), coords.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()));
	    if(it != coords.end())
	    {
		Spell::spell_t spell = spells.at(it - coords.begin() + current_index);

		if(canselect)
		{
		    if(spell != Spell::NONE && hero)
		    {
			if(hero->GetSpellPoints() >= Spell::Mana(spell))
			{
			    curspell = spell;
			    break;
			}
			else
			{
			    cursor.Hide();
			    std::string str = _("That spell costs %{mana} mana. You only have %{point} mana, so you can't cast the spell.");
			    String::Replace(str, "%{mana}", Spell::Mana(spell));
			    String::Replace(str, "%{point}", hero->GetSpellPoints());
			    Dialog::Message("", str, Font::BIG, Dialog::OK);
			    cursor.Show();
			    display.Flip();
			}
		    }
		}
		else
		if(spell != Spell::NONE)
		{
		    cursor.Hide();
		    Dialog::SpellInfo(Spell::GetName(spell), Spell::GetDescription(spell), spell, true);
		    cursor.Show();
		    display.Flip();
		}
	    }
	}

	if(le.MousePressRight(pos))
	{
	    std::vector<Rect>::const_iterator it = std::find_if(coords.begin(), coords.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()));
	    if(it != coords.end())
	    {
		Spell::spell_t spell = spells.at(it - coords.begin() + current_index);
		if(spell != Spell::NONE)
		{
		    cursor.Hide();
		    Dialog::SpellInfo(Spell::GetName(spell), Spell::GetDescription(spell), spell, false);
		    cursor.Show();
		    display.Flip();
		}
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

void SpellBook::SetFilter(std::vector<Spell::spell_t> & spells, filter_t filter) const
{
    spells.clear();
    spells.resize(spells_level1.size() + spells_level2.size() + spells_level3.size() + spells_level4.size() + spells_level5.size(), Spell::NONE);
    std::vector<Spell::spell_t>::iterator it = spells.begin();

    if(spells_level1.size())
    	it = std::copy(spells_level1.begin(), spells_level1.end(), it);

    if(spells_level2.size())
    	it = std::copy(spells_level2.begin(), spells_level2.end(), it);

    if(spells_level3.size())
    	it = std::copy(spells_level3.begin(), spells_level3.end(), it);

    if(spells_level4.size())
    	it = std::copy(spells_level4.begin(), spells_level4.end(), it);

    if(spells_level5.size())
    	it = std::copy(spells_level5.begin(), spells_level5.end(), it);

    if(filter != ALL)
    {
	it = std::remove_if(spells.begin(), spells.end(), std::bind2nd(SpellFiltered(), filter));
	if(spells.end() != it) spells.resize(it - spells.begin());
    }
}

void SpellBookRedrawLists(const std::vector<Spell::spell_t> & spells, std::vector<Rect> & coords, const size_t cur, const Point & pt, const HeroBase *hero)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

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

    if(coords.size()) coords.clear();
    
    Point tp = info_rt;
    tp += Point(9, 6);
    int mp = hero ? hero->GetSpellPoints() : 0;
    std::string mps;
    for(int i=100; i>=1; i/=10)
    {
	mps = " ";
	if(mp >= i)
	{
	    mps = "";
	    String::AddInt(mps, (mp%(i*10))/i);
	}
	Text text(mps, Font::SMALL);
	text.Blit(tp);
	tp.y += text.h();
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
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::IndexSprite(spell));
	    const Rect rect(pt.x + ox - icon.w() / 2, pt.y + oy - icon.h() / 2, icon.w(), icon.h() + 10);

    	    display.Blit(icon, rect.x, rect.y);

    	    std::string str(Spell::GetName(spell));
    	    str.append(" [");
	    String::AddInt(str, Spell::Mana(spell));
	    str.append("]");
            size_t pos;
            if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
            {
                std::string str1 = str.substr(0, pos);
        	std::string str2 = str.substr(pos);

                Text text(str1, Font::SMALL);
                text.Blit(pt.x + ox - text.w() / 2, pt.y + oy + 22);
                text.Set(str2);
                text.Blit(pt.x + ox - text.w() / 2, pt.y + oy + 31);
            }
            else
            {
                Text text(str, Font::SMALL);
                text.Blit(pt.x + ox - text.w() / 2, pt.y + oy + 25);
	    }
    	    oy += 80;

    	    coords.push_back(rect);
	}
    }
    cursor.Show();
    display.Flip();
}
