/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#ifdef WITH_EDITOR

#include "gamedefs.h"
#include "agg.h"
#include "settings.h"
#include "dialog.h"
#include "world.h"
#include "cursor.h"
#include "direction.h"
#include "maps_tiles.h"
#include "ground.h"
#include "editor_interface.h"
#include "game.h"

namespace Game
{
    namespace Editor
    {
	void EventRadarPoint(EditorInterface &, const Point &);
	void EventLeftPanelPoint(EditorInterface &, const Point &);
	void EventGameAreaPoint(EditorInterface &, const Point &);
	void EventGroundMode(EditorInterface &, const Point &, u16, u16);
	void EventObjectMode(EditorInterface &, const Point &);

	void ModifyGround(const Point &, u8, u8, u16);
	u16 GetStartGroundTile(u16 ground);
	u16 GetStartGround2Tile(u16 ground);
	u16 GetStartFilledTile(u16 ground);
	u16 GetStartFilled2Tile(u16 ground);
	bool GetIndexShapeForTile(const s32 &, u16 ground, std::pair<u16, u8> &);
    }
}

struct AroundGrounds
{
    u16 v[9]; /* ground: top left, top, top right, right, bottom right, bottom, bottom left, left, center */

    AroundGrounds(const s32 & center)
    {
	std::fill(v, ARRAY_COUNT_END(v), Maps::Ground::UNKNOWN);

	if(Maps::isValidAbsIndex(center))
	{
	    v[0] = world.GetTiles(Maps::isValidDirection(center, Direction::TOP_LEFT) ? Maps::GetDirectionIndex(center, Direction::TOP_LEFT) : center).GetGround();
	    v[1] = world.GetTiles(Maps::isValidDirection(center, Direction::TOP) ? Maps::GetDirectionIndex(center, Direction::TOP) : center).GetGround();
	    v[2] = world.GetTiles(Maps::isValidDirection(center, Direction::TOP_RIGHT) ? Maps::GetDirectionIndex(center, Direction::TOP_RIGHT) : center).GetGround();
	    v[3] = world.GetTiles(Maps::isValidDirection(center, Direction::RIGHT) ? Maps::GetDirectionIndex(center, Direction::RIGHT) : center).GetGround();
	    v[4] = world.GetTiles(Maps::isValidDirection(center, Direction::BOTTOM_RIGHT) ? Maps::GetDirectionIndex(center, Direction::BOTTOM_RIGHT) : center).GetGround();
	    v[5] = world.GetTiles(Maps::isValidDirection(center, Direction::BOTTOM) ? Maps::GetDirectionIndex(center, Direction::BOTTOM) : center).GetGround();
	    v[6] = world.GetTiles(Maps::isValidDirection(center, Direction::BOTTOM_LEFT) ? Maps::GetDirectionIndex(center, Direction::BOTTOM_LEFT) : center).GetGround();
	    v[7] = world.GetTiles(Maps::isValidDirection(center, Direction::LEFT) ? Maps::GetDirectionIndex(center, Direction::LEFT) : center).GetGround();
	    v[8] = world.GetTiles(center).GetGround();
	}
    }

    u16 operator() (void) const
    {
	u16 res = 0;

	for(u8 ii = 0; ii < ARRAY_COUNT(v); ++ii)
	    res |= v[ii];

	return res;
    }

    u16 GetGrounds(u16 directs) const
    {
	u16 res = Maps::Ground::UNKNOWN;

	if(Direction::TOP_LEFT & directs)	res |= v[0];
	if(Direction::TOP & directs)		res |= v[1];
	if(Direction::TOP_RIGHT & directs)	res |= v[2];
	if(Direction::RIGHT & directs)		res |= v[3];
	if(Direction::BOTTOM_RIGHT & directs)	res |= v[4];
	if(Direction::BOTTOM & directs)		res |= v[5];
	if(Direction::BOTTOM_LEFT & directs)	res |= v[6];
	if(Direction::LEFT & directs)		res |= v[7];
	if(Direction::CENTER & directs)		res |= v[8];

	return res;
    }

    u16 GetAroundID(u16 ground) const
    {
	u16 res = 0;

	if(v[0] & ground) res |= Direction::TOP_LEFT;
	if(v[1] & ground) res |= Direction::TOP;
	if(v[2] & ground) res |= Direction::TOP_RIGHT;
	if(v[3] & ground) res |= Direction::RIGHT;
	if(v[4] & ground) res |= Direction::BOTTOM_RIGHT;
	if(v[5] & ground) res |= Direction::BOTTOM;
	if(v[6] & ground) res |= Direction::BOTTOM_LEFT;
	if(v[7] & ground) res |= Direction::LEFT;

	return res;
    }
};

u16 Game::Editor::GetStartGroundTile(u16 ground)
{
    u16 res = 0;

    switch(ground)
    {
	case Maps::Ground::DESERT:	res = 262; break;
	case Maps::Ground::SNOW:	res = 92;  break;
	case Maps::Ground::SWAMP:	res = 146; break;
	case Maps::Ground::WASTELAND:	res = 361; break;
	case Maps::Ground::BEACH:	res = 415; break;
	case Maps::Ground::LAVA:	res = 208; break;
	case Maps::Ground::DIRT:	res = 321; break;
	case Maps::Ground::GRASS:	res = 30;  break;
	case Maps::Ground::WATER:	res = 0;   break;
	default: break;
    }

    return res;
}

u16 Game::Editor::GetStartGround2Tile(u16 ground)
{
    return GetStartGroundTile(ground) + Rand::Get(3);
}

