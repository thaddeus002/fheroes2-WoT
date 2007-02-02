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

#include "cursor.h"
#include "gamedefs.h"
#include "spritecursor.h"

void SpriteCursor::Move(const Point &pt)
{
    Move(pt.x, pt.y);
}

void SpriteCursor::Move(s16 ax, s16 ay)
{
    if(ax < 0 || ay < 0) return;

    rect.x = ax;
    rect.y = ay;

    bool hide = Cursor::Visible() ? true : false;

    if(hide) Cursor::Hide();

    background.Restore();
    background.Save(rect);

    display.Blit(*this, Point(ax, ay));

    if(hide) Cursor::Show();
}

void SpriteCursor::Redraw(void)
{
    bool hide = Cursor::Visible() ? true : false;

    if(hide) Cursor::Hide();

    display.Blit(*this, Point(rect.x, rect.y));

    if(hide) Cursor::Show();
}
