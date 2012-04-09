/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2BATTLE_BOARD_H
#define H2BATTLE_BOARD_H

#include <functional>

#include "battle.h"
#include "battle_cell.h"

#define ARENAW 11
#define ARENAH 9
#define ARENASIZE ARENAW * ARENAH

namespace Battle
{
    inline direction_t & operator++ (direction_t & d){ return d = ( CENTER == d ? TOP_LEFT : direction_t(d << 1)); }
    inline direction_t & operator-- (direction_t & d){ return d = ( TOP_LEFT == d ? CENTER : direction_t(d >> 1)); }

    typedef std::vector<s16> Indexes;

    struct Board : public std::vector<Cell>
    {
	Board();

	void		Reset(void);

	Rect		GetArea(void) const;
	void		SetArea(const Rect &);

	s16		GetIndexAbsPosition(const Point &) const;
	Indexes		GetPassableQualityPositions(const Unit & b);
	Indexes		GetNearestTroopIndexes(s16, const Indexes*) const;
	Indexes		GetAStarPath(const Unit &, const Position &, bool debug = true);
	std::string	AllUnitsInfo(void) const;

	void		SetEnemyQuality(const Unit &);
	void		SetPositionQuality(const Unit &);
	void		SetScanPassability(const Unit &);

	void		SetCobjObjects(const Maps::Tiles &);
	void		SetCobjObject(u16 icn, s16);
	void		SetCovrObjects(u16 icn);

	static Cell*		GetCell(s16, direction_t = CENTER);
	static bool		isNearIndexes(s16, s16);
	static bool		isValidIndex(s16);
	static bool		isCastleIndex(s16);
	static bool		isMoatIndex(s16);
	static bool		isBridgeIndex(s16);
	static bool		isImpassableIndex(s16);
	static bool		isOutOfWallsIndex(s16);
	static bool		isReflectDirection(u8);
	static direction_t	GetReflectDirection(u8);
	static direction_t	GetDirection(s16, s16);
	static u16		GetDistance(s16, s16);
	static bool		isValidDirection(s16, u8);
	static s16		GetIndexDirection(s16, u8);
        static Indexes		GetDistanceIndexes(s16, u8);
        static Indexes		GetAroundIndexes(s16);
	static Indexes		GetAroundIndexes(const Unit &);
	static Indexes		GetMoveWideIndexes(s16, bool reflect);
	static bool		isValidMirrorImageIndex(s16, const Unit*);
    };

    struct ShortestDistance : public std::binary_function <s16, s16, bool>
    {
	ShortestDistance(s16 index) : center(index) {}

	bool operator() (s16 index1, s16 index2) const
	{
	    return Board::GetDistance(center, index1) < Board::GetDistance(center, index2);
	}

	s16 center;
    };
}

#endif
