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

#ifndef H2EDITORINTERFACE_H
#define H2EDITORINTERFACE_H

#include "gamedefs.h"
#include "button.h"
#include "splitter.h"
#include "game.h"

namespace Game
{
namespace Editor
{
    class Interface
    {
    public:
    	static Interface & Get(void);

	static void	DrawTopNumberCell(void);
	static void	DrawLeftNumberCell(void);

	void	Draw(void);
	void	Scroll(const u8 scroll);

	Button btnLeftTopScroll;
        Button btnRightTopScroll;
        Button btnTopScroll;
        Button btnLeftBottomScroll;
        Button btnLeftScroll;
        Button btnRightScroll;
        Button btnRightBottomScroll;
        Button btnBottomScroll;
        Button btnSelectGround;
        Button btnSelectObject;
        Button btnSelectInfo;
        Button btnSelectRiver;
        Button btnSelectRoad;
        Button btnSelectClear;
        Button btnSizeSmall;
        Button btnSizeMedium;
        Button btnSizeLarge;
        Button btnSizeManual;
        Button btnZoom;
        Button btnUndo;
        Button btnNew;
        Button btnSpec;
        Button btnFile;
        Button btnSystem;

	Splitter split_h;
	Splitter split_v;

    private:
	Interface();
    };
};
};

#endif
