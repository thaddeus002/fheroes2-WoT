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
#include "engine.h"
#include "button.h"
#include "game.h"
#include "audio_interface.h"
#include "SDL.h"

u8 SelectCountPlayers(void);

Game::menu_t Game::NewGame(void)
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
	if(le.MouseClickLeft(buttonCancelGame) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;

        // right info
	if(le.MousePressRight(buttonStandartGame)) Dialog::Message("Standard Game", "A single player game playing out a single map.", Font::BIG);
	if(le.MousePressRight(buttonCampainGame)) Dialog::Message("Campaign Game", "A single player game playing through a series of maps.", Font::BIG);
	if(le.MousePressRight(buttonMultiGame)) Dialog::Message("Multi-Player Game", "A multi-player game, with several human players completing against each other on a single map.", Font::BIG);
	if(le.MousePressRight(buttonCancelGame)) Dialog::Message("Cancel", "Cancel back to the main menu.", Font::BIG);
		 
    }

    return QUITGAME;
}

Game::menu_t Game::NewMulti(void)
{
    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::BTNHOTST);
    AGG::PreloadObject(ICN::REDBACK);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    Settings & conf = Settings::Get();

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);

    const Sprite &panel = AGG::GetICN(ICN::REDBACK, 0);
    display.Blit(panel, 405, 5);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonHotSeat(455, 45, ICN::BTNMP, 0, 1);
    Button buttonNetwork(455, 110, ICN::BTNMP, 2, 3);
    Button buttonCancelGame(455, 375, ICN::BTNMP, 8, 9);

    buttonHotSeat.Draw();
    buttonNetwork.Draw();
    buttonCancelGame.Draw();

    cursor.Show();
    display.Flip();

    // newgame loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonHotSeat) ? buttonHotSeat.PressDraw() : buttonHotSeat.ReleaseDraw();
	le.MousePressLeft(buttonNetwork) ? buttonNetwork.PressDraw() : buttonNetwork.ReleaseDraw();
	le.MousePressLeft(buttonCancelGame) ? buttonCancelGame.PressDraw() : buttonCancelGame.ReleaseDraw();

	if(le.MouseClickLeft(buttonHotSeat))
	{
	    conf.SetPreferablyCountPlayers(SelectCountPlayers());
	    return conf.PreferablyCountPlayers() ? NEWHOTSEAT : MAINMENU;
	}
	if(le.MouseClickLeft(buttonNetwork))
	{
	    conf.SetPreferablyCountPlayers(SelectCountPlayers());
	    return conf.PreferablyCountPlayers() ? NEWNETWORK : MAINMENU;
	}
	if(le.MouseClickLeft(buttonCancelGame) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;

        // right info
	if(le.MousePressRight(buttonHotSeat)) Dialog::Message("Hot Seat", "Play a Hot Seat game, where 2 to 4 players play around the same computer, switching into the 'Hot Seat' when it is their turn.", Font::BIG);
	if(le.MousePressRight(buttonNetwork)) Dialog::Message("Network", "Play a network game, where 2 players use their own computers connected through a LAN (Local Area Network).", Font::BIG);
	if(le.MousePressRight(buttonCancelGame)) Dialog::Message("Cancel", "Cancel back to the main menu.", Font::BIG);
		 
    }

    return QUITGAME;
}

u8 SelectCountPlayers(void)
{
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

    Button button2Players(455, 45, ICN::BTNHOTST, 0, 1);
    Button button3Players(455, 110, ICN::BTNHOTST, 2, 3);
    Button button4Players(455, 175, ICN::BTNHOTST, 4, 5);
    Button button5Players(455, 240, ICN::BTNHOTST, 6, 7);
    Button button6Players(455, 305, ICN::BTNHOTST, 8, 9);
    Button buttonCancel(455, 375, ICN::BTNNEWGM, 6, 7);

    button2Players.Draw();
    button3Players.Draw();
    button4Players.Draw();
    button5Players.Draw();
    button6Players.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    // newgame loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(button2Players) ? button2Players.PressDraw() : button2Players.ReleaseDraw();
	le.MousePressLeft(button3Players) ? button3Players.PressDraw() : button3Players.ReleaseDraw();
	le.MousePressLeft(button4Players) ? button4Players.PressDraw() : button4Players.ReleaseDraw();
	le.MousePressLeft(button5Players) ? button5Players.PressDraw() : button5Players.ReleaseDraw();
	le.MousePressLeft(button6Players) ? button6Players.PressDraw() : button6Players.ReleaseDraw();

	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	if(le.MouseClickLeft(button2Players)) return 2;
	if(le.MouseClickLeft(button3Players)) return 3;
	if(le.MouseClickLeft(button4Players)) return 4;
	if(le.MouseClickLeft(button5Players)) return 5;
	if(le.MouseClickLeft(button6Players)) return 6;

	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE)) return 0;

        // right info
	if(le.MousePressRight(button2Players)) Dialog::Message("2 Players", "Play with 2 human players, and optionally, up, to 4 additional computer players.", Font::BIG);
	if(le.MousePressRight(button3Players)) Dialog::Message("3 Players", "Play with 3 human players, and optionally, up, to 3 additional computer players.", Font::BIG);
	if(le.MousePressRight(button4Players)) Dialog::Message("4 Players", "Play with 4 human players, and optionally, up, to 2 additional computer players.", Font::BIG);
	if(le.MousePressRight(button5Players)) Dialog::Message("5 Players", "Play with 5 human players, and optionally, up, to 1 additional computer players.", Font::BIG);
	if(le.MousePressRight(button6Players)) Dialog::Message("6 Players", "Play with 6 human players.", Font::BIG);
	if(le.MousePressRight(buttonCancel)) Dialog::Message("Cancel", "Cancel back to the main menu.", Font::BIG);
    }

    return 0;
}
