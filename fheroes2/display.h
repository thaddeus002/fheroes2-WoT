/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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
#ifndef H2DISPLAY_H
#define H2DISPLAY_H

#include <string>
#include "basesurface.h"
#include "rect.h"

namespace Display
{
    typedef enum { SMALL = 640, MEDIUM = 800, LARGE = 1024, XLARGE = 1280 } resolution_t;

    void SetVideoMode(resolution_t mode);

    void HideCursor(void);
    void ShowCursor(void);
    void SetCaption(const std::string &caption);

class VideoSurface : public BaseSurface
{
public:
    static VideoSurface &GetDisplay(void);

    void Flip(void) const{ SDL_Flip(surface); };
    void FullScreen(void) const{ SDL_WM_ToggleFullScreen(surface); };

    VideoSurface & operator= (const VideoSurface & vs){ surface = SDL_GetVideoSurface(); return *this; };

private:
    VideoSurface() : BaseSurface(){ surface = SDL_GetVideoSurface(); };
    VideoSurface(const VideoSurface & vs) : BaseSurface(){ surface = SDL_GetVideoSurface(); };
};

};

static Display::VideoSurface &display = Display::VideoSurface::GetDisplay();

#endif
