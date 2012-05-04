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

struct Point;
class Castle;
class IndexDistance;

class MapsIndexes : public std::vector<s32>
{
};


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

    typedef MapsIndexes Indexes;

    class IndexesDistance : public std::vector<IndexDistance>
    {
	void Assign(const s32 & from, const Indexes &, u8 sort /* 0: none, 1: short first, 2: long first */);

    public:
	IndexesDistance(const s32 &, const Indexes &, u8 sort = 0);
	IndexesDistance(const s32 &, const s32 &, u16 dist, u8 sort = 0);
    };

    const char* SizeString(u16);
    const char* GetMinesName(u8);

    s32 GetDirectionIndex(const s32 & from, u16 direct);
    bool isValidDirection(const s32 & from, u16 direct);

    bool isValidAbsIndex(const s32 &);
    bool isValidAbsPoint(const Point & pt);
    bool isValidAbsPoint(s16 x, s16 y);

    Point GetPoint(const s32 &);

    s32 GetIndexFromAbsPoint(const Point & mp);
    s32 GetIndexFromAbsPoint(s16 px, s16 py);

    Indexes GetAllIndexes(void);
    Indexes GetAroundIndexes(const s32 &);
    Indexes GetAroundIndexes(const s32 &, u16 dist, bool sort = false); // sorting distance
    Indexes GetDistanceIndexes(const s32 & center, u16 dist);

    Indexes ScanAroundObject(const s32 &, u8 obj);
    Indexes ScanAroundObjects(const s32 &, const u8* objs);
    Indexes ScanAroundObject(const s32 &, u16 dist, u8 obj);
    Indexes ScanAroundObjects(const s32 &, u16 dist, const u8* objs);

    Indexes	GetTilesUnderProtection(const s32 &);
    bool	TileIsUnderProtection(const s32 &);
    bool	IsNearTiles(const s32 &, const s32 &);

    Indexes GetObjectPositions(u8 obj, bool check_hero);
    Indexes GetObjectPositions(const s32 &, u8 obj, bool check_hero);
    Indexes GetObjectsPositions(const u8* objs);

    u16 TileIsCoast(const s32 &, u16 direct = DIRECTION_ALL);

    void ClearFog(const s32 &, u8 scoute, u8 color);
    u16 GetApproximateDistance(const s32 &, const s32 &);


    void UpdateRNDSpriteForCastle(const Point & center, u8 race, bool castle);
    void UpdateSpritesFromTownToCastle(const Point & center);
    void MinimizeAreaForCastle(const Point & center);
}

#endif
