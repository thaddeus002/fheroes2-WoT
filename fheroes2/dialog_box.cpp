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
#include "rect.h"
#include "localevent.h"
#include "cursor.h"
#include "config.h"
#include "dialog.h"

#define BOX_TOP         99 
#define BOX_MIDDLE      45 
#define BOX_BOTTOM      81 
#define BOXE_TOP        88 
#define BOXE_MIDDLE     45 
#define BOXE_BOTTOM     81 
#define BOXAREA_TOP     30 
#define BOXAREA_MIDDLE  45 
#define BOXAREA_BOTTOM  30 

Dialog::Box::Box(u16 height, bool buttons)
{
    const std::string &buybuild = (H2Config::EvilInterface() ? "BUYBUILE.ICN" : "BUYBUILD.ICN");

    AGG::PreloadObject(buybuild);

    if(buttons) height += BUTTON_HEIGHT;

    u8 count = (height < BOXAREA_TOP + BOXAREA_BOTTOM ? 0 : 1 + (height - BOXAREA_TOP - BOXAREA_BOTTOM) / BOXAREA_MIDDLE);

    u16 byte16 = (H2Config::EvilInterface() ? BOXE_TOP + BOXE_BOTTOM + count * BOXE_MIDDLE : BOX_TOP + BOX_BOTTOM + count * BOX_MIDDLE);
    Rect pos((display.w() - BOX_WIDTH) / 2, (display.h() - byte16) / 2, BOX_WIDTH, byte16);

    byte16 = (H2Config::EvilInterface() ? pos.y + BOXE_TOP - 30 : pos.y + BOX_TOP - 30);
    area = Rect(pos.x + 41, byte16, BOXAREA_WIDTH, BOXAREA_TOP + BOXAREA_BOTTOM + count * BOXAREA_MIDDLE);
    if(buttons) area.h -= BUTTON_HEIGHT;

    bool localcursor = false;
    if(pos & Cursor::GetRect() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }

    back.Save(pos);

    Point pt(pos.x, pos.y);

    // left top sprite
    if(!H2Config::EvilInterface()) ++pt.x;
    display.Blit(AGG::GetICN(buybuild, 4), pt);

    // left middle sprite
    pt.x = pos.x;
    pt.y += AGG::GetICN(buybuild, 4).h();
    for(int i = 0; i < count; ++i){
	display.Blit(AGG::GetICN(buybuild, 5), pt);
	pt.y += AGG::GetICN(buybuild, 5).h();
    }

    // left bottom sprite
    display.Blit(AGG::GetICN(buybuild, 6), pt);

    // right top sprite
    pt.x = pos.x + AGG::GetICN(buybuild, 4).w();
    pt.y = pos.y;
    display.Blit(AGG::GetICN(buybuild, 0), pt);

    // right middle sprite
    pt.y += AGG::GetICN(buybuild, 0).h();
    for(int i = 0; i < count; ++i){
	display.Blit(AGG::GetICN(buybuild, 1), pt);
	pt.y += AGG::GetICN(buybuild, 1).h();
    }
    
    // right bottom sprite
    display.Blit(AGG::GetICN(buybuild, 2), pt);

    if(localcursor) Cursor::Show();
}

Dialog::Box::~Box()
{
    bool localcursor = false;

    if(Rect(back.GetPos(), back.w(), back.h()) & Cursor::GetRect() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }
    back.Restore();

    if(localcursor) Cursor::Show();
    display.Flip();
};
