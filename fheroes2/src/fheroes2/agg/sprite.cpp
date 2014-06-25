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

#include "settings.h"
#include "agg.h"
#include "icn.h"
#include "cursor.h"
#include "display.h"
#include "sprite.h"

bool SkipLocalAlpha(int icn)
{
    switch(icn)
    {
        case ICN::SYSTEM:
        case ICN::SYSTEME:
        case ICN::BUYBUILD:
        case ICN::BUYBUILE:
        case ICN::BOOK:
        case ICN::CSPANBKE:
        case ICN::CPANBKGE:
        case ICN::CAMPBKGE:

            return true;

        default: break;
    }

    return false;
}

Sprite::Sprite()
{
}

Sprite::Sprite(const Surface & sf, s32 ox, s32 oy) : SpritePos(sf, Point(ox, oy))
{
}

int Sprite::x(void) const
{
    return pos.x;
}

int Sprite::y(void) const
{
    return pos.y;
}

Sprite Sprite::CreateICN(int icn, const ICNHeader & header, const u8* cur, int size, bool reflect)
{
    Surface sf(Size(header.width, header.height), true);
    const u8* max = cur + size;
    u32 c = 0;
    Point pt(reflect ? sf.w() - 1 : 0, 0);

    RGBA shadow = RGBA(0, 0, 0, 0x40);

    while(1)
    {
        // 0x00 - end line
        if(0 == *cur)
        {
            ++pt.y;
            pt.x = reflect ? sf.w() - 1 : 0;
            ++cur;
        }
        else
        // 0x7F - count data
        if(0x80 > *cur)
        {
            c = *cur;
            ++cur;
            while(c-- && cur < max)
            {
                sf.DrawPoint(pt, AGG::GetPaletteColor(*cur));
                reflect ? pt.x-- : pt.x++;
                ++cur;
            }
        }
        else
        // 0x80 - end data
        if(0x80 == *cur)
        {
            break;
        }
        else
        // 0xBF - skip data
        if(0xC0 > *cur)
        {
            reflect ? pt.x -= *cur - 0x80 : pt.x += *cur - 0x80;
            ++cur;
        }
        else
        // 0xC0 - shadow
        if(0xC0 == *cur)
        {
            ++cur;
            c = *cur % 4 ? *cur % 4 : *(++cur);

            if(SkipLocalAlpha(icn) || sf.depth() == 8)
            {
                while(c--){ reflect ? pt.x-- : pt.x++; }
            }
            else
            {
                while(c--){ sf.DrawPoint(pt, shadow); reflect ? pt.x-- : pt.x++; }
            }

            ++cur;
        }
        else
        // 0xC1
        if(0xC1 == *cur)
        {
            ++cur;
            c = *cur;
            ++cur;
            while(c--){ sf.DrawPoint(pt, AGG::GetPaletteColor(*cur)); reflect ? pt.x-- : pt.x++; }
            ++cur;
        }
        else
        {
            c = *cur - 0xC0;
            ++cur;
            while(c--){ sf.DrawPoint(pt, AGG::GetPaletteColor(*cur)); reflect ? pt.x-- : pt.x++; }
            ++cur;
        }

        if(cur >= max)
        {
            DEBUG(DBG_ENGINE, DBG_WARN, "out of range: " << cur - max);
            break;
        }
    }

    return Sprite(sf, header.offsetX, header.offsetY);
}

void Sprite::AddonExtensionModify(Sprite & sp, int icn, int index)
{
    switch(icn)
    {
	case ICN::AELEM:
	    if(sp.w() > 3 && sp.h() > 3)
	    {
		sp.RenderContour(RGBA(0, 0x84, 0xe0)).Blit(-1, -1, sp);
	    }
	    break;

	default: break;
    }
}

Surface Sprite::ScaleQVGASurface(const Surface & src)
{
    s32 w = src.w() / 2;
    s32 h = src.h() / 2;
    return src.RenderScale(Size((w ? w : 1), (h ? h : 1)));
}

Sprite Sprite::ScaleQVGASprite(const Sprite & sp)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    Sprite res;

    if(sp.w() > 3 && sp.h() > 3)
    {
	int theme = 0;
	if(cursor.isVisible() && Cursor::WAIT != cursor.Themes())
	{
	    theme = cursor.Themes();
	    cursor.SetThemes(Cursor::WAIT);
	    cursor.Show();
	    display.Flip();
	}

	res.SetSurface(ScaleQVGASurface(sp));

	if(theme)
	{
	    cursor.SetThemes(theme);
	    cursor.Show();
	    display.Flip();
	}
    }

    const Point pt = sp.GetPos();
    res.SetPos(Point(pt.x / 2, pt.y / 2));

    return res;
}

void Sprite::ChangeColorIndex(u32 fc, u32 tc)
{
    SetSurface(RenderChangeColor(AGG::GetPaletteColor(fc), AGG::GetPaletteColor(tc)));
}

void Sprite::Blit(void) const
{
    Blit(Display::Get());
}

void Sprite::Blit(s32 dx, s32 dy) const
{
    Blit(Point(dx, dy), Display::Get());
}

void Sprite::Blit(const Point & dpt) const
{
    Blit(Rect(Point(0, 0), GetSize()), dpt, Display::Get());
}

void Sprite::Blit(const Rect & srt, s32 dx, s32 dy) const
{
    Blit(srt, Point(dx, dy), Display::Get());
}

void Sprite::Blit(const Rect & srt, const Point & dpt) const
{
    Blit(srt, dpt, Display::Get());
}
