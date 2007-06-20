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

    const Point & cur_pt(background.GetArea());
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
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, "WELLXTRA.ICN", 0, 1);

    message = "Number of Towns:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 28;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Number of Castles:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 52;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Number of Heroes:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 76;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Gold in Treasury:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 100;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Wood & Ore:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 124;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Gems, Cr, Slf & Mer:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 148;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Obelisks Found:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 172;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Total Army Strength:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 196;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Income:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 220;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Best Hero:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 306;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Best Hero Stats:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 350;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Personality:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 394;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);

    message = "Best Monster:";
    dst_pt.x = cur_pt.x + 208 - Text::width(message, Font::BIG);
    dst_pt.y = cur_pt.y + 438;
    Text(dst_pt.x, dst_pt.y, message, Font::BIG, true);


    display.Flip();
    Cursor::Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    le.ResetKey();

    // message loop
    bool exit = false;

    while(!exit)
    {
        le.HandleEvents();

        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)){ exit = true; }
    }

    le.ResetKey();

    Cursor::Show();
}
