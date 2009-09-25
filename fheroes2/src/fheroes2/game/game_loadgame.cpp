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

#include "gamedefs.h"
#include "agg.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"
#include "settings.h"
#include "pocketpc.h"
#include "game.h"

Game::menu_t Game::LoadCampain(void)
{
    Settings::Get().SetGameType(Game::CAMPAIGN);
    Error::Verbose("Load Campain Game: under construction.");
    return Game::LOADGAME;
}

Game::menu_t Game::LoadMulti(void)
{
    Error::Verbose("Load Multi Game: under construction.");
    return Game::LOADGAME;
}

Game::menu_t Game::LoadGame(void)
{
    Mixer::Pause();
    AGG::PlayMusic(MUS::MAINMENU);

    if(Settings::Get().PocketPC()) return PocketPC::LoadGame();
    
    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::BTNNEWGM);
    AGG::PreloadObject(ICN::REDBACK);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    const Point top((display.w() - back.w()) / 2, (display.h() - back.h()) / 2);
    display.Blit(back, top);

    const Sprite &panel = AGG::GetICN(ICN::REDBACK, 0);
    display.Blit(panel, top.x + 405, top.y + 5);

    LocalEvent & le = LocalEvent::Get();

    Button buttonStandartGame(top.x + 455, top.y + 45, ICN::BTNNEWGM, 0, 1);
    Button buttonCampainGame(top.x + 455, top.y + 110, ICN::BTNNEWGM, 2, 3);
    Button buttonMultiGame(top.x + 455, top.y + 175, ICN::BTNNEWGM, 4, 5);
    Button buttonCancelGame(top.x + 455, top.y + 375, ICN::BTNNEWGM, 6, 7);

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

	if(le.KeyPress(KEY_s) || le.MouseClickLeft(buttonStandartGame)) return LOADSTANDARD;
	if(le.KeyPress(KEY_c) || le.MouseClickLeft(buttonCampainGame)) return LOADCAMPAIN;
	if(le.KeyPress(KEY_m) || le.MouseClickLeft(buttonMultiGame)) return LOADMULTI;
	if(le.MouseClickLeft(buttonCancelGame) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;
    }

    return QUITGAME;
}

Game::menu_t Game::LoadStandard(void)
{
    Settings::Get().SetGameType(Game::STANDARD);

    // preload
    AGG::PreloadObject(ICN::HEROES);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    const Point top((display.w() - back.w()) / 2, (display.h() - back.h()) / 2);
    display.Blit(back, top);

    cursor.Show();
    display.Flip();

    std::string file;

    if(!Dialog::SelectFileLoad(file) || file.empty()) return MAINMENU;

    Game::Load(file);

    return STARTGAME;
}
