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

#include "agg.h"
#include "settings.h"
#include "game.h"
#include "game_interface.h"
#include "ground.h"
#include "world.h"
#include "castle.h"
#include "cursor.h"
#include "interface_radar.h"

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

u32 GetPaletteIndexFromGround(const u16 ground);

/* constructor */
Interface::Radar::Radar(Basic & basic) : BorderWindow(Rect(0, 0, RADARWIDTH, RADARWIDTH)), interface(basic), hide(true)
{
}

void Interface::Radar::SavePosition(void)
{
    Settings::Get().SetPosRadar(GetRect());
}

void Interface::Radar::SetPos(s16 ox, s16 oy)
{
    BorderWindow::SetPosition(ox, oy);
}

/* construct gui */
void Interface::Radar::Build(void)
{
    const Rect & area = GetArea();

    spriteArea.Set(area.w, area.h);

    const u8 n = world.w() == Maps::SMALL ? 4 : 2;
    sf_blue.Set(n, n);
    sf_green.Set(n, n);
    sf_red.Set(n, n);
    sf_yellow.Set(n, n);
    sf_orange.Set(n, n);
    sf_purple.Set(n, n);
    sf_gray.Set(n, n);
    sf_black.Set(n, n);

    sf_blue.Fill(sf_blue.GetColorIndex(COLOR_BLUE));
    sf_green.Fill(sf_green.GetColorIndex(COLOR_GREEN));
    sf_red.Fill(sf_red.GetColorIndex(COLOR_RED));
    sf_yellow.Fill(sf_yellow.GetColorIndex(COLOR_YELLOW));
    sf_orange.Fill(sf_orange.GetColorIndex(COLOR_ORANGE));
    sf_purple.Fill(sf_purple.GetColorIndex(COLOR_PURPLE));
    sf_gray.Fill(sf_gray.GetColorIndex(COLOR_GRAY));
    sf_black.Fill(0);

    Generate();

    const Size & rectMaps = interface.GetGameArea().GetRectMaps();
    const u16 & sw = static_cast<u16>(rectMaps.w * (area.w / static_cast<float>(world.w())));
    const u16 & sh = static_cast<u16>(rectMaps.h * (area.h / static_cast<float>(world.h())));
    cursorArea.Set(sw, sh);
    Cursor::DrawCursor(cursorArea, RADARCOLOR);
    cursorArea.Move(area.x, area.y);
}

/* generate mini maps */
void Interface::Radar::Generate(void)
{
    const Rect & area = GetArea();
    const u16 world_w = world.w();
    const u16 world_h = world.h();

    const u8 n = world.w() == Maps::SMALL ? 4 : 2;
    Surface tile_surface(n, n);

    for(s32 index = 0; index < world_w * world_h; ++index)
    {
	const Maps::Tiles & tile = world.GetTiles(index);
	u32 color = COLOR_ROAD;

	if(tile.isRoad())
		tile_surface.Fill(tile_surface.GetColorIndex(color));
	else
	if(0 != (color = GetPaletteIndexFromGround(tile.GetGround())))
		tile_surface.Fill(tile_surface.GetColorIndex(tile.GetObject() == MP2::OBJ_MOUNTS ? color + 2 : color));
	else
	    continue;

	float dstx = (index % world_w) * area.w / world_w;
	float dsty = (index / world_h) * area.h / world_w;

	tile_surface.Blit(static_cast<u16>(dstx), static_cast<u16>(dsty), spriteArea);
    }
}

void Interface::Radar::SetHide(bool f)
{
    hide = f;
}

void Interface::Radar::SetRedraw(void) const
{
     interface.SetRedraw(REDRAW_RADAR);
}

void Interface::Radar::Redraw(void)
{
    const Settings & conf = Settings::Get();
    const Rect & area = GetArea();

    // redraw border
    if(conf.ExtGameHideInterface() && conf.ShowRadar())
	BorderWindow::Redraw();

    if(!hide)
    {
	RedrawArea(Players::FriendColors());
	RedrawCursor();
    }
    else
    // hide radar
    if(!conf.ExtGameHideInterface() || conf.ShowRadar())
	AGG::GetICN((conf.ExtGameEvilInterface() ? ICN::HEROLOGE : ICN::HEROLOGO), 0).Blit(area.x, area.y);
}

