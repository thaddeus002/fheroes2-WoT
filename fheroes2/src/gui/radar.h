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
#ifndef H2RADAR_H
#define H2RADAR_H

#include "rect.h"

class Radar
{
public:
    static Radar & Get(void);
    ~Radar();

    const Rect & GetRect(void) const{ return pos; }

    void Build(void);
    void RedrawArea(const u8 color);
    void RedrawCursor(void);

private:
    Radar();

    void Generate(void);
    Rect pos;
    Surface *spriteArea;
    Surface *spriteCursor;
    SpriteCursor *cursor;
};

#endif
