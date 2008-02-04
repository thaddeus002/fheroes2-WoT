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
#include "cursor.h"
#include "display.h"
#include "button.h"

Button::Button(const Sprite &s1, const Sprite &s2) : sprite1(s1), sprite2(s2), pressed(false), disable(false)
{
    x = sprite1.x();
    y = sprite1.y();
    w = sprite1.w();
    h = sprite1.h();
}

Button::Button(const std::string &icn, u16 index1, u16 index2) 
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false), disable(false)
{
    x = sprite1.x();
    y = sprite1.y();
    w = sprite1.w();
    h = sprite1.h();
}

Button::Button(const Point &pt, const std::string &icn, u16 index1, u16 index2)
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false), disable(false)
{
    x = pt.x;
    y = pt.y;
    w = sprite1.w();
    h = sprite1.h();
}

Button::Button(u16 ox, u16 oy, const std::string &icn, u16 index1, u16 index2)
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false), disable(false)
{
    x = ox;
    y = oy;
    w = sprite1.w();
    h = sprite1.h();
}

void Button::Press(void)
{
    if(disable || pressed) return;

    pressed = true;
}

void Button::Release(void)
{
    if(disable || !pressed) return;

    pressed = false;
}

void Button::PressDraw(void)
{
    if(disable || pressed) return;

    Press();

    Draw();

    Display::Get().Flip();
}

void Button::ReleaseDraw(void)
{
    if(disable || !pressed) return;

    Release();

    Draw();

    Display::Get().Flip();
}

void Button::Draw(void)
{
    bool localcursor = false;
    Cursor & cursor = Cursor::Get();
    if(*this & cursor.GetRect() && cursor.isVisible()){ cursor.Hide(); localcursor = true; }

    Display::Get().Blit(pressed ? sprite2 : sprite1, x, y);

    if(localcursor) cursor.Show();
}
