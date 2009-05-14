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
std::string LocalEvent::screenshot_prefix("screenshot_");
void (* LocalEvent::redraw_cursor_func)(u16, u16) = NULL;

static KeySym SDLToKeySym(SDLKey key)
{
    switch(key)
    {
	default: break;
	case SDLK_ESCAPE:	return KEY_ESCAPE;
	case SDLK_RETURN:	return KEY_RETURN;
	case SDLK_BACKSPACE:	return KEY_BACKSPACE;
	case SDLK_EXCLAIM:    	return KEY_EXCLAIM;
	case SDLK_QUOTEDBL:    	return KEY_QUOTEDBL;
	case SDLK_HASH:    	return KEY_HASH;
	case SDLK_DOLLAR:    	return KEY_DOLLAR;
	case SDLK_AMPERSAND:    return KEY_AMPERSAND;
	case SDLK_QUOTE:    	return KEY_QUOTE;
	case SDLK_LEFTPAREN:    return KEY_LEFTPAREN;
	case SDLK_RIGHTPAREN:   return KEY_RIGHTPAREN;
	case SDLK_ASTERISK:     return KEY_ASTERISK;
	case SDLK_PLUS:    	return KEY_PLUS;
	case SDLK_COMMA:    	return KEY_COMMA;
	case SDLK_MINUS:    	return KEY_MINUS;
	case SDLK_PERIOD:    	return KEY_PERIOD;
	case SDLK_SLASH:    	return KEY_SLASH;
	case SDLK_COLON:	return KEY_COLON;
	case SDLK_SEMICOLON:	return KEY_SEMICOLON;
	case SDLK_LESS:		return KEY_LESS;
	case SDLK_EQUALS:	return KEY_EQUALS;
	case SDLK_GREATER:	return KEY_GREATER;
	case SDLK_QUESTION:	return KEY_QUESTION;
	case SDLK_AT:		return KEY_AT;
	case SDLK_LEFTBRACKET:	return KEY_LEFTBRACKET;
	case SDLK_BACKSLASH:	return KEY_BACKSLASH;
	case SDLK_RIGHTBRACKET:	return KEY_RIGHTBRACKET;
	case SDLK_CARET:	return KEY_CARET;
	case SDLK_UNDERSCORE:	return KEY_UNDERSCORE;
	case SDLK_LALT:		return KEY_ALT;
	case SDLK_RALT:		return KEY_ALT;
	case SDLK_LCTRL:	return KEY_CONTROL;
	case SDLK_RCTRL:	return KEY_CONTROL;
	case SDLK_LSHIFT:	return KEY_SHIFT;
	case SDLK_RSHIFT:	return KEY_SHIFT;
	case SDLK_SPACE:	return KEY_SPACE;
	case SDLK_DELETE:	return KEY_DELETE;
	case SDLK_PAGEUP:	return KEY_PAGEUP;
	case SDLK_PAGEDOWN:	return KEY_PAGEDOWN;
	case SDLK_F1:		return KEY_F1;
	case SDLK_F2:		return KEY_F2;
	case SDLK_F3:		return KEY_F3;
	case SDLK_F4:		return KEY_F4;
	case SDLK_F5:		return KEY_F5;
	case SDLK_F6:		return KEY_F6;
	case SDLK_F7:		return KEY_F7;
	case SDLK_F8:		return KEY_F8;
	case SDLK_F9:		return KEY_F9;
	case SDLK_F10:		return KEY_F10;
	case SDLK_F11:		return KEY_F11;
	case SDLK_F12:		return KEY_F12;
	case SDLK_PRINT:	return KEY_PRINT;
	case SDLK_LEFT:		return KEY_LEFT;
	case SDLK_RIGHT:	return KEY_RIGHT;
	case SDLK_UP:		return KEY_UP;
	case SDLK_DOWN:		return KEY_DOWN;
	case SDLK_0:		return KEY_0;
	case SDLK_1:		return KEY_1;
	case SDLK_2:		return KEY_2;
	case SDLK_3:		return KEY_3;
	case SDLK_4:		return KEY_4;
	case SDLK_5:		return KEY_5;
	case SDLK_6:		return KEY_6;
	case SDLK_7:		return KEY_7;
	case SDLK_8:		return KEY_8;
	case SDLK_9:		return KEY_9;
	case SDLK_a:		return KEY_a;
	case SDLK_b:		return KEY_b;
	case SDLK_c:		return KEY_c;
	case SDLK_d:		return KEY_d;
	case SDLK_e:		return KEY_e;
	case SDLK_f:		return KEY_f;
	case SDLK_g:		return KEY_g;
	case SDLK_h:		return KEY_h;
	case SDLK_i:		return KEY_i;
	case SDLK_j:		return KEY_j;
	case SDLK_k:		return KEY_k;
	case SDLK_l:		return KEY_l;
	case SDLK_m:		return KEY_m;
	case SDLK_n:		return KEY_n;
	case SDLK_o:		return KEY_o;
	case SDLK_p:		return KEY_p;
	case SDLK_q:		return KEY_q;
	case SDLK_r:		return KEY_r;
	case SDLK_s:		return KEY_s;
	case SDLK_t:		return KEY_t;
	case SDLK_u:		return KEY_u;
	case SDLK_v:		return KEY_v;
	case SDLK_w:		return KEY_w;
	case SDLK_x:		return KEY_x;
	case SDLK_y:		return KEY_y;
	case SDLK_z:		return KEY_z;
    }

    return KEY_NONE;
};

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
	    HandleKeyboardEvent(event.key, true);
	else
	if(SDL_KEYUP == event.type)
	    HandleKeyboardEvent(event.key, false);

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

void LocalEvent::HandleKeyboardEvent(SDL_KeyboardEvent & event, bool pressed)
{
    key_pressed = pressed;
    key_value = SDLToKeySym(event.keysym.sym);
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

u16 LocalEvent::KeyMod(void) const
{
    return SDL_GetModState();
}

KeySym LocalEvent::KeyValue(void) const
{
    return key_value;
}

bool LocalEvent::KeyPress(void) const
{
    return key_pressed;
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
#ifndef WITH_IMAGE
		stream << screenshot_prefix << std::time(0) << ".bmp";
#else
		stream << screenshot_prefix << std::time(0) << ".png";
#endif
            	if(display.Save(stream.str().c_str())) Error::Verbose("save: " + stream.str());
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

void LocalEvent::SetScreenshotPrefix(std::string & str)
{
    screenshot_prefix = str;
}
