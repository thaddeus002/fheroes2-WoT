/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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
#include "settings.h"
#include "text.h"
#include "pocketpc.h"

Game::menu_t PocketPC::LoadGame(void)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite = AGG::GetICN(ICN::HEROES, 0);
    Rect src_rt((sprite.w() - display.w()) / 2, 0, display.w(), display.h());
    display.Blit(sprite, src_rt, 0, 0);

    cursor.Show();
    display.Flip();

    std::string file;
    if(!Dialog::SelectFileLoad(file) || file.empty()) return Game::MAINMENU;
    Game::Load(file);

    return Game::STARTGAME;
}

Game::menu_t PocketPC::MainMenu(void)
{
    Game::SetFixVideoMode();

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite = AGG::GetICN(ICN::HEROES, 0);
    Rect src_rt((sprite.w() - display.w()) / 2, 0, display.w(), display.h());
    display.Blit(sprite, src_rt, 0, 0);

    const Sprite &board = AGG::GetICN(ICN::QWIKTOWN, 0);
    src_rt = Rect(13, 0, board.w() - 13, board.h() - 13);
    Point dst_pt((display.w() - src_rt.w) / 2, (display.h() - src_rt.h) / 2);
    display.Blit(board, src_rt, dst_pt.x , dst_pt.y);

    Text text;
    
    text.Set("Free Heroes II", Font::BIG);
    text.Blit(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 10);

    text.Set(conf.BuildVersion(), Font::SMALL);
    text.Blit(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 148);

    text.Set(_("New Game"), Font::BIG);
    const Rect rectNewGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 40, text.w(), text.h());
    text.Blit(rectNewGame);

    text.Set(_("Load Game"));
    const Rect rectLoadGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 70, text.w(), text.h());
    text.Blit(rectLoadGame);

    text.Set(_("Quit"));
    const Rect rectQuitGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 100, text.w(), text.h());
    text.Blit(rectQuitGame);

    cursor.Show();
    display.Flip();

    // mainmenu loop
    while(le.HandleEvents())
    {
	if(le.KeyPress(KEY_n) || le.MouseClickLeft(rectNewGame)) return Game::SELECTSCENARIO;
	else
	if(le.KeyPress(KEY_l) || le.MouseClickLeft(rectLoadGame)) return Game::LOADGAME;
	else
	if(le.KeyPress(KEY_q) || le.MouseClickLeft(rectQuitGame) || le.KeyPress(KEY_ESCAPE)) return Game::QUITGAME;
    }

    return Game::QUITGAME;
}
