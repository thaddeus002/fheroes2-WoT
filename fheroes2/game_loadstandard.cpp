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

#include "agg.h"
#include "cursor.h"
#include "sprite.h"
#include "localevent.h"
#include "button.h"
#include "display.h"
#include "game.h"

Game::menu_t Game::LoadStandard(void){

    // preload
    AGG::PreloadObject("HEROES.ICN");
    AGG::PreloadObject("REQBKG.ICN");
    AGG::PreloadObject("REQUEST.ICN");

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    // image background
    const Sprite &back = AGG::GetICN("HEROES.ICN", 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN("REQBKG.ICN", 0);
    display.Blit(panel, 140, 50);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonOk(174, 365, "REQUEST.ICN", 1, 2);
    Button buttonCancel(384, 365, "REQUEST.ICN", 3, 4);
    Button buttonPgUp(467, 105, "REQUEST.ICN", 5, 6);
    Button buttonPgDn(467, 307, "REQUEST.ICN", 7, 8);
    display.Flip();
    Cursor::Show();

    // loadstandard loop
    while(1){

	le.HandleEvents();

	le.MousePressLeft(buttonOk) ? buttonOk.Press() : buttonOk.Release();
	le.MousePressLeft(buttonCancel) ? buttonCancel.Press() : buttonCancel.Release();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.Press() : buttonPgUp.Release();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.Press() : buttonPgDn.Release();

	if(le.MouseClickLeft(buttonOk)) return MAINMENU;
	if(le.MouseClickLeft(buttonCancel)) return MAINMENU;
	//if(le.MouseClickLeft(buttonPgUp)) return MAINMENU;
	//if(le.MouseClickLeft(buttonPgDn)) return MAINMENU;
    }

    return QUITGAME;
}
