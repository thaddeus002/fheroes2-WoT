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

Button::Button(const Sprite &s1, const Sprite &s2) : sprite1(s1), sprite2(s2), pressed(false)
{
    x = sprite1.x();
    y = sprite1.y();
    w = sprite1.w();
    h = sprite1.h();
    
    display.Blit(sprite1, x, y);
}

Button::Button(const std::string &icn, u16 index1, u16 index2) 
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false)
{
    x = sprite1.x();
    y = sprite1.y();
    w = sprite1.w();
    h = sprite1.h();
    
    display.Blit(sprite1, x, y);
}

Button::Button(const Point &pt, const std::string &icn, u16 index1, u16 index2)
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false)
{
    x = pt.x;
    y = pt.y;
    w = sprite1.w();
    h = sprite1.h();
    
    display.Blit(sprite1, pt);
}

Button::Button(u16 ox, u16 oy, const std::string &icn, u16 index1, u16 index2)
    : sprite1(AGG::GetICN(icn, index1)), sprite2(AGG::GetICN(icn, index2)), pressed(false)
{
    x = ox;
    y = oy;
    w = sprite1.w();
    h = sprite1.h();
    
    display.Blit(sprite1, x, y);
}

void Button::DrawPressButton(void)
{
    if(pressed) return;

    bool localcursor = false;
    if(*this & Cursor::GetRect() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }

    display.Blit(sprite2, x, y);

    if(localcursor) Cursor::Show();

    display.Flip();
    
    pressed = true;
}

void Button::DrawReleaseButton(void)
{
    if(!pressed) return;

    bool localcursor = false;
    if(*this & Cursor::GetRect() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }

    display.Blit(sprite1,  x, y);

    if(localcursor) Cursor::Show();

    display.Flip();

    pressed = false;
}
