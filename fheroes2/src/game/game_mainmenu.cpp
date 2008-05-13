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
    display.SetVideoMode(640, 480);

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

    const Sprite & lantern10 = AGG::GetICN(ICN::SHNGANIM, 0);
    display.Blit(lantern10, lantern10.x(), lantern10.y());

    const Sprite & lantern11 = AGG::GetICN(ICN::SHNGANIM, ICN::AnimationFrame(ICN::SHNGANIM, 0));
    display.Blit(lantern11, lantern11.x(), lantern11.y());

    buttonNewGame.Draw();
    buttonLoadGame.Draw();
    buttonHighScores.Draw();
    buttonCredits.Draw();
    buttonQuit.Draw();

    cursor.Show();
    display.Flip();

    u32 ticket = 0;
    u32 lantern_frame = 0;
    
    u8 trigger_anime1 = 0;
    u8 trigger_anime2 = 4;
    u8 trigger_anime3 = 8;
    u8 trigger_anime4 = 12;
    u8 trigger_anime5 = 16;

    // mainmenu loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonNewGame) ? buttonNewGame.PressDraw() : buttonNewGame.ReleaseDraw();
	le.MousePressLeft(buttonLoadGame) ? buttonLoadGame.PressDraw() : buttonLoadGame.ReleaseDraw();
	le.MousePressLeft(buttonHighScores) ? buttonHighScores.PressDraw() : buttonHighScores.ReleaseDraw();
	le.MousePressLeft(buttonCredits) ? buttonCredits.PressDraw() : buttonCredits.ReleaseDraw();
	le.MousePressLeft(buttonQuit) ? buttonQuit.PressDraw() : buttonQuit.ReleaseDraw();

	// anime new button
	if(le.MouseCursor(buttonNewGame))
	{
	    if(2 > trigger_anime1)
	    {
		DELAY(ANIMATION_HIGH);
		cursor.Hide();
		const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, ++trigger_anime1);
		display.Blit(sprite, sprite.x(), sprite.y());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	if(0 != trigger_anime1)
	{
	    trigger_anime1 = 0;

	    cursor.Hide();
	    const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, trigger_anime1);
	    display.Blit(sprite, sprite.x(), sprite.y());
	    cursor.Show();
	    display.Flip();
	}

	// anime load button
	if(le.MouseCursor(buttonLoadGame))
	{
	    if(6 > trigger_anime2)
	    {
		DELAY(ANIMATION_HIGH);
		cursor.Hide();
		const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, ++trigger_anime2);
		display.Blit(sprite, sprite.x(), sprite.y());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	if(4 != trigger_anime2)
	{
	    trigger_anime2 = 4;

	    cursor.Hide();
	    const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, trigger_anime2);
	    display.Blit(sprite, sprite.x(), sprite.y());
	    cursor.Show();
	    display.Flip();
	}

	// anime scores button
	if(le.MouseCursor(buttonHighScores))
	{
	    if(10 > trigger_anime3)
	    {
		DELAY(ANIMATION_HIGH);
		cursor.Hide();
		const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, ++trigger_anime3);
		display.Blit(sprite, sprite.x(), sprite.y());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	if(8 != trigger_anime3)
	{
	    trigger_anime3 = 8;

	    cursor.Hide();
	    const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, trigger_anime3);
	    display.Blit(sprite, sprite.x(), sprite.y());
	    cursor.Show();
	    display.Flip();
	}

	// anime credits button
	if(le.MouseCursor(buttonCredits))
	{
	    if(14 > trigger_anime4)
	    {
		DELAY(ANIMATION_HIGH);
		cursor.Hide();
		const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, ++trigger_anime4);
		display.Blit(sprite, sprite.x(), sprite.y());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	if(12 != trigger_anime4)
	{
	    trigger_anime4 = 12;

	    cursor.Hide();
	    const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, trigger_anime4);
	    display.Blit(sprite, sprite.x(), sprite.y());
	    cursor.Show();
	    display.Flip();
	}

	// anime quit button
	if(le.MouseCursor(buttonQuit))
	{
	    if(18 > trigger_anime5)
	    {
		DELAY(ANIMATION_HIGH);
		cursor.Hide();
		const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, ++trigger_anime5);
		display.Blit(sprite, sprite.x(), sprite.y());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	if(16 != trigger_anime5)
	{
	    trigger_anime5 = 16;

	    cursor.Hide();
	    const Sprite & sprite = AGG::GetICN(ICN::BTNSHNGL, trigger_anime5);
	    display.Blit(sprite, sprite.x(), sprite.y());
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(buttonNewGame)) return NEWGAME;
	else
	if(le.MouseClickLeft(buttonLoadGame)) return LOADGAME;
	else
	if(le.MouseClickLeft(buttonHighScores)) return HIGHSCORES;
	else
	if(le.MouseClickLeft(buttonCredits)) return CREDITS;
	else
	if(le.MouseClickLeft(buttonQuit) || le.KeyPress(KEY_ESCAPE)) return QUITGAME;

	// right info
	if(le.MousePressRight(buttonQuit)) Dialog::Message("Quit", "Quit Heroes of Might and return to the operating system.", Font::BIG);
	else
	if(le.MousePressRight(buttonLoadGame)) Dialog::Message("Load Game", "Load a previously saved game.", Font::BIG);
	else
	if(le.MousePressRight(buttonCredits)) Dialog::Message("Credits", "View the credits screen.", Font::BIG);
	else
	if(le.MousePressRight(buttonHighScores)) Dialog::Message("High Scores", "View the high score screen.", Font::BIG);
	else
	if(le.MousePressRight(buttonNewGame)) Dialog::Message("New Game", "Start a single or multi-player game.", Font::BIG);


	if(!(ticket % ANIMATION_LOW))
	{
	    cursor.Hide();
	    const Sprite & lantern12 = AGG::GetICN(ICN::SHNGANIM, ICN::AnimationFrame(ICN::SHNGANIM, 0, lantern_frame++));
	    display.Blit(lantern12, lantern12.x(), lantern12.y());
	    cursor.Show();
	    display.Flip();
	}

	++ticket;
    }

    return QUITGAME;
}