/* redraw radar area for color */
void Interface::Radar::RedrawArea(const u8 color)
{
    const Settings & conf = Settings::Get();

    if(!conf.ExtGameHideInterface() || conf.ShowRadar())
    {
	Display & display = Display::Get();
	const Rect & area = GetArea();

	const u16 world_w = world.w();
	const u16 world_h = world.h();
	Surface tile_surface;

	cursorArea.Hide();
	spriteArea.Blit(area.x, area.y, display);

	for(s32 index = 0; index < world_w * world_h; ++index)
	{
	    const Maps::Tiles & tile = world.GetTiles(index);
	    bool show_tile = ! tile.isFog(color);
#ifdef WITH_DEBUG
	     show_tile = IS_DEVEL() || ! tile.isFog(color);
#endif

	    if(! show_tile)
		tile_surface = sf_black;
	    else
	    switch(tile.GetObject())
	    {
		case MP2::OBJ_HEROES:
		{
		    const Heroes* hero = tile.GetHeroes();
		    if(hero) tile_surface = GetSurfaceFromColor(hero->GetColor());
		}
		break;

		case MP2::OBJ_CASTLE:
		case MP2::OBJN_CASTLE:
		{
		    const Castle* castle = world.GetCastle(index);
		    if(castle) tile_surface = GetSurfaceFromColor(castle->GetColor());
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
		    tile_surface = GetSurfaceFromColor(tile.QuantityColor()); break;

		default: continue;
	    }

	    if(tile_surface.isValid())
	    {
		float dstx = (index % world_w) * area.w / world_w;
		float dsty = (index / world_h) * area.h / world_w;

		tile_surface.Blit(area.x + static_cast<u16>(dstx), area.y + static_cast<u16>(dsty), display);
	    }
	}
    }
}

/* redraw radar cursor */
void Interface::Radar::RedrawCursor(void)
{
    const Settings & conf = Settings::Get();

    if(! conf.ExtGameHideInterface() || conf.ShowRadar())
    {
	const Rect & area = GetArea();
	const Rect & rectMaps = interface.GetGameArea().GetRectMaps();

	const u16 & sw = static_cast<u16>(rectMaps.w * (area.w / static_cast<float>(world.w())));
	const u16 & sh = static_cast<u16>(rectMaps.h * (area.h / static_cast<float>(world.h())));

	// check change game area
	if(cursorArea.w() != sw && cursorArea.h() != sh)
	{
	    cursorArea.Set(sw, sh);
	    Cursor::DrawCursor(cursorArea, RADARCOLOR);
	}

        cursorArea.Move(area.x + rectMaps.x * area.w / world.w(),
            		    area.y + rectMaps.y * area.h / world.h());
    }
}

Surface Interface::Radar::GetSurfaceFromColor(u8 color)
{
    switch(color)
    {
	case Color::BLUE:	return sf_blue;
	case Color::GREEN:	return sf_green;
	case Color::RED:	return sf_red;
	case Color::YELLOW:	return sf_yellow;
	case Color::ORANGE:	return sf_orange;
	case Color::PURPLE:	return sf_purple;
	case Color::NONE:	return sf_gray;
	default:		break;
    }

    return Surface();
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

void Interface::Radar::QueueEventProcessing(void)
{
    GameArea & gamearea = interface.GetGameArea();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();
    const Rect & area = GetArea();

    // move border
    if(conf.ShowRadar() &&
	BorderWindow::QueueEventProcessing())
    {
	RedrawCursor();
    }
    else
    // move cursor
    if(le.MouseCursor(area))
    {
	if(le.MouseClickLeft() || le.MousePressLeft())
	{
    	    const Point prev(gamearea.GetRectMaps());
    	    const Point & pt = le.GetMouseCursor();

	    if(area & pt)
	    {
		gamearea.SetCenter((pt.x - area.x) * world.w() / area.w, (pt.y - area.y) * world.h() / area.h);

    		if(prev != gamearea.GetRectMaps())
    		{
		    Cursor::Get().Hide();
        	    RedrawCursor();
        	    gamearea.SetRedraw();
    		}
	    }
	}
	else
	if(!conf.ExtPocketTapMode() && le.MousePressRight(GetRect()))
	    Dialog::Message(_("World Map"), _("A miniature view of the known world. Left click to move viewing area."), Font::BIG);
    }
}
