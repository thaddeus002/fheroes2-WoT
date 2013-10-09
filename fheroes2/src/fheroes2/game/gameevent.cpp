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
#include "game.h"
#include "gameevent.h"

#define SIZEMESSAGE 400

EventDate::EventDate(const u8* ptr, size_t sz)
{
    StreamBuf st(ptr, sz);

    // id
    if(0 == st.get())
    {
	// resource
	resource.wood = st.getLE32();
	resource.mercury = st.getLE32();
	resource.ore = st.getLE32();
	resource.sulfur = st.getLE32();
	resource.crystal = st.getLE32();
	resource.gems = st.getLE32();
	resource.gold = st.getLE32();

	st.skip(2);

	// allow computer
	computer = st.getLE16();

	// day of first occurent
	first = st.getLE16();

	// subsequent occurrences
	subsequent = st.getLE16();

	st.skip(6);

	colors = 0;
	// blue
	if(st.get()) colors |= Color::BLUE;
	// green
	if(st.get()) colors |= Color::GREEN;
	// red
	if(st.get()) colors |= Color::RED;
	// yellow
	if(st.get()) colors |= Color::YELLOW;
	// orange
	if(st.get()) colors |= Color::ORANGE;
	// purple
	if(st.get()) colors |= Color::PURPLE;

	// message
	message = Game::GetEncodeString(GetString(st.getRaw()));
	DEBUG(DBG_GAME, DBG_INFO, "add: " << message);
    }
    else
	DEBUG(DBG_GAME , DBG_WARN, "unknown id");
}

bool EventDate::isDeprecated(u32 date) const
{
    return 0 == subsequent && first < date;
}

bool EventDate::isAllow(int col, u32 date) const
{
    return ((first == date ||
	    (subsequent && (first < date && 0 == ((date - first) % subsequent)))) &&
	    (col & colors));
}

EventMaps::EventMaps(s32 index, const u8* ptr, const size_t sz)
{
    StreamBuf st(ptr, sz);

    // id
    if(1 == st.get())
    {
	SetIndex(index);

	// resource
	resource.wood = st.getLE32();
	resource.mercury = st.getLE32();
	resource.ore = st.getLE32();
	resource.sulfur = st.getLE32();
	resource.crystal = st.getLE32();
	resource.gems = st.getLE32();
	resource.gold = st.getLE32();

	// artifact
	artifact = st.getLE16();

	// allow computer
	computer = st.get();

	// cancel event after first visit
	cancel = st.get();

	st.skip(10);

	colors = 0;
	// blue
	if(st.get()) colors |= Color::BLUE;
	// green
	if(st.get()) colors |= Color::GREEN;
	// red
	if(st.get()) colors |= Color::RED;
	// yellow
	if(st.get()) colors |= Color::YELLOW;
	// orange
	if(st.get()) colors |= Color::ORANGE;
	// purple
	if(st.get()) colors |= Color::PURPLE;

	// message
        message = Game::GetEncodeString(GetString(st.getRaw()));
        DEBUG(DBG_GAME , DBG_INFO, "add: " << message);
    }
    else
	DEBUG(DBG_GAME, DBG_WARN, "unknown id");
}

void EventMaps::SetVisited(int color)
{
    if(cancel)
	colors = 0;
    else
	colors &= ~color;
}

bool EventMaps::isAllow(int col, s32 ii) const
{
    return ii == GetIndex() && (col & colors);
}

Riddle::Riddle(s32 index, const u8* ptr, size_t sz) : valid(false)
{
    StreamBuf st(ptr, sz);

    // id
    if(0 == st.get())
    {
	SetIndex(index);

	// resource
	resource.wood = st.getLE32();
	resource.mercury = st.getLE32();
	resource.ore = st.getLE32();
	resource.sulfur = st.getLE32();
	resource.crystal = st.getLE32();
	resource.gems = st.getLE32();
	resource.gold = st.getLE32();

	// artifact
	artifact = st.getLE16();

	// count answers
	u32 count = st.get();

	// answers
	for(u32 i = 0; i < 8; ++i)
	{
	    std::string answer = Game::GetEncodeString(GetString(st.getRaw(13)));

	    if(count-- && answer.size())
		answers.push_back(StringLower(answer));
	}

	// message
	message = Game::GetEncodeString(GetString(st.getRaw()));

	valid = true;
	DEBUG(DBG_GAME, DBG_INFO, "add: " << message);
    }
    else
	DEBUG(DBG_GAME , DBG_WARN, "unknown id");
}

bool Riddle::AnswerCorrect(const std::string & answer)
{
    return answers.end() != std::find(answers.begin(), answers.end(), StringLower(answer));
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
