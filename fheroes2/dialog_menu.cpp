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
#include "event.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "background.h"
#include "dialog.h"

Game::menu_t Dialog::Menu(void)
{
    // preload
    const std::string &cpanbkg = H2Config::EvilInterface() ? "CPANBKGE.ICN" : "CPANBKG.ICN";
    const std::string &cpanel  = H2Config::EvilInterface() ? "CPANELE.ICN" : "CPANEL.ICN";

    AGG::PreloadObject(cpanbkg);
    AGG::PreloadObject(cpanel);

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    // image box
    const Sprite &box = AGG::GetICN(cpanbkg, 0);

    Rect rb((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    Background back(rb);
    back.Save();

    display.Blit(box, rb);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonNew(rb.x + 62, rb.y + 31, cpanel, 0, 1);
    Button buttonLoad(rb.x + 195, rb.y + 31, cpanel, 2, 3);
    Button buttonSave(rb.x + 62, rb.y + 107, cpanel, 4, 5);
    Button buttonQuit(rb.x + 195, rb.y + 107, cpanel, 6, 7);
    Button buttonCancel(rb.x + 128, rb.y + 184, cpanel, 8, 9);

    display.Flip();
    Cursor::Show();

    Game::menu_t result = Game::QUITGAME;
    bool exit = false;

    // dialog menu loop
    while(! exit){

        le.HandleEvents();

        le.MousePressLeft(buttonNew) ? buttonNew.Press() : buttonNew.Release();
        le.MousePressLeft(buttonLoad) ? buttonLoad.Press() : buttonLoad.Release();
        le.MousePressLeft(buttonSave) ? buttonSave.Press() : buttonSave.Release();
        le.MousePressLeft(buttonQuit) ? buttonQuit.Press() : buttonQuit.Release();
        le.MousePressLeft(buttonCancel) ? buttonCancel.Press() : buttonCancel.Release();

        if(le.MouseClickLeft(buttonNew)) { result = Game::NEWGAME;  exit = true; }
        if(le.MouseClickLeft(buttonLoad)){ result = Game::LOADGAME; exit = true; }
        if(le.MouseClickLeft(buttonSave)){ result = Game::SAVEGAME; exit = true; }
        if(le.MouseClickLeft(buttonQuit)){ result = Game::QUITGAME; exit = true; }
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(SDLK_ESCAPE)){ result = Game::CANCEL; exit = true; }

    }

    // restore background
    Cursor::Hide();
    back.Restore();
    display.Flip();
    Cursor::Show();

    return result;
}
