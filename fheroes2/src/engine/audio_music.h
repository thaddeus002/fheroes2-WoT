/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#ifndef H2AUDIO_MUSIC_H
#define H2AUDIO_MUSIC_H

#include <vector>
#include "types.h"

namespace Music
{
    void	Play(const std::vector<u8> & body, bool loop);
    u8		Volume(int);
    void	Pause(void);
    void	Resume(void);
    void	Reset(void);
    bool	isPlaying(void);
    bool	isPaused(void);
};

#endif
