/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "text.h"
#include "dialog.h"

void Dialog::ThievesGuild(u8 count)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN(ICN::STONEBAK, 0), dst_pt);

    Text text;

    // bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    display.Blit(AGG::GetICN(ICN::WELLXTRA, 2), dst_pt);

    // text bar
    text.Set("Thieves' Guild: Player RanKings", Font::BIG);
    dst_pt.x = cur_pt.x + 280 - text.w() / 2;
    dst_pt.y = cur_pt.y + 461;
    text.Blit(dst_pt);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    text.Set("Number of Towns:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 28;
    text.Blit(dst_pt);

    text.Set("Number of Castles:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 52;
    text.Blit(dst_pt);

    text.Set("Number of Heroes:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 76;
    text.Blit(dst_pt);

    text.Set("Gold in Treasury:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 100;
    text.Blit(dst_pt);

    text.Set("Wood & Ore:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 124;
    text.Blit(dst_pt);

    text.Set("Gems, Cr, Slf & Mer:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 148;
    text.Blit(dst_pt);

    text.Set("Obelisks Found:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 172;
    text.Blit(dst_pt);

    text.Set("Total Army Strength:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 196;
    text.Blit(dst_pt);

    text.Set("Income:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 220;
    text.Blit(dst_pt);

    text.Set("Best Hero:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 306;
    text.Blit(dst_pt);

    text.Set("Best Hero Stats:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 350;
    text.Blit(dst_pt);

    text.Set("Personality:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 394;
    text.Blit(dst_pt);

    text.Set("Best Monster:");
    dst_pt.x = cur_pt.x + 208 - text.w();
    dst_pt.y = cur_pt.y + 438;
    text.Blit(dst_pt);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
    }
}
