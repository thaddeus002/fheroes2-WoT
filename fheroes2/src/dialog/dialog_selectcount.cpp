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

#include "agg.h"
#include "config.h"
#include "cursor.h"
#include "tools.h"
#include "text.h"
#include "localevent.h"
#include "button.h"
#include "dialog.h"

u16 Dialog::SelectCount(u16 max_count)
{
    Display & display = Display::Get();

    if(max_count < 2) return 0;

    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Box box(60, OK|CANCEL);

    const Rect & pos = box.GetArea();
    Point pt;

    // text
    std::string message("Move how many troops?");
    pt.x = pos.x + (pos.w - Text::width(message, Font::BIG)) / 2;
    pt.y = pos.y;
    Text(message, Font::BIG, pt);

    // sprite edit
    const Surface & sprite_edit = AGG::GetICN(ICN::TOWNWIND, 4);
    pt.x = pos.x + 80;
    pt.y = pos.y + 55;
    display.Blit(sprite_edit, pt);

    u16 result = max_count / 2;
    message.clear();
    String::AddInt(message, result);
    pt.x = pos.x + 80 + (sprite_edit.w() - Text::width(message, Font::BIG)) / 2;
    pt.y = pos.y + 56;
    Text(message, Font::BIG, pt);

    // buttons
    pt.x = pos.x + 150;
    pt.y = pos.y + 51;
    Button buttonUp(pt, ICN::TOWNWIND, 5, 6);

    pt.x = pos.x + 150;
    pt.y = pos.y + 67;
    Button buttonDn(pt, ICN::TOWNWIND, 7, 8);

    pt.x = pos.x;
    pt.y = pos.y + pos.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button buttonOk(pt, system, 1, 2);

    pt.x = pos.x + pos.w - AGG::GetICN(system, 3).w();
    pt.y = pos.y + pos.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button buttonCancel(pt, system, 3, 4);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    buttonUp.Draw();
    buttonDn.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
	le.MousePressLeft(buttonUp) ? buttonUp.PressDraw() : buttonUp.ReleaseDraw();
	le.MousePressLeft(buttonDn) ? buttonDn.PressDraw() : buttonDn.ReleaseDraw();

	// up
	if((le.MouseWheelUp(pos) ||
            le.MouseClickLeft(buttonUp)) && result < max_count)
	{
	    ++result;

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, result);
	    pt.x = pos.x + 80 + (sprite_edit.w() - Text::width(message, Font::BIG)) / 2;
	    pt.y = pos.y + 56;
	    Text(message, Font::BIG, pt);
	    cursor.Show();
	    display.Flip();
	}

	// down
	if((le.MouseWheelDn(pos) ||
            le.MouseClickLeft(buttonDn)) && 1 < result)
	{
	    --result;

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, result);
	    pt.x = pos.x + 80 + (sprite_edit.w() - Text::width(message, Font::BIG)) / 2;
	    pt.y = pos.y + 56;
	    Text(message, Font::BIG, pt);
	    cursor.Show();
	    display.Flip();
	}

        if(le.KeyPress(SDLK_RETURN) || le.MouseClickLeft(buttonOk)) return result;
	if(le.KeyPress(SDLK_ESCAPE) || le.MouseClickLeft(buttonCancel)) return 0;
    }

    return 0;
}
