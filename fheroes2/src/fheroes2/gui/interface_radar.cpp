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
#include "game.h"
#include "gamearea.h"
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
Interface::Radar::Radar() : spriteArea(NULL), spriteCursor(NULL), cursor(NULL),
    sf_blue(NULL), sf_green(NULL), sf_red(NULL), sf_yellow(NULL),
    sf_orange(NULL), sf_purple(NULL), sf_gray(NULL), sf_black(NULL)
{
    Rect::w = RADARWIDTH;
    Rect::h = RADARWIDTH;
}

Interface::Radar::~Radar()
{
    if(cursor) delete cursor;
    if(spriteArea) delete spriteArea;
    if(spriteCursor) delete spriteCursor;
    if(sf_blue) delete sf_blue;
    if(sf_green) delete sf_green;
    if(sf_red) delete sf_red;
    if(sf_yellow) delete sf_yellow;
    if(sf_orange) delete sf_orange;
    if(sf_purple) delete sf_purple;
    if(sf_gray) delete sf_gray;
    if(sf_black) delete sf_black;
}

void Interface::Radar::SetPos(s16 px, s16 py)
{
    Rect::x = px;
    Rect::y = py;
}

const Rect & Interface::Radar::GetArea(void) const
{
    return *this;
}

/* construct gui */
void Interface::Radar::Build(void)
{
    if(cursor) delete cursor;
    if(spriteArea) delete spriteArea;
    if(spriteCursor) delete spriteCursor;
    if(sf_blue) delete sf_blue;
    if(sf_green) delete sf_green;
    if(sf_red) delete sf_red;
    if(sf_yellow) delete sf_yellow;
    if(sf_orange) delete sf_orange;
    if(sf_purple) delete sf_purple;
    if(sf_gray) delete sf_gray;
    if(sf_black) delete sf_black;

    spriteArea = new Surface(RADARWIDTH, RADARWIDTH);
    spriteCursor = new Surface(static_cast<u16>(GameArea::w() * (RADARWIDTH / static_cast<float>(world.w()))),
                	static_cast<u16>(GameArea::h() * (RADARWIDTH / static_cast<float>(world.h()))));
    cursor = new SpriteCursor(*spriteCursor, x, y);

    const u8 n = world.w() == Maps::SMALL ? 4 : 2;
    sf_blue = new Surface(n, n);
    sf_green = new Surface(n, n);
    sf_red = new Surface(n, n);
    sf_yellow = new Surface(n, n);
    sf_orange = new Surface(n, n);
    sf_purple = new Surface(n, n);
    sf_gray = new Surface(n, n);
    sf_black = new Surface(n, n);

    sf_blue->Fill(sf_blue->GetColor(COLOR_BLUE));
    sf_green->Fill(sf_green->GetColor(COLOR_GREEN));
    sf_red->Fill(sf_red->GetColor(COLOR_RED));
    sf_yellow->Fill(sf_yellow->GetColor(COLOR_YELLOW));
    sf_orange->Fill(sf_orange->GetColor(COLOR_ORANGE));
    sf_purple->Fill(sf_purple->GetColor(COLOR_PURPLE));
    sf_gray->Fill(sf_gray->GetColor(COLOR_GRAY));
    sf_black->Fill(0);

    Generate();
    Cursor::DrawCursor(*spriteCursor, RADARCOLOR);
}

Interface::Radar & Interface::Radar::Get(void)
{
    static Radar radar0;

    return radar0;
}

