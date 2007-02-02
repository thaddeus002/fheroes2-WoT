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

#include <cstdlib>
#include "agg.h"
#include "game.h"
#include "radar.h"

#define RADARCOLOR      0x10	// index palette

/* constructor */
Radar::Radar(s16 rx, s16 ry, const MapsData &mp) : SDLmm::Surface(SDLmm::Surface::CreateSurface(SDL_SWSURFACE, RADARWIDTH, RADARWIDTH, DEFAULT_DEPTH)),
     maps(mp), area(rx, ry, RADARWIDTH, RADARWIDTH)
{
    GenerateFrom(maps.GetSurface());
}

/* redraw radar */
void Radar::Redraw(void){ display.Blit(*this, Point(area.x, area.y)); }

/* generate from surface */
void Radar::GenerateFrom(const SDLmm::Surface &surface)
{
    SDLmm::Surface & src = const_cast<SDLmm::Surface &>(surface);

    if(area.w > src.w() || area.h > src.h() ||
       area.w != area.h || src.w() != src.h()){ Error::Warning("Radar::Generate: incorrect param"); return; }

    u16 color = 0;
    u32 index = 0;

    u16 *p_src = NULL;
    u16 *p_dst = NULL;

    u16 width = 0;
    u16 height = 0;
    u16 width2 = 0;
    u16 dstX2 = 0;
    u16 dstY2 = 0;

    u8 count = 0;

    bool first = true;

    src.Lock();
    p_src = static_cast<u16 *>(src.pixels());

    // count min iteration    
    width = src.w();
    for(;;){
	++count;
	if((width >>= 1) <= area.w) break;
    }
    width = src.w() / 2;
    height = src.h() / 2;

    while(count){

	p_dst = new u16[width * height];

	// iteration 2х2 -> 1
	index = 0;
	width2 = 2 * width;

	for(u16 dstY = 0; dstY < height; ++dstY)
	    for(u16 dstX = 0; dstX < width; ++dstX){

		dstX2 = dstX * 2;
		dstY2 = dstY * 2;

		if((color = p_src[width2 * dstY2 + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
		    color == p_src[width2 * dstY2 + dstX2 + 1] || color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;

		else if((color = p_src[width2 * dstY2 + dstX2 + 1]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
	    	    color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;

		else if((color = p_src[width2 * (dstY2 + 1) + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1])
		    p_dst[index] = color;
	
		else
		    p_dst[index] = p_src[width2 * (dstY2 + 1) + dstX2 + 1];

		++index;
	    }

	if(!first && NULL != p_src) delete [] p_src;
	first = false;
	p_src = p_dst;
	p_dst = NULL;
	
	width = width / 2;
	height = height / 2;

	--count;
    }

    src.Unlock();

    // copy color from p_src to sprite radar
    Lock();
    memcpy(pixels(), p_src, sizeof(u16) * RADARWIDTH * RADARWIDTH);
    Unlock();

    delete [] p_src;

}

/* cursor radar constructor */
RadarCursor::RadarCursor(const Radar &radar, const MapsData &mp, const GameArea &ga) :
    Rect(radar.GetRect().x, radar.GetRect().y, static_cast<u16>(ga.GetWidth() * (RADARWIDTH / static_cast<float>(mp.GetWidth()))), static_cast<u16>(ga.GetHeight() * (RADARWIDTH / static_cast<float>(mp.GetHeight())))),
    SpriteCursor(static_cast<Rect>(*this)), maps(mp), area(ga)
{
    Fill(AGG::GetColorKey());
    SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, AGG::GetColorKey());

    // draw cursor
    u32 color = AGG::GetColor(RADARCOLOR);
    Lock();
    for(u8 i = 0; i < static_cast<Rect>(*this).w; ++i){
	SetPixel2(i, 0, color);
	SetPixel2(i + 1, 0, color);
        i += 3;
    }
    for(u8 i = 0; i < static_cast<Rect>(*this).w; ++i){
	SetPixel2(i, static_cast<Rect>(*this).h - 1, color);
	SetPixel2(i + 1, static_cast<Rect>(*this).h - 1, color);
        i += 3;
    }
    for(u8 i = 0; i < static_cast<Rect>(*this).h; ++i){
	SetPixel2(0, i, color);
	SetPixel2(0, i + 1, color);
        i += 3;
    }
    for(u8 i = 0; i < static_cast<Rect>(*this).h; ++i){
	SetPixel2(static_cast<Rect>(*this).w - 1, i, color);
	SetPixel2(static_cast<Rect>(*this).w - 1, i + 1, color);
        i += 3;
    }

    Unlock();
    SetDisplayFormat();
}

/* redraw radar cursor */
void RadarCursor::Redraw(void)
{
    Move(static_cast<Rect>(*this).x + area.GetOffsetX() * RADARWIDTH / maps.GetWidth(), static_cast<Rect>(*this).y + area.GetOffsetY() * RADARWIDTH / maps.GetHeight());
}
