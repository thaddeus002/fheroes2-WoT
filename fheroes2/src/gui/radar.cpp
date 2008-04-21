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
#include "config.h"
#include "game.h"
#include "gamearea.h"
#include "ground.h"
#include "world.h"
#include "castle.h"
#include "radar.h"

#define RADARCOLOR	0x40	// index palette
#define COLOR_DESERT	0x71
#define COLOR_SNOW	0x0B
#define COLOR_SWAMP	0xA6
#define COLOR_WASTELAND	0xCE
#define COLOR_BEACH	0xC7
#define COLOR_LAVA	0x19
#define COLOR_DIRT	0x31
#define COLOR_GRASS	0x61
#define COLOR_WATER	0xF0
#define COLOR_ROAD	0x7A

#define COLOR_BLUE	0x47
#define COLOR_GREEN	0x67
#define COLOR_RED	0xbd
#define COLOR_YELLOW	0x70
#define COLOR_ORANGE	0xcd
#define COLOR_PURPLE	0x87


/* constructor */
Radar::Radar() :
    pos(Display::Get().w() - BORDERWIDTH - RADARWIDTH, BORDERWIDTH, RADARWIDTH, RADARWIDTH), spriteArea(RADARWIDTH, RADARWIDTH),
    spriteCursor(static_cast<u16>(GameArea::GetRect().w * (RADARWIDTH / static_cast<float>(world.w()))),
                 static_cast<u16>(GameArea::GetRect().h * (RADARWIDTH / static_cast<float>(world.h())))),
    cursor(spriteCursor, pos.x, pos.y)
{
    Settings::Get().Original() ? GenerateOrigin() : GenerateRealistic();

    DrawCursor(spriteCursor);
}

/* generate mini maps (origin version) */
void Radar::GenerateOrigin(void)
{
    Point dst_pt(0, 0);
    u8  width_til = RADARWIDTH / world.w();
    u16 index = 0;

    Surface tile_surface(Maps::SMALL ? 4 : 2, Maps::SMALL ? 4 : 2, true);
	Color::color_t mycolor = world.GetMyKingdom().GetColor();
    
    for(u16 jj = 0; jj < world.h(); ++jj)
    {
	for(u16 ii = 0; ii < world.w(); ++ii)
	{
	    Maps::Tiles & tile = world.GetTiles(index);

	    int pass;
	    switch (tile.GetObject()) {
	        case MP2::OBJ_TREES:
	        case MP2::OBJ_MOUNTS:
	        case MP2::OBJ_DUNE:
	            pass = 2;
	            break;
	        default:
	            pass = 0;
	    }
		if(!H2Config::Debug() && tile.GetFog(mycolor)) {
			tile_surface.Fill(0);
		} else 
	    if(tile.FindCastle()) {
			const Castle *castle = world.GetCastle(index);
			switch(castle->GetColor()) {
			  case Color::BLUE:
		        tile_surface.Fill(AGG::GetColor(COLOR_BLUE));
			    break;
			  case Color::GREEN:
		        tile_surface.Fill(AGG::GetColor(COLOR_GREEN));
			    break;
			  case Color::RED:
		        tile_surface.Fill(AGG::GetColor(COLOR_RED));
			    break;
			  case Color::YELLOW:
		        tile_surface.Fill(AGG::GetColor(COLOR_YELLOW));
			    break;
			  case Color::ORANGE:
		        tile_surface.Fill(AGG::GetColor(COLOR_ORANGE));
			    break;
			  case Color::PURPLE:
		        tile_surface.Fill(AGG::GetColor(COLOR_PURPLE));
			    break;
			  case Color::GRAY:
		        tile_surface.Fill(0xffffffff);
			    break;
			}
	    } else
		if(tile.FindMines()) {
		// TODO correct color for mine
	        tile_surface.Fill(0xffffffff);
		} else
	    if(tile.isRoad())
		tile_surface.Fill(AGG::GetColor(COLOR_ROAD));
	    else if(pass >= 0)
	    switch(tile.GetGround())
	    {
		case Maps::Ground::DESERT:	tile_surface.Fill(AGG::GetColor(COLOR_DESERT+pass)); break;
		case Maps::Ground::SNOW:	tile_surface.Fill(AGG::GetColor(COLOR_SNOW+pass)); break;
		case Maps::Ground::SWAMP:	tile_surface.Fill(AGG::GetColor(COLOR_SWAMP+pass)); break;
		case Maps::Ground::WASTELAND:	tile_surface.Fill(AGG::GetColor(COLOR_WASTELAND+pass)); break;
		case Maps::Ground::BEACH:	tile_surface.Fill(AGG::GetColor(COLOR_BEACH+pass)); break;
		case Maps::Ground::LAVA:	tile_surface.Fill(AGG::GetColor(COLOR_LAVA+pass)); break;
		case Maps::Ground::DIRT:	tile_surface.Fill(AGG::GetColor(COLOR_DIRT+pass)); break;
		case Maps::Ground::GRASS:	tile_surface.Fill(AGG::GetColor(COLOR_GRASS+pass)); break;
		case Maps::Ground::WATER:	tile_surface.Fill(AGG::GetColor(COLOR_WATER+pass)); break;
		default:			tile_surface.Fill(AGG::GetColor(COLOR_GRASS+pass)); break;
	    }

	    spriteArea.Blit(tile_surface, dst_pt);

	    dst_pt.x += width_til;

	    // X scale large maps
	    if(Maps::LARGE == world.w() && !(ii % 3))
	    {
		spriteArea.Blit(tile_surface, dst_pt);

		dst_pt.x += width_til;
	    }

	    ++index;
	}

	dst_pt.x = 0;
	dst_pt.y += width_til;

	// Y scale large maps
	if(Maps::LARGE == world.h() && !(jj % 3))
	{
	    const Rect src_rt(0, dst_pt.y - 1, RADARWIDTH, width_til);
	    spriteArea.Blit(spriteArea, src_rt, dst_pt);

	    dst_pt.y += width_til;
	}
    }
}

