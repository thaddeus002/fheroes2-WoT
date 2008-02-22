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

#include "error.h"
#include "display.h"
#include "localevent.h"

bool LocalEvent::keep_going = true;
bool LocalEvent::mouse_motion = false;
bool LocalEvent::mouse_pressed = false;
bool LocalEvent::key_pressed = false;
u8 LocalEvent::mouse_state = 0;
u8 LocalEvent::mouse_button = 0;
Point LocalEvent::mouse_cu(-1, -1);
Point LocalEvent::mouse_pl(-1, -1);
Point LocalEvent::mouse_pm(-1, -1);
Point LocalEvent::mouse_pr(-1, -1);
Point LocalEvent::mouse_rl(-1, -1);
Point LocalEvent::mouse_rm(-1, -1);
Point LocalEvent::mouse_rr(-1, -1);
SDLKey LocalEvent::key_value = SDLK_EURO;
void (* LocalEvent::redraw_cursor_func)(u16, u16) = NULL;

LocalEvent::LocalEvent()
{
}

LocalEvent & LocalEvent::GetLocalEvent(void)
{
    static LocalEvent le;

    return le;
}

bool LocalEvent::HandleEvents(void)
{
    SDL_Event event;

    mouse_motion = false;
    key_pressed = false;

    while(SDL_PollEvent(&event))
    {
	// keyboard
	if(SDL_KEYDOWN == event.type)
	    HandleKeyboardEvent(event.key.keysym, true);
	else
	if(SDL_KEYUP == event.type)
	    HandleKeyboardEvent(event.key.keysym, false);

	// mouse motion
	if(SDL_MOUSEMOTION == event.type)
	    HandleMouseMotionEvent(event.motion);

	// mouse button
	if(SDL_MOUSEBUTTONDOWN == event.type || SDL_MOUSEBUTTONUP == event.type)
	{
	    // mouse wheel
	    if(SDL_BUTTON_WHEELDOWN == event.button.button || SDL_BUTTON_WHEELUP == event.button.button)
	    {
		HandleMouseWheelEvent(event.button);
		
		break;
	    }
	    else
		HandleMouseButtonEvent(event.button);
	}

	// exit
	if(SDL_QUIT == event.type){ Error::Except(" quit event: ok."); return false; }
    }

    SDL_Delay(1);

    return true;
}

void LocalEvent::HandleKeyboardEvent(SDL_keysym & keysym, bool pressed)
{
    key_pressed = pressed;
    key_value = keysym.sym;
}

void LocalEvent::HandleMouseMotionEvent(const SDL_MouseMotionEvent & motion)
{
    mouse_state = motion.state;
    mouse_motion = true;
    mouse_cu.x = motion.x;
    mouse_cu.y = motion.y;
}

void LocalEvent::HandleMouseButtonEvent(const SDL_MouseButtonEvent & button)
{
    mouse_pressed = (button.state == SDL_PRESSED);
    mouse_button = button.button;

    mouse_cu.x = button.x;
    mouse_cu.y = button.y;
    
    if(mouse_pressed)
	switch(button.button)
	{
	    case SDL_BUTTON_LEFT:
		mouse_pl.x = button.x;
		mouse_pl.y = button.y;
		break;

	    case SDL_BUTTON_MIDDLE:
		mouse_pm.x = button.x;
		mouse_pm.y = button.y;
		break;


	    case SDL_BUTTON_RIGHT:
		mouse_pr.x = button.x;
		mouse_pr.y = button.y;
		break;

	    default:
		break;
	}
    else
	switch(button.button)
	{
	    case SDL_BUTTON_LEFT:
		mouse_rl.x = button.x;
		mouse_rl.y = button.y;
		break;

	    case SDL_BUTTON_MIDDLE:
		mouse_rm.x = button.x;
		mouse_rm.y = button.y;
		break;


	    case SDL_BUTTON_RIGHT:
		mouse_rr.x = button.x;
		mouse_rr.y = button.y;
		break;

	    default:
		break;
	}
}

