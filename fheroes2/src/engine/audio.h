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

#ifndef H2AUDIO_H
#define H2AUDIO_H

#include <vector>
#include "types.h"

namespace Audio
{
    struct Spec : public SDL_AudioSpec
    {
	Spec();
    };

    struct CVT : public SDL_AudioCVT
    {
	CVT();
	
	bool Build(const Spec & src, const Spec & dst);
	bool Convert(void);

	bool valid;
    };

    class Mixer
    {
    public:
	~Mixer();

	static Mixer & Get(void);

	bool isValid(void) const;
	const Spec & HardwareSpec(void) const;

	void Play(const CVT & cvt);

    private:
	struct mixer_t
        {
            mixer_t() : data(NULL), size(0), pos(0) {};

    	    u8 *    data;
    	    u32     size;
    	    u32     pos;
        };

	Mixer();

	static void CallBack(void *unused, u8 *stream, int size);
	static bool PredicateIsFreeSound(const mixer_t & header);

	Spec hardware;
	std::vector<mixer_t> sounds;
	bool valid;
    };
};

#endif
