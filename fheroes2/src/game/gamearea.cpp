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

#include "cursor.h"
#include "error.h"
#include "engine.h"
#include "sprite.h"
#include "agg.h"
#include "config.h"
#include "world.h"
#include "maps.h"
#include "game.h"
#include "game_focus.h"
#include "route.h"
#include "gamearea.h"

void	RedrawBoat(const Maps::Tiles & tile, const Point & dst);
void	RedrawHeroes(const Maps::Tiles & tile, const Point & dst);
void	RedrawMonster(const Maps::Tiles & tile, const Point & dst);

GameArea & GameArea::Get(void)
{
    static GameArea ga;

    return ga;
}

/* fixed src rect image */
void GameArea::SrcRectFixed(Rect & src, Point & dst, const u16 rw, const u16 rh)
{
    src = Rect(0, 0, rw, rh);

    if(dst.x < BORDERWIDTH)
    {
        src.x = BORDERWIDTH - dst.x;
        dst.x = BORDERWIDTH;
    }

    if(dst.y < BORDERWIDTH)
    {
        src.y = BORDERWIDTH - dst.y;
        dst.y = BORDERWIDTH;
    }

    if(dst.x + rw > BORDERWIDTH + TILEWIDTH * w())
    {
	src.w = BORDERWIDTH + TILEWIDTH * w() - dst.x;
    }

    if(dst.y + rh > BORDERWIDTH + TILEWIDTH * h())
    {
	src.h = BORDERWIDTH + TILEWIDTH * h() - dst.y;
    }
}

GameArea::GameArea() : Rect(0, 0, 0, 0), gx(Rect::x), gy(Rect::y), gw(Rect::w), gh(Rect::h)
{
}

void GameArea::Build(void)
{
    gx = 0;
    gy = 0;

    gw = (Display::Get().w() - RADARWIDTH - 3 * BORDERWIDTH) / TILEWIDTH;
    gh = (Display::Get().h() - 2 * BORDERWIDTH) / TILEWIDTH;
}

u16 GameArea::GetLeftTopIndexMaps(void)
{
    return Maps::GetIndexFromAbsPoint(x(), y());
}

/* readraw rect, default all */
void GameArea::Redraw(const Rect & rt) const
{
    Redraw(rt.x, rt.y, rt.w, rt.h);
}

