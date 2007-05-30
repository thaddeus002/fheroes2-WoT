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

#include <vector>
#include "agg.h"
#include "config.h"
#include "sprite.h"
#include "localevent.h"
#include "button.h"
#include "dialog.h"
#include "world.h"
#include "gamearea.h"
#include "cursor.h"
#include "radar.h"
#include "castle.h"
#include "heroes.h"
#include "splitter.h"
#include "game_statuswindow.h"
#include "maps_tiles.h"
#include "ground.h"
#include "error.h"
#include "game_selectfocus.h"
#include "game.h"

namespace Game
{
    Cursor::themes_t GetCursor(const gamefocus_t & focus);
    u16 GetIndexMaps(const Point &pt);
};

Game::menu_t Game::StartGame(void)
{
    // Load maps
    world.LoadMaps(H2Config::GetFileMaps());

    GameArea areaMaps;
    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);

    // cursor
    Cursor::Hide();

    Game::DrawInterface();

    // Create radar
    Radar radar;

    areaMaps.Redraw();
    radar.Redraw();

    // Create radar cursor
    radar.UpdatePosition();

    //
    const std::string &icnscroll = ( H2Config::EvilInterface() ? "SCROLLE.ICN" : "SCROLL.ICN" );
    const std::string &icnbtn = ( H2Config::EvilInterface() ? "ADVEBTNS.ICN" : "ADVBTNS.ICN" );

    Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollTop(BORDERWIDTH / 2, 0, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    Point pt_stw, pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt ,pt_set, pt_shd, pt_scd;

    // coord button heroes scroll up
    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // coord button castle scroll up
    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    u8 icon_count = 0;

    // recalculate buttons coordinate
    switch(H2Config::GetVideoMode()){

        default:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 392;
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
	    icon_count = 4;
            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
	    break;
	
	case Display::MEDIUM:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 488;
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
	    icon_count = 7;
            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
    	    break;

	case Display::LARGE:
	case Display::XLARGE:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 520;
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
	    icon_count = 8;
            // coord heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
	    break;
    }

    Button buttonScrollHeroesUp(pt_shu, icnscroll, 0, 1);
    Button buttonScrollCastleUp(pt_scu, icnscroll, 0, 1);
    Button buttonNextHero(pt_her, icnbtn, 0, 1);
    Button buttonMovement(pt_act, icnbtn, 2, 3);
    Button buttonKingdom(pt_cas, icnbtn, 4, 5);
    Button buttonSpell(pt_mag, icnbtn, 6, 7);
    Button buttonEndTur(pt_end, icnbtn, 8, 9);
    Button buttonAdventure(pt_inf, icnbtn, 10, 11);
    Button buttonFile(pt_opt, icnbtn, 12, 13);
    Button buttonSystem(pt_set, icnbtn, 14, 15);
    Button buttonScrollHeroesDown(pt_shd, icnscroll, 2, 3);
    Button buttonScrollCastleDown(pt_scd, icnscroll, 2, 3);

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();


    Game::SelectFocusCastles selectCastles(myCastles);
    Game::SelectFocusHeroes  selectHeroes(myHeroes);

    selectCastles.Reset();
    selectHeroes.Reset();

    const std::vector<Rect> & coordsCastlesIcon = selectCastles.GetCoords();
    const std::vector<Rect> & coordsHeroesIcon = selectHeroes.GetCoords();

    // splitter
    Splitter splitHeroes(AGG::GetICN(icnscroll, 4), Rect(pt_shu.x + 3, pt_shu.y + 18, 10, pt_shd.y - pt_shu.y - 20), Splitter::VERTICAL);
    Splitter splitCastles(AGG::GetICN(icnscroll, 4), Rect(pt_scu.x + 3, pt_scu.y + 18, 10, pt_scd.y - pt_scu.y - 20), Splitter::VERTICAL);

    splitHeroes.SetRange(0, myHeroes.size() > icon_count ? myHeroes.size() - icon_count : 0);
    splitCastles.SetRange(0, myCastles.size() > icon_count ? myCastles.size() - icon_count : 0);

    // game focus
    Game::gamefocus_t focus;

    // focus set to castle
    if(myCastles.size())
    {
	focus.type = Game::CASTLE;
	focus.center = (*myCastles.front()).GetCenter();
	selectCastles.Select(0);
    }
    else
    // focus set to heroes
    if(myHeroes.size())
    {
	focus.type = Game::HEROES;
	focus.center = (*myHeroes.front()).GetCenter();
	selectHeroes.Select(0);
    }
    // unknown focus
    else
    {
	Error::Warning("Game::StartGame: unknown game focus. exiting.");
	return QUITGAME;
    }

    selectCastles.Redraw();
    selectHeroes.Redraw();

    // center to focus
    areaMaps.Center(focus.center);
    radar.UpdatePosition();

    // status window
    Game::StatusWindow statusWindow(pt_stw, myKingdom);
    statusWindow.Redraw();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    display.Flip();

    Cursor::Show();

    u32 ticket = 0;
    // startgame loop
    while(1){

	le.HandleEvents();

	// scroll area maps
	if(le.MouseCursor(areaScrollLeft))  { Cursor::Set(Cursor::SCROLL_LEFT);   areaMaps.Scroll(GameArea::LEFT);   radar.UpdatePosition(); continue; }
	if(le.MouseCursor(areaScrollRight)) { Cursor::Set(Cursor::SCROLL_RIGHT);  areaMaps.Scroll(GameArea::RIGHT);  radar.UpdatePosition(); continue; }
	if(le.MouseCursor(areaScrollTop))   { Cursor::Set(Cursor::SCROLL_TOP);    areaMaps.Scroll(GameArea::TOP);    radar.UpdatePosition(); continue; }
	if(le.MouseCursor(areaScrollBottom)){ Cursor::Set(Cursor::SCROLL_BOTTOM); areaMaps.Scroll(GameArea::BOTTOM); radar.UpdatePosition(); continue; }

	// restore game cursor
	if(le.MouseCursor(area_pos))
	    Cursor::Set(Game::GetCursor(focus));
	else
	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel)){ Cursor::Set(Cursor::POINTER); }

	// insert next event here


	/*/
	 *
	 *
	/*/


	// left mouse on maps - action
	if(le.MousePressLeft(area_pos))
	{
	    const u16 indextile = GetIndexMaps(le.MouseCursor());
	    const Maps::Tiles & tile = world.GetTiles(indextile);
	    
	    switch(tile.GetObject())
	    {
		case MP2::OBJ_CASTLE:
		case MP2::OBJN_CASTLE:
		    // open castle
		    if(Castle *castle = const_cast<Castle *>(world.GetCastle(indextile)))
		    {
			Color::color_t color = (*castle).GetColor();
	
			if(H2Config::GetMyColor() == color)
			{
			    // center area to castle
			    Cursor::Hide();
			    if(selectHeroes.isSelected())
			    {
				selectHeroes.Reset();
				selectHeroes.Redraw();
			    }
			    focus.type = Game::CASTLE;
			    focus.center = (*castle).GetCenter();
			    areaMaps.Center(focus.center);
			    radar.UpdatePosition();
			    selectCastles.SelectFromCenter((*castle).GetCenter());
			    selectCastles.Redraw();
			    display.Flip();
			    // and open dialog castle
			    (*castle).OpenDialog();
			    statusWindow.Redraw();
			    display.Flip();
			    Cursor::Show();
			}
		    }
		    break;
		
		case MP2::OBJ_HEROES:
		    // open heroes
		    if(Heroes *hero = const_cast<Heroes *>(world.GetHeroes(indextile)))
		    {
			Color::color_t color = (*hero).GetColor();
	
			if(H2Config::GetMyColor() == color)
			{
			    // center area to heroes
			    Cursor::Hide();
			    if(selectCastles.isSelected())
			    {
				selectCastles.Reset();
				selectCastles.Redraw();
			    }
			    focus.type = Game::HEROES;
			    focus.center = (*hero).GetCenter();
			    areaMaps.Center(focus.center);
			    radar.UpdatePosition();
			    selectHeroes.SelectFromCenter((*hero).GetCenter());
			    selectHeroes.Redraw();
			    display.Flip();
			    // and open dialog heroes
			    (*hero).OpenDialog();
			    statusWindow.Redraw();
			    display.Flip();
			    Cursor::Show();
			}
		    }
		    break;

		default: break;
	    }
	}

	// right mouse on maps - info object
	if(le.MousePressRight(area_pos))
	{
	    const u16 indextile = GetIndexMaps(le.MouseCursor());
	    const Maps::Tiles & tile = world.GetTiles(indextile);

	    if(H2Config::Debug()) tile.DebugInfo(indextile);

	    u8 object = tile.GetObject();

	    const std::string & info = (MP2::OBJ_ZERO == object || MP2::OBJ_EVENT == object ?
		Maps::Ground::String(tile.GetGround()) : MP2::StringObject(object));

	    Cursor::Hide();

	    switch(object)
	    {
		case MP2::OBJ_CASTLE:
		case MP2::OBJN_CASTLE:
		    if(const Castle *castle = world.GetCastle(indextile)) Dialog::QuickInfo(*castle);
		    break;

		case MP2::OBJ_HEROES:
		    if(const Heroes *hero = world.GetHeroes(indextile)) Dialog::QuickInfo(*hero);
		    break;

		default:
		    Dialog::QuickInfo(info);
		    break;
	    }
	    Cursor::Show();
	}

	// draw push buttons
	le.MousePressLeft(buttonScrollHeroesUp) ? buttonScrollHeroesUp.Press() : buttonScrollHeroesUp.Release();
	le.MousePressLeft(buttonScrollCastleUp) ? buttonScrollCastleUp.Press() : buttonScrollCastleUp.Release();
	le.MousePressLeft(buttonNextHero) ? buttonNextHero.Press() : buttonNextHero.Release();
	le.MousePressLeft(buttonMovement) ? buttonMovement.Press() : buttonMovement.Release();
	le.MousePressLeft(buttonKingdom) ? buttonKingdom.Press() : buttonKingdom.Release();
	le.MousePressLeft(buttonSpell) ? buttonSpell.Press() : buttonSpell.Release();
	le.MousePressLeft(buttonEndTur) ? buttonEndTur.Press() : buttonEndTur.Release();
	le.MousePressLeft(buttonAdventure) ? buttonAdventure.Press() : buttonAdventure.Release();
	le.MousePressLeft(buttonFile) ? buttonFile.Press() : buttonFile.Release();
	le.MousePressLeft(buttonSystem) ? buttonSystem.Press() : buttonSystem.Release();
	le.MousePressLeft(buttonScrollHeroesDown) ? buttonScrollHeroesDown.Press() : buttonScrollHeroesDown.Release();
	le.MousePressLeft(buttonScrollCastleDown) ? buttonScrollCastleDown.Press() : buttonScrollCastleDown.Release();

	// point radar
	if(le.MousePressLeft(radar.GetRect()) && le.MouseCursor(radar.GetRect())){
	    Rect prev(areaMaps.GetRect());
	    areaMaps.CenterFromRadar(le.MouseCursor());
	    if(prev != areaMaps.GetRect()){
		Cursor::Hide();
		radar.UpdatePosition();
		display.Flip();
		Cursor::Show();
	    }
	}
	
	// click Scroll Heroes Up
	if((le.MouseWheelUp(selectHeroes.GetMaxRect()) ||
	    le.MouseWheelUp(splitHeroes.GetRect()) ||
	    le.MouseClickLeft(buttonScrollHeroesUp))  && selectHeroes.Prev())
	{
	    Cursor::Hide();
	    selectHeroes.Redraw();
	    splitHeroes.Backward();
	    display.Flip();
	    Cursor::Show();
	}

	// click Scroll Castle Up
	if((le.MouseWheelUp(selectCastles.GetMaxRect()) ||
	    le.MouseWheelUp(splitCastles.GetRect()) ||
	    le.MouseClickLeft(buttonScrollCastleUp)) && selectCastles.Prev())
	{
	    Cursor::Hide();
	    selectCastles.Redraw();
	    splitCastles.Backward();
	    display.Flip();
	    Cursor::Show();
	}

	// click Scroll Heroes Down
	if((le.MouseWheelDn(selectHeroes.GetMaxRect()) ||
	    le.MouseWheelDn(splitHeroes.GetRect()) ||
	    le.MouseClickLeft(buttonScrollHeroesDown)) && 
	    myHeroes.size() > selectHeroes.GetCoords().size() &&
	    selectHeroes.Next())
	{
	    Cursor::Hide();
	    selectHeroes.Redraw();
	    splitHeroes.Forward();
	    display.Flip();
	    Cursor::Show();
	}

	// click Scroll Castle Down
	if((le.MouseWheelDn(selectCastles.GetMaxRect()) ||
	    le.MouseWheelDn(splitCastles.GetRect())  ||
	    le.MouseClickLeft(buttonScrollCastleDown)) &&
	    myCastles.size() > selectCastles.GetCoords().size() &&
	    selectCastles.Next())
	{
	    Cursor::Hide();
	    selectCastles.Redraw();
	    splitCastles.Forward();
	    display.Flip();
	    Cursor::Show();
	}


        // move splitter cursor castle
        if(le.MousePressLeft(splitCastles.GetRect()) && myCastles.size() > icon_count && le.MouseCursor(splitCastles.GetRect()))
        {
/*
    	    //    Error::Verbose("move on splitter");
    	    u16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
    	    if(seek > curmaps->size() - LISTMAXITEM) seek = curmaps->size() - LISTMAXITEM;

    	    it_list_head = curmaps->begin() + seek;
    	    Cursor::Hide();
    	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, LISTMAXITEM);
    	    backgroundInfo.Restore();
    	    Scenario::DrawSelectInfo(it_current);
    	    split.Move(seek);
    	    display.Flip();
    	    Cursor::Show();
*/
	}

	// click Heroes Icons
	if(u8 count_icons = coordsHeroesIcon.size())
	{
	    for(u8 ii = 0; ii < count_icons; ++ii)
	    {
		const Rect & coord = coordsHeroesIcon[ii];
		const u8 icons_index = selectHeroes.GetTopIndex() + ii;

		// left click (center to hero)
		if(le.MouseClickLeft(coord) && icons_index < myHeroes.size())
		{
		    //double click
		    if(icons_index == selectHeroes.GetCursorIndex())
		    {
			const Point & center = selectHeroes.GetCenter(ii);
			Heroes *hero = const_cast<Heroes *>(world.GetHeroes(center.x, center.y));
			// center area to castle
			Cursor::Hide();
			areaMaps.Center(center);
			radar.UpdatePosition();
			display.Flip();
			Cursor::Show();
			// and open dialog
			(*hero).OpenDialog();
		    }
		    else
		    {
			Cursor::Hide();
			if(selectCastles.isSelected())
			{
			    selectCastles.Reset();
			    selectCastles.Redraw();
			}
			selectHeroes.Select(ii);
			selectHeroes.Redraw();
			focus.type = Game::HEROES;
			focus.center = selectHeroes.GetCenter(ii);
			areaMaps.Center(focus.center);
			radar.UpdatePosition();
			display.Flip();
			Cursor::Show();
		    }
		}
		else
		// right click (show info heroes)
		if(le.MousePressRight(coord) && icons_index < myHeroes.size())
		{
		    const Point & center = selectHeroes.GetCenter(ii);
		    const Heroes *hero = world.GetHeroes(center.x, center.y);
		    if(hero)
		    {
			Cursor::Hide();
			Dialog::QuickInfo(*hero);
			Cursor::Show();
		    }
		}
	    }
	}

	// click Castle Icons
	if(u8 count_icons = coordsCastlesIcon.size())
	{
	    for(u8 ii = 0; ii < count_icons; ++ii)
	    {
		const Rect & coord = coordsCastlesIcon[ii];
		const u8 icons_index = selectCastles.GetTopIndex() + ii;

		// left click (center to castle)
		if(le.MouseClickLeft(coord) && icons_index < myCastles.size())
		{
		    // double click
		    if(icons_index == selectCastles.GetCursorIndex())
		    {
			const Point & center = selectCastles.GetCenter(ii);
			Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));
			// center area to castle
			Cursor::Hide();
			areaMaps.Center(center);
			radar.UpdatePosition();
			display.Flip();
			Cursor::Show();
			// and open dialog
			(*castle).OpenDialog();
		    }
		    else
		    {
			Cursor::Hide();
			if(selectHeroes.isSelected())
			{
			    selectHeroes.Reset();
			    selectHeroes.Redraw();
			}
			selectCastles.Select(ii);
			selectCastles.Redraw();
			focus.type = Game::CASTLE;
			focus.center = selectCastles.GetCenter(ii);
			areaMaps.Center(focus.center);
			radar.UpdatePosition();
			display.Flip();
			Cursor::Show();
		    }
		}
		else
		// right click (show info castle)
		if(le.MousePressRight(coord) && icons_index < myCastles.size())
		{
		    const Point & center = selectCastles.GetCenter(ii);
		    Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));
		    
		    if(castle)
		    {
			Cursor::Hide();
			Dialog::QuickInfo(*castle);
			Cursor::Show();
		    }
		}
	    }
	}

	// click Next Hero
	if(le.MouseClickLeft(buttonNextHero))
	{
	}

	// click Continue Movement
	if(le.MouseClickLeft(buttonMovement))
	{
	}

	// click Kingdom Summary
	if(le.MouseClickLeft(buttonKingdom))
	{
	}

	// click Cast Spell
	if(le.MouseClickLeft(buttonSpell))
	{
	}

	// click End Turn
	if(le.MouseClickLeft(buttonEndTur))
	{
	    Cursor::Hide();
	    world.NextDay();

	    Color::color_t human = H2Config::GetMyColor();

	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color != human)
		const_cast<Kingdom &>(world.GetKingdom(color)).AITurns(statusWindow);

	    statusWindow.SetState(Game::StatusWindow::DAY);
	    statusWindow.Redraw();
	    display.Flip();
	    Cursor::Show();
	}

        // click AdventureOptions
	if(le.MouseClickLeft(buttonAdventure))
	{
	    switch(Dialog::AdventureOptions())
	    {
		default:
		    break;
	    }
        }

	// click FileOptions
	if(le.MouseClickLeft(buttonFile))
	{
	    Game::menu_t result = Dialog::FileOptions();
	    
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

	// click SystemOptions
	if(le.MouseClickLeft(buttonSystem)&& Dialog::OK == Dialog::SystemOptions())
	{
	    // Change and save system settings
	}
	
	// click StatusWindow
	if(le.MouseClickLeft(statusWindow.GetRect()))
	{
	    Cursor::Hide();
	    statusWindow.NextState();
	    statusWindow.Redraw();
	    display.Flip();
	    Cursor::Show();
	}

	// right info
	if(le.MousePressRight(radar.GetRect())) Dialog::Message("World Map", "A miniature view of the known world. Left click to move viewing area.", Font::BIG);
	if(le.MousePressRight(buttonNextHero)) Dialog::Message("Next Hero", "Select the next Hero.", Font::BIG);
	if(le.MousePressRight(buttonMovement)) Dialog::Message("Continue Movement", "Continue the Hero's movement along the current path.", Font::BIG);
	if(le.MousePressRight(buttonKingdom)) Dialog::Message("Kingdom Summary", "View a Summary of your Kingdom.", Font::BIG);
	if(le.MousePressRight(buttonSpell)) Dialog::Message("Cast Spell", "Cast an adventure spell.", Font::BIG);
	if(le.MousePressRight(buttonEndTur)) Dialog::Message("End Turn", "End your turn and left the computer take its turn.", Font::BIG);
	if(le.MousePressRight(buttonAdventure)) Dialog::Message("Adventure Options", "Bring up the adventure options menu.", Font::BIG);
	if(le.MousePressRight(buttonFile)) Dialog::Message("File Options", "Bring up the file options menu, alloving you to load menu, save etc.", Font::BIG);
	if(le.MousePressRight(buttonSystem)) Dialog::Message("System Options", "Bring up the system options menu, alloving you to customize your game.", Font::BIG);
	if(le.MousePressRight(statusWindow.GetRect())) Dialog::Message("Status Window", "This window provides information on the status of your hero or kingdom, and shows the date. Left click here to cycle throungh these windows.", Font::BIG);

	// ESC
	if(le.KeyPress(SDLK_ESCAPE) && (Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;
	
	++ticket;
    }

    return QUITGAME;
}

/* return game cursor */
Cursor::themes_t Game::GetCursor(const Game::gamefocus_t & focus)
{
    Cursor::themes_t result = Cursor::POINTER;
    const u16 index_maps = GetIndexMaps(LocalEvent::MouseCursor());
    const Maps::Tiles & tile = world.GetTiles(index_maps);

    switch(tile.GetObject())
    {
        case MP2::OBJ_MONSTER:
            if(Game::HEROES == focus.type) result = Cursor::FIGHT;
            break;

        case MP2::OBJN_CASTLE:
    	    if(const Castle * castle = world.GetCastle(index_maps))
    	    {
    		result = (*castle).GetColor() == H2Config::GetMyColor() ? Cursor::CASTLE : Cursor::FIGHT;
    	    }
            break;

        case MP2::OBJ_CASTLE:
    	    if(const Castle * castle = world.GetCastle(index_maps))
    	    {
        	if(Game::HEROES == focus.type)
        	    result = (*castle).GetColor() == H2Config::GetMyColor() ? Cursor::ACTION : Cursor::FIGHT;
		else
        	if(Game::CASTLE == focus.type && (*castle).GetColor() == H2Config::GetMyColor())
		    result = Cursor::CASTLE;
            }
            break;

	case MP2::OBJ_HEROES:
    	    if(const Heroes * heroes = world.GetHeroes(index_maps))
    	    {
    		result = (*heroes).GetColor() == H2Config::GetMyColor() ? Cursor::HEROES : Cursor::FIGHT;
            }
            break;

        case MP2::OBJ_BOAT:
    	    if(Game::HEROES == focus.type) result = Cursor::BOAT;
            break;

        case MP2::OBJ_TREASURECHEST:
            if(Game::HEROES == focus.type)
		result = (Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION);
	    else
            if(Game::BOAT == focus.type)
		result = (Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT : Cursor::POINTER);
            break;

        case MP2::OBJ_STONES:
        case MP2::OBJ_OILLAKE:
        case MP2::OBJ_CRATER:
        case MP2::OBJ_MOUNTS:
        case MP2::OBJ_TREES:
        case MP2::OBJN_WAGONCAMP:
        case MP2::OBJN_SAWMILL:
        case MP2::OBJN_MINES:
        case MP2::OBJ_WATERLAKE:
        case MP2::OBJN_ALCHEMYTOWER:
        case MP2::OBJN_EXCAVATION:
        case MP2::OBJN_FORT:
        case MP2::OBJN_DRAGONCITY:
        case MP2::OBJN_SHIPWRECK:
        case MP2::OBJN_DERELICTSHIP:
        case MP2::OBJN_MAGELLANMAPS:
	case MP2::OBJN_TROLLBRIDGE:
            break;

        case MP2::OBJ_ALCHEMYTOWER:
        case MP2::OBJ_SIGN:
        case MP2::OBJ_SKELETON:
        case MP2::OBJ_DAEMONCAVE:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_GAZEBO:
        case MP2::OBJ_ANCIENTLAMP:
        case MP2::OBJ_GRAVEYARD:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUNT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_PEASANTHUNT:
        case MP2::OBJ_PEASANTHUNT2:
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_LIGHTHOUSE:
        case MP2::OBJ_WATERMILL:
        case MP2::OBJ_MINES:
	case MP2::OBJ_OBELISK:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_SAWMILL:
	case MP2::OBJ_ORACLE:
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_DERELICTSHIP:
	case MP2::OBJ_DESERTTENT:
	case MP2::OBJ_STONELIGHTS:
	case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_WINDMILL:
	case MP2::OBJ_ARTIFACT:
	case MP2::OBJ_WATCHTOWER:
	case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_TREECITY:
	case MP2::OBJ_RUINS:
	case MP2::OBJ_FORT:
        case MP2::OBJ_TRADINGPOST:
        case MP2::OBJ_ABANDONEDMINE:
        case MP2::OBJ_STANDINGSTONES:
        case MP2::OBJ_IDOL:
        case MP2::OBJ_TREEKNOWLEDGE:
        case MP2::OBJ_DOCTORHUNT:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:
        case MP2::OBJ_PIRAMID:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_SPHINX:
        case MP2::OBJ_WAGON:
        case MP2::OBJ_ARTESIANSPRING:
        case MP2::OBJ_TROLLBRIDGE:
        case MP2::OBJ_WITCHHUNT:
        case MP2::OBJ_XANADU:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_MAGICWELL:
        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_OBSERVATIONTOWER:
        case MP2::OBJ_FREEMANFOUNDRY:
            if(Game::HEROES == focus.type) result = Cursor::ACTION;
            break;

        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_WHIRLPOOL:
        case MP2::OBJ_BUOY:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_MAGELLANMAPS:
	    if(Game::BOAT == focus.type) result = Cursor::REDBOAT;
    	    break;
																																															
        case MP2::OBJ_COAST:
	    if(Game::BOAT == focus.type) result = Cursor::ANCHOR;
    	    break;

	default:
            if(Game::HEROES == focus.type)
		result = (Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::MOVE);
	    else
            if(Game::BOAT == focus.type)
		result = (Maps::Ground::WATER == tile.GetGround() ? Cursor::BOAT : Cursor::POINTER);
	    else
            if(Game::CASTLE == focus.type) result = Cursor::POINTER;
	    break;
    }

    return result;
}

/* convert coord to index map */
u16 Game::GetIndexMaps(const Point & pt)
{
    const Rect & area_pos = GameArea::GetRect();

    u16 result = (area_pos.y + (pt.y - BORDERWIDTH) / TILEWIDTH) * world.w() + area_pos.x + (pt.x - BORDERWIDTH) / TILEWIDTH;

    if(result > world.w() * world.h() - 1) Error::Except("Game::GetIndexMaps: position, out of range.");

    return result;
}
