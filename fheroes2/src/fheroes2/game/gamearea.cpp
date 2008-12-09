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
#include "settings.h"
#include "world.h"
#include "maps.h"
#include "game.h"
#include "game_focus.h"
#include "route.h"
#include "gamearea.h"

void	RedrawBoat(const Maps::Tiles & tile, const Point & dst);
void	RedrawHeroes(const Maps::Tiles & tile);
void	RedrawMonster(const Maps::Tiles & tile, const Point & dst);
void	RedrawClopOrClofSpriteFog(const u16 dst_index, const u8 ox, const u8 oy);

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
    	    // monster
    	    case MP2::OBJ_MONSTER:	RedrawMonster(tile, dst); break;
    	    default: break;
	}
    }

    // top
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox)
	    world.GetTiles(gx + ox, gy + oy).RedrawTop();

    // ext object
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox) if(GetRect() & Point(gx + ox, gy + oy))
    {
	const Maps::Tiles & tile = world.GetTiles(gx + ox, gy + oy);
    	const Point dst(BORDERWIDTH + TILEWIDTH * ox, BORDERWIDTH + TILEWIDTH * oy);

	switch(tile.GetObject())
	{
    	    // heroes
    	    case MP2::OBJ_HEROES:	RedrawHeroes(tile); break;
    	    default: break;
	}
    }

    // route
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::HEROES == focus.Type() &&
	focus.GetHeroes().GetPath().isShow())
    {
	const Heroes & hero = focus.GetHeroes();
	u16 from = Maps::GetIndexFromAbsPoint(hero.GetCenter());
	s16 green = hero.GetPath().GetAllowStep();

	const bool skipfirst = hero.isEnableMove() && 45 > hero.GetSpriteIndex() && 2 < (hero.GetSpriteIndex() % 9);

	Route::Path::const_iterator it1 = hero.GetPath().begin();
	Route::Path::const_iterator it2 = hero.GetPath().end();
	Route::Path::const_iterator it3 = it1;

	for(; it1 != it2; ++it1)
	{
	    from = Maps::GetDirectionIndex(from, (*it1).Direction());
    	    const u16 tile_x = from % world.w();
    	    const u16 tile_y = from / world.h();
	    ++it3;
	    --green;

            if(tile_x < gx + rx || tile_y < gy + ry || tile_x >= gx + rx + rw || tile_y >= gy + ry + rh) continue;
	    if(it1 == hero.GetPath().begin() && skipfirst) continue;

	    const u16 index = (it3 == it2 ? 0 : Route::Path::GetIndexSprite((*it1).Direction(), (*it3).Direction()));
	    const Sprite & sprite = AGG::GetICN(0 > green ? ICN::ROUTERED : ICN::ROUTE, index);
    	    Point dst_pt(BORDERWIDTH + TILEWIDTH * (tile_x - gx) + sprite.x() - 14, BORDERWIDTH + TILEWIDTH * (tile_y - gy) + sprite.y());
	    display.Blit(sprite, dst_pt);
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

    // redraw fog
    for(u8 oy = ry; oy < ry + rh; ++oy)
	for(u8 ox = rx; ox < rx + rw; ++ox)
    {
	const Maps::Tiles & tile = world.GetTiles(gx + ox, gy + oy);

	if(tile.isFog(Settings::Get().MyColor())) RedrawClopOrClofSpriteFog(tile.GetIndex(), ox, oy);
    }
}

/* scroll area */
void GameArea::Scroll(const u8 scroll)
{
    if(scroll & GameArea::LEFT && 0 < gx) --gx;
    else
    if(scroll & GameArea::RIGHT && world.w() - gw > gx) ++gx;

    if(scroll & GameArea::TOP && 0 < gy) --gy;
    else
    if(scroll & GameArea::BOTTOM && world.h() - gh > gy) ++gy;
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
}

void RedrawHeroes(const Maps::Tiles & tile)
{
    const Heroes *hero = world.GetHeroes(tile.GetIndex());

    if(hero)
    {
	hero->Redraw();
    }
}

void RedrawMonster(const Maps::Tiles & tile, const Point & dst)
{
    Display & display = Display::Get();
    Monster::monster_t monster = Monster::Monster(tile);
    Point dst_pt;
    Rect src_rt;
    u16 dst_index = MAXU16;

    // draw attack sprite
    if(Maps::ScanAroundObject(tile.GetIndex(), MP2::OBJ_HEROES, false, dst_index))
    {
	bool revert = false;

	switch(Direction::Get(tile.GetIndex(), dst_index))
	{
	    case Direction::TOP_LEFT:
	    case Direction::LEFT:
	    case Direction::BOTTOM_LEFT:	revert = true;
	    default: break;
	}

	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, monster * 9 + (revert ? 8 : 7));

	dst_pt.x = dst.x + sprite_first.x() + 16;
	dst_pt.y = dst.y + TILEWIDTH + sprite_first.y();

	GameArea::SrcRectFixed(src_rt, dst_pt, sprite_first.w(), sprite_first.h());
	display.Blit(sprite_first, src_rt, dst_pt);
    }
    else
    {
	// draw first sprite
	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, monster * 9);

	dst_pt.x = dst.x + sprite_first.x() + 16;
	dst_pt.y = dst.y + TILEWIDTH + sprite_first.y();

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
}

