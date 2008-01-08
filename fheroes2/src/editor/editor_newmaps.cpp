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
#include "dialog.h"
#include "cursor.h"
#include "sprite.h"
#include "localevent.h"
#include "button.h"
#include "display.h"
#include "game.h"

Game::menu_t Game::Editor::NewMaps(void)
{
    // preload
    AGG::PreloadObject("BTNESIZE.ICN");
    AGG::PreloadObject("REDBACK.ICN");

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    // image background
    const Sprite &back = AGG::GetICN("EDITOR.ICN", 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN("REDBACK.ICN", 0);
    display.Blit(panel, 405, 5);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonSmall(455, 45, "BTNESIZE.ICN", 0, 1);
    Button buttonMedium(455, 110, "BTNESIZE.ICN", 2, 3);
    Button buttonLarge(455, 175, "BTNESIZE.ICN", 4, 5);
    Button buttonXLarge(455, 240, "BTNESIZE.ICN", 6, 7);
    Button buttonCancel(455, 375, "BTNESIZE.ICN", 8, 9);

    Cursor::Show();
    display.Flip();

    // NewMap loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonSmall) ? buttonSmall.Press() : buttonSmall.Release();
	le.MousePressLeft(buttonMedium) ? buttonMedium.Press() : buttonMedium.Release();
	le.MousePressLeft(buttonLarge) ? buttonLarge.Press() : buttonLarge.Release();
	le.MousePressLeft(buttonXLarge) ? buttonXLarge.Press() : buttonXLarge.Release();
	le.MousePressLeft(buttonCancel) ? buttonCancel.Press() : buttonCancel.Release();

	//if(le.MouseClickLeft(buttonSmall)) return ;
	//if(le.MouseClickLeft(buttonMedium)) return ;
	//if(le.MouseClickLeft(buttonLarge)) return ;
	//if(le.MouseClickLeft(buttonXLarge)) return ;
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(SDLK_ESCAPE)) return MAINMENU;

        // right info
	//if(le.MousePressRight(buttonCancelGame)) Dialog::Message("Quit", "Quit out of the map editor.", Font::BIG);
    }

    return MAINMENU;
}
