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
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

u16 Dialog::ResourceInfo(const std::string &header, const std::string &message, const Resource::funds_t &rs, bool yesNo /* = false */)
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
    const u8 extra = (4 > rs.GetValidItems() ? 50 : (6 > rs.GetValidItems() ? 100 : 160));

    Box box((header.size() ? box1.h() + 10 : 0) + (message.size() ? box2.h() + 10 : 0) + extra, Dialog::OK);

    Rect pos = box.GetArea();

    if(header.size())
    {
        box1.Blit(pos);
        pos.y += box1.h() + 10;
    }

    if(message.size())
    {
        box2.Blit(pos);
        pos.y += box2.h() + 10;
    }

    // draw resource
    Resource::AlignDraw(rs, pos);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point pt[2];
    Button *button[2] = { NULL };
    answer_t result[2] = { Dialog::ZERO };

    if(!yesNo)
    {
        pt[0].x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
        pt[0].y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
        button[0] = new Button(pt[0], system, 1, 2);
        result[0] = Dialog::OK;
    }
    else
    {
        pt[0].x = box.GetArea().x;
        pt[0].y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 5).h();
	    button[0] = new Button(pt[0], system, 5, 6);
	    result[0] = YES;
        pt[1].x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 7).w();
        pt[1].y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 7).h();
	    button[1] = new Button(pt[1], system, 7, 8);
	    result[1] = NO;
    }

    u8 idx;
    for(idx = 0; idx < (u8)sizeof(button)/sizeof(button[0]); idx++)
        if(button[idx])
            button[idx]->Draw();

    cursor.Show();
    display.Flip();

    u16 ret = Dialog::ZERO;

    // message loop
    while(le.HandleEvents() && ret == Dialog::ZERO)
    {
        for(idx = 0; idx < (u8)sizeof(button)/sizeof(button[0]); idx++)
            if(button[idx])
            {
                le.MousePressLeft(*button[idx]) ? button[idx]->PressDraw() : button[idx]->ReleaseDraw();
                if(le.MouseClickLeft(*button[idx]))
                    ret = result[idx];
            }

        if(le.KeyPress(KEY_RETURN))
            ret = result[0];
        else if(le.KeyPress(KEY_ESCAPE))
            ret = result[1];
    }
    
    cursor.Hide();
    delete button[0];
    delete button[1];
    
    return ret;
}
