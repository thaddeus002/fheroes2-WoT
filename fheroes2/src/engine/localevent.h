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

#include "rect.h"
#include "thread.h"
#include "types.h"

class Point;
class Rect;

struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;
union SDL_Event;
struct SDL_keysym;

enum KeyMod { MOD_NONE = KMOD_NONE, MOD_CTRL = KMOD_CTRL, MOD_SHIFT = KMOD_SHIFT, MOD_ALT = KMOD_ALT, MOD_CAPS = KMOD_CAPS };

enum KeySym
{
    KEY_NONE,
    KEY_BACKSPACE,
    KEY_RETURN,
    KEY_ESCAPE,
    KEY_SPACE,
    KEY_EXCLAIM,
    KEY_QUOTEDBL,
    KEY_HASH,
    KEY_DOLLAR,
    KEY_AMPERSAND,
    KEY_QUOTE,
    KEY_LEFTPAREN,
    KEY_RIGHTPAREN,
    KEY_ASTERISK,
    KEY_PLUS,
    KEY_COMMA,
    KEY_MINUS,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_COLON,
    KEY_SEMICOLON,
    KEY_LESS,
    KEY_EQUALS,
    KEY_GREATER,
    KEY_QUESTION,
    KEY_AT,
    KEY_LEFTBRACKET,
    KEY_BACKSLASH,
    KEY_RIGHTBRACKET,
    KEY_CARET,
    KEY_UNDERSCORE,
    KEY_ALT,
    KEY_CONTROL,
    KEY_SHIFT,
    KEY_DELETE,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
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
    KEY_0,
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
    static LocalEvent & Get(void);

    void SetGlobalFilterMouseEvents(void (*pf)(u16, u16));
    void SetGlobalFilterKeysEvents(void (*pf)(u16, u16));
    void SetGlobalFilter(bool);
    void SetTapMode(bool);
    void SetMouseOffsetX(s16);
    void SetMouseOffsetY(s16);

    static void SetStateDefaults(void);
    static void SetState(u8 type, bool enable);
    static u8   GetState(u8 type);

    bool HandleEvents(bool delay = true);

    bool MouseMotion(void) const;
    bool MouseMotion(const Rect &rt) const;

    const Point & GetMouseCursor(void);
    const Point & GetMousePressLeft(void) const{ return mouse_pl; };
    const Point & GetMousePressMiddle(void) const{ return mouse_pm; };
    const Point & GetMousePressRight(void) const{ return mouse_pr; };

    const Point & GetMouseReleaseLeft(void) const{ return mouse_rl; };
    const Point & GetMouseReleaseMiddle(void) const{ return mouse_rm; };
    const Point & GetMouseReleaseRight(void) const{ return mouse_rr; };

    void ResetPressLeft(void);
    void ResetPressRight(void);
    void ResetPressMiddle(void);

    void ResetReleaseLeft(void);
    void ResetReleaseRight(void);
    void ResetReleaseMiddle(void);
    
    bool MouseClickLeft(void);
    bool MouseClickMiddle(void);
    bool MouseClickRight(void);

    bool MouseClickLeft(const Rect &rt);
    bool MouseClickMiddle(const Rect &rt);
    bool MouseClickRight(const Rect &rt);

    bool MouseWheelUp(void) const;
    bool MouseWheelDn(void) const;

    bool MousePressLeft(void) const;
    bool MousePressLeft(const Rect &rt) const;
    bool MousePressLeft(const Point &pt, u16 w, u16 h) const;
    bool MousePressMiddle(void) const;
    bool MousePressMiddle(const Rect &rt) const;
    bool MousePressRight(void) const;
    bool MousePressRight(const Rect &rt) const;

    bool MouseReleaseLeft(void) const;
    bool MouseReleaseLeft(const Rect &rt) const;
    bool MouseReleaseMiddle(void) const;
    bool MouseReleaseMiddle(const Rect &rt) const;
    bool MouseReleaseRight(void) const;
    bool MouseReleaseRight(const Rect &rt) const;

    bool MouseWheelUp(const Rect &rt) const;
    bool MouseWheelDn(const Rect &rt) const;

    bool MouseCursor(const Rect &rt) const;

    bool KeyPress(void) const;
    bool KeyPress(KeySym key) const;
    KeySym KeyValue(void) const;
    u16	   KeyMod(void) const;

private:
    LocalEvent();

    void HandleMouseMotionEvent(const SDL_MouseMotionEvent & motion);
    void HandleMouseButtonEvent(const SDL_MouseButtonEvent & button);
    void HandleMouseWheelEvent(const SDL_MouseButtonEvent & button);
    void HandleKeyboardEvent(SDL_KeyboardEvent &, bool pressed);

    static int GlobalFilterEvents(const SDL_Event *event);

    enum flag_t
    {
	KEY_PRESSED	= 0x0001,
	MOUSE_MOTION	= 0x0002,
	MOUSE_PRESSED	= 0x0004,
	GLOBAL_FILTER	= 0x0008,
	CLICK_LEFT	= 0x0010,
	CLICK_RIGHT	= 0x0020,
	CLICK_MIDDLE	= 0x0040,
	TAP_MODE	= 0x0080,
	MOUSE_OFFSET	= 0x0100,
	CLOCK_ON	= 0x0200,
    };

    void SetModes(flag_t);
    void ResetModes(flag_t);

    u16    modes;
    KeySym key_value;
    u8     mouse_state;
    u8     mouse_button;

    Point mouse_st;	// mouse offset for pocketpc

    Point mouse_pl;	// press left
    Point mouse_pm;	// press middle
    Point mouse_pr;	// press right

    Point mouse_rl;	// release left
    Point mouse_rm;	// release middle
    Point mouse_rr;	// release right

    Point mouse_cu;	// point cursor

    void (*redraw_cursor_func)(u16, u16);
    void (*keyboard_filter_func)(u16, u16);

    Time clock;
};

#endif
