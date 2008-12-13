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

#include <string>
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "world.h"
#include "settings.h"
#include "dialog.h"

void Dialog::PuzzleMaps(void)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN(ICN::STONEBAK, 0), dst_pt);

    const Surface & sf = world.GetUltimateArtifactArea();
    //const u8 open_puzzle = world.CountCapturedObject(MP2::OBJ_OBELISK, Settings::Get().MyColor());
    //const u8 max_obelisk = world.CountObeliskOnMaps();

    dst_pt.x = cur_pt.x + (640 - sf.w())/2;
    dst_pt.y = cur_pt.y + (480 - sf.h())/2;
    display.Blit(sf, dst_pt);

    // draw border around 

    // draw puzzle


    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
    }
}
