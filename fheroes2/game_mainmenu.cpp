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
#include "dialog.h"
#include "animation.h"
#include "sprite.h"
#include "localevent.h"
#include "display.h"
#include "button.h"
#include "game.h"

#include "error.h"
Game::menu_t Game::MainMenu(void)
{
    // preload
    AGG::PreloadObject("HEROES.ICN");
    AGG::PreloadObject("BTNSHNGL.ICN");
    AGG::PreloadObject("SHNGANIM.ICN");

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    // image background
    const Sprite &sprite = AGG::GetICN("HEROES.ICN", 0);
    display.Blit(sprite);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonNewGame("BTNSHNGL.ICN", 0, 3);
    Button buttonLoadGame("BTNSHNGL.ICN", 4, 7);
    Button buttonHighScores("BTNSHNGL.ICN", 8, 11);
    Button buttonCredits("BTNSHNGL.ICN", 12, 15);
    Button buttonQuit("BTNSHNGL.ICN", 16, 19);

    AnimationButton animeButtonNewGame("BTNSHNGL.ICN", 0, 3);
    AnimationButton animeButtonLoadGame("BTNSHNGL.ICN", 4, 3);
    AnimationButton animeButtonHighScores("BTNSHNGL.ICN", 8, 3);
    AnimationButton animeButtonCredits("BTNSHNGL.ICN", 12, 3);
    AnimationButton animeButtonQuit("BTNSHNGL.ICN", 16, 3);

    Animation animeLantern(Point(0, 0), "SHNGANIM.ICN", 0, 40, Animation::INFINITY | Animation::RING | Animation::LOW);

    display.Flip();
    Cursor::Show();

    // mainmenu loop
    while(1){

	le.HandleEvents();

	le.MousePressLeft(buttonNewGame) ? buttonNewGame.Press() : buttonNewGame.Release();
	le.MousePressLeft(buttonLoadGame) ? buttonLoadGame.Press() : buttonLoadGame.Release();
	le.MousePressLeft(buttonHighScores) ? buttonHighScores.Press() : buttonHighScores.Release();
	le.MousePressLeft(buttonCredits) ? buttonCredits.Press() : buttonCredits.Release();
	le.MousePressLeft(buttonQuit) ? buttonQuit.Press() : buttonQuit.Release();

	le.MouseCursor(buttonNewGame) ? animeButtonNewGame.DrawSprite() : animeButtonNewGame.Reset();
	le.MouseCursor(buttonLoadGame) ? animeButtonLoadGame.DrawSprite() : animeButtonLoadGame.Reset();
	le.MouseCursor(buttonHighScores) ? animeButtonHighScores.DrawSprite() : animeButtonHighScores.Reset();
	le.MouseCursor(buttonCredits) ? animeButtonCredits.DrawSprite() : animeButtonCredits.Reset();
	le.MouseCursor(buttonQuit) ? animeButtonQuit.DrawSprite() : animeButtonQuit.Reset();

	if(le.MouseClickLeft(buttonNewGame)) return NEWGAME;
	if(le.MouseClickLeft(buttonLoadGame)) return LOADGAME;
	if(le.MouseClickLeft(buttonHighScores)) return HIGHSCORES;
	if(le.MouseClickLeft(buttonCredits)) return CREDITS;
	if(le.MouseClickLeft(buttonQuit)) return QUITGAME;

	// right info
	if(le.MousePressRight(buttonQuit)) Dialog::Message("Quit", "Quit Heroes of Might and return to the operating system.", Font::BIG);
	if(le.MousePressRight(buttonLoadGame)) Dialog::Message("Load Game", "Load a previously saved game.", Font::BIG);
	if(le.MousePressRight(buttonCredits)) Dialog::Message("Credits", "View the credits screen.", Font::BIG);
	if(le.MousePressRight(buttonHighScores)) Dialog::Message("High Scores", "View the high score screen.", Font::BIG);
	if(le.MousePressRight(buttonNewGame)) Dialog::Message("New Game", "Start a single or multi-player game.", Font::BIG);
	
	if(le.KeyPress(SDLK_ESCAPE)) return QUITGAME;

	animeLantern.DrawSprite();
    }

    return QUITGAME;
}
