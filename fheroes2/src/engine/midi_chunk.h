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

#ifndef MIDI_CHUNK_H
#define MIDI_CHUNK_H

#include <ostream>
#include <istream>
#include <vector>
#include "midi.h"

namespace MIDI
{
    class Chunk
    {
	public:
	char		id[4];
	u32		size;
	char *		data;

	Chunk();
	Chunk(const char *i, const u32 s, const char *p = NULL);
	Chunk(std::istream & i);
	Chunk(const char *p);
	Chunk(const Chunk & c);
	~Chunk();

	Chunk &	operator= (const Chunk & c);

	bool Write(std::ostream & o) const;
	bool Write(char *p) const;

	bool Read(std::istream & i);
	bool Read(const char *p);
	bool Read(const std::vector<char> & b);

	void Dump(void) const;
    };
};

#endif
