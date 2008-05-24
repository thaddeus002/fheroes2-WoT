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
#include "types.h"
#include "error.h"
#include "display.h"

bool needrise = false;

Display::Display()
{
    videosurface = true;
}

Display::~Display()
{
}

Display & Display::operator= (const Display & dp)
{
    surface = SDL_GetVideoSurface();

    return *this;
}

void Display::SetVideoMode(const Size & mode, bool fullscreen)
{
    SetVideoMode(mode.w, mode.h, fullscreen);
}

void Display::SetVideoMode(const u16 w, const u16 h, bool fullscreen)
{
    Display & display = Display::Get();

    if(display.valid() && display.w() == w && display.h() == h) return;

    u32 videoflags = SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWACCEL;

    if(fullscreen || (display.valid() && (display.flags() & SDL_FULLSCREEN))) videoflags |= SDL_FULLSCREEN;

    if(!SDL_SetVideoMode(w, h, 0, videoflags))
    {
	SDL_SetVideoMode(640, 480, 0, videoflags);

	Error::Warning(std::string(SDL_GetError()));
    }
}

/* flip */
void Display::Flip(void)
{
    Display & display = Display::Get();

    if(needrise) display.Rise();
    else SDL_Flip(display.surface);
}

/* full screen */
void Display::FullScreen(void)
{
    Display & display = Display::Get();

    SDL_WM_ToggleFullScreen(display.surface);
}

/* set caption main window */
void Display::SetCaption(const std::string & caption)
{
    SDL_WM_SetCaption(caption.c_str(), NULL);
}

/* set icons window */
void Display::SetIcons(const Surface & icons)
{
    SDL_WM_SetIcon(const_cast<SDL_Surface *>(icons.GetSurface()), NULL);
}

/* hide system cursor */
void Display::HideCursor(void)
{
    SDL_ShowCursor(SDL_DISABLE);
}

/* show system cursor */
void Display::ShowCursor(void)
{
    SDL_ShowCursor(SDL_ENABLE);
}

void Display::Fade()
{
    Display & display = Display::Get();
    if(needrise || display.w() != 640 || display.h() != 480) return;
    Surface temp(display);
    temp.SetDisplayFormat();
    temp.Blit(display);
    const u32 black = temp.MapRGB(0, 0, 0);
    u8 ii = 250;
    while(ii > 0)
    {
	display.Fill(black);
	temp.SetAlpha(ii);
	display.Blit(temp);
        display.Flip();
	ii -= 10;
	DELAY(10);
    }
    temp.SetAlpha(255);
    display.Blit(temp);
    needrise = true;
}

void Display::Rise()
{
    Display & display = Display::Get();
    if(!needrise) return;
    needrise = false;
    Surface temp(display);
    temp.SetDisplayFormat();
    temp.Blit(display);
    const u32 black = temp.MapRGB(0, 0, 0);
    u8 ii = 0;
    while(ii < 250)
    {
	display.Fill(black);
	temp.SetAlpha(ii);
	display.Blit(temp);
        display.Flip();
	ii += 10;
	DELAY(10);
    }
    temp.SetAlpha(255);
    display.Blit(temp);
    display.Flip();
}

/* get video display */
Display & Display::Get(void)
{
    static Display inside;

    if(! inside.surface) inside.surface = SDL_GetVideoSurface();

    return inside;
}
