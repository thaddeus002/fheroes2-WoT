/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include "gamedefs.h"
#include "direction.h"

#define TILEWIDTH 32

class Point;
class Castle;

typedef std::vector<s32> MapsIndexes;

namespace Maps
{
    enum mapsize_t
    {
	ZERO	= 0,
        SMALL	= 36,
        MEDIUM	= 72,
        LARGE	= 108,
        XLARGE	= 144,
	XLARGE2 = 252,
	XLARGE3	= 324
    };

    const char* SizeString(u16);
    const char* GetMinesName(u8);

    s32 GetDirectionIndex(const s32 & from, u16 direct);
    bool isValidDirection(const s32 & from, u16 direct);

    bool isValidAbsIndex(const s32 &);
    bool isValidAbsPoint(const Point & pt);
    bool isValidAbsPoint(s16 x, s16 y);

    s32 GetIndexFromAbsPoint(const Point & mp);
    s32 GetIndexFromAbsPoint(s16 px, s16 py);

    MapsIndexes GetAllIndexes(void);
    MapsIndexes GetAroundIndexes(const s32 &, u16 filter = DIRECTION_ALL);
    MapsIndexes GetDistanceIndexes(const s32 &, u16 dist, bool sort = false);
    MapsIndexes ScanAroundObjectV(const s32 &, u8 obj);
    MapsIndexes ScanAroundObjectsV(const s32 &, const u8* objs);
    MapsIndexes GetTilesUnderProtection(const s32 &);
    bool	TileIsUnderProtection(const s32 &);
    MapsIndexes ScanDistanceObject(const s32 &, u8 obj, u16 dist);
    MapsIndexes ScanDistanceObjects(const s32 &, const u8* objs, u16 dist);

    u16 TileIsCoast(const s32 &, u16 direct = DIRECTION_ALL);

    void ClearFog(const s32 &, u8 scoute, u8 color);
    u16 GetApproximateDistance(const s32 &, const s32 &);


    void UpdateRNDSpriteForCastle(const Point & center, u8 race, bool castle);
    void UpdateSpritesFromTownToCastle(const Point & center);
    void MinimizeAreaForCastle(const Point & center);
}

MapsIndexes & MapsIndexesFilteredObjects(MapsIndexes & indexes, const u8* objs);
MapsIndexes & MapsIndexesFilteredObject(MapsIndexes & indexes, u8 obj);

#endif
