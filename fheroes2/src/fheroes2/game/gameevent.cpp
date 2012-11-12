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

#include <algorithm>
#include "color.h"
#include "dialog.h"
#include "settings.h"
#include "gameevent.h"

#define SIZEMESSAGE 400

EventDate::EventDate(const void *ptr)
{
    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x00 != *ptr8)
    {
	DEBUG(DBG_GAME , DBG_WARN, "unknown id");
	return;
    }
    ++ptr8;

    // resource
    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.wood = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.mercury = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.ore = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.sulfur = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.crystal = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gems = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gold = byte32;

    // skip artifact
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;

    // allow computer
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;
    computer = byte16;

    // day of first occurent
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;
    first = byte16;

    // subsequent occurrences
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;
    subsequent = byte16;

    ptr8 += 6;

    colors = 0;

    // blue
    if(*ptr8) colors |= Color::BLUE;
    ++ptr8;

    // green
    if(*ptr8) colors |= Color::GREEN;
    ++ptr8;

    // red
    if(*ptr8) colors |= Color::RED;
    ++ptr8;

    // yellow
    if(*ptr8) colors |= Color::YELLOW;
    ++ptr8;

    // orange
    if(*ptr8) colors |= Color::ORANGE;
    ++ptr8;

    // purple
    if(*ptr8) colors |= Color::PURPLE;
    ++ptr8;

    // message
    message = Game::GetEncodeString(reinterpret_cast<const char *>(ptr8));

    //if(SIZEMESSAGE < message.size()) DEBUG(DBG_GAME , DBG_WARN, "long message, incorrect block?");

    DEBUG(DBG_GAME, DBG_INFO, "add: " << message);
}

bool EventDate::isDeprecated(u16 date) const
{
    return 0 == subsequent && first < date;
}

bool EventDate::isAllow(u8 col, u16 date) const
{
    return ((first == date ||
	    (subsequent && (first < date && 0 == ((date - first) % subsequent)))) &&
	    (col & colors));
}

EventMaps::EventMaps(s32 index, const void *ptr)
{
    SetIndex(index);

    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x01 != *ptr8)
    {
	DEBUG(DBG_GAME, DBG_WARN, "unknown id");
	return;
    }
    ++ptr8;

    // resource
    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.wood = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.mercury = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.ore = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.sulfur = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.crystal = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gems = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gold = byte32;

    // artifact
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;
    artifact = byte16;

    // allow computer
    computer = *ptr8;
    ++ptr8;

    // cancel event after first visit
    cancel = *ptr8;
    ptr8 += 11;

    colors = 0;

    // blue
    if(*ptr8) colors |= Color::BLUE;
    ++ptr8;

    // green
    if(*ptr8) colors |= Color::GREEN;
    ++ptr8;

    // red
    if(*ptr8) colors |= Color::RED;
    ++ptr8;

    // yellow
    if(*ptr8) colors |= Color::YELLOW;
    ++ptr8;

    // orange
    if(*ptr8) colors |= Color::ORANGE;
    ++ptr8;

    // purple
    if(*ptr8) colors |= Color::PURPLE;
    ++ptr8;

    // message
    message = Game::GetEncodeString(reinterpret_cast<const char *>(ptr8));

    DEBUG(DBG_GAME , DBG_INFO, "add: " << message);
}

void EventMaps::SetVisited(u8 color)
{
    if(cancel)
	colors = 0;
    else
	colors &= ~color;
}

bool EventMaps::isAllow(u8 col, s32 ii) const
{
    return ii == GetIndex() && (col & colors);
}

Riddle::Riddle(s32 index, const void *ptr) : valid(false)
{
    SetIndex(index);

    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x00 != *ptr8)
    {
	DEBUG(DBG_GAME , DBG_WARN, "unknown id");
	return;
    }
    ++ptr8;

    // resource
    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.wood = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.mercury = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.ore = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.sulfur = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.crystal = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gems = byte32;

    byte32 = ReadLE32(ptr8);
    ptr8 += 4;;
    resource.gold = byte32;

    // artifact
    byte16 = ReadLE16(ptr8);
    ++ptr8;
    ++ptr8;
    artifact = byte16;

    // count answers
    u8 count = *ptr8;
    ++ptr8;

    // answers
    for(u8 i = 0; i < 8; ++i)
    {
	std::string str = StringLower(reinterpret_cast<const char *>(ptr8));

	if(count-- && str.size())
	{
	    answers.push_back(Game::GetEncodeString(str.c_str()));
	    answers.push_back(Game::GetEncodeString(reinterpret_cast<const char *>(ptr8)));
	};
	ptr8 += 13;
    }

    // message
    message = Game::GetEncodeString(reinterpret_cast<const char *>(ptr8));

    valid = true;

    DEBUG(DBG_GAME, DBG_INFO, "add: " << message);
}

bool Riddle::AnswerCorrect(const std::string & answer)
{
    return answers.end() != std::find(answers.begin(), answers.end(), answer);
}

void Riddle::SetQuiet(void)
{
    valid = false;
    artifact = Artifact::UNKNOWN;
    resource.Reset();
}

StreamBase & operator<< (StreamBase & msg, const EventDate & obj)
{
    return msg <<
	obj.resource <<
	obj.computer <<
	obj.first <<
	obj.subsequent <<
	obj.colors <<
	obj.message;
}

StreamBase & operator>> (StreamBase & msg, EventDate & obj)
{
    return msg >>
	obj.resource >>
	obj.computer >>
	obj.first >>
	obj.subsequent >>
	obj.colors >>
	obj.message;
}


StreamBase & operator<< (StreamBase & msg, const EventMaps & obj)
{
    return msg <<
	obj.center <<
	obj.resource <<
	obj.artifact <<
	obj.computer <<
	obj.cancel <<
	obj.colors <<
	obj.message;
}

StreamBase & operator>> (StreamBase & msg, EventMaps & obj)
{
    return msg >>
	obj.center >>
	obj.resource >>
	obj.artifact >>
	obj.computer >>
	obj.cancel >>
	obj.colors >>
	obj.message;
}

StreamBase & operator<< (StreamBase & msg, const Riddle & obj)
{
    return msg <<
	obj.center <<
	obj.resource <<
	obj.artifact <<
	obj.answers <<
	obj.message <<
	obj.valid;
}

StreamBase & operator>> (StreamBase & msg, Riddle & obj)
{
    return msg >>
	obj.center >>
	obj.resource >>
	obj.artifact >>
	obj.answers >>
	obj.message >>
	obj.valid;
}
