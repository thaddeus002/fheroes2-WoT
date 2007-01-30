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

#include <string>
#include "config.h"
#include "error.h"
#include "gamedefs.h"

void Display::SetVideoMode(Display::resolution_t mode)
{
    u16 xres, yres;

    switch(mode){

	default:
	case SMALL:
	    xres = 640;
	    yres = 480;
	    break;

	case MEDIUM:
	    xres = 800;
	    yres = 576;
	    break;

	case LARGE:
	    xres = 1024;
	    yres = 768;
	    break;

	case XLARGE:
	    xres = 1280;
	    yres = 1024;
	    break;
    }

    if(display.valid() && display.w() == xres && display.h() == yres) return;

    u32 videoflags = SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWACCEL;
    if(H2Config::FullScreen()) videoflags |= SDL_FULLSCREEN; 

    if(!display.SetVideoMode(xres, yres, DEFAULT_DEPTH, videoflags)){
	display.SetVideoMode(640, 480, DEFAULT_DEPTH, videoflags);
	Error::Warning(SDLmm::GetError());
    }
}

void Display::SetCaption(const std::string &caption)
{
    display.SetCaption(caption, "FHEROES2");
}

void Display::HideCursor(void){ SDL_ShowCursor(SDL_DISABLE); }

void Display::ShowCursor(void){ SDL_ShowCursor(SDL_ENABLE); }
