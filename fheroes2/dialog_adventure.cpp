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

Dialog::answer_t Dialog::AdventureOptions(void)
{
    // preload
    const std::string &apanbkg = H2Config::EvilInterface() ? "APANBKGE.ICN" : "APANBKG.ICN";
    const std::string &apanel  = H2Config::EvilInterface() ? "APANELE.ICN" : "APANEL.ICN";

    AGG::PreloadObject(apanbkg);
    AGG::PreloadObject(apanel);

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);
    Cursor::themes_t cursor = Cursor::Get();

    // image box
    const Sprite &box = AGG::GetICN(apanbkg, 0);

    Rect rb((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    Background back(rb);
    back.Save();

    display.Blit(box, rb);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonWorld(rb.x + 62, rb.y + 30, apanel, 0, 1);
    Button buttonPuzzle(rb.x + 195, rb.y + 30, apanel, 2, 3);
    Button buttonInfo(rb.x + 62, rb.y + 107, apanel, 4, 5);
    Button buttonDig(rb.x + 195, rb.y + 107, apanel, 6, 7);
    Button buttonCancel(rb.x + 128, rb.y + 184, apanel, 8, 9);

    display.Flip();
    Cursor::Show();

    Dialog::answer_t result = Dialog::ZERO;
    bool exit = false;

    // dialog menu loop
    while(! exit){

        le.HandleEvents();

        le.MousePressLeft(buttonWorld) ? buttonWorld.Press() : buttonWorld.Release();
        le.MousePressLeft(buttonPuzzle) ? buttonPuzzle.Press() : buttonPuzzle.Release();
        le.MousePressLeft(buttonInfo) ? buttonInfo.Press() : buttonInfo.Release();
        le.MousePressLeft(buttonDig) ? buttonDig.Press() : buttonDig.Release();
        le.MousePressLeft(buttonCancel) ? buttonCancel.Press() : buttonCancel.Release();

        if(le.MouseClickLeft(buttonWorld)) { result = Dialog::CANCEL; exit = true; }
        if(le.MouseClickLeft(buttonPuzzle)){ result = Dialog::CANCEL; exit = true; }
        if(le.MouseClickLeft(buttonInfo)){ result = Dialog::CANCEL; exit = true; }
        if(le.MouseClickLeft(buttonDig)){ result = Dialog::CANCEL; exit = true; }
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(SDLK_ESCAPE)){ result = Dialog::CANCEL; exit = true; }

    }

    le.ResetKey();

    // restore background
    Cursor::Hide();
    back.Restore();
    Cursor::Set(cursor);
    Cursor::Show();
    display.Flip();

    return result;
}
