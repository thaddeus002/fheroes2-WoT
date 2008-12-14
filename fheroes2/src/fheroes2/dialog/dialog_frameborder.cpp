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
#include "cursor.h"
#include "dialog.h"

Dialog::FrameBorder::FrameBorder(bool fade, u16 enclosedWidth, u16 enclosedHeight)
: doFade(fade)
{
    Display & display = Display::Get();
    const Sprite & surdbkg = (H2Config::EvilInterface() ? AGG::GetICN(ICN::SURDRBKE, 0) : AGG::GetICN(ICN::SURDRBKG, 0));
    
    Rect pos;
    
    const u16 totalWidth = enclosedWidth + SHADOWWIDTH + 2 * BORDERWIDTH;
    const u16 totalHeight = enclosedHeight + SHADOWWIDTH + 2 * BORDERWIDTH;

    pos.x = 640 == display.w() ? 0 : (display.w() - totalWidth) / 2;
    pos.y = 640 == display.w() ? 0 : (display.h() - totalHeight) / 2;
    pos.w = 640 == display.w() ? 640 : totalWidth;
    pos.h = 640 == display.w() ? 480 : totalHeight;

    area.x = 640 == display.w() ? 0 : pos.x + BORDERWIDTH + SHADOWWIDTH;
    area.y = 640 == display.w() ? 0 : pos.y + BORDERWIDTH;
    area.w = 640;
    area.h = 480;

    if(Cursor::Get().isVisible()){ Cursor::Get().Hide(); };
    back.Save(pos);

    if(640 != display.w() || 480 != display.h())
    {
	Rect  src_rt;
	Point dst_pt;

	// top left angle
	src_rt = Rect(0, 0, SHADOWWIDTH + BORDERWIDTH * 3, BORDERWIDTH);
	dst_pt = Point(pos.x, pos.y);
	display.Blit(surdbkg, src_rt, dst_pt);

	// top bar
	src_rt = Rect(SHADOWWIDTH + BORDERWIDTH * 3, 0, surdbkg.w() - SHADOWWIDTH - BORDERWIDTH * 6, BORDERWIDTH);
	dst_pt = Point(pos.x + SHADOWWIDTH + BORDERWIDTH * 3, pos.y);
	while(dst_pt.x + src_rt.w < pos.x + pos.w)
	{
	    display.Blit(surdbkg, src_rt, dst_pt);
	    dst_pt.x += src_rt.w;
	}
	src_rt.w = pos.x + pos.w - dst_pt.x;
	display.Blit(surdbkg, src_rt, dst_pt);

	// top right angle
	src_rt = Rect(surdbkg.w() - BORDERWIDTH * 3, 0, BORDERWIDTH * 3, BORDERWIDTH);
	dst_pt = Point(pos.x + pos.w - src_rt.w, pos.y);
	display.Blit(surdbkg, src_rt, dst_pt);

	// bottom left angle
	src_rt = Rect(0, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, SHADOWWIDTH + BORDERWIDTH * 3, BORDERWIDTH + SHADOWWIDTH);
	dst_pt = Point(pos.x, pos.y + pos.h - BORDERWIDTH - SHADOWWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);

	// bottom bar
	src_rt = Rect(SHADOWWIDTH + BORDERWIDTH * 3, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, surdbkg.w() - SHADOWWIDTH - BORDERWIDTH * 6, BORDERWIDTH + SHADOWWIDTH);
	dst_pt = Point(pos.x + SHADOWWIDTH + BORDERWIDTH * 3, pos.y + pos.h - BORDERWIDTH - SHADOWWIDTH);
	while(dst_pt.x + src_rt.w < pos.x + pos.w)
	{
	    display.Blit(surdbkg, src_rt, dst_pt);
	    dst_pt.x += src_rt.w;
	}
	src_rt.w = pos.x + pos.w - dst_pt.x - BORDERWIDTH * 3;
	display.Blit(surdbkg, src_rt, dst_pt);

	// bottom right angle
	src_rt = Rect(surdbkg.w() - BORDERWIDTH * 3, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, BORDERWIDTH * 3, BORDERWIDTH + SHADOWWIDTH);
	dst_pt = Point(pos.x + pos.w - src_rt.w, pos.y + pos.h - BORDERWIDTH - SHADOWWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);

	// left top angle
	src_rt = Rect(0, BORDERWIDTH, SHADOWWIDTH + BORDERWIDTH, BORDERWIDTH * 2);
	dst_pt = Point(pos.x, pos.y + BORDERWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);

	// left bar
	src_rt = Rect(0, BORDERWIDTH * 3, SHADOWWIDTH + BORDERWIDTH, surdbkg.h() - BORDERWIDTH * 6 - SHADOWWIDTH);
	dst_pt = Point(pos.x, pos.y + BORDERWIDTH * 3);
	while(dst_pt.y + src_rt.h < pos.y + pos.h - SHADOWWIDTH - BORDERWIDTH * 3)
	{
	    display.Blit(surdbkg, src_rt, dst_pt);
	    dst_pt.y += src_rt.h;
	}
	src_rt.h = pos.y + pos.h - dst_pt.y - SHADOWWIDTH - BORDERWIDTH * 3;
	display.Blit(surdbkg, src_rt, dst_pt);

	// left bottom angle
	src_rt = Rect(0, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH * 3, SHADOWWIDTH + BORDERWIDTH, BORDERWIDTH * 2);
	dst_pt = Point(pos.x, pos.y + pos.h - BORDERWIDTH * 3 - SHADOWWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);

	// right top angle
	src_rt = Rect(surdbkg.w() - BORDERWIDTH, BORDERWIDTH, BORDERWIDTH, BORDERWIDTH * 2);
	dst_pt = Point(pos.x + pos.w - BORDERWIDTH, pos.y + BORDERWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);

	// right bar
	src_rt = Rect(surdbkg.w() - BORDERWIDTH, BORDERWIDTH * 3, BORDERWIDTH, surdbkg.h() - BORDERWIDTH * 6 - SHADOWWIDTH);
	dst_pt = Point(pos.x + pos.w - BORDERWIDTH, pos.y + BORDERWIDTH * 3);
	while(dst_pt.y + src_rt.h < pos.y + pos.h - SHADOWWIDTH - BORDERWIDTH * 3)
	{
	    display.Blit(surdbkg, src_rt, dst_pt);
	    dst_pt.y += src_rt.h;
	}
	src_rt.h = pos.y + pos.h - dst_pt.y - SHADOWWIDTH - BORDERWIDTH * 3;
	display.Blit(surdbkg, src_rt, dst_pt);

	// right bottom angle
	src_rt = Rect(surdbkg.w() - BORDERWIDTH, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH * 3, BORDERWIDTH, BORDERWIDTH * 2);
	dst_pt = Point(pos.x + pos.w - BORDERWIDTH, pos.y + pos.h - BORDERWIDTH * 3 - SHADOWWIDTH);
	display.Blit(surdbkg, src_rt, dst_pt);
    }
    else if(fade)
        Display::Fade();
}

Dialog::FrameBorder::~FrameBorder()
{
    if(Cursor::Get().isVisible()){ Cursor::Get().Hide(); };
    if(doFade)
        Display::Fade();
    back.Restore();
    Display::Flip();
}
