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
#include "game.h"
#include "error.h"

Game::menu_t Game::LoadGame(void)
{
    Mixer::Pause();
    AGG::PlayMusic(MUS::MAINMENU);

    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::BTNNEWGM);
    AGG::PreloadObject(ICN::REDBACK);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN(ICN::REDBACK, 0);
    display.Blit(panel, 405, 5);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonStandartGame(455, 45, ICN::BTNNEWGM, 0, 1);
    Button buttonCampainGame(455, 110, ICN::BTNNEWGM, 2, 3);
    Button buttonMultiGame(455, 175, ICN::BTNNEWGM, 4, 5);
    Button buttonCancelGame(455, 375, ICN::BTNNEWGM, 6, 7);

    buttonStandartGame.Draw();
    buttonCampainGame.Draw();
    buttonMultiGame.Draw();
    buttonCancelGame.Draw();

    cursor.Show();
    display.Flip();

    // loadgame loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonStandartGame) ? buttonStandartGame.PressDraw() : buttonStandartGame.ReleaseDraw();
	le.MousePressLeft(buttonCampainGame) ? buttonCampainGame.PressDraw() : buttonCampainGame.ReleaseDraw();
	le.MousePressLeft(buttonMultiGame) ? buttonMultiGame.PressDraw() : buttonMultiGame.ReleaseDraw();
	le.MousePressLeft(buttonCancelGame) ? buttonCancelGame.PressDraw() : buttonCancelGame.ReleaseDraw();

	if(le.MouseClickLeft(buttonStandartGame)) return LOADSTANDARD;
	if(le.MouseClickLeft(buttonCampainGame)) return LOADCAMPAIN;
	if(le.MouseClickLeft(buttonMultiGame)) return LOADMULTI;
	if(le.MouseClickLeft(buttonCancelGame) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;
    }

    return QUITGAME;
}
