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

#ifdef WITH_EDITOR

#include "maps.h"
#include "cursor.h"
#include "editor_gui.h"

SizeCursor::SizeCursor(u8 sw, u8 sh)
{
    ModifyCursor(sw * TILEWIDTH, sh * TILEWIDTH);
}

u8 SizeCursor::w(void)
{
    return Surface::w() / TILEWIDTH;
}

u8 SizeCursor::h(void)
{
    return Surface::h() / TILEWIDTH;
}

void SizeCursor::ModifySize(const Size & sz)
{
    ModifySize(sz.w, sz.h);
}

void SizeCursor::ModifySize(const u8 w, const u8 h)
{
    ModifyCursor(w * TILEWIDTH, h * TILEWIDTH);
}

void SizeCursor::ModifyCursor(u16 w, u16 h)
{
    if(Surface::w() != w || Surface::h() != h)
    {
	Set(w, h);
	Cursor::DrawCursor(*this, 0x40);
    }
}

#endif
