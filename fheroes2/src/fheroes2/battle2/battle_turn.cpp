/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "game.h"
#include "settings.h"
#include "battle_arena.h"
#include "battle_stats.h"
#include "battle_interface.h"

void Battle2::Arena::RemoteTurn(const Stats & b, Actions & a)
{
#ifdef WITH_NET
    FH2LocalClient & client = FH2LocalClient::Get();

    // localclient part
    if(client.IsConnected())
    {
	HumanTurn(b, a);

	// send turn
        while(a.size())
        {
            client.Send(a.front());
            a.pop_front();
	}
    }
    // remoteclient part
    else
    {
    }
#endif
}
