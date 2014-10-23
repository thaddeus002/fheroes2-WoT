/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include "mp2.h"
#include "game.h"
#include "maps_objects.h"

#define SIZEMESSAGE 400

MapEvent::MapEvent() : ObjectSimple(MP2::OBJ_EVENT), computer(false), cancel(true), colors(0)
{
}

void MapEvent::LoadFromMP2(s32 index, StreamBuf st)
{
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
        message = Game::GetEncodeString(st.toString());
	DEBUG(DBG_GAME , DBG_INFO, "event" << ": " << message);
    }
    else
	DEBUG(DBG_GAME, DBG_WARN, "unknown id");
}

void MapEvent::SetVisited(int color)
{
    if(cancel)
	colors = 0;
    else
	colors &= ~color;
}

bool MapEvent::isAllow(int col) const
{
    return col & colors;
}

MapSphinx::MapSphinx() : ObjectSimple(MP2::OBJ_SPHINX), valid(false)
{
}

void MapSphinx::LoadFromMP2(s32 index, StreamBuf st)
{
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
	    std::string answer = Game::GetEncodeString(st.toString(13));

	    if(count-- && answer.size())
		answers.push_back(StringLower(answer));
	}

	// message
	message = Game::GetEncodeString(st.toString());

	valid = true;
	DEBUG(DBG_GAME , DBG_INFO, "sphinx" << ": " << message);
    }
    else
	DEBUG(DBG_GAME , DBG_WARN, "unknown id");
}

bool MapSphinx::AnswerCorrect(const std::string & answer)
{
    return answers.end() != std::find(answers.begin(), answers.end(), StringLower(answer));
}

void MapSphinx::SetQuiet(void)
{
    valid = false;
    artifact = Artifact::UNKNOWN;
    resource.Reset();
}

StreamBase & operator<< (StreamBase & msg, const MapEvent & obj)
{
    return msg <<
	static_cast<const ObjectSimple &>(obj) <<
	static_cast<const MapPosition &>(obj) <<
	obj.resource <<
	obj.artifact <<
	obj.computer <<
	obj.cancel <<
	obj.colors <<
	obj.message;
}

StreamBase & operator>> (StreamBase & msg, MapEvent & obj)
{
    if(FORMAT_VERSION_3186 > Game::GetLoadVersion())
    {
	static_cast<ObjectSimple &>(obj) = ObjectSimple(MP2::OBJ_EVENT);
    }
    else
	msg >>
	    static_cast<ObjectSimple &>(obj);

    return msg >>
	static_cast<MapPosition &>(obj) >>
	obj.resource >>
	obj.artifact >>
	obj.computer >>
	obj.cancel >>
	obj.colors >>
	obj.message;
}

StreamBase & operator<< (StreamBase & msg, const MapSphinx & obj)
{
    return msg <<
	static_cast<const ObjectSimple &>(obj) <<
	static_cast<const MapPosition &>(obj) <<
	obj.resource <<
	obj.artifact <<
	obj.answers <<
	obj.message <<
	obj.valid;
}

StreamBase & operator>> (StreamBase & msg, MapSphinx & obj)
{
    if(FORMAT_VERSION_3186 > Game::GetLoadVersion())
    {
	static_cast<ObjectSimple &>(obj) = ObjectSimple(MP2::OBJ_SPHINX);
    }
    else
	msg >>
	    static_cast<ObjectSimple &>(obj);

    return msg >>
	static_cast<MapPosition &>(obj) >>
	obj.resource >>
	obj.artifact >>
	obj.answers >>
	obj.message >>
	obj.valid;
}

MapSign::MapSign() : ObjectSimple(MP2::OBJ_SIGN)
{
}

MapSign::MapSign(s32 index, const std::string & msg) : ObjectSimple(MP2::OBJ_SIGN)
{
    SetIndex(index);
    message = msg;
}

void MapSign::LoadFromMP2(s32 index, StreamBuf st)
{
    st.skip(9);
    message = st.toString();

    SetIndex(index);
    message = Game::GetEncodeString(message);
    DEBUG(DBG_GAME , DBG_INFO, "sign" << ": " << message);
}

StreamBase & operator<< (StreamBase & msg, const MapSign & obj)
{
    return msg <<
	static_cast<const ObjectSimple &>(obj) <<
	static_cast<const MapPosition &>(obj) <<
	obj.message;
}

StreamBase & operator>> (StreamBase & msg, MapSign & obj)
{
    return msg >>
	static_cast<ObjectSimple &>(obj) >>
	static_cast<MapPosition &>(obj) >>
	obj.message;
}