void RedrawClopOrClofSpriteFog(const u16 dst_index, const u8 ox, const u8 oy)
{
    const s16 dstx = BORDERWIDTH + TILEWIDTH * ox;
    const s16 dsty = BORDERWIDTH + TILEWIDTH * oy;
    Display & display = Display::Get();
    const u16 around = Maps::GetDirectionAroundFog(dst_index);

    // TIL::CLOF32
    if(DIRECTION_ALL == around)
	display.Blit(AGG::Cache::Get().GetTIL(TIL::CLOF32, dst_index % 4), dstx, dsty);
    else
    {
	u8 index = 0;
	bool revert = false;

	// see ICN::CLOP32: sprite 6, 7, 8
	if(around & (Direction::CENTER | Direction::TOP) && !(around & (Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)))
	{ index = 6; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::LEFT)))
	{ index = 7; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::LEFT) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::RIGHT)))
	{ index = 7; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::BOTTOM) && !(around & (Direction::TOP | Direction::LEFT | Direction::RIGHT)))
	{ index = 8; revert = false; }
	else
	// see ICN::CLOP32: sprite 9, 29
	if(around & (DIRECTION_CENTER_COL) && !(around & (Direction::LEFT | Direction::RIGHT)))
	{ index = 9; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW) && !(around & (Direction::TOP | Direction::BOTTOM)))
	{ index = 29; revert = false; }
	else
	// see ICN::CLOP32: sprite 10
	if((around & Direction::CENTER) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)))
	{ index = 10; revert = false; }
	else
	// see ICN::CLOP32: sprite 15, 22
	if(around == (DIRECTION_ALL & (~Direction::TOP_RIGHT)))
	{ index = 15; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~Direction::TOP_LEFT)))
	{ index = 15; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~Direction::BOTTOM_RIGHT)))
	{ index = 22; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~Direction::BOTTOM_LEFT)))
	{ index = 22; revert = true; }
	else
	// see ICN::CLOP32: sprite 16, 17, 18, 23
	if(around == (DIRECTION_ALL & (~(Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT))))
	{ index = 16; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::BOTTOM_LEFT))))
	{ index = 16; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_RIGHT | Direction::BOTTOM_LEFT))))
	{ index = 17; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::BOTTOM_RIGHT))))
	{ index = 17; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::TOP_RIGHT))))
	{ index = 18; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT))))
	{ index = 23; revert = false; }
	else
	// see ICN::CLOP32: sprite 13, 14
	if(around == (DIRECTION_ALL & (~DIRECTION_TOP_RIGHT_CORNER)))
	{ index = 13; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_TOP_LEFT_CORNER)))
	{ index = 13; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_BOTTOM_RIGHT_CORNER)))
	{ index = 14; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_BOTTOM_LEFT_CORNER)))
	{ index = 14; revert = true; }
	else
	// see ICN::CLOP32: sprite 11, 12
	if(around & (Direction::CENTER | Direction::LEFT | Direction::BOTTOM_LEFT | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::TOP_RIGHT | Direction::RIGHT)))
	{ index = 11; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::BOTTOM_RIGHT | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::TOP_LEFT | Direction::LEFT)))
	{ index = 11; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::LEFT | Direction::TOP_LEFT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::BOTTOM_RIGHT | Direction::RIGHT)))
	{ index = 12; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::TOP_RIGHT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::BOTTOM_LEFT | Direction::LEFT)))
	{ index = 12; revert = true; }
	else
	// see ICN::CLOP32: sprite 19, 20, 22
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::TOP_LEFT) &&
	 !(around & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT | Direction::TOP_RIGHT)))
	{ index = 19; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::TOP_RIGHT) &&
	 !(around & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT | Direction::TOP_LEFT)))
	{ index = 19; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::BOTTOM_LEFT) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT | Direction::TOP_LEFT)))
	{ index = 20; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::BOTTOM_RIGHT) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_LEFT | Direction::TOP_LEFT)))
	{ index = 20; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT | Direction::BOTTOM_LEFT | Direction::TOP_LEFT)))
	{ index = 22; revert = false; }
	else
	// see ICN::CLOP32: sprite 24, 25, 26, 30
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::BOTTOM_LEFT) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_RIGHT)))
	{ index = 24; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::BOTTOM_RIGHT) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_LEFT)))
	{ index = 24; revert = true; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::LEFT | Direction::TOP_LEFT) &&
	 !(around & (Direction::RIGHT | Direction::BOTTOM_LEFT)))
	{ index = 25; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::RIGHT | Direction::TOP_RIGHT) &&
	 !(around & (Direction::LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 25; revert = true; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::BOTTOM_LEFT | Direction::LEFT) &&
	 !(around & (Direction::RIGHT | Direction::TOP_LEFT)))
	{ index = 26; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::BOTTOM_RIGHT | Direction::RIGHT) &&
	 !(around & (Direction::LEFT | Direction::TOP_RIGHT)))
	{ index = 26; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP_LEFT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_RIGHT)))
	{ index = 30; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP_RIGHT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_LEFT)))
	{ index = 30; revert = true; }
	else
	// see ICN::CLOP32: sprite 27, 28
	if(around & (Direction::CENTER | Direction::BOTTOM | Direction::LEFT) &&
	 !(around & (Direction::TOP | Direction::TOP_RIGHT | Direction::RIGHT | Direction::BOTTOM_LEFT)))
	{ index = 27; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::BOTTOM | Direction::RIGHT) &&
	 !(around & (Direction::TOP | Direction::TOP_LEFT | Direction::LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 27; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::LEFT | Direction::TOP) &&
	 !(around & (Direction::TOP_LEFT | Direction::RIGHT | Direction::BOTTOM | Direction::BOTTOM_RIGHT)))
	{ index = 28; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::TOP) &&
	 !(around & (Direction::TOP_RIGHT | Direction::LEFT | Direction::BOTTOM | Direction::BOTTOM_LEFT)))
	{ index = 28; revert = true; }
	else
	// see ICN::CLOP32: sprite 31, 32, 33
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_LEFT | Direction::TOP_RIGHT)))
	{ index = 31; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::RIGHT) &&
	 !(around & (Direction::LEFT | Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT)))
	{ index = 32; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::LEFT) &&
	 !(around & (Direction::RIGHT | Direction::TOP_LEFT | Direction::BOTTOM_LEFT)))
	{ index = 32; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 33; revert = false; }
	else
	// see ICN::CLOP32: sprite 0, 1, 2, 3, 4, 5
	if(around & (DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW) &&
	 !(around & (Direction::TOP)))
	{ index = dst_index % 2 ? 0 : 1; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | DIRECTION_TOP_ROW) &&
	 !(around & (Direction::BOTTOM)))
	{ index = dst_index % 2 ? 4 : 5; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | DIRECTION_LEFT_COL) &&
	 !(around & (Direction::RIGHT)))
	{ index = dst_index % 2 ? 2 : 3; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | DIRECTION_RIGHT_COL) &&
	 !(around & (Direction::LEFT)))
	{ index = dst_index % 2 ? 2 : 3; revert = true; }
	// unknown
	else
	{
	    display.Blit(AGG::Cache::Get().GetTIL(TIL::CLOF32, dst_index % 4), dstx, dsty);
	    return;
	}

	const Sprite & src = AGG::GetICN(ICN::CLOP32, index, revert);
	Surface sf(src);
	sf.SetDisplayFormat();
	display.Blit(sf, revert ? dstx + src.x() + TILEWIDTH - src.w() : dstx + src.x(), dsty + src.y());
    }
}

Cursor::themes_t GameArea::ScrollToCursor(const u8 scroll)
{
    switch(scroll)
    {
	case TOP:		return Cursor::SCROLL_TOP;
	case BOTTOM:		return Cursor::SCROLL_BOTTOM;
	case RIGHT:		return Cursor::SCROLL_RIGHT;
	case LEFT:		return Cursor::SCROLL_LEFT;
	case LEFT | TOP:	return Cursor::SCROLL_TOPLEFT;
	case LEFT | BOTTOM:	return Cursor::SCROLL_BOTTOMLEFT;
	case RIGHT | TOP:	return Cursor::SCROLL_TOPRIGHT;
	case RIGHT | BOTTOM:	return Cursor::SCROLL_BOTTOMRIGHT;

	default: break;
    }

    return Cursor::NONE;
}
