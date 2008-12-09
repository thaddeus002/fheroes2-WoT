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

#include "gamedefs.h"
#include "direction.h"

#define TILEWIDTH 32

class Point;
class Castle;

namespace Maps
{

    typedef enum {
	ZERO	= 0,
        SMALL	= 36,
        MEDIUM	= 72,
        LARGE	= 108,
        XLARGE	= 144
    } mapsize_t;

    u16 GetDirectionIndex(u16 from, Direction::vector_t vector);
    bool isValidDirection(u16 from, Direction::vector_t vector);

    bool isValidAbsPoint(const Point & pt);
    bool isValidAbsPoint(const s16 x, const s16 y);

    u16 GetTopIndex(u16 from);
    u16 GetTopRightIndex(u16 from);
    u16 GetRightIndex(u16 from);
    u16 GetBottomRightIndex(u16 from);
    u16 GetBottomIndex(u16 from);
    u16 GetBottomLeftIndex(u16 from);
    u16 GetLeftIndex(u16 from);
    u16 GetTopLeftIndex(u16 from);

    u16 GetIndexFromAbsPoint(const Point & mp);
    u16 GetIndexFromAbsPoint(s16 px, s16 py);

    int GetIndexFromAreaPoint(const Point & pt);
    int GetIndexFromAreaPoint(s16 px, s16 py);

    u32 AnimationTicket(void);
    void IncreaseAnimationTicket(void);

    u16 GetDirectionAroundFog(const u16 center, u8 color = 0);
    u16 GetDirectionAroundGround(const u16 center, const u16 ground);
    u8  GetCountAroundGround(const u16 center, const u16 ground);
    u16 GetMaxGroundAround(const u16 center);

    bool ScanAroundObject(const u16 center, const u8 obj, bool full, u16 & res);
    bool ScanDistanceObject(const u16 center, const u8 obj, const u8 dist, u16 & res);

    bool TileUnderProtection(const u16 index);
    bool TileUnderProtection(const u16 index, u16 & res);

    void ClearFog(const Point & center, const u8 scoute, const u8 color);
    u16 GetApproximateDistance(const u16 index1, const u16 index2);


    void UpdateRNDSpriteForCastle(const Point & center, u8 race, bool is_castle);
    void UpdateSpritesFromTownToCastle(const Point & center);
    void MinimizeAreaForCastle(const Point & center);
};

#endif