void LocalEvent::HandleMouseWheelEvent(const SDL_MouseButtonEvent & button)
{
    mouse_pressed = (button.state == SDL_PRESSED);
    mouse_button = button.button;

    mouse_cu.x = button.x;
    mouse_cu.y = button.y;

    if(mouse_pressed)
    {
	mouse_pm.x = button.x;
	mouse_pm.y = button.y;
    }
    else
    {
	mouse_rm.x = button.x;
	mouse_rm.y = button.y;
    }
}

bool LocalEvent::MouseClickLeft(const Rect &rt)
{
    if(!MouseLeft() && (rt & mouse_pl) && (rt & mouse_rl))
    {
	// reset cycle
	mouse_pl.x = -1;
	mouse_pl.y = -1;
	
	return true;
    }

    return false;
}

bool LocalEvent::MouseClickMiddle(const Rect &rt)
{
    if(!MouseMiddle() && (rt & mouse_pm) && (rt & mouse_rm))
    {
	// reset cycle
	mouse_pm.x = -1;
	mouse_pm.y = -1;
	
	return true;
    }

    return false;
}

bool LocalEvent::MouseClickRight(const Rect &rt)
{
    if(!MouseRight() && (rt & mouse_pr) && (rt & mouse_rr))
    {
	// reset cycle
	mouse_pr.x = -1;
	mouse_pr.y = -1;

	return true;
    }

    return false;
}

const Point & LocalEvent::MouseCursor(void)
{
    int x, y;

    SDL_GetMouseState(&x, &y);

    mouse_cu.x = x;
    mouse_cu.y = y;

    return mouse_cu;
}

void LocalEvent::SetGlobalFilterEvents(void (*pf)(u16, u16))
{
    redraw_cursor_func = pf;

    SDL_SetEventFilter(GlobalFilterEvents);
}

int LocalEvent::GlobalFilterEvents(const SDL_Event *event)
{
    Display & display = Display::Get();

    // motion
    if(SDL_MOUSEMOTION == event->type)
    {
        // redraw cursor
        if(redraw_cursor_func)
        {
    	    (*redraw_cursor_func)(event->motion.x, event->motion.y);

    	    display.Flip();
	}

        return 1;                                                                                                                 
    }                                                                                                                             

    // key
    if(SDL_KEYDOWN == event->type)
	switch(event->key.keysym.sym)
	{
    	    case SDLK_F4:
		display.FullScreen();
		return 0;

	    case SDLK_PRINT:
		if(display.SaveBMP("screenshot.bmp")) Error::Verbose("save: screenshot.bmp");
		return 0;

	    default:
		break;
    }

    return 1;
}

void LocalEvent::SetState(u8 type, bool enable)
{
    SDL_EventState(type, enable ? SDL_ENABLE : SDL_IGNORE);
}

u8 LocalEvent::GetState(u8 type)
{
    return SDL_EventState(type, SDL_QUERY);
}
         
void LocalEvent::SetStateDefaults(void)
{
    // enable events
    SetState(SDL_USEREVENT, true);
    SetState(SDL_KEYDOWN, true);
    SetState(SDL_KEYUP, true);
    SetState(SDL_MOUSEMOTION, true);
    SetState(SDL_MOUSEBUTTONDOWN, true);
    SetState(SDL_MOUSEBUTTONUP, true);
    SetState(SDL_QUIT, true);

    // ignore events
    SetState(SDL_ACTIVEEVENT, false);
    SetState(SDL_JOYAXISMOTION, false);
    SetState(SDL_JOYBALLMOTION, false);
    SetState(SDL_JOYHATMOTION, false);
    SetState(SDL_JOYBUTTONUP, false);
    SetState(SDL_JOYBUTTONDOWN, false);
    SetState(SDL_SYSWMEVENT, false);
    SetState(SDL_VIDEORESIZE, false);
    SetState(SDL_VIDEOEXPOSE, false);
}
