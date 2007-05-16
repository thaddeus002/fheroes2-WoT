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
#include "army.h"
#include "castle.h"
#include "tools.h"
#include "monster.h"
#include "cursor.h"
#include "gamearea.h"
#include "background.h"
#include "error.h"
#include "dialog.h"

void Dialog::QuickInfo(const std::string & object)
{
    // preload
    const std::string &qwikinfo = "QWIKINFO.ICN";

    AGG::PreloadObject(qwikinfo);

    // cursor
    Cursor::Hide();

    // image box
    const Sprite &box = AGG::GetICN(qwikinfo, 0);

    const Rect ar(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);
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

void Dialog::QuickInfo(const Castle & castle)
{
    const std::string &qwiktown = "QWIKTOWN.ICN";
    AGG::PreloadObject(qwiktown);

    // cursor
    Cursor::Hide();

    // image box
    const Sprite &box = AGG::GetICN(qwiktown, 0);

    const Rect ar(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);
    const Point & mp = LocalEvent::MouseCursor();
    
    Rect cur_rt; 
    u16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    u16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
    my *= TILEWIDTH;

    // top left
    if(mx <= ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my + TILEWIDTH, box.w(), box.h());
    else
    // top right
    if(mx > ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx - box.w(), my + TILEWIDTH, box.w(), box.h());
    else
    // bottom left
    if(mx <= ar.x + ar.w / 2 && my > ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my - box.h(), box.w(), box.h());
    else
    // bottom right
	cur_rt = Rect(mx - box.w(), my - box.h(), box.w(), box.h());
    
    Background back(cur_rt);
    back.Save();

    display.Blit(box, cur_rt);

    cur_rt = Rect(back.GetRect().x + 28 , back.GetRect().y + 12, 178, 140);
    Point dst_pt;
    std::string message;

    // castle name
    message = castle.GetName();
    dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
    dst_pt.y = cur_rt.y + 5;
    Text(dst_pt.x, dst_pt.y, message, Font::SMALL, true);

    u8 index = 0;

    switch(castle.GetRace())
    {
	case Race::KNGT: index = (castle.isCastle() ?  9 : 15); break;
	case Race::BARB: index = (castle.isCastle() ? 10 : 16); break;
	case Race::SORC: index = (castle.isCastle() ? 11 : 17); break;
	case Race::WRLK: index = (castle.isCastle() ? 12 : 18); break;
	case Race::WZRD: index = (castle.isCastle() ? 13 : 19); break;
	case Race::NECR: index = (castle.isCastle() ? 14 : 20); break;
	default: Error::Warning(": unknown race."); return;
    }
    
    const Sprite & sprite = AGG::GetICN("LOCATORS.ICN", index);

    dst_pt.x = cur_rt.x + (cur_rt.w - sprite.w()) / 2;
    dst_pt.y += 18;
    display.Blit(sprite, dst_pt);

    message = std::string("Defenders:");
    dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
    dst_pt.y += sprite.h() + 5;
    Text(dst_pt.x, dst_pt.y, message, Font::SMALL, true);

    u8 count = castle.GetCountArmy();

    if(! count)
    {
	message = std::string("None");
	dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
	dst_pt.y += 45;
	Text(dst_pt.x, dst_pt.y, message, Font::SMALL, true);
    }
    else
    {
        // рисуем в одну строку
        u8 current = 0;

	for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii)
        {
	    const Army::Troops & army = castle.GetArmy(ii);

	    if(army.Valid())
	    {
		const Sprite & monster = AGG::GetICN("MONS32.ICN", army.GetMonster());

                // align from count
		dst_pt.x = (cur_rt.w / CASTLEMAXARMY - monster.w()) / 2 + cur_rt.x + current * cur_rt.w / count + ((cur_rt.w / CASTLEMAXARMY) * (CASTLEMAXARMY - count) / (2 * count));
		dst_pt.y = cur_rt.y + 85;
		// alignt from height sprite
		dst_pt.y += (monster.h() > 32 ? -(monster.h() - 32) : 32 - monster.h());
                display.Blit(monster, dst_pt);

		// count message
                message.clear();
		String::AddInt(message, army.GetCount());
		dst_pt.x += (monster.w() - Text::width(message, Font::SMALL)) / 2;
		dst_pt.y = cur_rt.y + 118;
		Text(dst_pt.x, dst_pt.y, message, Font::SMALL, true);

        	current++;
            }
	}
    }																																																																																			
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
void Dialog::QuickInfo(const Heroes & hero)
{
    const std::string &qwikhero = "QWIKHERO.ICN";
    AGG::PreloadObject(qwikhero);
}
*/
