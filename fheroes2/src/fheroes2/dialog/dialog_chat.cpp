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

#include "dialog.h"

#define MAX_SPOOL	100

Dialog::NetworkChat::NetworkChat() : modify(true)
{
}

Dialog::NetworkChat::~NetworkChat()
{
}

Dialog::NetworkChat & Dialog::NetworkChat::Get()
{
    static Dialog::NetworkChat NetworkChat;
    
    return NetworkChat;
}

bool Dialog::NetworkChat::MessageReady(void)
{
    return msg.size();
}

const std::string & Dialog::NetworkChat::GetMessage(void) const
{
    return msg;
}

void Dialog::NetworkChat::ResetMessage(void)
{
    msg.clear();
}

void Dialog::NetworkChat::PushMessage(const std::string & str)
{
    spool.push_back(str);
    if(MAX_SPOOL < str.size()) spool.pop_back();
    modify = true;
}

void Dialog::NetworkChat::Hide(void) const
{
}

void Dialog::NetworkChat::Show(void) const
{
    // ICN::SURDRBKG
    // ICN::SURDRBKE
}
