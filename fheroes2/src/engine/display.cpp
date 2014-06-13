/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include "rect.h"
#include "types.h"
#include "error.h"
#include "surface.h"
#include "display.h"

UpdateRects::UpdateRects() : bits(NULL), len(0), bf(0), bw(0)
{
}

UpdateRects::~UpdateRects()
{
    delete [] bits;
}

void UpdateRects::SetVideoMode(int dw, int dh)
{
    if(dw < 640)
    {
	bw = 4;
	bf = 2;
    }
    else
    if(dw > 640)
    {
	bw = 16;
	bf = 4;
    }
    else
    {
	bw = 8;
	bf = 3;
    }

    // fix bw and bf
    while(((dw % bw) || (dh % bw)) && 1 < bf)
    {
	bw >>= 1;
	--bf;
    }

    len = (dw >> bf) * (dh >> bf);
    len = ((len % 8) ? (len >> 3) + 1 : len >> 3);

    if(bits) delete [] bits;
    bits = new u8 [len];
    std::fill(bits, bits + len, 0);

    rects.reserve(len / 4);
}

size_t UpdateRects::Size(void) const
{
    return rects.size();
}

void UpdateRects::Clear(void)
{
    std::fill(bits, bits + len, 0);
    rects.clear();
}

SDL_Rect* UpdateRects::Data(void)
{
    return rects.size() ? &rects[0] : NULL;
}

void UpdateRects::PushRect(int px, int py, int pw, int ph)
{
    Display & display = Display::Get();

    if(0 != pw && 0 != ph &&
	px + pw > 0 && py + ph > 0 &&
	px < display.w() && py < display.h())
    {
	if(px < 0)
	{
	    pw += px;
	    px = 0;
	}

	if(py < 0)
	{
	    ph += py;
	    py = 0;
	}

	if(px + pw > display.w())
	    pw = display.w() - px;

	if(py + ph > display.h())
	    ph = display.h() - py;

	const int dw = display.w() >> bf;
	int xx, yy;

	for(yy = py; yy < py + ph; yy += bw)
	    for(xx = px; xx < px + pw; xx += bw)
		SetBit((yy >> bf) * dw + (xx >> bf), 1);

	yy = py + ph - 1;
	for(xx = px; xx < px + pw; xx += bw)
	    SetBit((yy >> bf) * dw + (xx >> bf), 1);

	xx = px + pw - 1;
	for(yy = py; yy < py + ph; yy += bw)
	    SetBit((yy >> bf) * dw + (xx >> bf), 1);

	yy = py + ph - 1;
	xx = px + pw - 1;
	SetBit((yy >> bf) * dw + (xx >> bf), 1);
    }
}

bool UpdateRects::BitsToRects(void)
{
    Display & display = Display::Get();

    const int dbf = display.w() >> bf;
    const size_t len2 = len << 3;
    size_t index;
    SDL_Rect rect;
    SDL_Rect* prt = NULL;

    for(index = 0; index < len2; ++index)
    {
	if(GetBit(index))
	{
    	    if(NULL != prt)
	    {
		if(static_cast<size_t>(rect.y) == (index / dbf) * bw)
		    rect.w += bw;
		else
		{
		    rects.push_back(*prt);
		    prt = NULL;
		}
    	    }

    	    if(NULL == prt)
    	    {
		rect.x = (index % dbf) * bw;
		rect.y = (index / dbf) * bw;
		rect.w = bw;
		rect.h = bw;
		prt = &rect;
    	    }
	}
	else
	{
    	    if(prt)
    	    {
		rects.push_back(*prt);
		prt = NULL;
    	    }
	}
    }

    if(prt)
    {
	rects.push_back(*prt);
	prt = NULL;
    }

    return rects.size();
}

void UpdateRects::SetBit(u32 index, bool value)
{
    if(value != GetBit(index))
	bits[index >> 3] ^= (1 << (index % 8));
}

bool UpdateRects::GetBit(u32 index) const
{
    return (bits[index >> 3] >> (index % 8));
}

