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

#define MIX_MAXVOLUME	SDL_MIX_MAXVOLUME

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
    };

    class Mixer
    {
    public:
	~Mixer();

	enum { PLAY = 0x01, REPEATE = 0x02, REDUCE = 0x04, ENHANCE = 0x08 } flag_t;

	static Mixer & Get(void);

	bool isValid(void) const;
	const Spec & HardwareSpec(void) const;

	void Play(const std::vector<u8> & body, const u8 volume = MIX_MAXVOLUME, const u8 state = 0);
	void StopRepeate(void);
	void StopAll(void);

	void Reduce(void);
	void Enhance(void);

    private:
	struct mixer_t
        {
            mixer_t() : data(NULL), length(0), position(0), volume1(0), volume2(0), state(0) {};

	    const u8 *	data;
	    u32		length;
    	    u32		position;
    	    u8		volume1;
    	    u8		volume2;
    	    u8		state;
        };

	Mixer();

	static void CallBack(void *unused, u8 *stream, int size);
	static bool PredicateIsFreeSound(const mixer_t & header);
	static void PredicateStopAllSound(mixer_t & header);
	static void PredicateStopRepeateSound(mixer_t & header);
	static void PredicateReduceSound(mixer_t & header);
	static void PredicateEnhanceSound(mixer_t & header);

	Spec hardware;
	std::vector<mixer_t> sounds;
	bool valid;
    };
};

#endif
