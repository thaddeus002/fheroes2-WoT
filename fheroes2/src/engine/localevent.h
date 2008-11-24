/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov <afletdinov@mail.dc.baikal.ru>*
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>           *
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

enum KeySym
{
    KEY_NONE,
    KEY_ESCAPE,
    KEY_RETURN,
    KEY_BACKSPACE,
    KEY_CONTROL,
    KEY_SHIFT,
    KEY_BACKSLASH,
    KEY_SPACE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_PRINT,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_a,
    KEY_b,
    KEY_c,
    KEY_d,
    KEY_e,
    KEY_f,
    KEY_g,
    KEY_h,
    KEY_i,
    KEY_j,
    KEY_k,
    KEY_l,
    KEY_m,
    KEY_n,
    KEY_o,
    KEY_p,
    KEY_q,
    KEY_r,
    KEY_s,
    KEY_t,
    KEY_u,
    KEY_v,
    KEY_w,
    KEY_x,
    KEY_y,
    KEY_z
};

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

    bool KeyPress(KeySym key) const;
    
private:
    LocalEvent();

    void HandleMouseMotionEvent(const SDL_MouseMotionEvent & motion);
    void HandleMouseButtonEvent(const SDL_MouseButtonEvent & button);
    void HandleMouseWheelEvent(const SDL_MouseButtonEvent & button);

    void HandleKeyboardEvent(SDL_keysym & keysym, bool pressed);

    static int GlobalFilterEvents(const SDL_Event *event);

    static bool   keep_going;
    static KeySym key_value;
    static bool   key_pressed;
    static bool   mouse_motion;
    static bool   mouse_pressed;
    static u8     mouse_state;
    static u8     mouse_button;

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
