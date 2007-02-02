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

#include "gamedefs.h"
#include "mapsdata.h"
#include "spritecursor.h"
#include "gamearea.h"
#include "rect.h"

class Radar : private SDLmm::Surface
{
public:
    Radar(s16 rx, s16 ry, const MapsData & mp);

    const Rect & GetRect(void) const{ return area; }
    
    void Redraw(void);

private:
    void GenerateFrom(const SDLmm::Surface &surface);
    const MapsData & maps;
    Rect area;
};

class RadarCursor : private Rect, SpriteCursor
{
public:
    RadarCursor(const Radar &radar, const MapsData &mp, const GameArea &ga);

    void Redraw(void);

private:
    const MapsData &maps;
    const GameArea &area;
};

#endif
