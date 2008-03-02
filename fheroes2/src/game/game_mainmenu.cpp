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

Game::menu_t Game::MainMenu(void)
{
    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::BTNSHNGL);
    AGG::PreloadObject(ICN::SHNGANIM);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(Display::SMALL);

    // image background
    const Sprite &sprite = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(sprite);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonNewGame(ICN::BTNSHNGL, 0, 3);
    Button buttonLoadGame(ICN::BTNSHNGL, 4, 7);
    Button buttonHighScores(ICN::BTNSHNGL, 8, 11);
    Button buttonCredits(ICN::BTNSHNGL, 12, 15);
    Button buttonQuit(ICN::BTNSHNGL, 16, 19);

    const Point lt_pt(0, 0);
    Animation animeButtonNewGame(lt_pt, ICN::BTNSHNGL, 0, 3, false, Animation::HIGH);
    Animation animeButtonLoadGame(lt_pt, ICN::BTNSHNGL, 4, 3, false, Animation::HIGH);
    Animation animeButtonHighScores(lt_pt, ICN::BTNSHNGL, 8, 3, false, Animation::HIGH);
    Animation animeButtonCredits(lt_pt, ICN::BTNSHNGL, 12, 3, false, Animation::HIGH);
    Animation animeButtonQuit(lt_pt, ICN::BTNSHNGL, 16, 3, false, Animation::HIGH);

    Animation animeLantern(lt_pt, ICN::SHNGANIM, 0, 40, true, Animation::INFINITY | Animation::RING | Animation::LOW);

    buttonNewGame.Draw();
    buttonLoadGame.Draw();
    buttonHighScores.Draw();
    buttonCredits.Draw();
    buttonQuit.Draw();

    cursor.Show();
    display.Flip();

    u32 ticket = 0;

    // mainmenu loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonNewGame) ? buttonNewGame.PressDraw() : buttonNewGame.ReleaseDraw();
	le.MousePressLeft(buttonLoadGame) ? buttonLoadGame.PressDraw() : buttonLoadGame.ReleaseDraw();
	le.MousePressLeft(buttonHighScores) ? buttonHighScores.PressDraw() : buttonHighScores.ReleaseDraw();
	le.MousePressLeft(buttonCredits) ? buttonCredits.PressDraw() : buttonCredits.ReleaseDraw();
	le.MousePressLeft(buttonQuit) ? buttonQuit.PressDraw() : buttonQuit.ReleaseDraw();

	if((le.MouseCursor(buttonNewGame) ||
	   (!le.MouseCursor(buttonNewGame) && animeButtonNewGame.Reset())) &&
	   animeButtonNewGame.DrawSprite(ticket)) display.Flip();

	if((le.MouseCursor(buttonNewGame) ||
	   (!le.MouseCursor(buttonNewGame) && animeButtonNewGame.Reset()))  && 
	   animeButtonNewGame.DrawSprite(ticket)) display.Flip();

	if((le.MouseCursor(buttonLoadGame) ||
	  (!le.MouseCursor(buttonLoadGame) && animeButtonLoadGame.Reset())) &&
	  animeButtonLoadGame.DrawSprite(ticket)) display.Flip();

	if((le.MouseCursor(buttonHighScores) ||
	  (!le.MouseCursor(buttonHighScores) && animeButtonHighScores.Reset())) &&
	  animeButtonHighScores.DrawSprite(ticket)) display.Flip();

	if((le.MouseCursor(buttonCredits) ||
	  (!le.MouseCursor(buttonCredits) && animeButtonCredits.Reset())) &&
	  animeButtonCredits.DrawSprite(ticket)) display.Flip();

	if((le.MouseCursor(buttonQuit) ||
	  (!le.MouseCursor(buttonQuit) && animeButtonQuit.Reset())) &&
	  animeButtonQuit.DrawSprite(ticket)) display.Flip();

	if(le.MouseClickLeft(buttonNewGame)) return NEWGAME;
	if(le.MouseClickLeft(buttonLoadGame)) return LOADGAME;
	if(le.MouseClickLeft(buttonHighScores)) return HIGHSCORES;
	if(le.MouseClickLeft(buttonCredits)) return CREDITS;
	if(le.MouseClickLeft(buttonQuit) || le.KeyPress(KEY_ESCAPE)) return QUITGAME;

	// right info
	if(le.MousePressRight(buttonQuit)) Dialog::Message("Quit", "Quit Heroes of Might and return to the operating system.", Font::BIG);
	if(le.MousePressRight(buttonLoadGame)) Dialog::Message("Load Game", "Load a previously saved game.", Font::BIG);
	if(le.MousePressRight(buttonCredits)) Dialog::Message("Credits", "View the credits screen.", Font::BIG);
	if(le.MousePressRight(buttonHighScores)) Dialog::Message("High Scores", "View the high score screen.", Font::BIG);
	if(le.MousePressRight(buttonNewGame)) Dialog::Message("New Game", "Start a single or multi-player game.", Font::BIG);

	if(animeLantern.DrawSprite(ticket)) display.Flip();
	
	++ticket;
    }

    return QUITGAME;
}
