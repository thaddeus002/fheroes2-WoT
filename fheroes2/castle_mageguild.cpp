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

void Castle::OpenMageGuild(void)
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
    message = "The above spells have been added to your book.";
    dst_pt.x = cur_pt.x + 280 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 461;
    Text(message, Font::BIG, dst_pt);

    // sprite
    switch(race)
    {
        case Race::KNGT: message = "MAGEGLDK.ICN"; break;
        case Race::BARB: message = "MAGEGLDB.ICN"; break;
        case Race::SORC: message = "MAGEGLDS.ICN"; break;
        case Race::WRLK: message = "MAGEGLDW.ICN"; break;
        case Race::WZRD: message = "MAGEGLDZ.ICN"; break;
        case Race::NECR: message = "MAGEGLDN.ICN"; break;
	default: break;
    }
    const Sprite & sprite = AGG::GetICN(message, 0); // FIXME: index sprite from level guild
    dst_pt.x = cur_pt.x + 110 - sprite.w() / 2;
    dst_pt.y = cur_pt.y + 290 - sprite.h();
    display.Blit(sprite, dst_pt);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, "WELLXTRA.ICN", 0, 1);

    Cursor::Show();
    display.Flip();

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
