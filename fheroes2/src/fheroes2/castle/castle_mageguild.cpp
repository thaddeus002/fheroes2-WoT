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

#include <string>
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "castle.h"
#include "text.h"

void Castle::OpenMageGuild(void)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN(ICN::STONEBAK, 0), dst_pt);

    std::string message;

    // bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    display.Blit(AGG::GetICN(ICN::WELLXTRA, 2), dst_pt);

    // text bar
    message = "The above spells have been added to your book.";
    dst_pt.x = cur_pt.x + 280 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 461;
    Text(message, Font::BIG, dst_pt);

    const u8 level = mageguild.GetLevel();
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
    display.Blit(sprite, dst_pt);

    const Sprite & roll_hide = AGG::GetICN(ICN::TOWNWIND, 1);
    const Sprite & roll_show = AGG::GetICN(ICN::TOWNWIND, 0);

    // level 5
    std::vector<Rect> rectsLevel5;
    const Sprite & roll5 = (4 < level ? roll_show : roll_hide);
    u8 count = mageguild.isUpgrade() ? 2 : 1;
    for(u8 ii = 0; ii < count; ++ii)
    {
	dst_pt.x = cur_pt.x + 250 + ii * 110;
	dst_pt.y = cur_pt.y + 10;
	display.Blit(roll5, dst_pt.x - roll5.w() / 2, dst_pt.y);

	// spell icon
	if(4 < level)
	{
	    const Spell::spell_t spell = mageguild.GetSpell(5, ii);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));

	    display.Blit(icon, dst_pt.x + 5 - icon.w() / 2, dst_pt.y + 40 - icon.h() / 2);

	    const std::string & str = Spell::String(spell);
	    size_t pos;
	    if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
	    {
		std::string str1 = str.substr(0, pos);
		std::string str2 = str.substr(pos);

		Text(str1, Font::SMALL, dst_pt.x + 5 - Text::width(str1, Font::SMALL) / 2, dst_pt.y + 62);
		Text(str2, Font::SMALL, dst_pt.x + 5 - Text::width(str2, Font::SMALL) / 2, dst_pt.y + 71);
	    }
	    else
		Text(str, Font::SMALL, dst_pt.x + 5 - Text::width(str, Font::SMALL) / 2, dst_pt.y + 65);

	    rectsLevel5.push_back(Rect(dst_pt.x - roll5.w() / 2, dst_pt.y, roll5.w(), roll5.h()));
	}
    }

    // level 4
    std::vector<Rect> rectsLevel4;
    const Sprite & roll4 = (3 < level ? roll_show : roll_hide);
    count = mageguild.isUpgrade() ? 3 : 2;
    for(u8 ii = 0; ii < count; ++ii)
    {
	dst_pt.x = cur_pt.x + 250 + ii * 110;
	dst_pt.y = cur_pt.y + 95;
	display.Blit(roll4, dst_pt.x  - roll4.w() / 2, dst_pt.y);

	// spell icon
	if(3 < level)
	{
	    const Spell::spell_t spell = mageguild.GetSpell(4, ii);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));

	    display.Blit(icon, dst_pt.x + 5 - icon.w() / 2, dst_pt.y + 40 - icon.h() / 2);

	    const std::string & str = Spell::String(spell);
	    size_t pos;
	    if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
	    {
		std::string str1 = str.substr(0, pos);
		std::string str2 = str.substr(pos);

		Text(str1, Font::SMALL, dst_pt.x + 5 - Text::width(str1, Font::SMALL) / 2, dst_pt.y + 62);
		Text(str2, Font::SMALL, dst_pt.x + 5 - Text::width(str2, Font::SMALL) / 2, dst_pt.y + 71);
	    }
	    else
		Text(str, Font::SMALL, dst_pt.x + 5 - Text::width(str, Font::SMALL) / 2, dst_pt.y + 65);

	    rectsLevel4.push_back(Rect(dst_pt.x - roll4.w() / 2, dst_pt.y, roll4.w(), roll4.h()));
	}
    }

    // level 3
    std::vector<Rect> rectsLevel3;
    const Sprite & roll3 = (2 < level ? roll_show : roll_hide);
    count = mageguild.isUpgrade() ? 3 : 2;
    for(u8 ii = 0; ii < count; ++ii)
    {
	dst_pt.x = cur_pt.x + 250 + ii * 110;
	dst_pt.y = cur_pt.y + 180;
	display.Blit(roll3, dst_pt.x  - roll3.w() / 2, dst_pt.y);

	// spell icon
	if(2 < level)
	{
	    const Spell::spell_t spell = mageguild.GetSpell(3, ii);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));

	    display.Blit(icon, dst_pt.x + 5 - icon.w() / 2, dst_pt.y + 40 - icon.h() / 2);

	    const std::string & str = Spell::String(spell);
	    size_t pos;
	    if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
	    {
		std::string str1 = str.substr(0, pos);
		std::string str2 = str.substr(pos);

		Text(str1, Font::SMALL, dst_pt.x + 5 - Text::width(str1, Font::SMALL) / 2, dst_pt.y + 62);
		Text(str2, Font::SMALL, dst_pt.x + 5 - Text::width(str2, Font::SMALL) / 2, dst_pt.y + 71);
	    }
	    else
		Text(str, Font::SMALL, dst_pt.x + 5 - Text::width(str, Font::SMALL) / 2, dst_pt.y + 65);

	    rectsLevel3.push_back(Rect(dst_pt.x - roll3.w() / 2, dst_pt.y, roll3.w(), roll3.h()));
	}
    }

    // level 2
    std::vector<Rect> rectsLevel2;
    const Sprite & roll2 = (1 < level ? roll_show : roll_hide);
    count = mageguild.isUpgrade() ? 4 : 3;
    for(u8 ii = 0; ii < count; ++ii)
    {
	dst_pt.x = cur_pt.x + 250 + ii * 110;
	dst_pt.y = cur_pt.y + 265;
	display.Blit(roll2, dst_pt.x - roll2.w() / 2, dst_pt.y);
	
	// spell icon
	if(1 < level)
	{
	    const Spell::spell_t spell = mageguild.GetSpell(2, ii);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));
	    
	    display.Blit(icon, dst_pt.x + 5 - icon.w() / 2, dst_pt.y + 40 - icon.h() / 2);

	    const std::string & str = Spell::String(spell);
	    size_t pos;
	    if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
	    {
		std::string str1 = str.substr(0, pos);
		std::string str2 = str.substr(pos);

		Text(str1, Font::SMALL, dst_pt.x + 5 - Text::width(str1, Font::SMALL) / 2, dst_pt.y + 62);
		Text(str2, Font::SMALL, dst_pt.x + 5 - Text::width(str2, Font::SMALL) / 2, dst_pt.y + 71);
	    }
	    else
		Text(str, Font::SMALL, dst_pt.x + 5 - Text::width(str, Font::SMALL) / 2, dst_pt.y + 65);

	    rectsLevel2.push_back(Rect(dst_pt.x - roll2.w() / 2, dst_pt.y, roll2.w(), roll2.h()));
	}
    }

    // show level 1 icon spell
    std::vector<Rect> rectsLevel1;
    const Sprite & roll1 = (level ? roll_show : roll_hide);
    count = mageguild.isUpgrade() ? 4 : 3;
    for(u8 ii = 0; ii < count; ++ii)
    {
	dst_pt.x = cur_pt.x + 250 + ii * 110;
	dst_pt.y = cur_pt.y + 350;
	display.Blit(roll1, dst_pt.x  - roll1.w() / 2, dst_pt.y);
	
	// spell icon
	if(level)
	{
	    const Spell::spell_t spell = mageguild.GetSpell(1, ii);
	    const Sprite & icon = AGG::GetICN(ICN::SPELLS, Spell::GetIndexSprite(spell));

	    display.Blit(icon, dst_pt.x + 5 - icon.w() / 2, dst_pt.y + 40 - icon.h() / 2);

	    const std::string & str = Spell::String(spell);
	    size_t pos;
	    if(str.size() > 10 && std::string::npos != (pos = str.find(0x20)))
	    {
		std::string str1 = str.substr(0, pos);
		std::string str2 = str.substr(pos);

		Text(str1, Font::SMALL, dst_pt.x + 5 - Text::width(str1, Font::SMALL) / 2, dst_pt.y + 62);
		Text(str2, Font::SMALL, dst_pt.x + 5 - Text::width(str2, Font::SMALL) / 2, dst_pt.y + 71);
	    }
	    else
		Text(str, Font::SMALL, dst_pt.x + 5 - Text::width(str, Font::SMALL) / 2, dst_pt.y + 65);

	    rectsLevel1.push_back(Rect(dst_pt.x - roll1.w() / 2, dst_pt.y, roll1.w(), roll1.h()));
	}
    }

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
        
        // level 1
        if(level)
        {
    	    for(u8 ii = 0; ii < rectsLevel1.size(); ++ii)
    	    {
    		const Spell::spell_t & spell = mageguild.GetSpell(1, ii);

    		if(le.MouseClickLeft(rectsLevel1[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell);
    		    cursor.Show();
    		    display.Flip();
    		}
    		else
    		if(le.MousePressRight(rectsLevel1[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
    		    cursor.Show();
    		    display.Flip();
    		}
    	    }
        }

        // level 2
        if(1 < level)
        {
    	    for(u8 ii = 0; ii < rectsLevel2.size(); ++ii)
    	    {
    		const Spell::spell_t & spell = mageguild.GetSpell(2, ii);

    		if(le.MouseClickLeft(rectsLevel2[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell);
    		    cursor.Show();
    		    display.Flip();
    		}
    		else
    		if(le.MousePressRight(rectsLevel2[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
    		    cursor.Show();
    		    display.Flip();
    		}
    	    }
        }

        // level 3
        if(2 < level)
        {
    	    for(u8 ii = 0; ii < rectsLevel3.size(); ++ii)
    	    {
    		const Spell::spell_t & spell = mageguild.GetSpell(3, ii);

    		if(le.MouseClickLeft(rectsLevel3[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell);
    		    cursor.Show();
    		    display.Flip();
    		}
    		else
    		if(le.MousePressRight(rectsLevel3[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
    		    cursor.Show();
    		    display.Flip();
    		}
    	    }
        }

        // level 4
        if(3 < level)
        {
    	    for(u8 ii = 0; ii < rectsLevel4.size(); ++ii)
    	    {
    		const Spell::spell_t & spell = mageguild.GetSpell(4, ii);

    		if(le.MouseClickLeft(rectsLevel4[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell);
    		    cursor.Show();
    		    display.Flip();
    		}
    		else
    		if(le.MousePressRight(rectsLevel4[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
    		    cursor.Show();
    		    display.Flip();
    		}
    	    }
        }

        // level 5
        if(4 < level)
        {
    	    for(u8 ii = 0; ii < rectsLevel5.size(); ++ii)
    	    {
    		const Spell::spell_t & spell = mageguild.GetSpell(5, ii);

    		if(le.MouseClickLeft(rectsLevel5[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell);
    		    cursor.Show();
    		    display.Flip();
    		}
    		else
    		if(le.MousePressRight(rectsLevel5[ii]))
    		{
    		    cursor.Hide();
    		    Dialog::SpellInfo(Spell::String(spell), Spell::Description(spell), spell, false);
    		    cursor.Show();
    		    display.Flip();
    		}
    	    }
        }

    }
}
