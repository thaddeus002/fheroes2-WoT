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
#include "gamearea.h"
#include "error.h"
#include "maps.h"
#include "world.h"
#include "radar.h"

#define RADARCOLOR      0x10	// index palette

/* constructor */
Radar::Radar(s16 rx, s16 ry, const Surface &sf) :
    Surface(RADARWIDTH, RADARWIDTH), pos(rx, ry, RADARWIDTH, RADARWIDTH)
{
    GenerateFrom(sf);
}

/* redraw radar */
void Radar::Redraw(void){ display.Blit(*this, pos); }

/* generate from surface */
void Radar::GenerateFrom(const Surface &surface)
{
    Surface & src = const_cast<Surface &>(surface);

    if(pos.w > src.w() || pos.h > src.h() ||
       src.w() != src.h()){ Error::Warning("Radar::Generate: incorrect param"); return; }

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
    p_src = static_cast<u16 *>(const_cast<void *>(src.pixels()));

    // count min iteration    
    width = src.w();
    for(;;){
	++count;
	if((width >>= 1) <= pos.w) break;
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

    // correct if LARGE
    if(Maps::LARGE == World::GetWorld().w()){
	p_dst = new u16[RADARWIDTH * RADARWIDTH];

	u16 *pd1 = p_dst;
	u16 *ps1 = p_src;
	u16 *pd2 = p_dst;

	for(int iy = 0; iy < Maps::LARGE; ++iy){
	    pd2 = pd1;
	    for(int ix = 0; ix < Maps::LARGE; ++ix){
		*pd1 = *ps1;
		if(0 == (ix % 3)){ ++pd1; *pd1 = *ps1; }
		++pd1;
		++ps1;
	    }
	    if(0 == (iy % 3)){
		memcpy(pd1, pd2, sizeof(u16) * RADARWIDTH);
		pd1 += RADARWIDTH;
	    }
	}
	delete [] p_src;
	p_src = p_dst;
	p_dst = NULL;
    }

    // copy color from p_src to sprite radar
    Lock();
    memcpy(const_cast<void *>(pixels()), p_src, sizeof(u16) * RADARWIDTH * RADARWIDTH);
    Unlock();

    delete [] p_src;
}

/* draw radar cursor */
void Radar::DrawCursor(Surface &surface)
{
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
    surface.SetDisplayFormat();
}

void Radar::MoveCursor(SpriteCursor &cursor)
{
    cursor.Move(pos.x + GameArea::GetRect().x * RADARWIDTH / World::GetWorld().w(),
                pos.y + GameArea::GetRect().y * RADARWIDTH / World::GetWorld().h());
}