void GameArea::Redraw(const s16 rx, const s16 ry, const u16 rw, const u16 rh) const
{
    Display & display = Display::Get();

    // tile
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox)
	    world.GetTiles(gx + ox, gy + oy).RedrawTile();

    // bottom
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox)
	    world.GetTiles(gx + ox, gy + oy).RedrawBottom();

    // top
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox)
	    world.GetTiles(gx + ox, gy + oy).RedrawTop();

    // route
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::HEROES == focus.Type() &&
	focus.GetHeroes().GetColor() == H2Config::MyColor() &&
	focus.GetHeroes().GetPath().isShow())
    {
	u16 from = Maps::GetIndexFromAbsPoint(focus.GetHeroes().GetCenter());

	std::list<Route::Step>::const_iterator it1 = focus.GetHeroes().GetPath().begin();
	std::list<Route::Step>::const_iterator it2 = focus.GetHeroes().GetPath().end();
	std::list<Route::Step>::const_iterator it3 = it1;

	for(; it1 != it2; ++it1)
	{
    	    const u16 tile_x = (*it1).to_index % world.w();
    	    const u16 tile_y = (*it1).to_index / world.h();

    	    if(tile_x < gx + rx || tile_y < gy + ry || tile_x > gx + rx + rw || tile_y > gy + ry + rh) continue;

	    u16 index = 0;

	    if(++it3 != it2) index = Route::Path::GetIndexSprite(Direction::Get(from, (*it1).to_index), Direction::Get((*it1).to_index, (*it3).to_index));

	    const Sprite & sprite = AGG::GetICN(!(*it1).green_color ? ICN::ROUTERED : ICN::ROUTE, index);
    	    Point dst_pt(BORDERWIDTH + TILEWIDTH * (tile_x - gx) + sprite.x() - 16, BORDERWIDTH + TILEWIDTH * (tile_y - gy) + sprite.y());
    	    Rect src_rt;
    	    GameArea::SrcRectFixed(src_rt, dst_pt, sprite.w(), sprite.h());
	    display.Blit(sprite, src_rt, dst_pt);

    	    from = (*it1).to_index;
	}
    }

    // ext object
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox) if(GetRect() & Point(gx + ox, gy + oy))
    {
	const Maps::Tiles & tile = world.GetTiles(gx + ox, gy + oy);
    	const Point dst(BORDERWIDTH + TILEWIDTH * ox, BORDERWIDTH + TILEWIDTH * oy);

	switch(tile.GetObject())
	{
    	    // boat
    	    case MP2::OBJ_BOAT:		RedrawBoat(tile, dst); break;
    	    // heroes
    	    case MP2::OBJ_HEROES:	RedrawHeroes(tile, dst); break;
    	    // monster
    	    case MP2::OBJ_MONSTER:	RedrawMonster(tile, dst); break;

    	    default: break;
	}
    }

    // redraw grid
    if(H2Config::Debug())
    {

	for(u8 oy = ry; oy < ry + rh; ++oy)
	    for(u8 ox = rx; ox < rx + rw; ++ox) if(GetRect() & Point(gx + ox, gy + oy))
	{
    	    const s16 dstx = BORDERWIDTH + TILEWIDTH * ox;
    	    const s16 dsty = BORDERWIDTH + TILEWIDTH * oy;

    	    display.Lock();
    	    display.SetPixel(dstx, dsty, AGG::GetColor(0x40));
    	    display.Unlock();
	}
    }
    else
    // redraw fog
    {
	for(u8 oy = ry; oy < ry + rh; ++oy)
	    for(u8 ox = rx; ox < rx + rw; ++ox)
	{
		const Maps::Tiles & tile = world.GetTiles(gx + ox, gy + oy);

		if(tile.isFog(Settings::Get().MyColor()))
		{
    		    const s16 dstx = BORDERWIDTH + TILEWIDTH * ox;
    		    const s16 dsty = BORDERWIDTH + TILEWIDTH * oy;

		    // TIL::CLOF32
		    display.Blit(AGG::Cache::Get().GetTIL(TIL::CLOF32, tile.GetIndex() % 4), dstx, dsty);

		    // or ICN::CLOP32
		}
	}
    }
}

/* scroll area */
void GameArea::Scroll(GameArea::scroll_t scroll)
{
    switch(scroll)
    {
	case GameArea::LEFT:
	{
	    if(0 == gx) return;

	    --gx;

	    Redraw();
	}
	    break;
	case GameArea::RIGHT:
	{
	    if(world.w() - gw == gx) return;

	    ++gx;

	    Redraw();
	}
	    break;
	case GameArea::TOP:
	{
	    if(0 == gy) return;

	    --gy;

	    Redraw();
	}
	    break;
	case GameArea::BOTTOM:
	{
	    if(world.h() - gh == gy) return;

	    ++gy;

	    Redraw();
	}
	    break;
    }
}

/* scroll area from radar area_pos */
void GameArea::CenterFromRadar(const Point &pt)
{
    // left top point
    Point pos((pt.x - (Display::Get().w() - BORDERWIDTH - RADARWIDTH)) * world.w() / RADARWIDTH, 
	      (pt.y - BORDERWIDTH) * world.h() / RADARWIDTH);

    Center(pos);
}

