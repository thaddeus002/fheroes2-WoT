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
#ifndef H2DISPLAY_H
#define H2DISPLAY_H

#include <string>
#include "surface.h"
#include "rect.h"

class Display : public Surface
{
public:
    ~Display();

    typedef enum { SMALL = 640, MEDIUM = 800, LARGE = 1024, XLARGE = 1280, CUSTOM = MAXU16 } resolution_t;

    static Display &	Get(void);

    static void		SetVideoMode(resolution_t mode, bool fullscreen = false);

    static void		HideCursor(void);
    static void		ShowCursor(void);
    static void		SetCaption(const std::string & caption);
    static void		SetIcons(const Surface & icons);

    static void		Flip(void);
    static void		FullScreen(void);
    
    static void		Fade();
    static void		Rise();

    Display &		operator= (const Display & dp);

private:
    Display();
};

#endif
