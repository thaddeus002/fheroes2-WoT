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
#include "rect.h"
#include "gamedefs.h"
#include "error.h"
#include "display.h"

void Display::Initialize(void)
{
    if(0 > SDL_Init(SDL_INIT_VIDEO)) Error::Except(std::string(SDL_GetError()));              
    atexit(SDL_Quit);                                                                         
}

void Display::SetVideoMode(Display::resolution_t mode, bool fullscreen)
{
    u16 xres, yres;

    switch(mode)
    {
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

    Display::VideoSurface & display = Display::VideoSurface::GetDisplay();

    if(display.valid() && display.w() == xres && display.h() == yres) return;

    u32 videoflags = SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWACCEL;
    if(fullscreen || (display.valid() && (display.flags() & SDL_FULLSCREEN))) videoflags |= SDL_FULLSCREEN;
    if(!SDL_SetVideoMode(xres, yres, 0, videoflags)){
	SDL_SetVideoMode(640, 480, 0, videoflags);
	Error::Warning(std::string(SDL_GetError()));
    }
    
    if(!display.valid()) display = Display::VideoSurface::GetDisplay();
}

/* set caption main window */
void Display::SetCaption(const std::string &caption){ SDL_WM_SetCaption(caption.c_str(), "FHEROES2"); }

/* set icons window */
void Display::SetIcons(const Surface & icons)
{
    SDL_WM_SetIcon(const_cast<SDL_Surface *>(icons.GetSurface()), NULL);
}

/* hide system cursor */
void Display::HideCursor(void){ SDL_ShowCursor(SDL_DISABLE); }

/* show system cursor */
void Display::ShowCursor(void){ SDL_ShowCursor(SDL_ENABLE); }

/* get video display */
Display::VideoSurface &Display::VideoSurface::GetDisplay(void)
{
    static Display::VideoSurface inside;

    if(!inside.valid())	inside.surface = SDL_GetVideoSurface();

    return inside;
}
