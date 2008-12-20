/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
 *   <afletdinov@mail.dc.baikal.ru>                                        *
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

#ifndef H2ARENATILES_H
#define H2ARENATILES_H

#include "bitmodes.h"

class BattleArena;

class ArenaTiles : public BitModes
{
public:
    enum
    {
	// army1 troop
	A1TROOP1	= 0x00000001,
	A1TROOP2	= 0x00000002,
	A1TROOP3	= 0x00000003,
	A1TROOP4	= 0x00000004,
	A1TROOP5	= 0x00000005,

	// army2 troop
	A2TROOP1	= 0x00000006,
	A2TROOP2	= 0x00000007,
	A2TROOP3	= 0x00000008,
	A2TROOP4	= 0x00000009,
	A2TROOP5	= 0x0000000A,

	// tall object (need redraw)
	COBJ0002	= 0x0000000B,
	COBJ0003	= 0x0000000C,
	COBJ0005	= 0x0000000D,
	COBJ0008	= 0x0000000E,
	COBJ0009	= 0x0000000F,
	COBJ0013	= 0x00000010,
	COBJ0021	= 0x00000011,
	COBJ0022	= 0x00000012,
	COBJ0026	= 0x00000013,
	COBJ0027	= 0x00000014,
	COBJ0028	= 0x00000015,

	// other object
	INPASSABLE	= 0x0000001F,

	SHOWGRID	= 0x00020000,
	SHADOWMV	= 0x00040000,
	SHADOWCR	= 0x00080000,

    } flags_t;

    ArenaTiles(BattleArena &, u8);

    bool	isPassable(void) const;

private:
    const BattleArena & arena;
    const u8 index;
};

#endif
