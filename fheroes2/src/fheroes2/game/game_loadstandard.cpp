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
#include "engine.h"
#include "button.h"
#include "display.h"
#include "settings.h"
#include "game.h"
#include "SDL.h"

Game::menu_t Game::LoadStandard(void)
{
    Settings::Get().SetGameType(Game::STANDARD);

    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::REQBKG);
    AGG::PreloadObject(ICN::REQUEST);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN(ICN::REQBKG, 0);
    display.Blit(panel, 140, 50);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonOk(174, 365, ICN::REQUEST, 1, 2);
    Button buttonCancel(384, 365, ICN::REQUEST, 3, 4);
    Button buttonPgUp(467, 105, ICN::REQUEST, 5, 6);
    Button buttonPgDn(467, 307, ICN::REQUEST, 7, 8);

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();

    cursor.Show();
    display.Flip();

    // loadstandard loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

	if(le.MouseClickLeft(buttonOk)) return MAINMENU;
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;
	//if(le.MouseClickLeft(buttonPgUp)) return MAINMENU;
	//if(le.MouseClickLeft(buttonPgDn)) return MAINMENU;
    }

    return QUITGAME;
}
