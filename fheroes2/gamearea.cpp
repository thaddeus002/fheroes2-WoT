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
#include "game.h"
#include "cursor.h"
#include "error.h"
#include "gamearea.h"

Rect GameArea::pos = Rect(0, 0, 0, 0);

GameArea::GameArea(const MapsData & data) : maps(data)
{
    // static values
    // width area  = (display.w() - RADARWIDTH - 3 * BORDERWIDTH) / TILEWIDTH
    // height area = (display.h() - 2 * BORDERWIDTH) / TILEWIDTH

    pos.x = 0;
    pos.y = 0;

    switch(H2Config::GetVideoMode()){
	default:
	    pos.w = 14;
	    pos.h = 14;
	    break;
	case Display::MEDIUM:
	    pos.w = 19;
	    pos.h = 17;
	    break;
	case Display::LARGE:
	    pos.w = 26;
	    pos.h = 23;
	    break;
	case Display::XLARGE:
	    pos.w = 34;
	    pos.h = 31;
	    break;
    }
}

/* readraw all */
void GameArea::Redraw(void)
{
    maps.Redraw(pos);
}

/* readraw rect */
void GameArea::Redraw(const Rect &area_rt)
{
    maps.Redraw(area_rt, Point(area_rt.x, area_rt.y));
}

/* scroll area */
void GameArea::Scroll(GameArea::scroll_t scroll)
{
    switch(scroll){
	case GameArea::LEFT:
	    if(0 == pos.x) return;
	    Cursor::Hide();
	    --pos.x;
	    Redraw();
	    Cursor::Show();
	    break;
	case GameArea::RIGHT:
	    if(maps.w() - pos.w == pos.x) return;
	    Cursor::Hide();
	    ++pos.x;
	    Redraw();
	    Cursor::Show();
	    break;
	case GameArea::TOP:
	    if(0 == pos.y) return;
	    Cursor::Hide();
	    --pos.y;
	    Redraw();
	    Cursor::Show();
	    break;
	case GameArea::BOTTOM:
	    if(maps.h() - pos.h == pos.y) return;
	    Cursor::Hide();
	    ++pos.y;
	    Redraw();
	    Cursor::Show();
	    break;
    }
}

/* scroll area from radar pos */
void GameArea::CenterFromRadar(const Point &pt)
{
    // left top point
    pos.x = (pt.x - (display.w() - BORDERWIDTH - RADARWIDTH)) * maps.w() / RADARWIDTH;
    pos.y = (pt.y - BORDERWIDTH) * maps.h() / RADARWIDTH;

    Center(Point(pos.x, pos.y));
}

/* scroll area to center point maps */
void GameArea::Center(const Point &pt)
{
    // center
    pos.x = (0 > pos.x - pos.w / 2 ? 0 : pos.x - pos.w / 2);
    pos.y = (0 > pos.y - pos.h / 2 ? 0 : pos.y - pos.h / 2);

    // our of range
    if(pos.y > maps.h() - pos.h) pos.y = maps.h() - pos.h;
    if(pos.x > maps.w() - pos.w)  pos.x = maps.w() - pos.w;

    if(pt.x == pos.x && pt.y == pos.y) return;

    Redraw();
}

/* get index maps from point mouse click on game area */
u16 GameArea::GetIndexMaps(u16 mx, u16 my) const
{
    if(! (GetPosition() & Point(mx, my))){
	Error::Warning("GameArea::GetIndexMaps: click, out of range.");
	return 0;
    }

    u16 result = (pos.y + (my - BORDERWIDTH) / TILEWIDTH) * maps.w() + pos.x + (mx - BORDERWIDTH) / TILEWIDTH;

    if(result >= maps.w() * maps.h()){
	Error::Warning("GameArea::GetIndexMaps: position, out of range.");
	return 0;
    }

    return result;
}
