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

#include <algorithm>
#include "settings.h"
#include "spell.h"
#include "battle_arena.h"
#include "battle_command.h"

bool Battle::Actions::HaveCommand(u16 cmd) const
{
    return end() != std::find_if(begin(), end(), std::bind2nd(std::mem_fun_ref(&Command::isType), cmd));
}

Battle::Command::Command(u16 msg) : StreamBuf(16), type(msg)
{
    *this << type;
}

Battle::Command & Battle::Command::operator= (const Command & cmd)
{
    StreamBuf & stream = *this;
    stream = cmd;
    type = cmd.type;

    return *this;
}

Battle::Command::Command(const Command & cmd) : StreamBuf(cmd), type(cmd.type)
{
}

Battle::Command::Command(const StreamBuf & sb) : StreamBuf(sb), type(0)
{
    *this >> type;
}

Battle::Command::Command(u16 cmd, s32 param1, s32 param2, const Indexes & param3) : StreamBuf(16), type(cmd)
{
    StreamBase & base = *this;
    *this << type;

    switch(type)
    {
        case MSG_BATTLE_MOVE:
            base << static_cast<u32>(param1) << static_cast<s16>(param2) << static_cast<u8>(1) << param3; // uid, dst, path
            break;

        default: break;
    }
}

Battle::Command::Command(u16 cmd, s32 param1, s32 param2, s32 param3, s32 param4) : StreamBuf(16), type(cmd)
{
    StreamBase & base = *this;
    *this << type;

    switch(type)
    {
        case MSG_BATTLE_AUTO:
            base << static_cast<u8>(param1); // color
            break;

        case MSG_BATTLE_SURRENDER:
        case MSG_BATTLE_RETREAT:
            break;

        case MSG_BATTLE_TOWER:
        case MSG_BATTLE_CATAPULT: // battle_arena.cpp
            break;

        case MSG_BATTLE_CAST:
            switch(static_cast<u8>(param1))
            {
                case Spell::MIRRORIMAGE:
                    base << static_cast<u8>(param1) << static_cast<s16>(param2); // spell, who
                    break;

                case Spell::TELEPORT:
                    base << static_cast<u8>(param1) << static_cast<s16>(param2) << static_cast<s16>(param3); // spell, src, dst
                    break;

                default:
                    base << static_cast<u8>(param1) << static_cast<s16>(param2); // spell, dst
                    break;
            }
            break;

        case MSG_BATTLE_END_TURN:
            base << static_cast<u32>(param1); // uid
            break;

        case MSG_BATTLE_SKIP:
            base << static_cast<u32>(param1) << static_cast<u8>(param2); // uid, hard_skip
            break;

        case MSG_BATTLE_MOVE:
            base << static_cast<u32>(param1) << static_cast<s16>(param2) << static_cast<u8>(0); // uid, dst, path
            break;

        case MSG_BATTLE_ATTACK:
            base << static_cast<u32>(param1) << static_cast<u32>(param2) << static_cast<s16>(param3) << static_cast<u8>(param4); // uid, uid, dst, direction
            break;

        case MSG_BATTLE_MORALE:
            base << static_cast<u32>(param1) << static_cast<u8>(param2); // uid, state
            break;

        default: break;
    }
}
