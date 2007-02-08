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

class Background : public Surface
{
public:
    Background(const Rect &rt = Rect()) : Surface(), pos(rt){};
    Background(const Point &pt, u16 w, u16 h) : Surface(), pos(pt, w, h){};

    void Save(void);
    void Save(s16 ax, s16 ay){ pos.x = ax; pos.y = ay; Save(); };
    void Save(const Point &pt){ pos = pt; Save(); };
    void Save(const Rect &rt){ pos = rt; Save(); };

    void Restore(void){ display.Blit(*this, pos); };
    const Point & GetPos(void) const{ return pos; };
    const Rect & GetRect(void) const{ return pos; };

private:
    Rect pos;
};

#endif
