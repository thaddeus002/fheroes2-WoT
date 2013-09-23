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
#include "text.h"
#include "settings.h"
#include "cursor.h"
#include "dialog.h"

#define  ANGLEWIDTH 44

Dialog::FrameBorder::FrameBorder(int brd) : border(brd)
{
}

Dialog::FrameBorder::~FrameBorder()
{
    if(Cursor::Get().isVisible()){ Cursor::Get().Hide(); };
    background.Restore();
}

Dialog::FrameBorder::FrameBorder(const Size & sz, const Surface & sf) : border(BORDERWIDTH)
{
    Display & display = Display::Get();
    SetPosition((display.w() - sz.w - BORDERWIDTH * 2) / 2, (display.h() - sz.h - BORDERWIDTH * 2) / 2, sz.w, sz.h);
    Redraw(sf, Rect(0, 0, sf.w(), sf.h()), Display::Get(), GetRect());
}

Dialog::FrameBorder::FrameBorder(const Size & sz) : border(BORDERWIDTH)
{
    Display & display = Display::Get();
    SetPosition((display.w() - sz.w - BORDERWIDTH * 2) / 2, (display.h() - sz.h - BORDERWIDTH * 2) / 2, sz.w, sz.h);
    RedrawRegular(GetRect());
}

Dialog::FrameBorder::FrameBorder(s32 posx, s32 posy, u32 encw, u32 ench) : border(BORDERWIDTH)
{
    SetPosition(posx, posy, encw, ench);
    RedrawRegular(GetRect());
}

bool Dialog::FrameBorder::isValid(void) const
{
    return background.isValid();
}

void Dialog::FrameBorder::SetPosition(s32 posx, s32 posy, u32 encw, u32 ench)
{
    if(background.isValid())
	background.Restore();

    rect.x = posx;
    rect.y = posy;

    if(encw && ench)
    {
	rect.w = encw + 2 * border;
	rect.h = ench + 2 * border;

    	background.Save(rect);

	area.w = encw;
	area.h = ench;
    }
    else
    	background.Save(Point(posx, posy));

    area.x = posx + border;
    area.y = posy + border;

    top = Rect(posx, posy, area.w, border);
}

const Rect & Dialog::FrameBorder::GetTop(void) const
{
    return top;
}

const Rect & Dialog::FrameBorder::GetRect(void) const
{
    return rect;
}

const Rect & Dialog::FrameBorder::GetArea(void) const
{
    return area;
}

void Dialog::FrameBorder::RedrawRegular(const Rect & dstrt)
{
    const Surface & sf = AGG::GetICN((Settings::Get().ExtGameEvilInterface() ? ICN::SURDRBKE : ICN::SURDRBKG), 0);
    const u32 shadow = 16;

    Redraw(sf, Rect(shadow, 0, sf.w() - shadow, sf.h() - shadow), Display::Get(), dstrt);
}

void Dialog::FrameBorder::Redraw(const Surface & srcsf, const Rect & srcrt, Surface & dstsf, const Rect & dstrt)
{
    u32 mw = dstrt.w < srcrt.w ? dstrt.w : srcrt.w;
    u32 mh = dstrt.h < srcrt.h ? dstrt.h : srcrt.h;

    u32 cw = mw / 3;
    u32 ch = mh / 3;
    s32 cx = srcrt.x + (srcrt.w - cw) / 2;
    s32 cy = srcrt.y + (srcrt.h - ch) / 2;
    u32 bw = mw - 2 * cw;
    u32 bh = mh - 2 * ch;


    u32 ox = (dstrt.w - (dstrt.w / bw) * bw) / 2;
    u32 oy = (dstrt.h - (dstrt.h / bh) * bh) / 2;

    // body
    if(bw < dstrt.w && bh < dstrt.h)
	for(u32 yy = 0; yy < (dstrt.h / bh); ++yy)
	    for(u32 xx = 0; xx < (dstrt.w / bw); ++xx)
		srcsf.Blit(Rect(cx, cy, bw, bh), dstrt.x + ox + xx * bw, dstrt.y + oy + yy * bh, dstsf);

    // top, bottom bar
    for(u32 xx = 0; xx < (dstrt.w / bw); ++xx)
    {
	s32 dstx = dstrt.x + ox + xx * bw;
	srcsf.Blit(Rect(cx, srcrt.y, bw, ch), dstx, dstrt.y, dstsf);
	srcsf.Blit(Rect(cx, srcrt.y + srcrt.h - ch, bw, ch), dstx, dstrt.y + dstrt.h - ch, dstsf);
    }

    // left, right bar
    for(u32 yy = 0; yy < (dstrt.h / bh); ++yy)
    {
	s32 dsty = dstrt.y + oy + yy * bh;
	srcsf.Blit(Rect(srcrt.x, cy, cw, bh), dstrt.x, dsty, dstsf);
	srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, cy, cw, bh), dstrt.x + dstrt.w - cw, dsty, dstsf);
    }

    // top left angle
    srcsf.Blit(Rect(srcrt.x, srcrt.y, cw, ch), dstrt.x, dstrt.y, dstsf);

    // top right angle
    srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, srcrt.y, cw, ch), dstrt.x + dstrt.w - cw, dstrt.y, dstsf);

    // bottom left angle
    srcsf.Blit(Rect(srcrt.x, srcrt.y + srcrt.h - ch, cw, ch), dstrt.x, dstrt.y + dstrt.h - ch, dstsf);

    // bottom right angle
    srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, srcrt.y + srcrt.h - ch, cw, ch), dstrt.x + dstrt.w - cw, dstrt.y + dstrt.h - ch, dstsf);
}
