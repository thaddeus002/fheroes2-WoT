/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#ifndef H2DISPLAY_H
#define H2DISPLAY_H

#include <string>
#include "surface.h"
#include "rect.h"

class UpdateRects
{
public:
    UpdateRects();
    ~UpdateRects();

    void 	SetVideoMode(int, int);
    void	PushRect(int, int, int, int);
    void	Clear(void);
    size_t	Size(void) const;
    SDL_Rect*	Data(void);
    bool	BitsToRects(void);

protected:
    void	SetBit(u32, bool);
    bool	GetBit(u32) const;

    std::vector<SDL_Rect>	rects;
    u8*				bits;
    int				len;
    int				bf;
    int				bw;
};

class Display : public Surface
{
public:
    ~Display();

    bool                isDisplay(void) const;
    static Display &    Get(void);

    static void         SetVideoMode(int w, int h, u32 flags);
    static int          GetMaxMode(Size &, bool enable_rotate);
    static std::string  GetInfo(void);

    static void         HideCursor(void);
    static void         ShowCursor(void);

    static void         SetCaption(const char*);
    static void         SetIcons(Surface &);

    void                AddUpdateRect(int, int, int, int);
    void                Flip();
    void                Fade(void);
    void                Rise(void);
    void                FullScreen(void);

private:
    Display();

    UpdateRects update_rects;
    bool        dirty;
};

#endif
