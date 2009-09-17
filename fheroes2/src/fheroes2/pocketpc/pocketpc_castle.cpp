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
#include "button.h"
#include "castle.h"
#include "selectarmybar.h"
#include "pocketpc.h"

Dialog::answer_t PocketPC::CastleOpenDialog(Castle & castle)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
//    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const u16 window_w = 320;
    const u16 window_h = 224;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH, (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw();

    const Rect & dst_rt = frameborder.GetArea();
    const Sprite & backSprite = AGG::GetICN(ICN::SWAPWIN, 0);
    const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
    display.Blit(background, Rect(0, 0, window_w, window_h), dst_rt);

    Text text;
    text.Set(castle.GetName(), Font::SMALL);
    text.Blit(dst_rt.x + (dst_rt.w - text.w()) / 2, dst_rt.y + 3);

    text.Set("Under Construction", Font::BIG);
    text.Blit(dst_rt.x + (dst_rt.w - text.w()) / 2, dst_rt.y + 60);
    
    // crest
    display.Blit(AGG::GetICN(ICN::BRCREST, 6), dst_rt.x + 2, dst_rt.y + 112);
    display.Blit(AGG::GetICN(ICN::BRCREST, Color::GetIndex(castle.GetColor())), dst_rt.x + 6, dst_rt.y + 116);

    display.Blit(AGG::GetICN(ICN::BRCREST, 6), dst_rt.x + 2, dst_rt.y + 167);
    display.Blit(AGG::GetICN(ICN::BRCREST, Color::GetIndex(castle.GetColor())), dst_rt.x + 6, dst_rt.y + 171);

    // army bar1
    const Rect rt1(36, 267, 43, 53);
    Surface sfb1(rt1.w, rt1.h);
    sfb1.Blit(backSprite, rt1, 0, 0);
    Surface sfc1(rt1.w, rt1.h - 10);
    Cursor::DrawCursor(sfc1, 0x10, true);
                        
    SelectArmyBar selectCastleArmy;
    selectCastleArmy.SetArmy(castle.GetArmy());
    selectCastleArmy.SetPos(dst_rt.x + 62, dst_rt.y + 112);
    selectCastleArmy.SetInterval(2);
    selectCastleArmy.SetBackgroundSprite(sfb1);
    selectCastleArmy.SetCursorSprite(sfc1);
    selectCastleArmy.SetUseMons32Sprite();
    selectCastleArmy.SetCastle(castle);
    selectCastleArmy.Redraw();

    Button buttonExit(dst_rt.x + dst_rt.w / 2 + 130, dst_rt.y + dst_rt.h - 125, ICN::HSBTNS, 2, 3);
    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        // exit
        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_ESCAPE)) return Dialog::CANCEL;
    }

    return Dialog::ZERO;
}
