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

#include <string>
#include "agg.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "dialog.h"
#include "background.h"
#include "tools.h"
#include "payment.h"
#include "text.h"

Dialog::answer_t Dialog::BuySpellBook(bool enable)
{
    Display & display = Display::Get();

    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    std::string string_message = "To cast spells, you must first buy a spell book for ";
    String::AddInt(string_message, BUY_SPELL_BOOK_GOLD);
    string_message += " gold. Do you wish to buy one?";

    Dialog::Box box(100, true);

    const Rect & box_rt = box.GetArea();

    Point dst_pt;

    TextBox(string_message, Font::BIG, box_rt);

    const Sprite & sprite1 = AGG::GetICN(ICN::RESOURCE, 7);
    dst_pt.x = box_rt.x + (box_rt.w - sprite1.w()) / 2;
    dst_pt.y = box_rt.y + Text::height(string_message, Font::BIG, box_rt.w) + 10;
    display.Blit(sprite1, dst_pt);

    const Sprite & sprite2 = AGG::GetICN(ICN::ARTIFACT, 82);
    dst_pt.x = box_rt.x + (box_rt.w - sprite2.w()) / 2;
    dst_pt.y += 5;
    display.Blit(sprite2, dst_pt);

    // buttons
    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 5).h();
    Button button1(dst_pt, system, 5, 6);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 7).h();
    Button button2(dst_pt, system, 7, 8);

    if(!enable)
    {
	button1.Press();
	button1.SetDisable(true);
    }

    button1.Draw();
    button2.Draw();
	
    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
        if(button1.isEnable()) le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(le.KeyPress(KEY_RETURN) ||
    	    (button1.isEnable() && le.MouseClickLeft(button1))) return Dialog::YES;

        if(le.KeyPress(KEY_ESCAPE) ||
    	    le.MouseClickLeft(button2)) return Dialog::NO;
    }

    return Dialog::ZERO;
}
