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
#ifndef H2EVENT_H
#define H2EVENT_H

#include "gamedefs.h"

class LocalEvent
{
public:
    static LocalEvent & GetLocalEvent(void);
    static void SetState(u8 type, bool enable){ SDL_EventState(type, enable ? SDL_ENABLE : SDL_IGNORE); };
    static u8   GetState(u8 type){ return SDL_EventState(type, SDL_QUERY); };

    bool operator() (void){ return keep_going; };

    void HandleEvents(void);

    bool MouseMotion(void) const{ return mouse_motion; };
    bool MouseMotion(const Rect &rt) const{ return mouse_motion ? rt & mouse_cu : false; };
    bool MouseLeft(void) const{ return mouse_pressed && SDL_BUTTON_LEFT == mouse_button; };
    bool MouseMiddle(void) const{ return mouse_pressed && SDL_BUTTON_MIDDLE  == mouse_button; };
    bool MouseRight(void) const{ return mouse_pressed && SDL_BUTTON_RIGHT == mouse_button; };

    const Point & MousePressLeft(void){ return mouse_pl; };
    const Point & MousePressMiddle(void){ return mouse_pm; };
    const Point & MousePressRight(void){ return mouse_pr; };

    const Point & MouseReleaseLeft(void){ return mouse_rl; };
    const Point & MouseReleaseMiddle(void){ return mouse_rm; };
    const Point & MouseReleaseRight(void){ return mouse_rr; };

    bool MouseClickLeft(const Rect &rt);
    bool MouseClickMiddle(const Rect &rt);
    bool MouseClickRight(const Rect &rt);

    bool MouseWheelUp(void) const{ return mouse_pressed && SDL_BUTTON_WHEELUP == mouse_button; };
    bool MouseWheelDn(void) const{ return mouse_pressed && SDL_BUTTON_WHEELDOWN == mouse_button; };

    static const Point & MouseCursor(void){ return mouse_cu; };

    bool MousePressLeft(const Rect &rt) const{ return MouseLeft() ? rt & mouse_pl : false; };
    bool MousePressLeft(const Point &pt, u16 w, u16 h) const{ return MouseLeft() ? Rect(pt.x, pt.y, w, h) & mouse_pl : false; };
    bool MousePressMiddle(const Rect &rt) const{ return MouseMiddle() ? rt & mouse_pm : false; };
    bool MousePressRight(const Rect &rt) const{ return MouseRight() ? rt & mouse_pr : false; };

    bool MouseReleaseLeft(const Rect &rt) const{ return MouseLeft() ? false : rt & mouse_rl; };
    bool MouseReleaseMiddle(const Rect &rt) const{ return MouseMiddle() ? false : rt & mouse_rm; };
    bool MouseReleaseRight(const Rect &rt) const{ return MouseRight() ? false : rt & mouse_rr; };

    bool MouseWheelUp(const Rect &rt) const{ return MouseWheelUp() ? rt & mouse_cu : false; };
    bool MouseWheelDn(const Rect &rt) const{ return MouseWheelDn() ? rt & mouse_cu : false; };

    bool MouseCursor(const Rect &rt) const{ return rt & mouse_cu; };

    bool KeyPress(SDLKey key) const{ return key == key_value && key_pressed; };
    
    void ResetKey(void){ key_pressed = false; key_value = SDLK_EURO; };

private:
    LocalEvent(){};

    bool HandleMouseMotionEvent(u8 state, u16 x, u16 y, s16 xrel, s16 yrel);
    bool HandleMouseButtonEvent(u8 button, u16 x, u16 y, bool pressed);

    bool HandleKeyboardEvent(SDL_keysym & keysym, bool pressed);

    static bool keep_going;
    static SDLKey key_value;
    static bool key_pressed;
    static bool mouse_motion;
    static bool mouse_pressed;
    static u8   mouse_state;
    static u8   mouse_button;
    static u16  mouse_x;
    static u16  mouse_y;

    static Point mouse_pl;	// press left
    static Point mouse_pm;	// press middle
    static Point mouse_pr;	// press right

    static Point mouse_rl;	// release left
    static Point mouse_rm;	// release middle
    static Point mouse_rr;	// release right

    static Point mouse_cu;	// point cursor
};

#endif