/* generate mini maps (realistic version) */
void Radar::GenerateRealistic(void)
{
    Point dst_pt(0, 0);
    u8  width_til = RADARWIDTH / world.w();
    u16 index = 0;

    Surface src_surface(TILEWIDTH, TILEWIDTH);
    Surface tile_surface(Maps::SMALL ? 4 : 2, Maps::SMALL ? 4 : 2);

    for(u16 jj = 0; jj < world.h(); ++jj)
    {
	for(u16 ii = 0; ii < world.w(); ++ii)
	{
	    src_surface.Blit(world.GetTiles(index).GetSurface());

	    tile_surface.ScaleFrom(src_surface);

	    spriteArea.Blit(tile_surface, dst_pt);

	    dst_pt.x += width_til;

	    // X scale large maps
	    if(Maps::LARGE == world.w() && !(ii % 3))
	    {
		spriteArea.Blit(tile_surface, dst_pt);

		dst_pt.x += width_til;
	    }

	    ++index;
	}

	dst_pt.x = 0;
	dst_pt.y += width_til;

	// Y scale large maps
	if(Maps::LARGE == world.h() && !(jj % 3))
	{
	    const Rect src_rt(0, dst_pt.y - 1, RADARWIDTH, width_til);
	    spriteArea.Blit(spriteArea, src_rt, dst_pt);

	    dst_pt.y += width_til;
	}
    }
}

/* draw radar cursor */
void Radar::DrawCursor(Surface &surface)
{
    if(! surface.valid()) return;

    surface.SetColorKey();

    u16 width  = surface.w();
    u16 height = surface.h();

    // draw cursor
    u32 color = AGG::GetColor(RADARCOLOR);
    surface.Lock();
    for(u8 i = 0; i < width; ++i){
	surface.SetPixel2(i, 0, color);
	if(i + 1 < width) surface.SetPixel2(i + 1, 0, color);
        i += 3;
    }
    for(u8 i = 0; i < width; ++i){
	surface.SetPixel2(i, height - 1, color);
	if(i + 1 < width) surface.SetPixel2(i + 1, height - 1, color);
        i += 3;
    }
    for(u8 i = 0; i < height; ++i){
	surface.SetPixel2(0, i, color);
	if(i + 1 < height) surface.SetPixel2(0, i + 1, color);
        i += 3;
    }
    for(u8 i = 0; i < height; ++i){
	surface.SetPixel2(width - 1, i, color);
	if(i + 1 < height) surface.SetPixel2(width - 1, i + 1, color);
        i += 3;
    }
    surface.Unlock();
}

/* redraw radar area */
void Radar::RedrawArea(void)
{ 
    Settings::Get().Original() ? GenerateOrigin() : GenerateRealistic();
    Display::Get().Blit(spriteArea, pos.x, pos.y); 
}

/* redraw radar cursor */
void Radar::RedrawCursor(void)
{
    cursor.Hide();
    Settings::Get().Original() ? GenerateOrigin() : GenerateRealistic();
    Display::Get().Blit(spriteArea, pos.x, pos.y); 
    cursor.Move(pos.x + GameArea::GetRect().x * RADARWIDTH / world.w(),
                pos.y + GameArea::GetRect().y * RADARWIDTH / world.h());
    cursor.Show();
}
