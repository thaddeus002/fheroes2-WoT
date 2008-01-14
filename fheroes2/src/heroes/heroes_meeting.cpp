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
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "heroes.h"
#include "portrait.h"

void Heroes::MeetingDialog(Heroes & heroes2)
{
    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);

    Point dst_pt(cur_pt);

    std::string message;



    display.Blit(AGG::GetICN("STONEBAK.ICN", 0), dst_pt);

    u8 w = 22;

    Rect src_rt(w, w, 640 - 2 * w, 480 - 2 * w);

    // background
    dst_pt.x = cur_pt.x + w;
    dst_pt.y = cur_pt.y + w;
    display.Blit(AGG::GetICN("SWAPWIN.ICN", 0), src_rt, dst_pt);

    // header
    message = GetName() + " meets " + heroes2.GetName();
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 26;
    Text(message, Font::BIG, dst_pt);

    // portrait
    dst_pt.x = cur_pt.x + 93;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(GetHeroes(), Portrait::BIG), dst_pt);

    dst_pt.x = cur_pt.x + 447;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(heroes2.GetHeroes(), Portrait::BIG), dst_pt);

    // attack skill
    message.clear();
    message += "Attack Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetAttack());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetAttack());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    // defense skill
    message.clear();
    message += "Defense Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetDefense());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    // spell power
    message.clear();
    message += "Spell Power";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetPower());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    // knowledge
    message.clear();
    message += "Knowledge";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetKnowledge());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    // seconfary skill
    //"MINISS.ICN"



    // button exit
    dst_pt.x = cur_pt.x + 280;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, "SWAPBTN.ICN", 0, 1);

    buttonExit.Draw();

    Cursor::Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)) break;
    }
}
