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

Game::menu_t Game::NewGame(void){

    // preload
    AGG::PreloadObject("HEROES.ICN");
    AGG::PreloadObject("BTNNEWGM.ICN");
    AGG::PreloadObject("REDBACK.ICN");

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(Display::SMALL);

    // image background
    const Sprite &back = AGG::GetICN("HEROES.ICN", 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN("REDBACK.ICN", 0);
    display.Blit(panel, 405, 5);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonStandartGame(455, 45, "BTNNEWGM.ICN", 0, 1);
    Button buttonCampainGame(455, 110, "BTNNEWGM.ICN", 2, 3);
    Button buttonMultiGame(455, 175, "BTNNEWGM.ICN", 4, 5);
    Button buttonCancelGame(455, 375, "BTNNEWGM.ICN", 6, 7);

    buttonStandartGame.Draw();
    buttonCampainGame.Draw();
    buttonMultiGame.Draw();
    buttonCancelGame.Draw();

    cursor.Show();
    display.Flip();

    // newgame loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonStandartGame) ? buttonStandartGame.PressDraw() : buttonStandartGame.ReleaseDraw();
	le.MousePressLeft(buttonCampainGame) ? buttonCampainGame.PressDraw() : buttonCampainGame.ReleaseDraw();
	le.MousePressLeft(buttonMultiGame) ? buttonMultiGame.PressDraw() : buttonMultiGame.ReleaseDraw();
	le.MousePressLeft(buttonCancelGame) ? buttonCancelGame.PressDraw() : buttonCancelGame.ReleaseDraw();

	if(le.MouseClickLeft(buttonStandartGame)) return NEWSTANDARD;
	if(le.MouseClickLeft(buttonCampainGame)) return NEWCAMPAIN;
	if(le.MouseClickLeft(buttonMultiGame)) return NEWMULTI;
	if(le.MouseClickLeft(buttonCancelGame) || le.KeyPress(SDLK_ESCAPE)) return MAINMENU;

        // right info
	if(le.MousePressRight(buttonStandartGame)) Dialog::Message("Standard Game", "A single player game playing out a single map.", Font::BIG);
	if(le.MousePressRight(buttonCampainGame)) Dialog::Message("Campaign Game", "A single player game playing through a series of maps.", Font::BIG);
	if(le.MousePressRight(buttonMultiGame)) Dialog::Message("Multi-Player Game", "A multi-player game, with several human players completing against each other on a single map.", Font::BIG);
	if(le.MousePressRight(buttonCancelGame)) Dialog::Message("Cancel", "Cancel back to the main menu.", Font::BIG);
		 
    }

    return QUITGAME;
}
