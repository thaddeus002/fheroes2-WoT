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
#include "config.h"
#include "sprite.h"
#include "game.h"

/* Redraw */
void Game::DrawInterface(void)
{
    Display & display = Display::Get();
    display.Fill(0x00, 0x00, 0x00);

    const ICN::icn_t icnlocator = H2Config::EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS;

    const Sprite & icnston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
    const Sprite & icnadv = AGG::GetICN(H2Config::EvilInterface() ? ICN::ADVBORDE : ICN::ADVBORD, 0);

    Rect srcrt;
    Point dstpt;

    const u8 count_w = (display.w() - 640) / TILEWIDTH;
    const u8 count_h = (display.h() - 480) / TILEWIDTH;

    // TOP BORDER
    srcrt.x = 0;
    srcrt.y = 0;
    srcrt.w = 223;
    srcrt.h = BORDERWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.x = 223;
    srcrt.w = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = 0;
    for(u8 ii = 0; ii < count_w + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.x += TILEWIDTH;
    }
    srcrt.x += TILEWIDTH;
    srcrt.w = icnadv.w() - srcrt.x;
    display.Blit(icnadv, srcrt, dstpt);

    // BOTTOM BORDER
    srcrt.x = 0;
    srcrt.y = icnadv.h() - BORDERWIDTH;
    srcrt.w = 223;
    srcrt.h = BORDERWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = display.h() - BORDERWIDTH;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.x = 223;
    srcrt.w = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = display.h() - BORDERWIDTH;
    for(u8 ii = 0; ii < count_w + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.x += TILEWIDTH;
    }
    srcrt.x += TILEWIDTH;
    srcrt.w = icnadv.w() - srcrt.x;
    display.Blit(icnadv, srcrt, dstpt);

    // LEFT BORDER
    srcrt.x = 0;
    srcrt.y = 0;
    srcrt.w = BORDERWIDTH;
    srcrt.h = 255;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = 255;
    srcrt.h = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    for(u8 ii = 0; ii < count_h + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += TILEWIDTH;
    }
    srcrt.y += TILEWIDTH;
    srcrt.h = icnadv.h() - srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);

    // MIDDLE BORDER
    srcrt.x = icnadv.w() - RADARWIDTH - 2 * BORDERWIDTH;
    srcrt.y = 0;
    srcrt.w = BORDERWIDTH;
    srcrt.h = 255;
    dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = 255;
    srcrt.h = TILEWIDTH;
    dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
    dstpt.y = srcrt.y;
    for(u8 ii = 0; ii < count_h + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += TILEWIDTH;
    }
    srcrt.y += TILEWIDTH;
    srcrt.h = icnadv.h() - srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);

    // RIGHT BORDER
    srcrt.x = icnadv.w() - BORDERWIDTH;
    srcrt.y = 0;
    srcrt.w = BORDERWIDTH;
    srcrt.h = 255;
    dstpt.x = display.w() - BORDERWIDTH;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = 255;
    srcrt.h = TILEWIDTH;
    dstpt.x = display.w() - BORDERWIDTH;
    dstpt.y = srcrt.y;
    for(u8 ii = 0; ii < count_h + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += TILEWIDTH;
    }
    srcrt.y += TILEWIDTH;
    srcrt.h = icnadv.h() - srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);

    // ICON PANEL
    srcrt.x = icnadv.w() - RADARWIDTH - BORDERWIDTH;
    srcrt.y = RADARWIDTH + BORDERWIDTH;
    srcrt.w = RADARWIDTH;
    srcrt.h = 48;
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = srcrt.y + srcrt.h;
    dstpt.y = dstpt.y + srcrt.h;
    srcrt.h = 32;
    const u8 ext_icon = count_h > 3 ? 6 : ( count_h < 3 ? 2 : 5);
    for(u8 ii = 0; ii < ext_icon; ++ii)
    {
	display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += srcrt.h;
    }
    srcrt.y = srcrt.y + 64;
    srcrt.h = 48;
    display.Blit(icnadv, srcrt, dstpt);

    const u16 button_coord = dstpt.y + srcrt.h;

    // ICON PANEL BACKGROUND
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 5;    // background panel heroes
    dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
    for(u8 ii = 0; ii < ext_icon + 2; ++ii)
    {
	display.Blit(AGG::GetICN(icnlocator, ii + 1), dstpt);
            dstpt.y += 32;
    }
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 77;   // background panel castle
    dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
    for(u8 ii = 0; ii < ext_icon + 2; ++ii)
    {
	display.Blit(AGG::GetICN(icnlocator, ii + 1), dstpt);
        dstpt.y += 32;
    }

    // BOTTOM PANEL BACKGROUND
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
    dstpt.y = button_coord + 2 * 36;
    if(display.h() - BORDERWIDTH - icnston.h() > dstpt.y)
    {
        const u8 count_item = (display.h() - BORDERWIDTH - dstpt.y) / TILEWIDTH;
        Rect srcrt;
        srcrt.x = 0;
	srcrt.y = 0;
        srcrt.w = icnston.w();
        srcrt.h = 12;
        display.Blit(icnston, srcrt, dstpt);
        srcrt.y = 12;
        srcrt.h = 32;
        dstpt.y = dstpt.y + 12;
        for(u8 ii = 0; ii < count_item; ++ii)
        {
            display.Blit(icnston, srcrt, dstpt);
            dstpt.y = dstpt.y + TILEWIDTH;
        }
        dstpt.y = display.h() - BORDERWIDTH - 12;
        srcrt.y = icnston.h() - 12;
        display.Blit(icnston, srcrt, dstpt);
    }
    else
	display.Blit(icnston, dstpt);
}
