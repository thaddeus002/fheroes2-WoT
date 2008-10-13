/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include <iostream>
#include "midi_mtrk.h"

using namespace MIDI;

struct meta_t
{
    meta_t() : command(0), quantity(0), duration(0){};
    meta_t(u8 c, u8 q, u8 d) : command(c), quantity(q), duration(d){};
    u8 command;
    u8 quantity;
    u8 duration;
};

MTrk::MTrk(const char *p, const u32 s)
{
    const char *ptr = p;
    bool end = false;

    while(ptr && !end && ptr < (p + s))
    {
	u32 delta = 0;
	const u8 s = MIDI::UnpackDelta(ptr, delta);
	ptr += s;
	const u8 status = *ptr;
	ptr += 1;

	switch(status >> 4)
	{
	    // meta
	    case 0x0F:
	    {
		u32 size = 0;
		const u8 s = MIDI::UnpackDelta(ptr + 1, size);
		if(0xFF == status && 0x2F == *ptr)
		{
		    end = true;
		    events.push_back(new Event(delta, status, 1 + s + size, ptr));
		}
		ptr += 1 + s + size;
	    }
	    break;

	    // note off
	    case 0x08:
	    // note on
	    case 0x09:
	    // key pressure
	    case 0x0A:
	    // control change
	    case 0x0B:
	    // pitch bend
	    case 0x0E:
	    {
		events.push_back(new Event(delta, status, 2, ptr));
		ptr += 2;
	    }
	    break;

	    // program change
	    case 0x0C:
	    // chanel pressure
	    case 0x0D:
	    {
		events.push_back(new Event(delta, status, 1, ptr));
		ptr += 1;
	    }
	    break;

	    // unused command
	    default:
		end = true;
		CloseEvents();
		fprintf(stderr, "unkn st: %hhX, l: %d\n", status, p + s - ptr);
	    break;
	}
    }
}


MTrk::MTrk(const MTrk & t)
{
    std::list<Event *>::const_iterator it1 = t.events.begin();
    std::list<Event *>::const_iterator it2 = t.events.end();

    for(; it1 != it2; ++it1) if(*it1) events.push_back(new Event(**it1));
}

MTrk::~MTrk()
{
    if(events.size())
    {
	std::list<Event *>::const_iterator it1 = events.begin();
        std::list<Event *>::const_iterator it2 = events.end();

	for(; it1 != it2; ++it1) if(*it1) delete *it1;
    }
}

bool MTrk::Write(std::ostream & o) const
{
    if(o.fail()) return false;

    o.write(ID_MTRK, 4);

    u32 size = 0;

    std::list<Event *>::const_iterator it1 = events.begin();
    std::list<Event *>::const_iterator it2 = events.end();
    for(; it1 != it2; ++it1) if(*it1) size += (*it1)->Size();

#if __BYTE_ORDER == __LITTLE_ENDIAN
    size = MIDI::Swap32(size);
#endif
    o.write(reinterpret_cast<char *>(&size), 4);

    if(events.size())
    {
	it1 = events.begin();
	it2 = events.end();
	for(; it1 != it2; ++it1) if(*it1) (*it1)->Write(o);
    }

    return true;
}

void MTrk::AddEvent(const Event & e)
{
    events.push_back(new Event(e));
}

void MTrk::CloseEvents(void)
{
    events.push_back(new Event(0, 0xFF, 2, "\057\000"));
}

void MTrk::Dump(void) const
{
    std::cout << "[MTrk]\n";

    if(events.size())
    {
	std::list<Event *>::const_iterator it1 = events.begin();
        std::list<Event *>::const_iterator it2 = events.end();
	for(; it1 != it2; ++it1) if(*it1) (*it1)->Dump();
    }
    std::cout << std::endl;
}

void MTrk::ImportXmiEVNT(const Chunk & evnt)
{
    const char *ptr = evnt.data;
    std::list<meta_t> notesoff;
    char buf[2];

    u32 delta = 0;
    bool end = false;

    while(ptr && !end && ptr < (evnt.data  + evnt.size))
    {

	const u8 status = *ptr;

	// insert event: note off
	{
	    std::list<meta_t>::iterator it1 = notesoff.begin();
	    std::list<meta_t>::const_iterator it2 = notesoff.end();

	    // apply delta
	    for(; it1 != it2; ++it1)
		if((*it1).duration > delta)
		{
		    (*it1).duration -= delta;
		}

	    it1 = notesoff.begin();
	    it2 = notesoff.end();

	    // insert event
	    for(; it1 != it2; ++it1)
		if((*it1).duration < delta)
		{
		    buf[0] = (*it1).quantity;
		    buf[1] = 0x7F;

		    delta -= (*it1).duration;
		    // note off
		    events.push_back(new Event((*it1).duration, (*it1).command, 2, buf));
		    it1 = notesoff.erase(it1);
		}
		else
		if((*it1).duration == 0)
		{
		    buf[0] = (*it1).quantity;
		    buf[1] = 0x7F;

		    // note off
		    events.push_back(new Event(delta, (*it1).command, 2, buf));
		    it1 = notesoff.erase(it1);
		    delta = 0;
		}
	}

	switch(status >> 4)
	{
	    // meta
	    case 0x0F:
	    {
		ptr += 1;
		u32 size = 0;
		const u8 s = MIDI::UnpackDelta(ptr + 1, size);
		// store only end event
		if(0xFF == status && 0x2F == *ptr)
		{
		    end = true;
		    events.push_back(new Event(delta, status, 1 + s + size, ptr));
		}
		ptr += 1 + s + size;
		delta = 0;
	    }
	    break;

	    // key pressure
	    case 0x0A:
	    // control change
	    case 0x0B:
	    // pitch bend
	    case 0x0E:
	    {
		events.push_back(new Event(delta, status, 2, ptr + 1));
		ptr += 3;
		delta = 0;
	    }
	    break;

	    // note on
	    case 0x09:
	    {
		ptr += 1;
		const u8 m = *ptr;
		events.push_back(new Event(delta, status, 2, ptr));
		ptr += 2;
		u32 duration = 0;
		const u8 s = MIDI::UnpackDelta(ptr, duration);
		ptr += s;
		notesoff.push_back(meta_t(status - 0x10, m, duration));
		delta = 0;
	    }
	    break;

	    // program change
	    case 0x0C:
	    // chanel pressure
	    case 0x0D:
	    {
		events.push_back(new Event(delta, status, 1, ptr + 1));
		ptr += 2;
		delta = 0;
	    }
	    break;

	    // unused command
	    default:
		end = true;
		CloseEvents();
		fprintf(stderr, "unknown cmd: %hhX, len: %d\n", status, evnt.data + evnt.size - ptr);
	    break;
	}

	// read delta
	while(0 < *ptr)
	{
	    delta += *ptr;
	    ++ptr;
	}
    }
}
