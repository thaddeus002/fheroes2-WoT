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
#ifndef H2BUTTON_H
#define H2BUTTON_H

#include "icn.h"
#include "gamedefs.h"

class Button : public Rect
{
public:
    Button();
    Button(const Point &pt, const ICN::icn_t icn, u16 index1, u16 index2);
    Button(u16 ox, u16 oy, const ICN::icn_t icn, u16 index1, u16 index2);

    bool isEnable(void) const{ return !disable; };
    bool isDisable(void) const{ return disable; };
    bool isPressed(void) const{ return pressed; };
    bool isReleased(void) const{ return !pressed; };

    void Press(void);
    void Release(void);

    void SetPos(const Point & pt);
    void SetPos(const u16 ox, const u16 oy);
    void SetSprite(const ICN::icn_t icn, const u16 index1, const u16 index2);
    void SetDisable(bool fl){ disable = fl; pressed = fl; };

    void Draw(void);
    void PressDraw(void);
    void ReleaseDraw(void);

private:
    ICN::icn_t	_icn;
    u16		_index1;
    u16		_index2;

    bool pressed;
    bool disable;
};

#endif
