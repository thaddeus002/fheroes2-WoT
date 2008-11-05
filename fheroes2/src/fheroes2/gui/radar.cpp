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

#include "gamedefs.h" 
#include "agg.h"
#include "settings.h"
#include "game.h"
#include "gamearea.h"
#include "ground.h"
#include "world.h"
#include "castle.h"
#include "sprite.h"
#include "cursor.h"
#include "radar.h"

#define RADARCOLOR	0x40	// index palette
#define COLOR_DESERT	0x70
#define COLOR_SNOW	0x0A
#define COLOR_SWAMP	0xA0
#define COLOR_WASTELAND	0xD6
#define COLOR_BEACH	0xC6
#define COLOR_LAVA	0x19
#define COLOR_DIRT	0x30
#define COLOR_GRASS	0x60
#define COLOR_WATER	0xF0
#define COLOR_ROAD	0x7A

#define COLOR_BLUE	0x47
#define COLOR_GREEN	0x67
#define COLOR_RED	0xbd
#define COLOR_YELLOW	0x70
#define COLOR_ORANGE	0xcd
#define COLOR_PURPLE	0x87
#define COLOR_GRAY	0x10

u32 GetPaletteIndexFromColor(const u8 color);
u32 GetPaletteIndexFromGround(const u16 ground);

/* constructor */
Radar::Radar() : spriteArea(NULL), spriteCursor(NULL), cursor(NULL)
{
}

Radar::~Radar()
{
    if(cursor) delete cursor;
    if(spriteArea) delete spriteArea;
    if(spriteCursor) delete spriteCursor;
}

/* construct gui */
void Radar::Build(void)
{
    pos = Rect(Display::Get().w() - BORDERWIDTH - RADARWIDTH, BORDERWIDTH, RADARWIDTH, RADARWIDTH);

    if(cursor) delete cursor;
    if(spriteArea) delete spriteArea;
    if(spriteCursor) delete spriteCursor;

    spriteArea = new Surface(RADARWIDTH, RADARWIDTH);
    spriteCursor = new Surface(static_cast<u16>(GameArea::w() * (RADARWIDTH / static_cast<float>(world.w()))),
                	static_cast<u16>(GameArea::h() * (RADARWIDTH / static_cast<float>(world.h()))));
    cursor = new SpriteCursor(*spriteCursor, pos.x, pos.y);

    Generate();
    Cursor::DrawCursor(*spriteCursor, RADARCOLOR);
}

Radar & Radar::Get(void)
{
    static Radar radar0;

    return radar0;
}

/* generate mini maps */
void Radar::Generate(void)
{
    const u16 world_w = world.w();
    const u16 world_h = world.h();

    Surface tile_surface(Maps::SMALL ? 4 : 2, Maps::SMALL ? 4 : 2);

    for(u16 index = 0; index < world_w * world_h; ++index)
    {
	const Maps::Tiles & tile = world.GetTiles(index);
	u32 color = COLOR_ROAD;

	if(tile.isRoad())
		tile_surface.Fill(AGG::GetColor(color));
	else
	if(0 != (color = GetPaletteIndexFromGround(tile.GetGround())))
		tile_surface.Fill(AGG::GetColor(tile.GetObject() == MP2::OBJ_MOUNTS ? color + 2 : color));
	else
	    continue;

	float dstx = (index % world_w) * RADARWIDTH / world_w;
	float dsty = (index / world_h) * RADARWIDTH / world_w;

	spriteArea->Blit(tile_surface, static_cast<u16>(dstx), static_cast<u16>(dsty));
    }
}

void Radar::HideArea(void)
{
    Display::Get().Blit(AGG::GetICN((Settings::Get().EvilInterface() ? ICN::HEROLOGE : ICN::HEROLOGO), 0), pos.x, pos.y);
}

/* redraw radar area for color */
void Radar::RedrawArea(const u8 color)
{
    Display & display = Display::Get();

    const u16 world_w = world.w();
    const u16 world_h = world.h();
    Surface tile_surface(Maps::SMALL ? 4 : 2, Maps::SMALL ? 4 : 2);

    cursor->Hide();
    display.Blit(*spriteArea, pos.x, pos.y);

    for(u16 index = 0; index < world_w * world_h; ++index)
    {
	const Maps::Tiles & tile = world.GetTiles(index);

	if(!Settings::Get().Debug() && tile.isFog(color))
	    tile_surface.Fill(0);
	else
	    switch(tile.GetObject())
	    {
		case MP2::OBJ_HEROES:
		{
		    const Heroes *hero = world.GetHeroes(index);
		    if(hero) tile_surface.Fill(AGG::GetColor(GetPaletteIndexFromColor(hero->GetColor())));
		}
		break;

		case MP2::OBJ_CASTLE:
		case MP2::OBJN_CASTLE:
		{
		    const Castle *castle = world.GetCastle(index);
		    if(castle) tile_surface.Fill(AGG::GetColor(GetPaletteIndexFromColor(castle->GetColor())));
		}
		break;

		case MP2::OBJ_DRAGONCITY:
		//case MP2::OBJN_DRAGONCITY:
		case MP2::OBJ_LIGHTHOUSE:
		//case MP2::OBJN_LIGHTHOUSE:
		case MP2::OBJ_ALCHEMYLAB:
		//case MP2::OBJN_ALCHEMYLAB:
		case MP2::OBJ_MINES:
		//case MP2::OBJN_MINES:
		case MP2::OBJ_SAWMILL:
		//case MP2::OBJN_SAWMILL:
		    tile_surface.Fill(AGG::GetColor(GetPaletteIndexFromColor(world.ColorCapturedObject(index)))); break;

		default: continue;
	    }

	float dstx = (index % world_w) * RADARWIDTH / world_w;
	float dsty = (index / world_h) * RADARWIDTH / world_w;

	display.Blit(tile_surface, pos.x + static_cast<u16>(dstx), pos.y + static_cast<u16>(dsty));
    }
}

/* redraw radar cursor */
void Radar::RedrawCursor(void)
{
    cursor->Hide();
    cursor->Move(pos.x + GameArea::x() * RADARWIDTH / world.w(),
                pos.y + GameArea::y() * RADARWIDTH / world.h());
    cursor->Show();
}

u32 GetPaletteIndexFromColor(const u8 color)
{
    switch(color)
    {
	case Color::BLUE:	return (COLOR_BLUE);
	case Color::GREEN:	return (COLOR_GREEN);
	case Color::RED:	return (COLOR_RED);
	case Color::YELLOW:	return (COLOR_YELLOW);
	case Color::ORANGE:	return (COLOR_ORANGE);
	case Color::PURPLE:	return (COLOR_PURPLE);
	case Color::GRAY:	return (COLOR_GRAY);
	default:		break;
    }

    return 0;
}

u32 GetPaletteIndexFromGround(const u16 ground)
{
    switch(ground)
    {
	case Maps::Ground::DESERT:	return (COLOR_DESERT);
	case Maps::Ground::SNOW:	return (COLOR_SNOW);
	case Maps::Ground::SWAMP:	return (COLOR_SWAMP);
	case Maps::Ground::WASTELAND:	return (COLOR_WASTELAND);
	case Maps::Ground::BEACH:	return (COLOR_BEACH);
	case Maps::Ground::LAVA:	return (COLOR_LAVA);
	case Maps::Ground::DIRT:	return (COLOR_DIRT);
	case Maps::Ground::GRASS:	return (COLOR_GRASS);
	case Maps::Ground::WATER:	return (COLOR_WATER);
	default: break;
    }

    return 0;
}