Display::Display() : dirty(false)
{
}

Display::~Display()
{
}

bool Display::isDisplay(void) const
{
    return true;
}

void Display::SetVideoMode(int w, int h, u32 flags)
{
    Display & display = Display::Get();

    if(display.isValid() && display.w() == w && display.h() == h) return;

    if(display.surface && (display.surface->flags & SDL_FULLSCREEN)) flags |= SDL_FULLSCREEN;
    display.surface = SDL_SetVideoMode(w, h, 0, flags);

    if(!display.surface)
	Error::Except(__FUNCTION__, SDL_GetError());

    display.update_rects.SetVideoMode(display.w(), display.h());
}

/* flip */
void Display::Flip()
{
    if(dirty)
    {
	if(surface->flags & SDL_HWSURFACE)
	    SDL_Flip(surface);
	else
	if(update_rects.BitsToRects())
	{
	    SDL_UpdateRects(surface, update_rects.Size(), update_rects.Data());
	    update_rects.Clear();
	}

	dirty = false;
    }
}

/* full screen */
void Display::FullScreen(void)
{
    SDL_WM_ToggleFullScreen(surface);
}

/* set caption main window */
void Display::SetCaption(const char* str)
{
    SDL_WM_SetCaption(str, NULL);
}

/* set icons window */
void Display::SetIcons(Surface & icons)
{
    SDL_WM_SetIcon(icons(), NULL);
}

/* hide system cursor */
void Display::HideCursor(void)
{
    SDL_ShowCursor(SDL_DISABLE);
}

/* show system cursor */
void Display::ShowCursor(void)
{
    SDL_ShowCursor(SDL_ENABLE);
}

void Display::Fade(void)
{
    Surface temp(w(), h(), false);
    temp.Fill(ColorBlack);
    int alpha = 0;

    while(alpha < 70)
    {
	temp.Blit(alpha, 0, 0, *this);
        Flip();
	alpha += 5;
	DELAY(10);
    }
}

void Display::Rise(void)
{
    Surface temp(w(), h(), false);
    temp.Fill(ColorBlack);
    int alpha = 71;

    while(alpha > 5)
    {
	temp.Blit(alpha, 0, 0, *this);
	Flip();
	alpha -= 5;
	DELAY(10);
    }
}

/* get video display */
Display & Display::Get(void)
{
    static Display inside;
    return inside;
}

int Display::GetMaxMode(Size & result, bool rotate)
{
    SDL_Rect** modes = SDL_ListModes(NULL, SDL_ANYFORMAT);

    if(modes == (SDL_Rect **) 0)
    {
        std::cerr <<  "Display::" << "GetMaxMode: " << "no modes available" << std::endl;
	return 0;
    }
    else
    if(modes == (SDL_Rect **) -1)
    {
        //std::cout <<  "Display::" << "GetMaxMode: " << "all modes available" << std::endl;
	return -1;
    }
    else
    {
	int max = 0;
	int cur = 0;

	for(int ii = 0; modes[ii]; ++ii)
	{
	    if(max < modes[ii]->w * modes[ii]->h)
	    {
		max = modes[ii]->w * modes[ii]->h;
		cur = ii;
	    }
	}

	result.w = modes[cur]->w;
	result.h = modes[cur]->h;

	if(rotate && result.w < result.h)
	{
	    cur = result.w;
	    result.w = result.h;
	    result.h = cur;
	}
    }
    return 1;
}

void Display::AddUpdateRect(int px, int py, int pw, int ph)
{
    if(0 == (surface->flags & SDL_HWSURFACE))
	update_rects.PushRect(px, py, pw, ph);
    dirty = true;
}

std::string Display::GetInfo(void)
{
    Display & display = Display::Get();
    std::ostringstream os;
    char namebuf[12];

    os << "Display::" << "GetInfo: " <<
	display.w() << "x" << display.h() << 
	", bpp: " << static_cast<int>(display.depth()) <<
	", driver: " << SDL_VideoDriverName(namebuf, 12);

    return os.str();
}
