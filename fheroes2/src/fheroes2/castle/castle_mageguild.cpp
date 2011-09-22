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
#include <vector>
#include <string>
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "castle.h"
#include "race.h"
#include "settings.h"
#include "mageguild.h"
#include "text.h"

RowSpells::RowSpells(const Point & pos, const Castle & castle, u8 lvl)
{
    const MageGuild & guild = castle.GetMageGuild();
    bool hide = castle.GetLevelMageGuild() < lvl;
    const Sprite & roll_show = AGG::GetICN(ICN::TOWNWIND, 0);
    const Sprite & roll_hide = AGG::GetICN(ICN::TOWNWIND, 1);
    const Sprite & roll = (hide ? roll_hide : roll_show);

    u8 count = 0;

    switch(lvl)
    {
	case 1:
	case 2: count = 3; break;
	case 3:
	case 4: count = 2; break;
	case 5: count = 1; break;
	default: break;
    }

    for(u8 ii = 0; ii < count; ++ii)
	coords.push_back(Rect(pos.x + coords.size() * (Settings::Get().QVGA() ? 72 : 110) - roll.w() / 2, pos.y, roll.w(), roll.h()));

    if(castle.HaveLibraryCapability())
    {
	if(! hide && castle.isLibraryBuild())
	    coords.push_back(Rect(pos.x + coords.size() * (Settings::Get().QVGA() ? 72 : 110) - roll_show.w() / 2, pos.y, roll_show.w(), roll_show.h()));
	else
	    coords.push_back(Rect(pos.x + coords.size() * (Settings::Get().QVGA() ? 72 : 110) - roll_hide.w() / 2, pos.y, roll_hide.w(), roll_hide.h()));
    }

    spells.reserve(6);
    spells = guild.GetSpells(castle.GetLevelMageGuild(), castle.isLibraryBuild(), lvl);
    spells.resize(coords.size(), Spell::NONE);
}

void RowSpells::Redraw(void)
{
    const Sprite & roll_show = AGG::GetICN(ICN::TOWNWIND, 0);
    const Sprite & roll_hide = AGG::GetICN(ICN::TOWNWIND, 1);

    std::vector<Rect>::const_iterator it1 = coords.begin();
    std::vector<Rect>::const_iterator it2 = coords.end();

    for(; it1 != it2; ++it1)
    {
	const Rect & dst = (*it1);
	const Spell & spell = spells[it1 - coords.begin()];

	// roll hide
	if(dst.w < roll_show.w() || spell == Spell::NONE)
	{
	    roll_hide.Blit(dst);
	}
	// roll show
	else
	{
	    roll_show.Blit(dst);

	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, spell.IndexSprite());

	    if(Settings::Get().QVGA())
	    {
		icon.Blit(dst.x + 2 + (dst.w - icon.w()) / 2, dst.y + 20 - icon.h() / 2);
	    }
	    else
	    {
		icon.Blit(dst.x + 5 + (dst.w - icon.w()) / 2, dst.y + 40 - icon.h() / 2);

		TextBox text(spell.GetName(), Font::SMALL, 78);
		text.Blit(dst.x + 18, dst.y + 62);
	    }
	}
    }
}

bool RowSpells::QueueEventProcessing(void)
{
    LocalEvent & le = LocalEvent::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    std::vector<Rect>::const_iterator it = std::find_if(coords.begin(), coords.end(),
					    std::bind2nd(RectIncludePoint(), le.GetMouseCursor()));
    if(it == coords.end())
	return false;

    if(le.MouseClickLeft() || le.MousePressRight())
    {
	const Spell & spell = spells[it - coords.begin()];
	if(spell != Spell::NONE)
	{
    	    cursor.Hide();
    	    Dialog::SpellInfo(spell, !le.MousePressRight());
    	    cursor.Show();
    	    display.Flip();
	}
    }

    return true;
}

void Castle::OpenMageGuild(void)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    frameborder.Redraw();
    
    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    AGG::GetICN(ICN::STONEBAK, 0).Blit(dst_pt);

    std::string message;
    Text text;

    // bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    AGG::GetICN(ICN::WELLXTRA, 2).Blit(dst_pt);

    // text bar
    text.Set(_("The above spells have been added to your book."), Font::BIG);
    dst_pt.x = cur_pt.x + 280 - text.w() / 2;
    dst_pt.y = cur_pt.y + 461;
    text.Blit(dst_pt);

    const u8 level = GetLevelMageGuild();
    // sprite
    ICN::icn_t icn = ICN::UNKNOWN;
    switch(race)
    {
        case Race::KNGT: icn = ICN::MAGEGLDK; break;
        case Race::BARB: icn = ICN::MAGEGLDB; break;
        case Race::SORC: icn = ICN::MAGEGLDS; break;
        case Race::WRLK: icn = ICN::MAGEGLDW; break;
        case Race::WZRD: icn = ICN::MAGEGLDZ; break;
        case Race::NECR: icn = ICN::MAGEGLDN; break;
	default: break;
    }
    const Sprite & sprite = AGG::GetICN(icn, level - 1);
    dst_pt.x = cur_pt.x + 90 - sprite.w() / 2;
    dst_pt.y = cur_pt.y + 290 - sprite.h();
    sprite.Blit(dst_pt);

    RowSpells spells5(Point(cur_pt.x + 250, cur_pt.y +  5),  *this, 5);
    RowSpells spells4(Point(cur_pt.x + 250, cur_pt.y +  95), *this, 4);
    RowSpells spells3(Point(cur_pt.x + 250, cur_pt.y + 185), *this, 3);
    RowSpells spells2(Point(cur_pt.x + 250, cur_pt.y + 275), *this, 2);
    RowSpells spells1(Point(cur_pt.x + 250, cur_pt.y + 365), *this, 1);

    spells1.Redraw();
    spells2.Redraw();
    spells3.Redraw();
    spells4.Redraw();
    spells5.Redraw();
    
    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || HotKeyCloseWindow) break;

        if(spells1.QueueEventProcessing() ||
    	    spells2.QueueEventProcessing() ||
    	    spells3.QueueEventProcessing() ||
    	    spells4.QueueEventProcessing() ||
    	    spells5.QueueEventProcessing());
    }
}
