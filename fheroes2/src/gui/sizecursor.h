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
#ifndef H2SIZECURSOR_H
#define H2SIZECURSOR_H

#include "gamedefs.h"

class Size;

class SizeCursor
{
public:
    SizeCursor();
    ~SizeCursor();

    void ModifySize(const u8 w, const u8 h);
    void ModifySize(const Size & sz);

    void Hide(void);
    void Show(void);

    bool isHide(void){ return hide; };

    u8 w(void);
    u8 h(void);

    void Move(const u16 px, const u16 py);
    void Move(const Point & pt);

private:
    void ModifyCursor(const u8 w, const u8 h);

    Surface		*sf;
    SpriteCursor	*sc;
    
    bool		hide;
};

#endif
