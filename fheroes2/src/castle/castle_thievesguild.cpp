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
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "castle.h"
#include "background.h"
#include "tools.h"
#include "text.h"

void Castle::OpenThievesGuild(void)
{
    // cursor
    Cursor::Hide();

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN("STONEBAK.ICN", 0), dst_pt);

    std::string message;

    // bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    display.Blit(AGG::GetICN("WELLXTRA.ICN", 2), dst_pt);

    // text bar
    message = "Thieves' Guild: Player RanKings";
    dst_pt.x = cur_pt.x + 280 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 461;
    Text(message, Font::BIG, dst_pt);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, "WELLXTRA.ICN", 0, 1);

    message = "Number of Towns:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 28;
    Text(message, Font::BIG, dst_pt);

    message = "Number of Castles:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 52;
    Text(message, Font::BIG, dst_pt);

    message = "Number of Heroes:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 76;
    Text(message, Font::BIG, dst_pt);

    message = "Gold in Treasury:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 100;
    Text(message, Font::BIG, dst_pt);

    message = "Wood & Ore:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 124;
    Text(message, Font::BIG, dst_pt);

    message = "Gems, Cr, Slf & Mer:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 148;
    Text(message, Font::BIG, dst_pt);

    message = "Obelisks Found:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 172;
    Text(message, Font::BIG, dst_pt);

    message = "Total Army Strength:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 196;
    Text(message, Font::BIG, dst_pt);

    message = "Income:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 220;
    Text(message, Font::BIG, dst_pt);

    message = "Best Hero:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 306;
    Text(message, Font::BIG, dst_pt);

    message = "Best Hero Stats:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 350;
    Text(message, Font::BIG, dst_pt);

    message = "Personality:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 394;
    Text(message, Font::BIG, dst_pt);

    message = "Best Monster:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 438;
    Text(message, Font::BIG, dst_pt);


    Cursor::Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)) break;
    }
}
