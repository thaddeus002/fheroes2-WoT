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

#include <sstream>
#include <ctime>
#include "SDL.h"
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
KeySym LocalEvent::key_value = KEY_NONE;
void (* LocalEvent::redraw_cursor_func)(u16, u16) = NULL;

struct KeyMap
{
    SDLKey sdl;
    KeySym desdl;
} mapping[] = {
    { SDLK_UNKNOWN, KEY_NONE },
    { SDLK_ESCAPE, KEY_ESCAPE },
    { SDLK_RETURN, KEY_RETURN },
    { SDLK_BACKSPACE, KEY_BACKSPACE },
    { SDLK_LCTRL, KEY_CONTROL },
    { SDLK_RCTRL, KEY_CONTROL },
    { SDLK_LSHIFT, KEY_SHIFT },
    { SDLK_RSHIFT, KEY_SHIFT },
    { SDLK_BACKSLASH, KEY_BACKSLASH },
    { SDLK_SPACE, KEY_SPACE },
    { SDLK_F1, KEY_F1 },
    { SDLK_F2, KEY_F2 },
    { SDLK_F3, KEY_F3 },
    { SDLK_F4, KEY_F4 },
    { SDLK_F5, KEY_F5 },
    { SDLK_F6, KEY_F6 },
    { SDLK_F7, KEY_F7 },
    { SDLK_F8, KEY_F8 },
    { SDLK_F9, KEY_F9 },
    { SDLK_F10, KEY_F10 },
    { SDLK_F11, KEY_F11 },
    { SDLK_F12, KEY_F12 },
    { SDLK_PRINT, KEY_PRINT },
    { SDLK_LEFT, KEY_LEFT },
    { SDLK_RIGHT, KEY_RIGHT },
    { SDLK_UP, KEY_UP },
    { SDLK_DOWN, KEY_DOWN },
    { SDLK_1, KEY_1 },
    { SDLK_2, KEY_2 },
    { SDLK_3, KEY_3 },
    { SDLK_4, KEY_4 },
    { SDLK_5, KEY_5 },
    { SDLK_6, KEY_6 },
    { SDLK_7, KEY_7 },
    { SDLK_8, KEY_8 },
    { SDLK_9, KEY_9 },
    { SDLK_a, KEY_a },
    { SDLK_b, KEY_b },
    { SDLK_c, KEY_c },
    { SDLK_d, KEY_d },
    { SDLK_e, KEY_e },
    { SDLK_f, KEY_f },
    { SDLK_g, KEY_g },
    { SDLK_h, KEY_h },
    { SDLK_i, KEY_i },
    { SDLK_j, KEY_j },
    { SDLK_k, KEY_k },
    { SDLK_l, KEY_l },
    { SDLK_m, KEY_m },
    { SDLK_n, KEY_n },
    { SDLK_o, KEY_o },
    { SDLK_p, KEY_p },
    { SDLK_q, KEY_q },
    { SDLK_r, KEY_r },
    { SDLK_s, KEY_s },
    { SDLK_t, KEY_t },
    { SDLK_u, KEY_u },
    { SDLK_v, KEY_v },
    { SDLK_w, KEY_w },
    { SDLK_x, KEY_x },
    { SDLK_y, KEY_y },
    { SDLK_z, KEY_z }
};

static KeySym SDLToKeySym(SDLKey &key)
{
    for(u16 i = 0; i < sizeof(mapping) / sizeof(mapping[0]); i++)
        if(mapping[i].sdl == key)
            return mapping[i].desdl;
    return KEY_NONE;
}

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

bool LocalEvent::MouseMotion(void) const
{
    return mouse_motion;
}

bool LocalEvent::MouseMotion(const Rect &rt) const
{
    return mouse_motion ? rt & mouse_cu : false;
}

bool LocalEvent::MouseLeft(void) const
{
    return mouse_pressed && SDL_BUTTON_LEFT == mouse_button;
}

bool LocalEvent::MouseMiddle(void) const
{
    return mouse_pressed && SDL_BUTTON_MIDDLE  == mouse_button;
}

bool LocalEvent::MouseRight(void) const
{
    return mouse_pressed && SDL_BUTTON_RIGHT == mouse_button;
}

void LocalEvent::HandleKeyboardEvent(SDL_keysym & keysym, bool pressed)
{
    key_pressed = pressed;
    key_value = SDLToKeySym(keysym.sym);
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

bool LocalEvent::MouseWheelUp(void) const
{
    return mouse_pressed && SDL_BUTTON_WHEELUP == mouse_button;
}

bool LocalEvent::MouseWheelDn(void) const
{
    return mouse_pressed && SDL_BUTTON_WHEELDOWN == mouse_button;
}

bool LocalEvent::MousePressLeft(const Rect &rt) const
{
    return MouseLeft() ? rt & mouse_pl : false;
}

bool LocalEvent::MousePressLeft(const Point &pt, u16 w, u16 h) const
{
    return MouseLeft() ? Rect(pt.x, pt.y, w, h) & mouse_pl : false;
}

bool LocalEvent::MousePressMiddle(const Rect &rt) const
{
    return MouseMiddle() ? rt & mouse_pm : false;
}

bool LocalEvent::MousePressRight(const Rect &rt) const
{
    return MouseRight() ? rt & mouse_pr : false;
}

bool LocalEvent::MouseReleaseLeft(const Rect &rt) const
{
    return MouseLeft() ? false : rt & mouse_rl;
}

bool LocalEvent::MouseReleaseMiddle(const Rect &rt) const
{
    return MouseMiddle() ? false : rt & mouse_rm;
}

bool LocalEvent::MouseReleaseRight(const Rect &rt) const
{
    return MouseRight() ? false : rt & mouse_rr;
}

bool LocalEvent::MouseWheelUp(const Rect &rt) const
{
    return MouseWheelUp() ? rt & mouse_cu : false;
}

bool LocalEvent::MouseWheelDn(const Rect &rt) const
{
    return MouseWheelDn() ? rt & mouse_cu : false;
}

bool LocalEvent::MouseCursor(const Rect &rt) const
{
    return rt & mouse_cu;
}

const Point & LocalEvent::MouseCursor(void)
{
    int x, y;

    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);

    mouse_cu.x = x;
    mouse_cu.y = y;

    return mouse_cu;
}

bool LocalEvent::KeyPress(KeySym key) const
{
    return key == key_value && key_pressed;
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
    	    (*redraw_cursor_func)(event->motion.x, event->motion.y);

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
	    {
		std::ostringstream stream;
    		stream << std::time(0);
        
        	std::string name("screenshot_");
        	name += stream.str();
        	name += ".bmp";

            	if(display.SaveBMP(name.c_str())) Error::Verbose("save: " + name);
	    }
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
