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
#ifndef H2LOCALEVENT_H
#define H2LOCALEVENT_H

#include "types.h"

class Point;
class Rect;

struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;
union SDL_Event;
struct SDL_keysym;

class LocalEvent
{
public:
    static LocalEvent & GetLocalEvent(void);

    static const Point & MouseCursor(void);
    static void SetGlobalFilterEvents(void (*pf)(u16, u16));

    static void SetStateDefaults(void);
    static void SetState(u8 type, bool enable);
    static u8   GetState(u8 type);

    bool operator() (void) const{ return keep_going; };

    bool HandleEvents(void);

    bool MouseMotion(void) const;
    bool MouseMotion(const Rect &rt) const;
    bool MouseLeft(void) const;
    bool MouseMiddle(void) const;
    bool MouseRight(void) const;

    const Point & MousePressLeft(void) const{ return mouse_pl; };
    const Point & MousePressMiddle(void) const{ return mouse_pm; };
    const Point & MousePressRight(void) const{ return mouse_pr; };

    const Point & MouseReleaseLeft(void) const{ return mouse_rl; };
    const Point & MouseReleaseMiddle(void) const{ return mouse_rm; };
    const Point & MouseReleaseRight(void) const{ return mouse_rr; };

    bool MouseClickLeft(const Rect &rt);
    bool MouseClickMiddle(const Rect &rt);
    bool MouseClickRight(const Rect &rt);

    bool MouseWheelUp(void) const;
    bool MouseWheelDn(void) const;

    bool MousePressLeft(const Rect &rt) const;
    bool MousePressLeft(const Point &pt, u16 w, u16 h) const;
    bool MousePressMiddle(const Rect &rt) const;
    bool MousePressRight(const Rect &rt) const;

    bool MouseReleaseLeft(const Rect &rt) const;
    bool MouseReleaseMiddle(const Rect &rt) const;
    bool MouseReleaseRight(const Rect &rt) const;

    bool MouseWheelUp(const Rect &rt) const;
    bool MouseWheelDn(const Rect &rt) const;

    bool MouseCursor(const Rect &rt) const;

    bool KeyPress(int key) const;
    
private:
    LocalEvent();

    void HandleMouseMotionEvent(const SDL_MouseMotionEvent & motion);
    void HandleMouseButtonEvent(const SDL_MouseButtonEvent & button);
    void HandleMouseWheelEvent(const SDL_MouseButtonEvent & button);

    void HandleKeyboardEvent(SDL_keysym & keysym, bool pressed);

    static int GlobalFilterEvents(const SDL_Event *event);

    static bool keep_going;
    static int  key_value;
    static bool key_pressed;
    static bool mouse_motion;
    static bool mouse_pressed;
    static u8   mouse_state;
    static u8   mouse_button;

    static Point mouse_pl;	// press left
    static Point mouse_pm;	// press middle
    static Point mouse_pr;	// press right

    static Point mouse_rl;	// release left
    static Point mouse_rm;	// release middle
    static Point mouse_rr;	// release right

    static Point mouse_cu;	// point cursor

    static void (*redraw_cursor_func)(u16, u16);
};

#endif