/* scroll area to center point maps */
void GameArea::Center(const Point &pt)
{
    Point pos;

    // center
    pos.x = (0 > pt.x - gw / 2 ? 0 : pt.x - gw / 2);
    pos.y = (0 > pt.y - gh / 2 ? 0 : pt.y - gh / 2);

    // our of range
    if(pos.y > world.h() - gh) pos.y = world.h() - gh;
    if(pos.x > world.w() - gw) pos.x = world.w() - gw;

    if(pos.x == gx && pos.y == gy) return;

    // possible fast scroll
    if(pos.y == gy && 1 == (pos.x - gx)) Scroll(GameArea::RIGHT);
    else
    if(pos.y == gy && -1 == (pos.x - gx)) Scroll(GameArea::LEFT);
    else
    if(pos.x == gx && 1 == (pos.y - gy)) Scroll(GameArea::BOTTOM);
    else
    if(pos.x == gx && -1 == (pos.y - gy)) Scroll(GameArea::TOP);
    else
    // diagonal
    if(-1 == (pos.y - gy) && 1 == (pos.x - gx))
    {
	Scroll(GameArea::TOP);
	Scroll(GameArea::RIGHT);
    }
    else
    if(-1 == (pos.y - gy) && -1 == (pos.x - gx))
    {
	Scroll(GameArea::TOP);
	Scroll(GameArea::LEFT);
    }
    else
    if(1 == (pos.y - gy) && 1 == (pos.x - gx))
    {
	Scroll(GameArea::BOTTOM);
	Scroll(GameArea::RIGHT);
    }
    else
    if(1 == (pos.y - gy) && -1 == (pos.x - gx))
    {
	Scroll(GameArea::BOTTOM);
	Scroll(GameArea::LEFT);
    }

    else
    {
	gx = pos.x;
	gy = pos.y;

	Redraw();
    }
}

void RedrawBoat(const Maps::Tiles & tile, const Point & dst)
{
    if(Settings::Get().Editor())
        Display::Get().Blit(AGG::GetICN(ICN::OBJNWAT2, 23), dst);
    else
    {
        const Sprite & sprite = AGG::GetICN(ICN::BOAT32, 18);
        Point dst_pt(dst.x + sprite.x(), dst.y + sprite.y() + TILEWIDTH);
        Rect src_rt;
        GameArea::SrcRectFixed(src_rt, dst_pt, sprite.w(), sprite.h());
        Display::Get().Blit(sprite, src_rt, dst_pt);
    }

    tile.RedrawTop();

    if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT))
        world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).RedrawTop();

    if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT))
        world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).RedrawTop();

    if(Maps::isValidDirection(tile.GetIndex(), Direction::BOTTOM))
    {
        world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::BOTTOM)).RedrawBottom();
        world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::BOTTOM)).RedrawTop();
    }
}

void RedrawHeroes(const Maps::Tiles & tile, const Point & dst)
{
    const Heroes *hero = world.GetHeroes(tile.GetIndex());

    if(hero)
    {
	hero->Redraw();

	tile.RedrawTop();

	//if(Maps::isValidDirection(tile.GetIndex(), hero->GetDirection()))
    	//    world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), hero->GetDirection())).RedrawTop();

	//if(Maps::isValidDirection(tile.GetIndex(), hero->GetDirection()))
    	//    world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), hero->GetDirection())).RedrawBottom();

	if(Maps::isValidDirection(tile.GetIndex(), Direction::BOTTOM))
    	    world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::BOTTOM)).RedrawTop();
    }
}

void RedrawMonster(const Maps::Tiles & tile, const Point & dst)
{
    Display & display = Display::Get();

    Monster::monster_t monster = Monster::Monster(tile);

    // draw first sprite
    const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, monster * 9);

    Point dst_pt(dst.x + sprite_first.x() + 16, dst.y + TILEWIDTH + sprite_first.y());
    Rect src_rt;

    GameArea::SrcRectFixed(src_rt, dst_pt, sprite_first.w(), sprite_first.h());

    display.Blit(sprite_first, src_rt, dst_pt);

    // draw second sprite
    const Sprite & sprite_next = AGG::GetICN(ICN::MINIMON, monster * 9 + 1 + (Maps::AnimationTicket() % 6));

    dst_pt.x = dst.x + sprite_next.x() + 16;
    dst_pt.y = dst.y + TILEWIDTH + sprite_next.y();

    GameArea::SrcRectFixed(src_rt, dst_pt, sprite_next.w(), sprite_next.h());

    display.Blit(sprite_next, src_rt, dst_pt);

    if(Maps::isValidDirection(tile.GetIndex(), Direction::BOTTOM))
    	world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::BOTTOM)).RedrawTop();
}
