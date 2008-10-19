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

#define MAXVOLUME		MIX_MAX_VOLUME
#define CHANNEL_RESERVED	22
#define CHANNEL_FREE		8

namespace Music
{
    void	Play(const std::vector<u8> & body);
    void	Volume(const u8 vol);
    void	Pause(void);
    void	Resume(void);
    void	Reset(void);
    bool	isPlaying(void);
    bool	isPaused(void);
};

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

    class Cdrom
    {
    public:
	static Cdrom & Get(void);
	~Cdrom();

	bool isValid(void) const{ return cd; };
	void Play(const u8 track);
	void Pause(void);

    private:
	Cdrom();
	SDL_CD *cd;
    };

    class Mixer
    {
    public:
	~Mixer();

	//enum { PLAY = 0x01, REPEATE = 0x02, REDUCE = 0x04, ENHANCE = 0x08 } flag_t;

	static Mixer & Get(void);

	bool isValid(void) const;
	const Spec & HardwareSpec(void) const;

	static u8   Volume(const int ch, const int vol = -1);
	static void Pause(const int ch = -1);
        static void PauseLoops(void);
	static void Resume(const int ch = -1);
        static void ResumeLoops(void);
	static void Reset(const int ch = -1);
	static u8   isPlaying(const int ch);
	static u8   isPaused(const int ch);
	static void PlayRAW(const std::vector<u8> & body, const int ch = -1);
	static void LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch);

	static void Reduce(void);
	static void Enhance(void);

    private:
	Mixer();
	static void FreeChunk(const int ch);

	Spec hardware;
	bool valid;
    };
};

#endif
