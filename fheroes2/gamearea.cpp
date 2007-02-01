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

#include "config.h"
#include "gamearea.h"

GameArea::GameArea(const MapsData & data) : maps(data)
{
    // static values
    // width area  = (display.w() - RADARWIDTH - 3 * BORDERWIDTH) / TILEWIDTH
    // height area = (display.h() - 2 * BORDERWIDTH) / TILEWIDTH

    x = 0;
    y = 0;

    switch(H2Config::GetVideoMode()){
	default:
	    w = 14;
	    h = 14;
	    break;
	case Display::MEDIUM:
	    w = 19;
	    h = 17;
	    break;
	case Display::LARGE:
	    w = 26;
	    h = 23;
	    break;
	case Display::XLARGE:
	    w = 34;
	    h = 31;
	    break;
    }
}

/* readraw all */
void GameArea::Redraw(void)
{
    Rect srcrt(x * TILEWIDTH, y * TILEWIDTH, w * TILEWIDTH, h * TILEWIDTH);
    Point dstpt(BORDERWIDTH, BORDERWIDTH);
    maps.Redraw(srcrt, dstpt);
}

/* readraw rect */
void GameArea::Redraw(const Rect &area_rt)
{
    Rect srcrt(area_rt.x * TILEWIDTH, area_rt.y * TILEWIDTH, area_rt.w * TILEWIDTH, area_rt.h * TILEWIDTH);
    Point dstpt(BORDERWIDTH + area_rt.x * TILEWIDTH, BORDERWIDTH + area_rt.y * TILEWIDTH);
    maps.Redraw(srcrt, dstpt);
}

/* scroll area */
void GameArea::Scroll(GameArea::scroll_t scroll)
{
    switch(scroll){
	case GameArea::LEFT:
	    if(0 == x) return;
	    Cursor::Hide();
	    --x;
	    Redraw();
	    display.Flip();
	    Cursor::Show();
	    break;
	case GameArea::RIGHT:
	    if(maps.GetWidth() - w == x) return;
	    Cursor::Hide();
	    ++x;
	    Redraw();
	    display.Flip();
	    Cursor::Show();
	    break;
	case GameArea::TOP:
	    if(0 == y) return;
	    Cursor::Hide();
	    --y;
	    Redraw();
	    display.Flip();
	    Cursor::Show();
	    break;
	case GameArea::BOTTOM:
	    if(maps.GetHeight() - h == y) return;
	    Cursor::Hide();
	    ++y;
	    Redraw();
	    display.Flip();
	    Cursor::Show();
	    break;
    }
}