u16 Game::Editor::GetStartFilledTile(u16 ground)
{
    // 30%
    if(0 == Rand::Get(6))
	return GetStartFilled2Tile(ground);

    u16 res = 0;

    switch(ground)
    {
	case Maps::Ground::DESERT:	res = 300; break;
	case Maps::Ground::SNOW:	res = 130; break;
	case Maps::Ground::SWAMP:	res = 184; break;
	case Maps::Ground::WASTELAND:	res = 399; break;
	case Maps::Ground::BEACH:	res = 415; break;
	case Maps::Ground::LAVA:	res = 246; break;
	case Maps::Ground::DIRT:	res = 337; break;
	case Maps::Ground::GRASS:	res = 68;  break;
	case Maps::Ground::WATER:	res = 16;  break;
	default: break;
    }

    res += Rand::Get(7);

    return res;
}

u16 Game::Editor::GetStartFilled2Tile(u16 ground)
{
    u16 res = 0;
    u8 count = 8;

    switch(ground)
    {
	case Maps::Ground::DESERT:	res = 308; count = 13; break;
	case Maps::Ground::SNOW:	res = 138; break;
	case Maps::Ground::SWAMP:	res = 192; count = 16; break;
	case Maps::Ground::WASTELAND:	res = 407; break;
	case Maps::Ground::BEACH:	res = 423; count = 9; break;
	case Maps::Ground::LAVA:	res = 254; break;
	case Maps::Ground::DIRT:	res = 345; count = 16; break;
	case Maps::Ground::GRASS:	res = 76; count = 16; break;
	case Maps::Ground::WATER:	res = 24; count = 6; break;
	default: break;
    }

    res += Rand::Get(count - 1);

    return res;
}

void Game::Editor::EventRadarPoint(EditorInterface & I, const Point & pt)
{
    const Point prev(I.gameArea.GetRectMaps());

    I.gameArea.SetCenter((pt.x - I.radar.GetArea().x) * world.w() / RADARWIDTH, (pt.y - I.radar.GetArea().y) * world.h() / RADARWIDTH);

    if(prev != I.gameArea.GetRectMaps())
    {
	Cursor & cursor = Cursor::Get();

	cursor.Hide();
	cursor.SetThemes(cursor.POINTER);
	I.sizeCursor.Hide();
	I.split_h.Move(I.gameArea.GetRectMaps().x);
	I.split_v.Move(I.gameArea.GetRectMaps().y);
	I.needRedraw = true;
    }
}

void Game::Editor::EventLeftPanelPoint(EditorInterface & I, const Point & pt)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);
    I.sizeCursor.Hide();
    cursor.Show();
    display.Flip();

}

