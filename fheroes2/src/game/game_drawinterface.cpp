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
    display.SetVideoMode(H2Config::GetVideoMode());
    display.Fill(0x00, 0x00, 0x00);

    const std::string &icnadv = ( H2Config::EvilInterface() ? "ADVBORDE.ICN" : "ADVBORD.ICN" );
    const std::string &icnlocator = ( H2Config::EvilInterface() ? "LOCATORE.ICN" : "LOCATORS.ICN" );
    const std::string &icnston = ( H2Config::EvilInterface() ? "STONBAKE.ICN" : "STONBACK.ICN" );
    const std::string &icnbtn = ( H2Config::EvilInterface() ? "ADVEBTNS.ICN" : "ADVBTNS.ICN" );
    const std::string &icnscroll = ( H2Config::EvilInterface() ? "SCROLLE.ICN" : "SCROLL.ICN" );

    Rect srcrt;
    Point dstpt;

    // sprite portret heroes scroll up
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    dstpt.y = RADARWIDTH + 2 * BORDERWIDTH;
    display.Blit(AGG::GetICN(icnscroll, 0), dstpt);
    // sprite portret castle scroll up
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    display.Blit(AGG::GetICN(icnscroll, 0), dstpt);

    switch(H2Config::GetVideoMode()){

	default:
	    dstpt.x = 0;
	    dstpt.y = 0;
	    display.Blit(AGG::GetICN(icnadv, 0), dstpt);

            // TOP PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 5;    // background panel.heroes
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 4; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 77;   // background panel.castle
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 4; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }

            // BOTTOM PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = 392;
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
	    
	    // sprite button next hero
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 320;
	    display.Blit(AGG::GetICN(icnbtn, 0), dstpt);
	    // sprite button action
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 2), dstpt);
	    // sprite button castle
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 4), dstpt);
	    // sprite button magic
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 6), dstpt);
	    // sprite button end tur
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 320 + AGG::GetICN(icnbtn, 0).h();
	    display.Blit(AGG::GetICN(icnbtn, 8), dstpt);
	    // sprite button info
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 10), dstpt);
	    // sprite button option
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 12), dstpt);
	    // sprite button settings
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 14), dstpt);

	    // sprite portret heroes scroll down
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            dstpt.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 4 - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    // sprite portret castle scroll down
            dstpt.x = display.w() - BORDERWIDTH - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    break;
	    
	case Display::MEDIUM:

            // TOP BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = 196;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 80;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // BOTTOM BORDER
            srcrt.x = 0;
            srcrt.y = AGG::GetICN(icnadv, 0).h() - BORDERWIDTH;
            srcrt.w = 196;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 80;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
	    dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
    	    dstpt.x += srcrt.w;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // LEFT BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 176;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // RIGHT BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 176;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // MIDDLE BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - 2 * BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 176;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // TOP PANEL
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - BORDERWIDTH;
            srcrt.y = RADARWIDTH + BORDERWIDTH;
            srcrt.w = RADARWIDTH;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.y += 32;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);

            // TOP PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 5;    // background panel heroes
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 7; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 77;   // background panel castle
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 7; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }

            // BOTTOM PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = 488;
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);

	    // sprite button next hero
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 416;
	    display.Blit(AGG::GetICN(icnbtn, 0), dstpt);
	    // sprite button action
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 2), dstpt);
	    // sprite button castle
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 4), dstpt);
	    // sprite button magic
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 6), dstpt);
	    // sprite button end tur
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 416 + AGG::GetICN(icnbtn, 0).h();
	    display.Blit(AGG::GetICN(icnbtn, 8), dstpt);
	    // sprite button info
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 10), dstpt);
	    // sprite button option
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 12), dstpt);
	    // sprite button settings
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 14), dstpt);

	    // sprite portret heroes scroll down
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            dstpt.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 7 - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    // sprite portret castle scroll down
            dstpt.x = display.w() - BORDERWIDTH - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    break;

	case Display::LARGE:

            // TOP BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = 140;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 192;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // BOTTOM BORDER
            srcrt.x = 0;
            srcrt.y = AGG::GetICN(icnadv, 0).h() - BORDERWIDTH;
            srcrt.w = 140;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 192;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // LEFT BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 166;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 144;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // RIGHT BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 166;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 144;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // MIDDLE BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - 2 * BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 166;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 144;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // TOP PANEL
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - BORDERWIDTH;
            srcrt.y = RADARWIDTH + BORDERWIDTH;
            srcrt.w = RADARWIDTH;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.y += 32;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);

            // TOP PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 5;    // background panel heroes
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 8; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 77;   // background panel castle
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 8; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }

            // BOTTOM PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = 520;
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);

	    // sprite button next hero
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 448;
	    display.Blit(AGG::GetICN(icnbtn, 0), dstpt);
	    // sprite button action
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 2), dstpt);
	    // sprite button castle
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 4), dstpt);
	    // sprite button magic
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 6), dstpt);
	    // sprite button end tur
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 448 + AGG::GetICN(icnbtn, 0).h();
	    display.Blit(AGG::GetICN(icnbtn, 8), dstpt);
	    // sprite button info
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 10), dstpt);
	    // sprite button option
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 12), dstpt);
	    // sprite button settings
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 14), dstpt);

	    // sprite portret heroes scroll down
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            dstpt.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 8 - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    // sprite portret castle scroll down
            dstpt.x = display.w() - BORDERWIDTH - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    break;

	case Display::XLARGE:
            // TOP BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = 128;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 214;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w - 1;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // BOTTOM BORDER
            srcrt.x = 0;
            srcrt.y = AGG::GetICN(icnadv, 0).h() - BORDERWIDTH;
            srcrt.w = 140;
            srcrt.h = BORDERWIDTH;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.x += srcrt.w;
            srcrt.w = 214;
            dstpt.x = srcrt.x;
            dstpt.y = display.h() - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.x += srcrt.w - 1;
            srcrt.w = AGG::GetICN(icnadv, 0).w() - srcrt.x;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // LEFT BORDER
            srcrt.x = 0;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 160;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 181;
            dstpt.x = srcrt.x;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // RIGHT BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 160;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 181;
            dstpt.x = display.w() - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // MIDDLE BORDER
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - 2 * BORDERWIDTH;
            srcrt.y = 0;
            srcrt.w = BORDERWIDTH;
            srcrt.h = 160;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 181;
            dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.h = AGG::GetICN(icnadv, 0).h() - srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            // TOP PANEL
            srcrt.x = AGG::GetICN(icnadv, 0).w() - RADARWIDTH - BORDERWIDTH;
            srcrt.y = RADARWIDTH + BORDERWIDTH;
            srcrt.w = RADARWIDTH;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            srcrt.y += srcrt.h;
            srcrt.h = 32;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = srcrt.y;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);
            dstpt.y += srcrt.h;
            srcrt.y += 32;
            srcrt.h = 64;
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    display.Blit(AGG::GetICN(icnadv, 0), srcrt, dstpt);

            // TOP PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 5;    // background panel heroes
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 8; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 77;   // background panel castle
            dstpt.y = RADARWIDTH + BORDERWIDTH + 21;
            for(int i = 0; i < 8; ++i){
		display.Blit(AGG::GetICN(icnlocator, i + 1), dstpt);
                dstpt.y += 32;
            }

            // BOTTOM PANEL BACKGROUND ELEMENT
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
            dstpt.y = 520;
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);
            dstpt.y += AGG::GetICN(icnston, 0).h();
	    display.Blit(AGG::GetICN(icnston, 0), dstpt);

	    // sprite button next hero
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 448;
	    display.Blit(AGG::GetICN(icnbtn, 0), dstpt);
	    // sprite button action
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 2), dstpt);
	    // sprite button castle
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 4), dstpt);
	    // sprite button magic
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 6), dstpt);
	    // sprite button end tur
	    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    dstpt.y = 448 + AGG::GetICN(icnbtn, 0).h();
	    display.Blit(AGG::GetICN(icnbtn, 8), dstpt);
	    // sprite button info
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 10), dstpt);
	    // sprite button option
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 12), dstpt);
	    // sprite button settings
	    dstpt.x += AGG::GetICN(icnbtn, 0).w();
	    display.Blit(AGG::GetICN(icnbtn, 14), dstpt);

	    // sprite portret heroes scroll down
            dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            dstpt.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 8 - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    // sprite portret castle scroll down
            dstpt.x = display.w() - BORDERWIDTH - 15;
	    display.Blit(AGG::GetICN(icnscroll, 2), dstpt);
	    break;
    }

}