/* generate mini maps */
void Interface::Radar::Generate(void)
{
    const u16 world_w = world.w();
    const u16 world_h = world.h();

    const u8 n = world.w() == Maps::SMALL ? 4 : 2;
    Surface tile_surface(n, n);

    for(u16 index = 0; index < world_w * world_h; ++index)
    {
	const Maps::Tiles & tile = world.GetTiles(index);
	u32 color = COLOR_ROAD;

	if(tile.isRoad())
		tile_surface.Fill(tile_surface.GetColor(color));
	else
	if(0 != (color = GetPaletteIndexFromGround(tile.GetGround())))
		tile_surface.Fill(tile_surface.GetColor(tile.GetObject() == MP2::OBJ_MOUNTS ? color + 2 : color));
	else
	    continue;

	float dstx = (index % world_w) * RADARWIDTH / world_w;
	float dsty = (index / world_h) * RADARWIDTH / world_w;

	spriteArea->Blit(tile_surface, static_cast<u16>(dstx), static_cast<u16>(dsty));
    }
}

void Interface::Radar::HideArea(void)
{
    Display::Get().Blit(AGG::GetICN((Settings::Get().EvilInterface() ? ICN::HEROLOGE : ICN::HEROLOGO), 0), x, y);
}

void Interface::Radar::Redraw(void)
{
    RedrawArea(Settings::Get().MyColor());
    RedrawCursor();
}

/* redraw radar area for color */
void Interface::Radar::RedrawArea(const u8 color)
{
    Display & display = Display::Get();

    const u16 world_w = world.w();
    const u16 world_h = world.h();
    const Surface *tile_surface = NULL;

    cursor->Hide();
    display.Blit(*spriteArea, x, y);

    for(u16 index = 0; index < world_w * world_h; ++index)
    {
	const Maps::Tiles & tile = world.GetTiles(index);

	if(!Settings::Get().Debug() && tile.isFog(color))
	    tile_surface = sf_black;
	else
	    switch(tile.GetObject())
	    {
		case MP2::OBJ_HEROES:
		{
		    const Heroes *hero = world.GetHeroes(index);
		    if(hero) tile_surface = GetSurfaceFromColor(hero->GetColor());
		}
		break;

		case MP2::OBJ_CASTLE:
		case MP2::OBJN_CASTLE:
		{
		    const Castle *castle = world.GetCastle(index);
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
		    tile_surface = GetSurfaceFromColor(world.ColorCapturedObject(index)); break;

		default: continue;
	    }

	if(tile_surface)
	{
	    float dstx = (index % world_w) * RADARWIDTH / world_w;
	    float dsty = (index / world_h) * RADARWIDTH / world_w;

	    display.Blit(*tile_surface, x + static_cast<u16>(dstx), y + static_cast<u16>(dsty));
	}
    }
}

/* redraw radar cursor */
void Interface::Radar::RedrawCursor(void)
{
    cursor->Hide();
    cursor->Move(x + GameArea::x() * RADARWIDTH / world.w(),
                y + GameArea::y() * RADARWIDTH / world.h());
    cursor->Show();
}

Surface* Interface::Radar::GetSurfaceFromColor(const u8 color)
{
    switch(color)
    {
	case Color::BLUE:	return sf_blue;
	case Color::GREEN:	return sf_green;
	case Color::RED:	return sf_red;
	case Color::YELLOW:	return sf_yellow;
	case Color::ORANGE:	return sf_orange;
	case Color::PURPLE:	return sf_purple;
	case Color::GRAY:	return sf_gray;
	default:		break;
    }

    return NULL;
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
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    GameArea & gamearea = GameArea::Get();
    LocalEvent & le = LocalEvent::Get();

    if(le.MouseCursor(*this) &&
        (le.MouseClickLeft(*this) || le.MousePressLeft(*this)))
    {
        const Point prev(gamearea.GetRect());
        const Point & pt = le.GetMouseCursor();
	gamearea.Center((pt.x - x) * world.w() / RADARWIDTH, (pt.y - y) * world.h() / RADARWIDTH);
        if(prev != gamearea.GetRect())
        {
            cursor.Hide();
            RedrawCursor();
            gamearea.Redraw();
            cursor.Show();
            display.Flip();
        }
    }

    if(le.MousePressRight(*this)) Dialog::Message(_("World Map"), _("A miniature view of the known world. Left click to move viewing area."), Font::BIG);
}
