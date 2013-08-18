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
#include "icn.h"
#include "cursor.h"
#include "display.h"
#include "sprite.h"

bool SkipLocalAlpha(u16 icn)
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

Sprite::Sprite() : offsetX(0), offsetY(0)
{
}

Sprite::Sprite(const Sprite & sp) : offsetX(sp.x()), offsetY(sp.y())
{
    Set(sp, true);
}

Sprite::Sprite(const Surface & sf, s16 ox, s16 oy) : offsetX(ox), offsetY(oy)
{
    Set(sf, true);
}

Sprite & Sprite::operator= (const Surface & sf)
{
    Set(sf, true);
    return *this;
}

Sprite & Sprite::operator= (const Sprite & sp)
{
    Set(sp, true);
    offsetX = sp.x();
    offsetY = sp.y();
    return *this;
}

void Sprite::Reset(void)
{
    offsetX = 0;
    offsetY = 0;
    Surface::Reset();
}

void Sprite::SetOffset(s16 ox, s16 oy)
{
    offsetX = ox;
    offsetY = oy;
}

void Sprite::DrawICN(int icn, const u8* cur, int size, bool reflect, Surface & sf)
{
    if(NULL == cur || 0 == size) return;

    const u8 *max = cur + size;

    u8  c = 0;
    u16 x = reflect ? sf.w() - 1 : 0;
    u16 y = 0;

    Surface sf_tmp;
    Surface* sf_cur = sf.amask() ? &sf : &sf_tmp;
    u32 shadow = sf_cur->isValid() ? sf_cur->MapRGB(0, 0, 0, 0x40) : 0;

    // lock surface
    sf.Lock();

    while(1)
    {
	// 0x00 - end line
	if(0 == *cur)
	{
	    ++y;
	    x = reflect ? sf.w() - 1 : 0;
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
		sf.SetPixel(x, y, sf.GetColorIndex(*cur));
		reflect ? x-- : x++;
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
	    reflect ? x -= *cur - 0x80 : x += *cur - 0x80;
	    ++cur;
	}
	else
	// 0xC0 - shadow
	if(0xC0 == *cur)
	{
	    ++cur;
	    c = *cur % 4 ? *cur % 4 : *(++cur);

	    if(SkipLocalAlpha(icn) || 8 == sf.depth())
	    {
		while(c--){ reflect ? x-- : x++; }
	    }
	    else
	    {
		if(! sf_cur->isValid())
		{
		    sf_cur->Set(sf.w(), sf.h(), true);
		    shadow = sf_cur->MapRGB(0, 0, 0, 0x40);
		}

		while(c--){ sf_cur->SetPixel(x, y, shadow); reflect ? x-- : x++; }
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
	    while(c--){ sf.SetPixel(x, y, sf.GetColorIndex(*cur)); reflect ? x-- : x++; }
	    ++cur;
	}
	else
	{
	    c = *cur - 0xC0;
	    ++cur;
	    while(c--){ sf.SetPixel(x, y, sf.GetColorIndex(*cur)); reflect ? x-- : x++; }
	    ++cur;
	}

	if(cur >= max)
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, "out of range: " << cur - max);
	    break;
	}
    }

    // unlock surface
    sf.Unlock();

    if(sf_tmp.isValid())
    {
	sf.Blit(sf_tmp);
	Surface::Swap(sf_tmp, sf);
    }
}

u32 Sprite::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage() + sizeof(offsetX) + sizeof(offsetY);
}

void Sprite::ScaleMinifyByTwo(void)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    if(w() > 3 && h() > 3)
    {
	u16 theme = 0;
	if(cursor.isVisible() && Cursor::WAIT != cursor.Themes())
	{
	    theme = cursor.Themes();
	    cursor.SetThemes(Cursor::WAIT);
	    cursor.Show();
	    display.Flip();
	}

	Surface mini = Surface::ScaleMinifyByTwo(*this, cursor.isVisible());
	Surface::Swap(mini, *this);

	if(theme)
	{
	    cursor.SetThemes(theme);
	    cursor.Show();
	    display.Flip();
	}
    }

    offsetX /= 2;
    offsetY /= 2;
}

void Sprite::AddonExtensionModify(Sprite & sp, int icn, int index)
{
    switch(icn)
    {
	case ICN::AELEM:
	    if(sp.w() > 3 && sp.h() > 3)
	    {
		Surface sf = Surface::Contour(sp, sp.GetColorIndex(0xEF));
		sf.Blit(-1, -1, sp);
	    }
	    break;

	default: break;
    }
}


void Sprite::Blit(Surface & dst) const
{
    Surface::Blit(dst);
}

void Sprite::Blit(int dstx, int dsty, Surface & dst) const
{
    Surface::Blit(dstx, dsty, dst);
}

void Sprite::Blit(const Point & dpt, Surface & dst) const
{
    Surface::Blit(dpt, dst);
}

void Sprite::Blit(const Rect & srt, int dstx, int dsty, Surface & dst) const
{
    Surface::Blit(srt, dstx, dsty, dst);
}

void Sprite::Blit(const Rect & srt, const Point & dpt, Surface & dst) const
{
    Surface::Blit(srt, dpt, dst);
}

void Sprite::Blit(int alpha, int dstx, int dsty, Surface & dst) const
{
    Surface::Blit(alpha, dstx, dsty, dst);
}

void Sprite::Blit(int alpha, const Rect & srt, const Point & dpt, Surface & dst) const
{
    Surface::Blit(alpha, srt, dpt, dst);
}
