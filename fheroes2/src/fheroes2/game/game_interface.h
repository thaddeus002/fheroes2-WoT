/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#ifndef H2GAMEINTERFACE_H
#define H2GAMEINTERFACE_H

#include "gamedefs.h"
#include "interface_border.h"
#include "interface_radar.h"
#include "interface_buttons.h"
#include "interface_icons.h"
#include "interface_status.h"
#include "interface_gamearea.h"

enum redraw_t
{
    REDRAW_RADAR     = 0x01,
    REDRAW_HEROES    = 0x02,
    REDRAW_CASTLES   = 0x04,
    REDRAW_BUTTONS   = 0x08,
    REDRAW_STATUS    = 0x10,
    REDRAW_BORDER    = 0x20,
    REDRAW_GAMEAREA  = 0x40,
    REDRAW_CURSOR    = 0x80,

    REDRAW_ICONS     = REDRAW_HEROES | REDRAW_CASTLES,
    REDRAW_ALL       = 0xFF
};

namespace Interface
{
    class Basic
    {
    public:
    	static Basic & Get(void);

	bool    NeedRedraw(void) const;
	void    SetRedraw(u8);
    	void	Redraw(u8 f = 0);

	const Rect & GetAreaScrollLeft(void) const;
	const Rect & GetAreaScrollRight(void) const;
	const Rect & GetAreaScrollTop(void) const;
	const Rect & GetAreaScrollBottom(void) const;

	GameArea & gameArea;
	Radar & radar;
	IconsPanel & iconsPanel;
	ButtonsArea & buttonsArea;
	StatusWindow & statusWindow;
	BorderWindow & borderWindow;

    private:
	Basic();

	u8 redraw;

	Rect scrollLeft;
	Rect scrollRight;
	Rect scrollBottom;
	Rect scrollTop;
	
	Rect controlRadar;
	Rect controlIcons;
	Rect controlButtons;
	Rect controlStatus;
    };
};

#endif
