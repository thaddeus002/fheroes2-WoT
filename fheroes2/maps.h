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
#ifndef H2MAPS_H
#define H2MAPS_H

#include <vector>
#include <string>
#include "rect.h"
#include "resource.h"
#include "artifact.h"
#include "sprite.h"
#include "gamedefs.h"

#define TILEWIDTH 32

namespace Maps
{
    typedef enum {
        DESERT	= 0x0001,
        SNOW	= 0x0002,
        SWAMP	= 0x0004,
        WASTELAND = 0x0008,
        BEACH	= 0x0010,
        LAVA	= 0x0020,
        DIRT	= 0x0040,
        GRASS	= 0x0080,
        WATER	= 0x0100,
        ROAD	= 0x0200,
    } ground_t;

    typedef enum {
	NONE	= 0,
        SMALL	= 36,
        MEDIUM	= 72,
        LARGE	= 108,
        XLARGE	= 144
    } mapsize_t;

    typedef struct {
	Point    coord;
	//Point  center;
	ground_t ground;
	u8       object;
	std::vector<const Sprite *> *level1;
	std::vector<const Sprite *> *level2;

	union {
	    Resource::resource_t resource;
	    Artifact::artifact_t artifact;
	};

    } tiles_t;
};

#endif
