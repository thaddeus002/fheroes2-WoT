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

#ifndef MIDI_H
#define MIDI_H

#include "types.h"

namespace MIDI
{
    static __inline__ u32 Swap32(u32 x){ return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24)); };
    static __inline__ u16 Swap16(u16 x){ return((x<<8)|(x>>8)); };

    u32 ReadBE32(const char * p);
    u16 ReadBE16(const char * p);

    void WriteBE32(char * p, u32 x);
    void WriteBE16(char * p, u16 x);

    u8 UnpackDelta(const char *p, u32 & d);
    u8 PackDelta(char *p, const u32 & d);
};

#endif
