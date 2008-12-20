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

#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

bool Dialog::SelectGoldOrExp(const std::string &header, const std::string &message, const u16 gold, const u16 expr)
{
    Display & display = Display::Get();
    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;
        
    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite & sprite_gold = AGG::GetICN(ICN::RESOURCE, 6);
    const Sprite & sprite_expr = AGG::GetICN(ICN::EXPMRL, 4);

    Point pt;
    const u16 height1 = Text::height(header, Font::BIG, BOXAREA_WIDTH);
    const u16 height2 = Text::height(message, Font::BIG, BOXAREA_WIDTH);

    Box box(height1 + 20 + height2 + 10 + sprite_expr.h(), true);

    pt.x = box.GetArea().x + box.GetArea().w / 2 - AGG::GetICN(system, 9).w() - 20;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 5).h();
    Button button_yes(pt, system, 5, 6);

    pt.x = box.GetArea().x + box.GetArea().w / 2 + 20;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 7).h();
    Button button_no(pt, system, 7, 8);

    Rect pos = box.GetArea();
    Text text;
    std::string str;

    if(header.size())
    {
	TextBox(header, Font::BIG, pos);
        pos.y += height1 + 20;
    }

    if(message.size())
    {
        TextBox(message, Font::BIG, pos);
        pos.y += height2 + 20;
    }

    pos.y += sprite_expr.h();
    // sprite1
    pos.x = box.GetArea().x + box.GetArea().w / 2 - sprite_gold.w() - 30;
    display.Blit(sprite_gold, pos.x, pos.y - sprite_gold.h());
    // text
    str.clear();
    String::AddInt(str, gold);
    text.Set(str, Font::SMALL);
    text.Blit(pos.x + (sprite_gold.w() - text.w()) / 2, pos.y + 2);

    // sprite2
    pos.x = box.GetArea().x + box.GetArea().w / 2 + 30;
    display.Blit(sprite_expr, pos.x, pos.y - sprite_expr.h());
    // text
    str.clear();
    String::AddInt(str, expr);
    text.Set(str, Font::SMALL);
    text.Blit(pos.x + (sprite_expr.w() - text.w()) / 2, pos.y + 2);

    button_yes.Draw();
    button_no.Draw();

    cursor.Show();
    display.Flip();
    LocalEvent & le = LocalEvent::GetLocalEvent();
    bool result = false;

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(button_yes) ? button_yes.PressDraw() : button_yes.ReleaseDraw();
	le.MousePressLeft(button_no) ? button_no.PressDraw() : button_no.ReleaseDraw();

        if(le.MouseClickLeft(button_yes)){ result = true; break; }
        if(le.MouseClickLeft(button_no)){ result = false; break; }
    }

    cursor.Hide();

    return result;
}