void Game::Editor::EventGameAreaPoint(EditorInterface & I, const Point & pt)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    const s32 index_maps = I.gameArea.GetIndexFromMousePoint(pt);
    Maps::Tiles & tile = world.GetTiles(index_maps);
    const Rect tile_pos(BORDERWIDTH + ((u16) (pt.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (pt.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);
    //u8 object = tile.GetObject();

    cursor.SetThemes(cursor.POINTER);

    const u16 div_x = pt.x < BORDERWIDTH + TILEWIDTH * (I.gameArea.GetRectMaps().w - I.sizeCursor.w()) ?
			    (u16) ((pt.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH + BORDERWIDTH :
			    BORDERWIDTH + (I.gameArea.GetRectMaps().w - I.sizeCursor.w()) * TILEWIDTH;
    const u16 div_y = pt.y < BORDERWIDTH + TILEWIDTH * (I.gameArea.GetRectMaps().h - I.sizeCursor.h()) ?
			    (u16) ((pt.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH + BORDERWIDTH :
			    BORDERWIDTH + (I.gameArea.GetRectMaps().h - I.sizeCursor.h()) * TILEWIDTH;

    if(! I.sizeCursor.isVisible() || I.sizeCursor.GetPos().x != div_x || I.sizeCursor.GetPos().y != div_y)
    {
	cursor.Hide();
	I.sizeCursor.Hide();
	I.sizeCursor.Show(div_x, div_y);
	cursor.Show();
	display.Flip();
    }

    if(le.MousePressLeft())
    {
        if(I.btnSelectGround.isPressed() &&
	    1 < I.sizeCursor.w() && 1 < I.sizeCursor.h())
	    EventGroundMode(I, pt, div_x, div_y);
        else
	if(I.btnSelectObject.isPressed())
	    EventObjectMode(I, pt);
	//else
	//if(I.btnSizeManual.isPressed());
	else
	{
	    VERBOSE("point");
	}

	I.needRedraw = true;
    }
    else
    if(le.MousePressRight())
    {
	if(I.btnSelectInfo.isPressed())
	{
	    if(IS_DEVEL())
	    {
		DEBUG(DBG_GAME, DBG_INFO, tile.String());

		//const u16 around = Maps::GetDirectionAroundGround(tile.GetIndex(), tile.GetGround());
		//if(Direction::TOP_LEFT & around) VERBOSE("TOP_LEFT");
		//if(Direction::TOP & around) VERBOSE("TOP");
		//if(Direction::TOP_RIGHT & around) VERBOSE("TOP_RIGHT");
		//if(Direction::RIGHT & around) VERBOSE("RIGHT");
		//if(Direction::BOTTOM_RIGHT & around) VERBOSE("BOTTOM_RIGHT");
		//if(Direction::BOTTOM & around) VERBOSE("BOTTOM");
		//if(Direction::BOTTOM_LEFT & around) VERBOSE("BOTTOM_LEFT");
		//if(Direction::LEFT & around) VERBOSE("LEFT");

		// wait
		while(le.HandleEvents() && le.MousePressRight());
	    }
	    else
	    {
		//const std::string & info = (MP2::OBJ_ZERO == object || MP2::OBJ_EVENT == object ?
		//Maps::Ground::String(tile.GetGround()) : MP2::StringObject(object));

		//Dialog::QuickInfo(info);
	    }
	}
    }
}

void Game::Editor::EventGroundMode(EditorInterface & I, const Point & pt, u16 div_x, u16 div_y)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    I.sizeCursor.Hide();

    const Point topleft(I.gameArea.GetRectMaps().x + (div_x - BORDERWIDTH) / 32,
				I.gameArea.GetRectMaps().y + (div_y - BORDERWIDTH) / 32);

    if(I.selectTerrain != Maps::Ground::UNKNOWN)
	ModifyGround(topleft, I.sizeCursor.w(), I.sizeCursor.h(), I.selectTerrain);

    I.sizeCursor.Show();
    cursor.Show();

    display.Flip();

    // wait
    while(le.HandleEvents() && le.MousePressLeft());

    I.radar.Generate();
    I.radar.RedrawArea();
    I.radar.RedrawCursor();
    display.Flip();
}

void Game::Editor::EventObjectMode(EditorInterface & I, const Point & pt)
{
}

bool IS_EQUAL_VALS(u16 A, u16 B)
{
    return (A & B) == A;
}

/* return pair, first: index tile, second: shape - 0: none, 1: vert, 2: horz, 3: both */
bool Game::Editor::GetIndexShapeForTile(const s32 & center, u16 ground, std::pair<u16, u8> & res)
{
    const AroundGrounds around(center);

    /*
	1. water - any ground (GetStartGroundTile(ground))
	2. ground - other ground (GetStartGroundTile(ground))
	3. ground - water (+16)
    */

    const u16 ground_and = around.GetAroundID(ground);
    const u16 ground_not = ground == Maps::Ground::WATER ? around.GetAroundID(Maps::Ground::ALL) :
				around.GetAroundID(Maps::Ground::WATER | (Maps::Ground::ALL & ~ground));
    u16 marker_id = 0;

    // top
    if(IS_EQUAL_VALS(Direction::LEFT | Direction::RIGHT | Direction::BOTTOM, ground_and) &&
	IS_EQUAL_VALS(Direction::TOP, ground_not))
    {
	marker_id = around.GetGrounds(Direction::TOP);
	res = std::make_pair(GetStartGround2Tile(ground), 0);
    }
    else
    // bottom
    if(IS_EQUAL_VALS(Direction::LEFT | Direction::RIGHT | Direction::TOP, ground_and) &&
	IS_EQUAL_VALS(Direction::BOTTOM, ground_not))
    {
	marker_id = around.GetGrounds(Direction::BOTTOM);
	res = std::make_pair(GetStartGround2Tile(ground), 1);
    }
    else
    // right
    if(IS_EQUAL_VALS(Direction::LEFT | Direction::TOP | Direction::BOTTOM, ground_and) &&
	IS_EQUAL_VALS(Direction::RIGHT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::RIGHT);
	res = std::make_pair(GetStartGround2Tile(ground) + 8, 0);
    }
    else
    // left
    if(IS_EQUAL_VALS(Direction::RIGHT | Direction::TOP | Direction::BOTTOM, ground_and) &&
	IS_EQUAL_VALS(Direction::LEFT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::LEFT);
	res = std::make_pair(GetStartGround2Tile(ground) + 8, 2);
    }
    else
    // corner: top + top right + right
    if(IS_EQUAL_VALS(Direction::LEFT | Direction::BOTTOM | Direction::BOTTOM_LEFT, ground_and) &&
	IS_EQUAL_VALS(Direction::TOP | Direction::RIGHT, ground_not))
    {
	if(Maps::Ground::WATER != ground &&
	    around.GetGrounds(Direction::TOP) != around.GetGrounds(Direction::RIGHT))
	{
	    res = Maps::Ground::WATER == around.GetGrounds(Direction::TOP) ?
		    std::make_pair(GetStartGroundTile(ground) + 36, 0) :
		    std::make_pair(GetStartGroundTile(ground) + 37, 0);
	}
	else
	{
	    marker_id = around.GetGrounds(Direction::TOP | Direction::RIGHT);
	    res = std::make_pair(GetStartGround2Tile(ground) + 4, 0);
	}
    }
    else
    // corner: top + top left + left
    if(IS_EQUAL_VALS(Direction::RIGHT | Direction::BOTTOM | Direction::BOTTOM_RIGHT, ground_and) &&
	IS_EQUAL_VALS(Direction::TOP | Direction::LEFT, ground_not))
    {
	if(Maps::Ground::WATER != ground &&
	    around.GetGrounds(Direction::TOP) != around.GetGrounds(Direction::LEFT))
	{
	    res = Maps::Ground::WATER == around.GetGrounds(Direction::TOP) ?
		    std::make_pair(GetStartGroundTile(ground) + 36, 2) :
		    std::make_pair(GetStartGroundTile(ground) + 37, 2);
	}
	else
	{
	    marker_id = around.GetGrounds(Direction::TOP | Direction::LEFT);
	    res = std::make_pair(GetStartGround2Tile(ground) + 4, 2);
	}
    }
    else
    // corner: bottom + bottom right + right
    if(IS_EQUAL_VALS(Direction::LEFT | Direction::TOP | Direction::TOP_LEFT, ground_and) &&
	IS_EQUAL_VALS(Direction::BOTTOM | Direction::RIGHT, ground_not))
    {
	if(Maps::Ground::WATER != ground &&
	    around.GetGrounds(Direction::BOTTOM) != around.GetGrounds(Direction::RIGHT))
	{
	    res = Maps::Ground::WATER == around.GetGrounds(Direction::BOTTOM) ?
		    std::make_pair(GetStartGroundTile(ground) + 36, 1) :
		    std::make_pair(GetStartGroundTile(ground) + 37, 1);
	}
	else
	{
	    marker_id = around.GetGrounds(Direction::BOTTOM | Direction::RIGHT);
	    res = std::make_pair(GetStartGround2Tile(ground) + 4, 1);
	}
    }
    else
    // corner: bottom + bottom left + left
    if(IS_EQUAL_VALS(Direction::RIGHT | Direction::TOP | Direction::TOP_RIGHT, ground_and) &&
	IS_EQUAL_VALS(Direction::BOTTOM | Direction::LEFT, ground_not))
    {
	if(Maps::Ground::WATER != ground &&
	    around.GetGrounds(Direction::BOTTOM) != around.GetGrounds(Direction::LEFT))
	{
	    res = Maps::Ground::WATER == around.GetGrounds(Direction::BOTTOM) ?
		    std::make_pair(GetStartGroundTile(ground) + 36, 3) :
		    std::make_pair(GetStartGroundTile(ground) + 37, 3);
	}
	else
	{
	    marker_id = around.GetGrounds(Direction::BOTTOM | Direction::LEFT);
	    res = std::make_pair(GetStartGround2Tile(ground) + 4, 3);
	}
    }
    else
    // corner: top right
    if(IS_EQUAL_VALS(DIRECTION_ALL & ~(Direction::TOP_RIGHT | Direction::CENTER), ground_and) &&
	IS_EQUAL_VALS(Direction::TOP_RIGHT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::TOP_RIGHT);
	res = std::make_pair(GetStartGround2Tile(ground) + 12, 0);
    }
    else
    // corner: top left
    if(IS_EQUAL_VALS(DIRECTION_ALL & ~(Direction::TOP_LEFT | Direction::CENTER), ground_and) &&
	IS_EQUAL_VALS(Direction::TOP_LEFT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::TOP_LEFT);
	res = std::make_pair(GetStartGround2Tile(ground) + 12, 2);
    }
    else
    // corner: bottom right
    if(IS_EQUAL_VALS(DIRECTION_ALL & ~(Direction::BOTTOM_RIGHT | Direction::CENTER), ground_and) &&
	IS_EQUAL_VALS(Direction::BOTTOM_RIGHT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::BOTTOM_RIGHT);
	res = std::make_pair(GetStartGround2Tile(ground) + 12, 1);
    }
    else
    // corner: bottom left
    if(IS_EQUAL_VALS(DIRECTION_ALL & ~(Direction::BOTTOM_LEFT | Direction::CENTER), ground_and) &&
	IS_EQUAL_VALS(Direction::BOTTOM_LEFT, ground_not))
    {
	marker_id = around.GetGrounds(Direction::BOTTOM_LEFT);
	res = std::make_pair(GetStartGround2Tile(ground) + 12, 3);
    }
    else
    // filled
    if(IS_EQUAL_VALS(DIRECTION_ALL & ~Direction::CENTER, ground_and))
    {
	res = std::make_pair(GetStartFilledTile(ground), 0);
    }
    else
    {
	VERBOSE("UNKNOWN: x: " << center % world.w() << ", y: " << center / world.h() << ", ground_and: "
			<< static_cast<int>(ground_and) << ", ground_not: " << static_cast<int>(ground_not));
	return false;
    }

    // dirt fixed
    if(Maps::Ground::DIRT == ground)
    {
	if(Maps::Ground::WATER != marker_id)
	    res.first = GetStartFilledTile(ground);
    }
    else
    // coast fixed
    if(Maps::Ground::WATER != ground && Maps::Ground::WATER == marker_id)
	res.first += 16;

    return true;
}

void Game::Editor::ModifyGround(const Point & pt, u8 ww, u8 hh, u16 ground)
{
    // fill default
    for(u8 yy = 0; yy < hh; ++yy)
	for(u8 xx = 0; xx < ww; ++xx)
	    world.GetTiles(pt.x + xx, pt.y + yy).SetTile(GetStartFilledTile(ground), 0);

    std::pair<u16, u8> res;

    // fix border
    for(s8 yy = -1; yy < hh + 1; ++yy)
    {
	for(s8 xx = -1; xx < ww + 1; ++xx)
	{
	    if(Maps::isValidAbsPoint(pt.x + xx, pt.y + yy))
	    {
		const s32 center = Maps::GetIndexFromAbsPoint(pt.x + xx, pt.y + yy);
		Maps::Tiles & tile = world.GetTiles(center);

		if(tile.GetGround() != Maps::Ground::BEACH &&
		    GetIndexShapeForTile(center, tile.GetGround(), res))
		    tile.SetTile(res.first, res.second);
	    }
	}
    }
}

Game::menu_t Game::Editor::StartGame(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    EditorInterface & I = EditorInterface::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Game::SetFixVideoMode();
    Settings::Get().SetScrollSpeed(SCROLL_FAST2);

    I.Build();

    const Sprite & spritePanelGround = AGG::GetICN(ICN::EDITPANL, 0);
    const Sprite & spritePanelObject = AGG::GetICN(ICN::EDITPANL, 1);
    const Sprite & spritePanelInfo = AGG::GetICN(ICN::EDITPANL, 2);
    const Sprite & spritePanelRiver = AGG::GetICN(ICN::EDITPANL, 3);
    const Sprite & spritePanelRoad = AGG::GetICN(ICN::EDITPANL, 4);
    const Sprite & spritePanelClear = AGG::GetICN(ICN::EDITPANL, 5);

    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    I.Draw();

    I.btnSelectGround.Press();
    I.btnSizeMedium.Press();

    I.btnSelectGround.Draw();
    I.btnSizeMedium.Draw();

    const Point dstPanel(I.btnSelectRiver.x, I.btnSelectRiver.y + I.btnSelectRiver.h);
    
    const Rect rectTerrainWater(dstPanel.x + 29, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainGrass(dstPanel.x + 58, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSnow(dstPanel.x + 87, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSwamp(dstPanel.x + 29, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainLava(dstPanel.x + 58, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainDesert(dstPanel.x + 87, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainDirt(dstPanel.x + 29, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainWasteland(dstPanel.x + 58, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainBeach(dstPanel.x + 87, dstPanel.y + 68, 28, 28);

    const Rect rectObjectWater(dstPanel.x + 14, dstPanel.y + 11, 28, 28);
    const Rect rectObjectGrass(dstPanel.x + 43, dstPanel.y + 11, 28, 28);
    const Rect rectObjectSnow(dstPanel.x + 72, dstPanel.y + 11, 28, 28);
    const Rect rectObjectSwamp(dstPanel.x + 101, dstPanel.y + 11, 28, 28);
    const Rect rectObjectLava(dstPanel.x + 14, dstPanel.y + 39, 28, 28);
    const Rect rectObjectDesert(dstPanel.x + 43, dstPanel.y + 39, 28, 28);
    const Rect rectObjectDirt(dstPanel.x + 72, dstPanel.y + 39, 28, 28);
    const Rect rectObjectWasteland(dstPanel.x + 101, dstPanel.y + 39, 28, 28);
    const Rect rectObjectBeach(dstPanel.x + 14, dstPanel.y + 67, 28, 28);
    const Rect rectObjectTown(dstPanel.x + 43, dstPanel.y + 67, 28, 28);
    const Rect rectObjectMonster(dstPanel.x + 72, dstPanel.y + 67, 28, 28);
    const Rect rectObjectHero(dstPanel.x + 101, dstPanel.y + 67, 28, 28);
    const Rect rectObjectArtifact(dstPanel.x + 14, dstPanel.y + 95, 28, 28);
    const Rect rectObjectResource(dstPanel.x + 101, dstPanel.y + 95, 28, 28);

    SpriteCursor selectTerrainCursor(AGG::GetICN(ICN::TERRAINS, 9), rectTerrainWater.x - 1, rectTerrainWater.y - 1);
    selectTerrainCursor.Hide();

    SpriteCursor selectObjectCursor(AGG::GetICN(ICN::TERRAINS, 9), rectObjectWater.x - 1, rectObjectWater.y - 1);
    selectTerrainCursor.Hide();

    // redraw
    I.gameArea.Redraw(display, LEVEL_ALL);
    I.radar.RedrawArea();
    I.radar.RedrawCursor();
    spritePanelGround.Blit(dstPanel);
    selectTerrainCursor.Show();

    cursor.Show();
    display.Flip();

    //u32 ticket = 0;
    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(HotKeyPress(EVENT_DEFAULT_EXIT) && (Dialog::YES & Dialog::Message("", _("Are you sure you want to quit?"), Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;

	// scroll area maps left
	if(le.MouseCursor(I.scrollLeft))	I.gameArea.SetScroll(SCROLL_LEFT);
	else
	// scroll area maps right
	if(le.MouseCursor(I.scrollRight))	I.gameArea.SetScroll(SCROLL_RIGHT);

	// scroll area maps top
	if(le.MouseCursor(I.scrollTop))		I.gameArea.SetScroll(SCROLL_TOP);
	else
	// scroll area maps bottom
	if(le.MouseCursor(I.scrollBottom))	I.gameArea.SetScroll(SCROLL_BOTTOM);

	// point radar
	if(le.MouseCursor(I.radar.GetArea()) &&
	    (le.MouseClickLeft(I.radar.GetArea()) || le.MousePressLeft(I.radar.GetArea())))
	    EventRadarPoint(I, le.GetMouseCursor());
	else
	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel))
	    EventLeftPanelPoint(I, le.GetMouseCursor());
	else
	// cursor over game area
	if(le.MouseCursor(I.gameArea.GetArea()) &&
	    Maps::isValidAbsIndex(I.gameArea.GetIndexFromMousePoint(le.GetMouseCursor())))
	    EventGameAreaPoint(I, le.GetMouseCursor());

	// draw push buttons
	le.MousePressLeft(I.btnLeftTopScroll) ? I.btnLeftTopScroll.PressDraw() : I.btnLeftTopScroll.ReleaseDraw();
	le.MousePressLeft(I.btnRightTopScroll) ? I.btnRightTopScroll.PressDraw() : I.btnRightTopScroll.ReleaseDraw();
	le.MousePressLeft(I.btnTopScroll) ? I.btnTopScroll.PressDraw() : I.btnTopScroll.ReleaseDraw();
	le.MousePressLeft(I.btnLeftBottomScroll) ? I.btnLeftBottomScroll.PressDraw() : I.btnLeftBottomScroll.ReleaseDraw();
	le.MousePressLeft(I.btnLeftScroll) ? I.btnLeftScroll.PressDraw() : I.btnLeftScroll.ReleaseDraw();
	le.MousePressLeft(I.btnRightScroll) ? I.btnRightScroll.PressDraw() : I.btnRightScroll.ReleaseDraw();
	le.MousePressLeft(I.btnRightBottomScroll) ? I.btnRightBottomScroll.PressDraw() : I.btnRightBottomScroll.ReleaseDraw();
	le.MousePressLeft(I.btnBottomScroll) ? I.btnBottomScroll.PressDraw() : I.btnBottomScroll.ReleaseDraw();

	le.MousePressLeft(I.btnZoom) ? I.btnZoom.PressDraw() : I.btnZoom.ReleaseDraw();
	le.MousePressLeft(I.btnUndo) ? I.btnUndo.PressDraw() : I.btnUndo.ReleaseDraw();
        le.MousePressLeft(I.btnNew) ? I.btnNew.PressDraw() : I.btnNew.ReleaseDraw();
	le.MousePressLeft(I.btnSpec) ? I.btnSpec.PressDraw() : I.btnSpec.ReleaseDraw();
        le.MousePressLeft(I.btnFile) ? I.btnFile.PressDraw() : I.btnFile.ReleaseDraw();
	le.MousePressLeft(I.btnSystem) ? I.btnSystem.PressDraw() : I.btnSystem.ReleaseDraw();

	// click control button
	if(le.MouseClickLeft(I.btnSelectGround) ||
	    le.MouseClickLeft(I.btnSelectObject) ||
	    le.MouseClickLeft(I.btnSelectInfo) ||
	    le.MouseClickLeft(I.btnSelectRiver) ||
	    le.MouseClickLeft(I.btnSelectRoad) ||
	    le.MouseClickLeft(I.btnSelectClear))
	{
	    cursor.Hide();

	    if(I.btnSelectGround.isPressed()){ I.btnSelectGround.Release(); I.btnSelectGround.Draw(); }
	    else
	    if(I.btnSelectObject.isPressed()){ I.btnSelectObject.Release(); I.btnSelectObject.Draw(); }
	    else
	    if(I.btnSelectInfo.isPressed()){ I.btnSelectInfo.Release(); I.btnSelectInfo.Draw(); }
	    else
	    if(I.btnSelectRiver.isPressed()){ I.btnSelectRiver.Release(); I.btnSelectRiver.Draw(); }
	    else
	    if(I.btnSelectRoad.isPressed()){ I.btnSelectRoad.Release(); I.btnSelectRoad.Draw(); }
	    else
	    if(I.btnSelectClear.isPressed()){ I.btnSelectClear.Release(); I.btnSelectClear.Draw(); }

	    if(le.MouseCursor(I.btnSelectGround))
	    {
		I.selectTerrain = Maps::Ground::WATER;

		I.btnSizeSmall.SetDisable(false);
		I.btnSizeMedium.SetDisable(false);
		I.btnSizeLarge.SetDisable(false);
		I.btnSizeManual.SetDisable(false);

		I.btnSelectGround.Press();
		I.btnSelectGround.Draw();

		spritePanelGround.Blit(dstPanel);
		DEBUG(DBG_GAME, DBG_INFO, "select Terrain Mode");
		selectTerrainCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    }
	    else
	    if(le.MouseCursor(I.btnSelectObject))
	    {
		I.selectObject = 0;

		I.btnSizeSmall.SetDisable(true);
		I.btnSizeMedium.SetDisable(true);
		I.btnSizeLarge.SetDisable(true);
		I.btnSizeManual.SetDisable(true);

		I.btnSelectObject.Press();
		I.btnSelectObject.Draw();

		spritePanelObject.Blit(dstPanel);
		DEBUG(DBG_GAME , DBG_INFO, "select Object Mode");
		selectObjectCursor.Move(rectObjectWater.x - 1, rectObjectWater.y - 1);
	    }
	    else
	    if(le.MouseCursor(I.btnSelectInfo))
	    {
		I.btnSizeSmall.Release();
		I.btnSizeMedium.Release();
		I.btnSizeLarge.Release();
		I.btnSizeManual.Release();

		I.btnSizeSmall.Press();
		I.sizeCursor.ModifySize(1, 1);

		I.btnSizeSmall.SetDisable(true);
		I.btnSizeMedium.SetDisable(true);
		I.btnSizeLarge.SetDisable(true);
		I.btnSizeManual.SetDisable(true);

		I.btnSelectInfo.Press();
		I.btnSelectInfo.Draw();

		spritePanelInfo.Blit(dstPanel);
		DEBUG(DBG_GAME , DBG_INFO, "select Detail Mode");
	    }
	    else
	    if(le.MouseCursor(I.btnSelectRiver))
	    {
		I.btnSizeSmall.SetDisable(true);
		I.btnSizeMedium.SetDisable(true);
		I.btnSizeLarge.SetDisable(true);
		I.btnSizeManual.SetDisable(true);

		I.btnSelectRiver.Press();
		I.btnSelectRiver.Draw();

		spritePanelRiver.Blit(dstPanel);
		DEBUG(DBG_GAME , DBG_INFO, "select Stream Mode");
	    }
	    else
	    if(le.MouseCursor(I.btnSelectRoad))
	    {
		I.btnSizeSmall.SetDisable(true);
		I.btnSizeMedium.SetDisable(true);
		I.btnSizeLarge.SetDisable(true);
		I.btnSizeManual.SetDisable(true);

		I.btnSelectRoad.Press();
		I.btnSelectRoad.Draw();

		spritePanelRoad.Blit(dstPanel		);
		DEBUG(DBG_GAME , DBG_INFO, "select Road Mode");
	    }
	    else
	    if(le.MouseCursor(I.btnSelectClear))
	    {
		I.btnSizeSmall.SetDisable(false);
		I.btnSizeMedium.SetDisable(false);
		I.btnSizeLarge.SetDisable(false);
		I.btnSizeManual.SetDisable(false);

		I.btnSelectClear.Press();
		I.btnSelectClear.Draw();

		spritePanelClear.Blit(dstPanel);
		DEBUG(DBG_GAME , DBG_INFO, "Erase Mode");
	    }
		

	    if(I.btnSizeSmall.isEnable()) I.btnSizeSmall.isPressed() ? I.btnSizeSmall.Press() : I.btnSizeSmall.Release();
	    if(I.btnSizeMedium.isEnable()) I.btnSizeMedium.isPressed() ? I.btnSizeMedium.Press() : I.btnSizeMedium.Release();
	    if(I.btnSizeLarge.isEnable()) I.btnSizeLarge.isPressed() ? I.btnSizeLarge.Press() : I.btnSizeLarge.Release();
	    if(I.btnSizeManual.isEnable()) I.btnSizeManual.isPressed() ? I.btnSizeManual.Press() : I.btnSizeManual.Release();

	    if(I.btnSizeSmall.isEnable()) I.btnSizeSmall.Draw();
	    if(I.btnSizeMedium.isEnable()) I.btnSizeMedium.Draw();
	    if(I.btnSizeLarge.isEnable()) I.btnSizeLarge.Draw();
	    if(I.btnSizeManual.isEnable()) I.btnSizeManual.Draw();

	    cursor.Show();
	    display.Flip();
	}

	// click select size button
	if((I.btnSizeSmall.isEnable() && le.MouseClickLeft(I.btnSizeSmall)) ||
	   (I.btnSizeMedium.isEnable() && le.MouseClickLeft(I.btnSizeMedium)) ||
	   (I.btnSizeLarge.isEnable() && le.MouseClickLeft(I.btnSizeLarge)) ||
	   (I.btnSizeManual.isEnable() && le.MouseClickLeft(I.btnSizeManual)))
	{
	    cursor.Hide();

	    I.btnSizeSmall.Release();
	    I.btnSizeMedium.Release();
	    I.btnSizeLarge.Release();
	    I.btnSizeManual.Release();

	    if(le.MouseCursor(I.btnSizeSmall)){ I.btnSizeSmall.Press(); I.sizeCursor.ModifySize(1, 1); }
	    else
	    if(le.MouseCursor(I.btnSizeMedium)){ I.btnSizeMedium.Press(); I.sizeCursor.ModifySize(2, 2); }
	    else
	    if(le.MouseCursor(I.btnSizeLarge)){ I.btnSizeLarge.Press(); I.sizeCursor.ModifySize(4, 4); }
	    else
	    if(le.MouseCursor(I.btnSizeManual)){ I.btnSizeManual.Press(); I.sizeCursor.ModifySize(1, 1); }

	    I.btnSizeSmall.Draw();
	    I.btnSizeMedium.Draw();
	    I.btnSizeLarge.Draw();
	    I.btnSizeManual.Draw();
		
	    cursor.Show();
	    display.Flip();
	}

	// click select terrain
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainWater))
	{
	    I.selectTerrain = Maps::Ground::WATER;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "water");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainGrass))
	{
	    I.selectTerrain = Maps::Ground::GRASS;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainGrass.x - 1, rectTerrainGrass.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "grass");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainSnow))
	{
	    I.selectTerrain = Maps::Ground::SNOW;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainSnow.x - 1, rectTerrainSnow.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "snow");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainSwamp))
	{
	    I.selectTerrain = Maps::Ground::SWAMP;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainSwamp.x - 1, rectTerrainSwamp.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "swamp");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainLava))
	{
	    I.selectTerrain = Maps::Ground::LAVA;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainLava.x - 1, rectTerrainLava.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "lava");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainBeach))
	{
	    I.selectTerrain = Maps::Ground::BEACH;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainBeach.x - 1, rectTerrainBeach.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "beach");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainDirt))
	{
	    I.selectTerrain = Maps::Ground::DIRT;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainDirt.x - 1, rectTerrainDirt.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "dirt");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainWasteland))
	{
	    I.selectTerrain = Maps::Ground::WASTELAND;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainWasteland.x - 1, rectTerrainWasteland.y - 1);
	    cursor.Show();
	    display.Flip(); 
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "wasteland");
	}
	else
	if(I.btnSelectGround.isPressed() && le.MouseClickLeft(rectTerrainDesert))
	{
	    I.selectTerrain = Maps::Ground::DESERT;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainDesert.x - 1, rectTerrainDesert.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select terrain: " << "desert");
	}

	// click select object
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectWater))
	{
	    I.selectObject = 0;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectWater.x - 1, rectObjectWater.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "water");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectGrass))
	{
	    I.selectObject = 1;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectGrass.x - 1, rectObjectGrass.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "grass");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectSnow))
	{
	    I.selectObject = 2;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectSnow.x - 1, rectObjectSnow.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "snow");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectSwamp))
	{
	    I.selectObject = 3;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectSwamp.x - 1, rectObjectSwamp.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "swamp");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectLava))
	{
	    I.selectObject = 4;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectLava.x - 1, rectObjectLava.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "lava");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectDesert))
	{
	    I.selectObject = 5;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectDesert.x - 1, rectObjectDesert.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "desert");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectDirt))
	{
	    I.selectObject = 6;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectDirt.x - 1, rectObjectDirt.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "dirt");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectWasteland))
	{
	    I.selectObject = 7;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectWasteland.x - 1, rectObjectWasteland.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "wasteland");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectBeach))
	{
	    I.selectObject = 8;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectBeach.x - 1, rectObjectBeach.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "beach");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectTown))
	{
	    I.selectObject = 9;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectTown.x - 1, rectObjectTown.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "town");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectMonster))
	{
	    I.selectObject = 10;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectMonster.x - 1, rectObjectMonster.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "monster");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectHero))
	{
	    I.selectObject = 11;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectHero.x - 1, rectObjectHero.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "hero");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectArtifact))
	{
	    I.selectObject = 12;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectArtifact.x - 1, rectObjectArtifact.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "artifact");
	}
	else
	if(I.btnSelectObject.isPressed() && le.MouseClickLeft(rectObjectResource))
	{
	    I.selectObject = 13;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectResource.x - 1, rectObjectResource.y - 1);
	    cursor.Show();
	    display.Flip();
	    DEBUG(DBG_GAME , DBG_INFO, "select object: " << "resource");
	}

	// button click
	if(le.MouseClickLeft(I.btnZoom))
	{
	    VERBOSE("Game::Editor::StartGame: FIXME: click button Zoom");
	}
	if(le.MouseClickLeft(I.btnUndo))
	{
	    VERBOSE("Game::Editor::StartGame: FIXME: click button Undo");
	}
	if(le.MouseClickLeft(I.btnNew))
	{
	    return EDITNEWMAP;
	}
	if(le.MouseClickLeft(I.btnSpec))
	{
	    VERBOSE("Game::Editor::StartGame: FIXME: click button Spec");
	}
	if(le.MouseClickLeft(I.btnFile))
	{
	    switch(Dialog::FileOptions())
	    {
		case Game::NEWGAME:	return EDITNEWMAP;
		case Game::LOADGAME:	return EDITLOADMAP;
		case Game::SAVEGAME:	return EDITSAVEMAP;
		case Game::QUITGAME:	return QUITGAME;
		
		default: break;
	    }
	}
	if(le.MouseClickLeft(I.btnSystem))
	{
	    VERBOSE("Game::Editor::StartGame: FIXME: click button Options");
	}

	// press right info
	if(le.MousePressRight(I.btnZoom))
	    Dialog::Message(_("Magnify"), _("Change between zoom and normal view."), Font::BIG);
	else
	if(le.MousePressRight(I.btnUndo))
	    Dialog::Message(_("Undo"), _("Undo your last action. Press againt to redo the action."), Font::BIG);
	else
	if(le.MousePressRight(I.btnNew))
	    Dialog::Message(_("New"), _("Start a new map from scratch."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSpec))
	    Dialog::Message(_("Specifications"), _("Edit maps title, description, and other general information."), Font::BIG);
	else
	if(le.MousePressRight(I.btnFile))
	    Dialog::Message(_("File Options"), _("Open the file options menu, where you can save or load maps, or quit out of the editor."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSystem))
	    Dialog::Message(_("System Options"), _("View the editor system options, which let you customize the editor."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectGround))
	    Dialog::Message(_("Terrain Mode"), _("Used to draw the underlying grass, dirt, water, etc. on the map."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectObject))
	    Dialog::Message(_("Object Mode"), _("Used to place objects (mountains, trees, treasure, etc.) on the map."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectInfo))
	    Dialog::Message(_("Detail Mode"), _("Used for special editing of monsters, heroes and towns."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectRiver))
	    Dialog::Message(_("Stream Mode"), _("Allows you to draw streams by clicking and dragging."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectRoad))
	    Dialog::Message(_("Road Mode"), _("Allows you to draw roads by clicking and dragging."), Font::BIG);
	else
	if(le.MousePressRight(I.btnSelectClear))
	    Dialog::Message(_("Erase Mode"), _("Used to erase objects of the map."), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainWater))
	    Dialog::Message(_("Water"), _("Traversable only by boat."), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainGrass))
	    Dialog::Message(_("Grass"), _("No special modifiers."), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainSnow))
	    Dialog::Message(_("Snow"), _("Cost 1.5 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)"), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainSwamp))
	    Dialog::Message(_("Swamp"), _("Cost 1.75 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)"), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainLava))
	    Dialog::Message(_("Lava"), _("No special modifiers."), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainBeach))
	    Dialog::Message(_("Beach"), _("Cost 1.25 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)"), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainDirt))
	    Dialog::Message(_("Dirt"), _("No special modifiers."), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainWasteland))
	    Dialog::Message(_("Wasteland"), _("Cost 1.25 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)"), Font::BIG);
	else
	if(I.btnSelectGround.isPressed() && le.MousePressRight(rectTerrainDesert))
	    Dialog::Message(_("Desert"), _("Cost 2 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)"), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectWater))
	    Dialog::Message(_("Water Objects"), _("Used to select objects most appropriate for use on water."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectGrass))
	    Dialog::Message(_("Grass Objects"), _("Used to select objects most appropriate for use on grass."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectSnow))
	    Dialog::Message(_("Snow Objects"), _("Used to select objects most appropriate for use on snow."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectSwamp))
	    Dialog::Message(_("Swamp Objects"), _("Used to select objects most appropriate for use on swamp."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectLava))
	    Dialog::Message(_("Lava Objects"), _("Used to select objects most appropriate for use on lava."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectDesert))
	    Dialog::Message(_("Desert Objects"), _("Used to select objects most appropriate for use on desert."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectDirt))
	    Dialog::Message(_("Dirt Objects"), _("Used to select objects most appropriate for use on dirt."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectWasteland))
	    Dialog::Message(_("Wasteland Objects"), _("Used to select objects most appropriate for use on wasteland."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectBeach))
	    Dialog::Message(_("Beach Objects"), _("Used to select objects most appropriate for use on beach."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectTown))
	    Dialog::Message(_("Towns"), _("Used to place a town or castle."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectMonster))
	    Dialog::Message(_("Monsters"), _("Used to place a monster group."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectHero))
	    Dialog::Message(_("Heroes"), _("Used to place a hero."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectArtifact))
	    Dialog::Message(_("Artifact"), _("Used to place an artifact."), Font::BIG);
	else
	if(I.btnSelectObject.isPressed() && le.MousePressRight(rectObjectResource))
	    Dialog::Message(_("Treasures"), _("Used to place a resource or treasure."), Font::BIG);

	if(I.gameArea.NeedScroll() || I.needRedraw)
	{
	    cursor.Hide();
	    I.sizeCursor.Hide();
	    cursor.SetThemes(I.gameArea.GetScrollCursor());
	    I.gameArea.Scroll();
	    //I.Scroll(scrollDir);
	    I.split_h.Move(I.gameArea.GetRectMaps().x);
	    I.split_v.Move(I.gameArea.GetRectMaps().y);
	    EditorInterface::DrawTopNumberCell();
	    EditorInterface::DrawLeftNumberCell();
	    I.gameArea.Redraw(display, LEVEL_ALL);
	    I.radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();

	    I.needRedraw = false;
	}
    }

    return QUITGAME;
}

#endif
