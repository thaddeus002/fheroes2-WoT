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
#include <fstream>
#include "midi_xmi.h"
#include "midi_mid.h"
#include "engine.h"

int main(int argc, char **argv)
{
    if(argc != 3)
    {
	std::cout << argv[0] << " infile.xmi outfile.mid" << std::endl;

	return EXIT_SUCCESS;
    }

    MIDI::Xmi x;
    MIDI::Mid m;
    MIDI::MTrk track;

    x.Read(argv[1]);

    track.ImportXmiEVNT(x.EVNT());

    m.AddTrack(track);
    m.SetPPQN(64);

    // m.Dump();

    m.Write(argv[2]);

    return EXIT_SUCCESS;
}
