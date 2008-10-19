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
#include "localevent.h"
#include "display.h"
#include "button.h"
#include "game.h"

Game::menu_t Game::HighScores(void)
{
    // preload
    AGG::PreloadObject(ICN::HSBKG);
    AGG::PreloadObject(ICN::HISCORE);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    // image background
    const Sprite &back = AGG::GetICN(ICN::HSBKG, 0);
    display.Blit(back);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonDismiss(9, 315, ICN::HISCORE, 0, 1);
    Button buttonExit(604, 315, ICN::HISCORE, 4, 5);

    buttonDismiss.Draw();
    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    // highscores loop
    while(le.HandleEvents())
    {
        AGG::PlayMusic(MUS::MAINMENU);
    
	le.MousePressLeft(buttonDismiss) ? buttonDismiss.PressDraw() : buttonDismiss.ReleaseDraw();
	le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

	//if(le.MouseRight()) Error::Verbose(le.MousePressRight());

	//if(le.MouseClickLeft(buttonDismiss)) return MAINMENU;
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_ESCAPE)) return MAINMENU;
    }

    return QUITGAME;
}
