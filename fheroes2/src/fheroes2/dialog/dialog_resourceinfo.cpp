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
#include "text.h"
#include "localevent.h"
#include "button.h"
#include "tools.h"
#include "dialog.h"

void Dialog::ResourceInfo(const std::string &header, const std::string &message, const Resource::funds_t &rs)
{
    Display & display = Display::Get();
    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Box box(Text::height(header, Font::BIG, BOXAREA_WIDTH) + 50 + Text::height(message, Font::BIG, BOXAREA_WIDTH), Dialog::OK);

    Rect pos = box.GetArea();

    if(header.size())
    {
	TextBox(header, Font::BIG, pos);
        pos.y += Text::height(header, Font::BIG, BOXAREA_WIDTH) + 10;
    }

    if(message.size())
    {
        TextBox(message, Font::BIG, pos);
        pos.y += Text::height(message, Font::BIG, BOXAREA_WIDTH) + 5;
    }

    // draw resource
    Resource::AlignDraw(rs, pos);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point pt;
    
    pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button button(pt, system, 1, 2);

    button.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(button) ? button.PressDraw() : button.ReleaseDraw();

        if(le.MouseClickLeft(button)){ break; }

	if(le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)){ break; }
    }

    cursor.Hide();
}
