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
#include "event.h"
#include "button.h"
#include "dialog.h"
#include "mapsdata.h"
#include "gamearea.h"
#include "cursor.h"
#include "radar.h"
#include "game.h"

#include "error.h"

Game::menu_t Game::StartGame(void){

    // Load maps
    MapsData maps(H2Config::GetFileMaps());
    GameArea areaMaps(maps);

    // cursor
    Cursor::Hide();
    Cursor::themes_t cursor = Cursor::FIGHT; // test //Cursor::POINTER;

    Game::DrawInterface();

    // Create radar
    Radar areaRadar(display.w() - BORDERWIDTH - RADARWIDTH, BORDERWIDTH, maps);

    areaMaps.Redraw();
    areaRadar.Redraw();

    RadarCursor radarCursor(areaRadar);
    radarCursor.Redraw();

    const std::string &icnscroll = ( H2Config::EvilInterface() ? "SCROLLE.ICN" : "SCROLL.ICN" );
    const std::string &icnbtn = ( H2Config::EvilInterface() ? "ADVEBTNS.ICN" : "ADVBTNS.ICN" );

    Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollTop(BORDERWIDTH / 2, 0, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    Point pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt ,pt_set, pt_shd, pt_scd;

    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // recalculate buttons coordinate
    switch(H2Config::GetVideoMode()){

        default:
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 320;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 320;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 320;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 320;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 320 + AGG::GetICN(icnbtn, 0).h();

            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 4 - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 4 - 15;
	    break;
	
	case Display::MEDIUM:
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 416;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 416;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 416;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 416;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 416 + AGG::GetICN(icnbtn, 0).h();

            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 7 - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 7 - 15;
    	    break;

	case Display::LARGE:
	case Display::XLARGE:
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 448;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 448;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 448;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 448;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 448 + AGG::GetICN(icnbtn, 0).h();

            // coord heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 8 - 15;
            // coord castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 8 - 15;
	    break;
    }

    Button buttonScrollHeroesUp(pt_shu, icnscroll, 0, 1);
    Button buttonScrollCastleUp(pt_scu, icnscroll, 0, 1);
    Button buttonNextHero(pt_her, icnbtn, 0, 1);
    Button buttonAction(pt_act, icnbtn, 2, 3);
    Button buttonCastle(pt_cas, icnbtn, 4, 5);
    Button buttonMagic(pt_mag, icnbtn, 6, 7);
    Button buttonEndTur(pt_end, icnbtn, 8, 9);
    Button buttonInfo(pt_inf, icnbtn, 10, 11);
    Button buttonMenu(pt_opt, icnbtn, 12, 13);
    Button buttonSettings(pt_set, icnbtn, 14, 15);
    Button buttonScrollHeroesDown(pt_shd, icnscroll, 2, 3);
    Button buttonScrollCastleDown(pt_scd, icnscroll, 2, 3);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    display.Flip();


    Cursor::Show();

    // startgame loop
    while(1){

	le.HandleEvents();

	// scroll area maps
	if(le.MouseCursor(areaScrollLeft))  { Cursor::Set(Cursor::SCROLL_LEFT);   areaMaps.Scroll(GameArea::LEFT); radarCursor.Redraw(); continue; }
	if(le.MouseCursor(areaScrollRight)) { Cursor::Set(Cursor::SCROLL_RIGHT);  areaMaps.Scroll(GameArea::RIGHT); radarCursor.Redraw(); continue; }
	if(le.MouseCursor(areaScrollTop))   { Cursor::Set(Cursor::SCROLL_TOP);    areaMaps.Scroll(GameArea::TOP); radarCursor.Redraw();   continue; }
	if(le.MouseCursor(areaScrollBottom)){ Cursor::Set(Cursor::SCROLL_BOTTOM); areaMaps.Scroll(GameArea::BOTTOM); radarCursor.Redraw();  continue; }

	// restore game cursor
	if(le.MouseCursor(areaMaps.GetPosition())){ Cursor::Set(cursor); }
	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel)){ Cursor::Set(Cursor::POINTER); }











	// draw push buttons
	le.MousePressLeft(buttonScrollHeroesUp) ? buttonScrollHeroesUp.Press() : buttonScrollHeroesUp.Release();
	le.MousePressLeft(buttonScrollCastleUp) ? buttonScrollCastleUp.Press() : buttonScrollCastleUp.Release();
	le.MousePressLeft(buttonNextHero) ? buttonNextHero.Press() : buttonNextHero.Release();
	le.MousePressLeft(buttonAction) ? buttonAction.Press() : buttonAction.Release();
	le.MousePressLeft(buttonCastle) ? buttonCastle.Press() : buttonCastle.Release();
	le.MousePressLeft(buttonMagic) ? buttonMagic.Press() : buttonMagic.Release();
	le.MousePressLeft(buttonEndTur) ? buttonEndTur.Press() : buttonEndTur.Release();
	le.MousePressLeft(buttonInfo) ? buttonInfo.Press() : buttonInfo.Release();
	le.MousePressLeft(buttonMenu) ? buttonMenu.Press() : buttonMenu.Release();
	le.MousePressLeft(buttonSettings) ? buttonSettings.Press() : buttonSettings.Release();
	le.MousePressLeft(buttonScrollHeroesDown) ? buttonScrollHeroesDown.Press() : buttonScrollHeroesDown.Release();
	le.MousePressLeft(buttonScrollCastleDown) ? buttonScrollCastleDown.Press() : buttonScrollCastleDown.Release();

	// point radar
	if(le.MousePressLeft(areaRadar.GetRect()) && le.MouseCursor(areaRadar.GetRect())){
	    Rect prev(areaMaps.GetRect());
	    areaMaps.CenterFromRadar(le.MouseCursor());
	    if(prev != areaMaps.GetRect()){
		Cursor::Hide();
		radarCursor.Redraw();
		display.Flip();
		Cursor::Show();
	    }
	}

        // show dialog menu
	if(le.MouseClickLeft(buttonMenu))
	{
	    Game::menu_t result = Dialog::Menu();
	    
	    switch(result){

		case Game::NEWGAME:
		case Game::LOADGAME:
		case Game::QUITGAME:
		    return result;

		case Game::SAVEGAME:
		    break;

		default:
		    break;
	    }
	}

	// ESC
	if(le.KeyPress(SDLK_ESCAPE) && (Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;
    }

    return QUITGAME;
}
