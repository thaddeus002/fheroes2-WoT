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

#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

u16 Dialog::SpriteInfo(const std::string &header, const std::string &message, const Surface & sprite, u16 buttons)
{
    Display & display = Display::Get();
    const ICN::icn_t system = Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    TextBox box1(header, Font::BIG, BOXAREA_WIDTH);
    TextBox box2(message, Font::BIG, BOXAREA_WIDTH);
    const u8 spacer = Settings::Get().PocketPC() ? 5 : 10;

    Box box(box1.h() + spacer + box2.h() + spacer + sprite.h(), buttons);
    Rect pos = box.GetArea();

    if(header.size()) box1.Blit(pos);
    pos.y += box1.h() + spacer;

    if(message.size()) box2.Blit(pos);
    pos.y += box2.h() + spacer;

    // blit sprite
    pos.x = box.GetArea().x + (pos.w - sprite.w()) / 2;
    display.Blit(sprite, pos.x, pos.y);

    LocalEvent & le = LocalEvent::Get();

    Button *button1 = NULL;
    Button *button2 = NULL;
    Point pt;
    answer_t result1 = Dialog::ZERO;
    answer_t result2 = Dialog::ZERO;
    
    switch(buttons)
    {
	case YES|NO:
            pt.x = box.GetArea().x;
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 5).h();
	    button1 = new Button(pt, system, 5, 6);
	    result1 = YES;
            pt.x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 7).w();
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 7).h();
	    button2 = new Button(pt, system, 7, 8);
	    result2 = NO;
	    break;

	case OK|CANCEL:
            pt.x = box.GetArea().x;
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 1).h();
	    button1 = new Button(pt, system, 1, 2);
	    result1 = OK;
            pt.x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 3).w();
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 3).h();
	    button2 = new Button(pt, system, 3, 4);
	    result2 = CANCEL;
	    break;

	case OK:
            pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 1).h();
	    button1 = new Button(pt, system, 1, 2);
	    result1 = OK;
	    break;

	case CANCEL:
            pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 3).w()) / 2;
            pt.y = box.GetArea().y + box.GetArea().h - AGG::GetICN(system, 3).h();
	    button1 = new Button(pt, system, 3, 4);
	    result1 = CANCEL;
	    break;

	default:
	    break;
    }

    if(button1) (*button1).Draw();
    if(button2) (*button2).Draw();

    cursor.Show();
    display.Flip();

    // message loop
    u16 result = Dialog::ZERO;

    while(le.HandleEvents())
    {
        if(!buttons && !le.MousePressRight()) break;

	if(button1) le.MousePressLeft(*button1) ? button1->PressDraw() : button1->ReleaseDraw();
        if(button2) le.MousePressLeft(*button2) ? button2->PressDraw() : button2->ReleaseDraw();

        if(button1 && le.MouseClickLeft(*button1)){ result = result1; break; }
        if(button2 && le.MouseClickLeft(*button2)){ result = result2; break; }

	if(le.KeyPress(KEY_RETURN)){ result = Dialog::YES | Dialog::OK; break; }
	
	if(le.KeyPress(KEY_ESCAPE)){ result = Dialog::NO | Dialog::CANCEL; break; }
    }

    cursor.Hide();

    if(button1) delete button1;
    if(button2) delete button2;

    return result;
}
