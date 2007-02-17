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
#ifndef H2MAPSTILES_H
#define H2MAPSTILES_H

#include <vector>
#include "gamedefs.h"
#include "rect.h"
#include "mp2.h"
#include "sprite.h"

typedef struct {
    Point	coord;
    //Point	center;
    u16		ground;
    u8		object;
    std::vector<const Sprite *> *level1;
    std::vector<const Sprite *> *level2;
} mapstiles_t;

/* class Tiles */
class MapsTiles
{
public:
    MapsTiles(){};

    u16 GetGrounds(void) const{ return ground; };
    bool Move(void);

private:
    Point	area;
    Point	center;
    u16		ground;
    u8		object;
};

#endif
