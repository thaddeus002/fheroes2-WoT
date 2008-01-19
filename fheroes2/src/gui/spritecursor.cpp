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

#include "gamedefs.h"
#include "display.h"
#include "spritecursor.h"

void SpriteCursor::Move(const Point &pt)
{
    Move(pt.x, pt.y);
}

void SpriteCursor::Move(const Rect &rt)
{
    if(!rt.valid() || background.GetRect() == rt) return;

    background.Restore();
    background.Save(rt);

    display.Blit(spriteCursor, rt);
}

void SpriteCursor::Move(s16 ax, s16 ay)
{
    if(ax < 0 || ay < 0) return;
    
    if(background.GetRect().x == ax && background.GetRect().x == ay) return;

    background.Restore();
    background.Save(ax, ay);

    display.Blit(spriteCursor, ax, ay);
}

void SpriteCursor::Hide(void)
{
    background.Restore();
}

void SpriteCursor::Redraw(void)
{
    background.Restore();
    background.Save();

    display.Blit(spriteCursor, background.GetRect().x, background.GetRect().y);
}

void SpriteCursor::Show(void)
{
    Show(background.GetRect().x, background.GetRect().y);
}

void SpriteCursor::Show(const Point &pt)
{
    Show(pt.x, pt.y);
}

void SpriteCursor::Show(s16 ax, s16 ay)
{
    background.Save(ax, ay);

    display.Blit(spriteCursor, background.GetRect().x, background.GetRect().y);
}
