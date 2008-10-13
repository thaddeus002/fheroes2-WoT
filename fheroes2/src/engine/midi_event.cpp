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

#include <cstring>
#include "midi_event.h"

using namespace MIDI;

Event::Event::Event() : delta(0), status(0) 
{
}

Event::Event::Event(const u32 dl, const u8 st,  const u32 sz, const char *p) : delta(dl), status(st)
{
    if(sz) data.assign(p , p + sz);

    SetDelta(dl);
}

void Event::SetDelta(const u32 dl)
{
    sp = MIDI::PackDelta(pack, dl);
}

u32 Event::Size(void) const
{
    return 1 + sp + data.size();
}

bool Event::Write(std::ostream & o) const
{
    if(o.fail()) return false;

    o.write(pack, sp);
    o.write(&status, 1);
    if(data.size()) o.write(&data[0], data.size());

    return true;
}

void Event::Dump(void) const
{
    printf("          [dl:%d:st:%hhX:dt", delta, status);

    for(u32 ii = 0; ii < data.size(); ++ii) printf(":%hhX", data[ii]);

    printf("]\n");
}
