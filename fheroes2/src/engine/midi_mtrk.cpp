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

#include <iomanip>
#include <iostream>
#include <string.h>
#include "midi_mtrk.h"

using namespace MIDI;

struct meta_t
{
    meta_t() : command(0), quantity(0), duration(0){}
    meta_t(u8 c, u8 q, u32 d) : command(c), quantity(q), duration(d){}

    bool operator< (const meta_t & m) const{ return duration < m.duration; }

    u8 command;
    u8 quantity;
    u32 duration;
};


MTrk::MTrk(const u8 *p, const u32 s)
{
    const u8 *ptr = p;
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
		std::cerr << "unknown st: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(status) << ", ln: " << static_cast<int>(p + s - ptr) << std::endl;
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

u32 MTrk::Size(void) const
{
    u32 result = 8;	// id + size

    std::list<Event *>::const_iterator it1 = events.begin();
    std::list<Event *>::const_iterator it2 = events.end();
    for(; it1 != it2; ++it1) if(*it1) result += (*it1)->Size();

    return result;
}

bool MTrk::Write(std::ostream & o) const
{
    if(o.fail()) return false;

    o.write(ID_MTRK, 4);

    u32 size = 0;
    std::list<Event *>::const_iterator it1 = events.begin();
    std::list<Event *>::const_iterator it2 = events.end();
    for(; it1 != it2; ++it1) if(*it1) size += (*it1)->Size();

    u32 x = size;
    SwapBE32(x);
    o.write(reinterpret_cast<char *>(&x), 4);

    if(events.size())
    {
	it1 = events.begin();
	it2 = events.end();
	for(; it1 != it2; ++it1) if(*it1) (*it1)->Write(o);
    }

    return true;
}

bool MTrk::Write(u8 *p) const
{
    if(NULL == p) return false;

    memcpy(p, ID_MTRK, 4);
    p+= 4;

    u32 size = 0;
    std::list<Event *>::const_iterator it1 = events.begin();
    std::list<Event *>::const_iterator it2 = events.end();
    for(; it1 != it2; ++it1) if(*it1) size += (*it1)->Size();

    u32 x = size;
    WriteBE32(p, x);
    p+= 4;

    if(events.size())
    {
	it1 = events.begin();
        it2 = events.end();
	for(; it1 != it2; ++it1) if(*it1){ (*it1)->Write(p); p += (*it1)->Size(); }
    }

    return true;
}

void MTrk::AddEvent(const Event & e)
{
    events.push_back(new Event(e));
}

void MTrk::CloseEvents(void)
{
    events.push_back(new Event(0, 0xFF, 2, reinterpret_cast<const u8*>("\057\000")));
}

void MTrk::Dump(void) const
{
    std::cerr << "[MTrk]\n";

    if(events.size())
    {
	std::list<Event *>::const_iterator it1 = events.begin();
        std::list<Event *>::const_iterator it2 = events.end();
	for(; it1 != it2; ++it1) if(*it1) (*it1)->Dump();
    }
    std::cerr << std::endl;
}

void MTrk::ImportXmiEVNT(const Chunk & evnt)
{
    const u8 *ptr = evnt.data;

    u8 buf[2];
    u32 delta = 0;

    std::list<meta_t> notesoff;
    std::list<meta_t>::iterator it1, it2;

    while(ptr)
    {
	// interval
	if(*ptr < 128)
	{
	    delta += *ptr;
	    ++ptr;
	}
	else
	// command
	{
	    // end
	    if(0xFF == *ptr && 0x2F == *(ptr + 1))
	    {
		events.push_back(new Event(delta, *ptr, 2, ptr + 1));
		break;
	    }
	    else
	    switch(*ptr >> 4)
	    {
		// meta
		case 0x0F:
		{
		    u32 size = 0;
		    size += 1 + MIDI::UnpackDelta(ptr + 2, size);
		    ptr += size + 1;
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
		    events.push_back(new Event(delta, *ptr, 2, ptr + 1));
		    ptr += 3;
		    delta = 0;
		}
		break;

		// note on
		case 0x09:
		{
		    events.push_back(new Event(delta, *ptr, 2, ptr + 1));
		    u32 duration = 0;
		    const u8 s = MIDI::UnpackDelta(ptr + 3, duration);
		    notesoff.push_back(meta_t(*ptr - 0x10, *(ptr + 1), duration));
		    ptr += 3 + s;
		    delta = 0;
		}
		break;

		// program change
	        case 0x0C:
		// chanel pressure
		case 0x0D:
		{
		    events.push_back(new Event(delta, *ptr, 1, ptr + 1));
		    ptr += 2;
		    delta = 0;
		}
		break;

		// unused command
		default:
		    CloseEvents();
		    std::cerr << "unknown st: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*ptr) << ", ln: " << static_cast<int>(evnt.data + evnt.size - ptr) << std::endl;
		break;
	    }
	}

	// insert event: note off
	if(delta)
	{
	    // sort delta
	    notesoff.sort();

	    it1 = notesoff.begin();
	    it2 = notesoff.end();

	    // apply delta
	    for(; it1 != it2; ++it1)
		if((*it1).duration > delta) (*it1).duration -= delta;

	    it1 = notesoff.begin();
	    it2 = notesoff.end();

	    // insert event
	    for(; it1 != it2; ++it1)
	    {
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
		}
	    }
	}
    }
}
