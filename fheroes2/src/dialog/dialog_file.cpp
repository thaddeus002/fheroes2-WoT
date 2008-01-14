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
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "background.h"
#include "dialog.h"

Game::menu_t Dialog::FileOptions(void)
{
    // preload
    const std::string &cpanbkg = H2Config::EvilInterface() ? "CPANBKGE.ICN" : "CPANBKG.ICN";
    const std::string &cpanel  = H2Config::EvilInterface() ? "CPANELE.ICN" : "CPANEL.ICN";

    AGG::PreloadObject(cpanbkg);
    AGG::PreloadObject(cpanel);

    // cursor
    const Cursor::themes_t cursor = Cursor::Get();
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    // image box
    const Sprite &box = AGG::GetICN(cpanbkg, 0);

    Rect rb((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    Background back(rb);
    back.Save();

    display.Blit(box, rb.x, rb.y);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonNew(rb.x + 62, rb.y + 31, cpanel, 0, 1);
    Button buttonLoad(rb.x + 195, rb.y + 31, cpanel, 2, 3);
    Button buttonSave(rb.x + 62, rb.y + 107, cpanel, 4, 5);
    Button buttonQuit(rb.x + 195, rb.y + 107, cpanel, 6, 7);
    Button buttonCancel(rb.x + 128, rb.y + 184, cpanel, 8, 9);

    buttonNew.Draw();
    buttonLoad.Draw();
    buttonSave.Draw();
    buttonQuit.Draw();
    buttonCancel.Draw();

    Cursor::Show();
    display.Flip();

    Game::menu_t result = Game::QUITGAME;

    // dialog menu loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonNew) ? buttonNew.PressDraw() : buttonNew.ReleaseDraw();
        le.MousePressLeft(buttonLoad) ? buttonLoad.PressDraw() : buttonLoad.ReleaseDraw();
        le.MousePressLeft(buttonSave) ? buttonSave.PressDraw() : buttonSave.ReleaseDraw();
        le.MousePressLeft(buttonQuit) ? buttonQuit.PressDraw() : buttonQuit.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

        if(le.MouseClickLeft(buttonNew)) { result = Game::NEWGAME;  break; }
        if(le.MouseClickLeft(buttonLoad)){ result = Game::LOADGAME; break; }
        if(le.MouseClickLeft(buttonSave)){ result = Game::SAVEGAME; break; }
        if(le.MouseClickLeft(buttonQuit)){ result = Game::QUITGAME; break; }
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(SDLK_ESCAPE)){ result = Game::CANCEL; break; }

	if(le.MousePressRight(buttonNew)) Dialog::Message("", "Start a single or multi-player game.", Font::BIG);
	else
	if(le.MousePressRight(buttonLoad)) Dialog::Message("", "Load a previously saved game.", Font::BIG);
	else
	if(le.MousePressRight(buttonSave)) Dialog::Message("", "Save the current game.", Font::BIG);
	else
	if(le.MousePressRight(buttonQuit)) Dialog::Message("", "Quit out of Heroes of Might and Magick II.", Font::BIG);
	else
	if(le.MousePressRight(buttonCancel)) Dialog::Message("", "Exit this menu without doing anything.", Font::BIG);
    }

    // restore background
    Cursor::Hide();
    back.Restore();
    Cursor::Set(cursor);
    Cursor::Show();
    display.Flip();

    return result;
}
