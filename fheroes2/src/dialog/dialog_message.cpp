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
#include "dialog.h"

u16 Dialog::Message(const std::string &header, const std::string &message, Font::type_t ft, u16 buttons)
{
    const std::string &system = (H2Config::EvilInterface() ? "SYSTEME.ICN" : "SYSTEM.ICN");

    // preload
    AGG::PreloadObject(system);

    // cursor
    const Cursor::themes_t cursor = Cursor::Get();
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Box box(Text::height(header, ft, BOXAREA_WIDTH) + 20 + Text::height(message, ft, BOXAREA_WIDTH), buttons);

    Rect pos = box.GetArea();

    if(header.size())
    {
	TextBox(header, ft, pos);
        pos.y += Text::height(header, ft, BOXAREA_WIDTH) + 20;
    }

    if(message.size())
        TextBox(message, ft, pos);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button *button1 = NULL;
    Button *button2 = NULL;
    Point pt;
    answer_t result1 = Dialog::ZERO;
    answer_t result2 = Dialog::ZERO;
    
    switch(buttons){
	case YES|NO:
            pt.x = box.GetArea().x;
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 5).h();
	    button1 = new Button(pt, system, 5, 6);
	    result1 = YES;
            pt.x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 7).w();
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 7).h();
	    button2 = new Button(pt, system, 7, 8);
	    result2 = NO;
	    break;

	case OK|CANCEL:
            pt.x = box.GetArea().x;
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
	    button1 = new Button(pt, system, 1, 2);
	    result1 = OK;
            pt.x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 3).w();
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
	    button2 = new Button(pt, system, 3, 4);
	    result2 = CANCEL;
	    break;

	case OK:
            pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
	    button1 = new Button(pt, system, 1, 2);
	    result1 = OK;
	    break;

	case CANCEL:
            pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 3).w()) / 2;
            pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
	    button1 = new Button(pt, system, 3, 4);
	    result1 = CANCEL;
	    break;

	default:
	    break;
    }

    if(button1) (*button1).Draw();
    if(button2) (*button2).Draw();

    Cursor::Show();
    display.Flip();

    // message loop
    u16 result = Dialog::ZERO;

    while(le.HandleEvents())
    {
        if(!buttons && !le.MouseRight()) break;

	if(button1) le.MousePressLeft(*button1) ? button1->PressDraw() : button1->ReleaseDraw();
        if(button2) le.MousePressLeft(*button2) ? button2->PressDraw() : button2->ReleaseDraw();

        if(button1 && le.MouseClickLeft(*button1)){ result = result1; break; }
        if(button2 && le.MouseClickLeft(*button2)){ result = result2; break; }

	if(le.KeyPress(SDLK_RETURN)){ result = Dialog::YES | Dialog::OK; break; }
	
	if(le.KeyPress(SDLK_ESCAPE)){ result = Dialog::NO | Dialog::CANCEL; break; }
    }

    Cursor::Hide();

    if(button1) delete button1;
    if(button2) delete button2;

    Cursor::Set(cursor);
    Cursor::Show();

    return result;
}
