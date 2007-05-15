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

Game::menu_t Game::Editor::MainMenu(void){

    // preload
    AGG::PreloadObject("EDITOR.ICN");
    AGG::PreloadObject("BTNEMAIN.ICN");
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

    Button buttonNewMap(455, 45, "BTNEMAIN.ICN", 0, 1);
    Button buttonLoadMap(455, 110, "BTNEMAIN.ICN", 2, 3);
    Button buttonCancelGame(455, 375, "BTNEMAIN.ICN", 6, 7);

    display.Flip();
    Cursor::Show();

    // NewMap loop
    while(1){

	le.HandleEvents();

	le.MousePressLeft(buttonNewMap) ? buttonNewMap.Press() : buttonNewMap.Release();
	le.MousePressLeft(buttonLoadMap) ? buttonLoadMap.Press() : buttonLoadMap.Release();
	le.MousePressLeft(buttonCancelGame) ? buttonCancelGame.Press() : buttonCancelGame.Release();

	if(le.MouseClickLeft(buttonNewMap)) return EDITNEWMAP;
	if(le.MouseClickLeft(buttonLoadMap)) return EDITLOADMAP;
	if(le.MouseClickLeft(buttonCancelGame)) return QUITGAME;

        // right info
	//if(le.MousePressRight(buttonNewMap)) Dialog::Message("Standard Game", "A single player game playing out a single map.", Font::BIG);
	//if(le.MousePressRight(buttonLoadMap)) Dialog::Message("Campaign Game", "A single player game playing through a series of maps.", Font::BIG);
	if(le.MousePressRight(buttonCancelGame)) Dialog::Message("Cancel", "Cancel back to the main menu.", Font::BIG);
		 
    }

    return QUITGAME;
}
