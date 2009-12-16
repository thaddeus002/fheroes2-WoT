/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "settings.h"

#define SPELL_PER_PAGE		6
#define SPELL_PER_PAGE_SMALL	2

struct SpellFiltered : std::binary_function<Spell::spell_t, SpellBook::filter_t, bool>
{
    bool operator() (Spell::spell_t s, SpellBook::filter_t f) const { return ((SpellBook::ADVN & f) && Spell::isCombat(s)) || ((SpellBook::CMBT & f) && !Spell::isCombat(s)); };
};

void SpellBookRedrawLists(const std::vector<Spell::spell_t> &, std::vector<Rect> &, const size_t, const Point &, const HeroBase *);
void SpellBookRedrawSpells(const std::vector<Spell::spell_t> &, std::vector<Rect> &, const size_t, s16, s16);
void SpellBookRedrawMP(const Point &, u16);

SpellBook::SpellBook(const HeroBase *p) : hero(p), active(false)
{
}

Spell::spell_t SpellBook::Open(filter_t filt, bool canselect) const
{
    if(!active) return Spell::NONE;

    std::vector<Spell::spell_t> spells2(spells);

    if(hero)
    {
	const BagArtifacts & bag = hero->GetBagArtifacts();
	BagArtifacts::const_iterator it1 = bag.begin();
	BagArtifacts::const_iterator it2 = bag.end();
	for(; it1 != it2; ++it1)
	    if(*it1 == Artifact::SPELL_SCROLL)
	    {
		const Spell::spell_t scroll = Spell::FromInt((*it1).GetExt());
		if(Spell::NONE != scroll && spells2.end() == std::find(spells2.begin(), spells2.end(), scroll))
		    spells2.push_back(scroll);
	    }
    }

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    bool small = Settings::Get().PocketPC();

    const Cursor::themes_t oldcursor = cursor.Themes();

    if(small)
    {
        // wait for long operation: Sprite::ScaleMinifyByTwo
        cursor.SetThemes(cursor.WAIT);
        cursor.Show();
        display.Flip();
    }

    const Sprite & r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite & l_list = AGG::GetICN(ICN::BOOK, 0, true);

    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    SetFilter(spells2, filt);

    size_t current_index = 0;

    const Sprite & bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite & bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite & bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite & bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect pos((display.w() - (r_list.w() + l_list.w())) / 2, (display.h() - r_list.h()) / 2, r_list.w() + l_list.w(), r_list.h() + 70);
    Background back(pos);
    back.Save();

    const Rect prev_list(pos.x + (small ? 15 : 30), pos.y + (small ? 4 : 8), (small ? 15 : 30), (small ? 12 : 25));
    const Rect next_list(pos.x + (small ? 205 : 410), pos.y + (small ? 4: 8), (small ? 15 : 30), (small ? 12 :25));

    const Rect info_rt(pos.x + (small ? 64 : 125), pos.y + (small ? 137: 275), bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pos.x + (small ? 135: 270), pos.y + (small ? 135: 270), bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pos.x + (small ? 152: 304), pos.y + (small ? 138: 278), bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pos.x + (small ? 210: 420), pos.y + (small ? 142: 284), bookmark_clos.w(), bookmark_clos.h());

    Spell::spell_t curspell = Spell::NONE;
    
    std::vector<Rect> coords;
    coords.reserve(small ? SPELL_PER_PAGE_SMALL * 2 : SPELL_PER_PAGE * 2);

    SpellBookRedrawLists(spells2, coords, current_index, pos, hero);

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();

    // message loop
    while(le.HandleEvents())
    {
	if(le.MouseClickLeft(prev_list) && current_index)
	{
	    cursor.Hide();
	    current_index -= small ? SPELL_PER_PAGE_SMALL * 2 : SPELL_PER_PAGE * 2;
	    SpellBookRedrawLists(spells2, coords, current_index, pos, hero);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(next_list) && spells2.size() > (current_index + (small ? SPELL_PER_PAGE_SMALL * 2 : SPELL_PER_PAGE * 2)))
	{
	    cursor.Hide();
	    current_index += small ? SPELL_PER_PAGE_SMALL * 2 : SPELL_PER_PAGE * 2;
	    SpellBookRedrawLists(spells2, coords, current_index, pos, hero);
	    cursor.Show();
	    display.Flip();
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
	    cursor.Hide();
	    filt = ADVN;
	    current_index = 0;
	    SetFilter(spells2, filt);
	    SpellBookRedrawLists(spells2, coords, current_index, pos, hero);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(cmbt_rt) && filt != CMBT)
	{
	    cursor.Hide();
	    filt = CMBT;
	    current_index = 0;
	    SetFilter(spells2, filt);
	    SpellBookRedrawLists(spells2, coords, current_index, pos, hero);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(clos_rt) || le.KeyPress(KEY_ESCAPE)) break;
	else
	if(le.MouseClickLeft(pos))
	{
	    std::vector<Rect>::const_iterator it = std::find_if(coords.begin(), coords.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()));
	    if(it != coords.end())
	    {
		Spell::spell_t spell = spells2.at(it - coords.begin() + current_index);

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
	    std::vector<Rect>::const_iterator it = std::find_if(coords.begin(), coords.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()));
	    if(it != coords.end())
	    {
		Spell::spell_t spell = spells2.at(it - coords.begin() + current_index);
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

void SpellBook::SetFilter(std::vector<Spell::spell_t> & v, filter_t filter)
{
    if(filter != ALL)
    {
	std::vector<Spell::spell_t>::iterator it = std::remove_if(v.begin(), v.end(), std::bind2nd(SpellFiltered(), filter));
	if(v.end() != it) v.resize(it - v.begin());
    }
}

void SpellBookRedrawMP(const Point & dst, u16 mp)
{
    bool small = Settings::Get().PocketPC();
    if(mp)
    {
	Point tp(dst.x + (small ? 5 : 11), dst.y + (small ? 1 : 9));
	std::string mps;
	for(int i = 100; i >= 1; i /= 10) if(mp >= i)
	{
	    mps.clear();
	    String::AddInt(mps, (mp % (i * 10)) / i);
	    Text text(mps, Font::SMALL);
	    text.Blit(tp.x - text.w() / 2, tp.y);
	    tp.y += (small ? -2 : 0) + text.h();
	}
    }
}

void SpellBookRedrawLists(const std::vector<Spell::spell_t> & spells, std::vector<Rect> & coords, const size_t cur, const Point & pt, const HeroBase *hero)
{
    Display & display = Display::Get();
    bool small = Settings::Get().PocketPC();

    const Sprite & r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite & l_list = AGG::GetICN(ICN::BOOK, 0, true);
    const Sprite & bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite & bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite & bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite & bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect info_rt(pt.x + (small ? 64 : 125), pt.y + (small ? 137: 275), bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pt.x + (small ? 135: 270), pt.y + (small ? 135: 270), bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pt.x + (small ? 152: 304), pt.y + (small ? 138: 278), bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pt.x + (small ? 210: 420), pt.y + (small ? 142: 284), bookmark_clos.w(), bookmark_clos.h());

    display.Blit(l_list, pt.x, pt.y);
    display.Blit(r_list, pt.x + l_list.w(), pt.y);
    display.Blit(bookmark_info, info_rt);
    display.Blit(bookmark_advn, advn_rt);
    display.Blit(bookmark_cmbt, cmbt_rt);
    display.Blit(bookmark_clos, clos_rt);

    if(coords.size()) coords.clear();

    SpellBookRedrawMP(info_rt, hero ? hero->GetSpellPoints() : 0);
    SpellBookRedrawSpells(spells, coords, cur, pt.x, pt.y);
    SpellBookRedrawSpells(spells, coords, cur + (small ? SPELL_PER_PAGE_SMALL : SPELL_PER_PAGE), pt.x + (small ? 110 : 220), pt.y);
}

void SpellBookRedrawSpells(const std::vector<Spell::spell_t> & spells, std::vector<Rect> & coords, const size_t cur, s16 px, s16 py)
{
    Display & display = Display::Get();
    bool small = Settings::Get().PocketPC();

    u16 ox = 0;
    u16 oy = 0;

    for(u8 ii = 0; ii < (small ? SPELL_PER_PAGE_SMALL : SPELL_PER_PAGE); ++ii) if(spells.size() > cur + ii)
    {
	if(small)
	{
	    if(0 == (ii % SPELL_PER_PAGE_SMALL))
	    {
		oy = 25;
		ox = 60;
	    }
	}
	else
	{
	    if(0 == (ii % (SPELL_PER_PAGE / 2)))
	    {
		oy = 50;
		ox += 80;
	    }
	}

	const Spell::spell_t & spell = spells[ii + cur];
	const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::IndexSprite(spell));
	const Rect rect(px + ox - icon.w() / 2, py + oy - icon.h() / 2, icon.w(), icon.h() + 10);

    	display.Blit(icon, rect.x, rect.y);

    	std::string str(Spell::GetName(spell));
    	str.append(" [");
	String::AddInt(str, Spell::Mana(spell));
	str.append("]");
	    
	TextBox box(str, Font::SMALL, (small ? 94 : 80));
	box.Blit(px + ox - (small ? 47 : 40), py + oy + (small ? 22 : 25));

    	oy += small ? 65 : 80;

	coords.push_back(rect);
    }
}
