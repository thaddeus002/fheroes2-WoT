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

#ifndef MIDI_EVENT_H
#define MIDI_EVENT_H

#include <vector>
#include <ostream>
#include "midi.h"

namespace MIDI
{
    class Event
    {
    public:
	Event();
	Event(const u32 dl, const u8 st, const u32 sz, const char *p);

	u32	Size(void) const;
	const std::vector<char> & Data(void) const{ return data; };
	u32	Delta(void) const { return delta; };
	u8	Status(void) const { return status; };

	void	SetDelta(const u32 dl);

	void	Dump(void) const;
	bool	Write(std::ostream & o) const;

    protected:
	u32	delta;
	char	status;
	std::vector<char> data;

	char	pack[4];
	u8	sp;
    };
};

#endif
