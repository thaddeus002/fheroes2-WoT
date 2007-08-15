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
#include "cursor.h"
#include "error.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "gamearea.h"

Rect GameArea::area_pos = Rect(0, 0, 0, 0);
u32 GameArea::animation_ticket = 0;

GameArea::GameArea()
{
    area_pos.x = 0;
    area_pos.y = 0;

    switch(H2Config::GetVideoMode()){
	default:
	    area_pos.w = 14;
	    area_pos.h = 14;
	    break;
	case Display::MEDIUM:
	    area_pos.w = 19;
	    area_pos.h = 17;
	    break;
	case Display::LARGE:
	    area_pos.w = 26;
	    area_pos.h = 23;
	    break;
	case Display::XLARGE:
	    area_pos.w = 34;
	    area_pos.h = 31;
	    break;
    }
}

/* readraw rect */
void GameArea::Redraw(const Rect & area_rt)
{
    if(area_rt.x < 0 || area_rt.y < 0 || area_rt.x >= area_pos.x + area_pos.w || area_rt.y >= area_pos.y + area_pos.h) Error::Warning("GameArea::Redraw: coord out of range");

    for(u16 iy = 0; iy < area_rt.h; ++iy)
	for(u16 ix = 0; ix < area_rt.w; ++ix)
	    world.GetTiles(area_rt.x + ix, area_rt.y + iy).Blit(BORDERWIDTH + ix * TILEWIDTH, BORDERWIDTH + iy * TILEWIDTH, animation_ticket);
}

/* redraw animation tiles */
void GameArea::RedrawAnimation(void)
{
    for(u16 iy = 0; iy < area_pos.h; ++iy)
	for(u16 ix = 0; ix < area_pos.w; ++ix)
    {
	const Maps::Tiles & tile = world.GetTiles(area_pos.x + ix, area_pos.y + iy);
	const u16 ax = BORDERWIDTH + ix * TILEWIDTH;
	const u16 ay = BORDERWIDTH + iy * TILEWIDTH;

	if(tile.isAnimation(ax, ay)) tile.Blit(ax, ay, animation_ticket);
    }

    ++animation_ticket;
}

/* scroll area */
void GameArea::Scroll(GameArea::scroll_t scroll)
{
    switch(scroll){
	case GameArea::LEFT:
	    if(0 == area_pos.x) return;
	    Cursor::Hide();
	    --area_pos.x;
	    Redraw();
	    Cursor::Show();
	    display.Flip();
	    break;
	case GameArea::RIGHT:
	    if(world.w() - area_pos.w == area_pos.x) return;
	    Cursor::Hide();
	    ++area_pos.x;
	    Redraw();
	    Cursor::Show();
	    display.Flip();
	    break;
	case GameArea::TOP:
	    if(0 == area_pos.y) return;
	    Cursor::Hide();
	    --area_pos.y;
	    Redraw();
	    Cursor::Show();
	    display.Flip();
	    break;
	case GameArea::BOTTOM:
	    if(world.h() - area_pos.h == area_pos.y) return;
	    Cursor::Hide();
	    ++area_pos.y;
	    Redraw();
	    Cursor::Show();
	    display.Flip();
	    break;
    }
}

/* scroll area from radar area_pos */
void GameArea::CenterFromRadar(const Point &pt)
{
    // left top point
    Point pos((pt.x - (display.w() - BORDERWIDTH - RADARWIDTH)) * world.w() / RADARWIDTH, 
	      (pt.y - BORDERWIDTH) * world.h() / RADARWIDTH);

    Center(pos);
}

/* scroll area to center point maps */
void GameArea::Center(const Point &pt)
{
    Point pos;

    // center
    pos.x = (0 > pt.x - area_pos.w / 2 ? 0 : pt.x - area_pos.w / 2);
    pos.y = (0 > pt.y - area_pos.h / 2 ? 0 : pt.y - area_pos.h / 2);

    // our of range
    if(pos.y > world.h() - area_pos.h) pos.y = world.h() - area_pos.h;
    if(pos.x > world.w() - area_pos.w) pos.x = world.w() - area_pos.w;

    if(pos.x == area_pos.x && pos.y == area_pos.y) return;

    area_pos.x = pos.x;
    area_pos.y = pos.y;

    Redraw();
}

