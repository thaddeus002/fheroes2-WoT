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

#include "rect.h"
#include "spritecursor.h"
#include "error.h"
#include "maps.h"
#include "radar.h"
#include "gamearea.h"
#include "sizecursor.h"

SizeCursor::SizeCursor(u8 sw, u8 sh) : SpriteCursor(), cursor()
{
    ModifyCursor(sw, sh);
}

u8 SizeCursor::w(void)
{
    return cursor.w() / TILEWIDTH;
}

u8 SizeCursor::h(void)
{
    return cursor.h() / TILEWIDTH;
}

void SizeCursor::ModifySize(const Size & sz)
{
    ModifySize(sz.w, sz.h);
}

void SizeCursor::ModifySize(const u8 w, const u8 h)
{
    if(w > GameArea::GetRect().w || h > GameArea::GetRect().h)
    {
    	Error::Warning("SizeCursor::SizeCursor: size out of range.");
	ModifyCursor(1, 1);
    }
    else
	ModifyCursor(w, h);
}

void SizeCursor::ModifyCursor(const u8 w, const u8 h)
{
    if(cursor.w() == w && cursor.h() == h) return;

    cursor = Surface(w * TILEWIDTH, h * TILEWIDTH);

    cursor.SetColorKey();

    Radar::DrawCursor(cursor);

    SetSprite(cursor);
}
