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
#ifndef H2BACKGROUND_H
#define H2BACKGROUND_H

#include "rect.h"
#include "display.h"
#include "surface.h"

class Background : private Surface
{
public:
    Background(const Rect &rback = Rect()) : Surface(), back(rback){};

    void Save(const Rect &rback){ back = rback; Save(); };
    void Save(void);
    void Restore(void){ display.Blit(*this, back.x, back.y); };
    const Rect & GetRect(void) const{ return back; };

private:
    Rect back;
};

#endif
