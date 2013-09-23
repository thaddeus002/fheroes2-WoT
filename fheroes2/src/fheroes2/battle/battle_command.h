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

#ifndef H2BATTLE_COMMAND_H
#define H2BATTLE_COMMAND_H

#include "serialize.h"
#include "gamedefs.h"
#include "battle_board.h"

namespace Battle
{
    enum
    {
	MSG_BATTLE_RAW,

	MSG_BATTLE_BOARD,
	MSG_BATTLE_MOVE, 
	MSG_BATTLE_ATTACK,
	MSG_BATTLE_DEFENSE,
	MSG_BATTLE_DAMAGE,
	MSG_BATTLE_CAST,
	MSG_BATTLE_MORALE,
	MSG_BATTLE_LUCK,
	MSG_BATTLE_CATAPULT,
	MSG_BATTLE_TOWER,
	MSG_BATTLE_RETREAT,
	MSG_BATTLE_SURRENDER,
	MSG_BATTLE_SKIP,
	MSG_BATTLE_END_TURN,
	MSG_BATTLE_TURN,
	MSG_BATTLE_RESULT,
	MSG_BATTLE_AUTO,

	MSG_UNKNOWN
    };

    class Command : protected StreamBuf
    {
    public:
        Command(u32);
        Command(const Command &);
        explicit Command(const StreamBuf &);

        Command(u32 cmd, u32 param1, s32 param2, const Indexes &);
        Command(u32 cmd, u32 param1, s32 param2 = -1, s32 param3 = -1, s32 param4 = -1);

        Command & 	operator= (const Command &);

        u32		GetType(void) const { return type; }
        bool		isType(u32 msg) const { return type == msg; }
	StreamBuf &	GetStream(void) { return *this; }

    protected:
        u32     	type;
    };
}

#endif
