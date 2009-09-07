/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifdef WITH_NET

#include "localevent.h"
#include "client.h"

FH2Client::FH2Client()
{
}

bool FH2Client::IsConnected(void) const
{
    return Modes(ST_CONNECT) && sd;
}

bool FH2Client::Wait(Network::Message & packet, u16 id, bool debug)
{
    while(LocalEvent::Get().HandleEvents())
    {
        if(Ready())
        {
            if(!packet.Recv(*this))
            {
                Close();
                if(debug) std::cerr << "error" << std::endl;
                return false;
            }
            if(id == packet.GetID()) break;
        }
	DELAY(10);
    }
    packet.Dump();
    return true;
}

bool FH2Client::Send(Network::Message & packet, bool debug)
{
    std::cerr << "Send: ";
    if(!packet.Send(*this))
    {
	packet.Dump();
        Close();
        if(debug) std::cerr << "error" << std::endl;
        return false;
    }
    packet.Dump();
    return true;
}

bool FH2Client::Recv(Network::Message & packet, bool debug)
{
    std::cerr << "Recv: ";
    if(!packet.Recv(*this, debug))
    {
	packet.Dump();
        Close();
        if(debug) std::cerr << "error" << std::endl;
        return false;
    }
    packet.Dump();
    return true;
}

#endif
