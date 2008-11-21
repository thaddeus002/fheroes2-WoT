/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

#ifndef H2AUDIO_INTERFACE_H
#define H2AUDIO_INTERFACE_H

#include <vector>
#include "types.h"

extern const u32 MAXVOLUME;
#define CHANNEL_RESERVED	22
#define CHANNEL_FREE		6

namespace Audio
{
    struct Spec;
}

namespace Cdrom
{
    bool	isValid(void);
    void	Play(const u8 track, bool loop, bool force = false);
    void	Pause(void);
}

namespace Music
{
    void	Play(const std::vector<u8> & body, bool loop);
    void	Volume(const u8 vol);
    void	Pause(void);
    void	Resume(void);
    void	Reset(void);
    bool	isPlaying(void);
    bool	isPaused(void);
}

namespace Mixer
{
    bool	isValid(void);
    const Audio::Spec &HardwareSpec(void);
    u8		Volume(const int ch, const int vol = -1);
    void	Pause(const int ch = -1);
    void	PauseLoops(void);
    void	Resume(const int ch = -1);
    void	ResumeLoops(void);
    void	Reset(const int ch = -1);
    u8		isPlaying(const int ch);
    u8		isPaused(const int ch);
    void	PlayRAW(const std::vector<u8> & body, const int ch = -1);
    void	LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch);

    void	Reduce(void);
    void	Enhance(void);
}

#endif
