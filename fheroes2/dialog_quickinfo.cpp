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
#include "localevent.h"
#include "text.h"
#include "maps.h"
#include "cursor.h"
#include "gamearea.h"
#include "background.h"
#include "dialog.h"

#include "error.h"
void Dialog::QuickInfo(const std::string & object)
{
    // preload
    const std::string &qwikinfo = "QWIKINFO.ICN";

    AGG::PreloadObject(qwikinfo);

    // cursor
    Cursor::Hide();

    // image box
    const Sprite &box = AGG::GetICN(qwikinfo, 0);

    const Rect & ar = GameArea::GetPosition();
    const Point & mp = LocalEvent::MouseCursor();
    
    Rect pos; 
    u16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    u16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
    my *= TILEWIDTH;

    // top left
    if(mx <= ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	pos = Rect(mx + TILEWIDTH, my + TILEWIDTH, box.w(), box.h());
    else
    // top right
    if(mx > ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	pos = Rect(mx - box.w(), my + TILEWIDTH, box.w(), box.h());
    else
    // bottom left
    if(mx <= ar.x + ar.w / 2 && my > ar.y + ar.h / 2)
	pos = Rect(mx + TILEWIDTH, my - box.h(), box.w(), box.h());
    else
    // bottom right
	pos = Rect(mx - box.w(), my - box.h(), box.w(), box.h());
    
    Background back(pos);
    back.Save();

    display.Blit(box, pos);

    u16 tx = pos.x + BORDERWIDTH + (pos.w - BORDERWIDTH - Text::width(object, Font::SMALL)) / 2;
    u16 ty = pos.y + (pos.h - BORDERWIDTH - Text::height(pos.h, object, Font::SMALL)) / 2;
    Text(tx, ty, object, Font::SMALL, true);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    display.Flip();

    // quick info loop
    while(le.MouseRight()){ le.HandleEvents(); }

    // restore background
    back.Restore();
    Cursor::Show();
    display.Flip();
}

/*
void Dialog::QuickInfo(const Castle & castle)
{
    const std::string &qwiktown = "QWIKTOWN.ICN";
    AGG::PreloadObject(qwiktown);
}

void Dialog::QuickInfo(const Heroes & hero)
{
    const std::string &qwikhero = "QWIKHERO.ICN";
    AGG::PreloadObject(qwikhero);
}
*/
