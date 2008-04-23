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
#include "audio.h"
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
#include "route.h"
#include "game_focus.h"
#include "scripting.h"

namespace Game
{
    Cursor::themes_t GetCursor(const Focus & focus, const Maps::Tiles & tile);
    void OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void OpenHeroes(Heroes *heroes, GameArea & areaMaps, Radar & radar);
    void FocusToCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void FocusToHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar);
    Game::menu_t HumanTurn(GameArea & areaMaps, Radar & radar, StatusWindow & statusWindow, bool message);
};

Button *_buttonScrollHeroesUp;
Button *_buttonScrollCastleUp;
Button *_buttonNextHero;
Button *_buttonMovement;
Button *_buttonKingdom;
Button *_buttonSpell;
Button *_buttonEndTur;
Button *_buttonAdventure;
Button *_buttonFile;
Button *_buttonSystem;
Button *_buttonScrollHeroesDown;
Button *_buttonScrollCastleDown;

Splitter *_splitHeroes;
Splitter *_splitCastles;

u8 icon_count = 0;

Game::menu_t Game::StartGame(void)
{
    // Load maps
    world.LoadMaps(Settings::Get().FileInfo().FileMaps());

    if(Settings::Get().HotSeat()) {
        for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) 
            if(color & Settings::Get().Players())
                world.GetKingdom(color).SetControl(Game::Human);
    } else {
        // single player
        world.GetKingdom(Settings::Get().MyColor()).SetControl(Game::Human);
    }
    Settings::Get().SetHotSeat(false);

    GameArea areaMaps;

    Display & display = Display::Get();

    Audio::Mixer & mixer = Audio::Mixer::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Game::DrawInterface();

    // Create radar
    Radar radar;

    areaMaps.Redraw();
    radar.RedrawArea();

    //
    const ICN::icn_t icnscroll = H2Config::EvilInterface() ? ICN::SCROLLE : ICN::SCROLL;
    const ICN::icn_t icnbtn = H2Config::EvilInterface() ? ICN::ADVEBTNS : ICN::ADVBTNS;

    Point pt_stw, pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt ,pt_set, pt_shd, pt_scd;

    // coord button heroes scroll up
    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // coord button castle scroll up
    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // recalculate buttons coordinate
    switch(H2Config::VideoMode())
    {
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

    _buttonScrollHeroesUp = new Button(pt_shu, icnscroll, 0, 1);
    _buttonScrollCastleUp = new Button(pt_scu, icnscroll, 0, 1);
    _buttonNextHero = new Button(pt_her, icnbtn, 0, 1);
    _buttonMovement = new Button(pt_act, icnbtn, 2, 3);
    _buttonKingdom = new Button(pt_cas, icnbtn, 4, 5);
    _buttonSpell = new Button(pt_mag, icnbtn, 6, 7);
    _buttonEndTur = new Button(pt_end, icnbtn, 8, 9);
    _buttonAdventure = new Button(pt_inf, icnbtn, 10, 11);
    _buttonFile = new Button(pt_opt, icnbtn, 12, 13);
    _buttonSystem = new Button(pt_set, icnbtn, 14, 15);
    _buttonScrollHeroesDown = new Button(pt_shd, icnscroll, 2, 3);
    _buttonScrollCastleDown = new Button(pt_scd, icnscroll, 2, 3);

    Button &buttonScrollHeroesUp = *_buttonScrollHeroesUp;
    Button &buttonScrollCastleUp = *_buttonScrollCastleUp;
    Button &buttonNextHero = *_buttonNextHero;
    Button &buttonMovement = *_buttonMovement;
    Button &buttonKingdom = *_buttonKingdom;
    Button &buttonSpell = *_buttonSpell;
    Button &buttonEndTur = *_buttonEndTur;
    Button &buttonAdventure = *_buttonAdventure;
    Button &buttonFile = *_buttonFile;
    Button &buttonSystem = *_buttonSystem;
    Button &buttonScrollHeroesDown = *_buttonScrollHeroesDown;
    Button &buttonScrollCastleDown = *_buttonScrollCastleDown;

    // splitter
    _splitHeroes = new Splitter(AGG::GetICN(icnscroll, 4), Rect(pt_shu.x + 3, pt_shu.y + 18, 10, pt_shd.y - pt_shu.y - 20), Splitter::VERTICAL);
    _splitCastles = new Splitter(AGG::GetICN(icnscroll, 4), Rect(pt_scu.x + 3, pt_scu.y + 18, 10, pt_scd.y - pt_scu.y - 20), Splitter::VERTICAL);
    Splitter &splitHeroes = *_splitHeroes;
    Splitter &splitCastles = *_splitCastles;

    splitHeroes.SetRange(0, 0);
    splitCastles.SetRange(0, 0);

    Game::Focus & global_focus = Focus::Get();

    // status window
    Game::StatusWindow statusWindow(pt_stw);
    statusWindow.Redraw();

    buttonScrollHeroesUp.Draw();
    buttonScrollCastleUp.Draw();
    buttonNextHero.Draw();
    buttonMovement.Draw();
    buttonKingdom.Draw();
    buttonSpell.Draw();
    buttonEndTur.Draw();
    buttonAdventure.Draw();
    buttonFile.Draw();
    buttonSystem.Draw();
    buttonScrollHeroesDown.Draw();
    buttonScrollCastleDown.Draw();

    display.Flip();

    Game::menu_t m = ENDTURN;
    int humans = 0;
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(world.GetKingdom(color).isPlay() && world.GetKingdom(color).Control() == Game::Human) 
	    humans ++;

    while(1) {
	// AI move
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) {
	    if(world.GetKingdom(color).isPlay()) switch(world.GetKingdom(color).Control()) {
	        case Game::Human:
		    mixer.Enhance();
		    cursor.Show();
		    display.Flip();
		    statusWindow.SetState(Game::StatusWindow::DAY);
		    statusWindow.Redraw();

		    //Color::color_t human = H2Config::MyColor();
		    Settings::Get().SetMyColor(color);
    		    if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().GetPath().size())
    		    {
    		        global_focus.GetHeroes().GetPath().Hide();
    		        global_focus.GetHeroes().GetPath().Show();
		    }

		    m = HumanTurn(areaMaps, radar, statusWindow, humans > 1);

		    cursor.Hide();
		    mixer.Reduce();
		    if(m != ENDTURN) break;
		    break;
	        case Game::Network:
	            // TODO network game
		    break;
	        case Game::AI:
		    world.GetKingdom(color).AITurns(statusWindow);
		    break;
	    }
	    if(m != ENDTURN) break;
	}
	if(m != ENDTURN) break;
	world.NextDay();
    }

    delete _buttonScrollHeroesUp;
    delete _buttonScrollCastleUp;
    delete _buttonNextHero;
    delete _buttonMovement;
    delete _buttonKingdom;
    delete _buttonSpell;
    delete _buttonEndTur;
    delete _buttonAdventure;
    delete _buttonFile;
    delete _buttonSystem;
    delete _buttonScrollHeroesDown;
    delete _buttonScrollCastleDown;

    return m == ENDTURN ? QUITGAME : m;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar)
{
    if(! castle) return;

    Audio::Mixer::Get().Reduce();

    Cursor & cursor = Cursor::Get();
    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();

    Dialog::answer_t result = Dialog::ZERO;

    while(Dialog::CANCEL != result)
    {
	FocusToCastle(castle, areaMaps, radar);

	cursor.Hide();

	result = castle->OpenDialog();

	const u32 cursor_index = selectCastles.GetCursorIndex();

	if(Dialog::PREV == result)
	{
	     castle = cursor_index ? myCastles.at(cursor_index - 1) : myCastles.back();
	}
	else
	if(Dialog::NEXT == result)
	{
	     castle = myCastles.size() == cursor_index + 1 ? myCastles.front() : myCastles.at(cursor_index + 1);
	}

	cursor.Show();
    }

    if(Heroes *hero = const_cast<Heroes *>(castle->GetHeroes()))
	    FocusToHeroes(hero, areaMaps, radar);

    areaMaps.Redraw();

    Audio::Mixer::Get().Enhance();
}

/* focus to castle */
void Game::FocusToCastle(Castle *castle, GameArea & areaMaps, Radar & radar)
{
    if(! castle) return;

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    selectHeroes.Reset();

    Game::Focus & globalfocus = Game::Focus::Get();

    // hide path
    if(Game::Focus::HEROES == globalfocus.Type()) globalfocus.GetHeroes().GetPath().Hide();

    globalfocus.Set(*castle);

    areaMaps.Center(castle->GetCenter());
    radar.RedrawCursor();

    selectCastles.SelectFromCenter(castle->GetCenter());
    selectCastles.Redraw();
    selectHeroes.Redraw();

    cursor.Show();
}

/* open heroes wrapper */
void Game::OpenHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar)
{
    if(! hero) return;

    Audio::Mixer::Get().Reduce();

    Cursor & cursor = Cursor::Get();
    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    Dialog::answer_t result = Dialog::ZERO;

    while(Dialog::CANCEL != result)
    {
	FocusToHeroes(hero, areaMaps, radar);

	cursor.Hide();

	result = hero->OpenDialog();

	const u32 cursor_index = selectHeroes.GetCursorIndex();

	switch(result)
	{
	    case Dialog::PREV:
		hero = cursor_index ? myHeroes.at(cursor_index - 1) : myHeroes.back();
		break;
	
	    case Dialog::NEXT:
		hero = myHeroes.size() == cursor_index + 1 ? myHeroes.front() : myHeroes.at(cursor_index + 1);
		break;
	
	    case Dialog::DISMISS:
	    {
		Error::Verbose("Game::OpenHeroes: FIXME dismiss hero.");
		//Kingdom & kingdom = world.GetMyKingdom();
		//const Maps::Tiles & tile = world.GetTiles((*hero).GetCenter());

                //kingdom.RemoveHeroes(hero);
	    /*
		// FIX redraw:
		// 	tile
		// 	top tile
		// 	selectHeroes
		//	selectCastle if selectHeroes = 0
    	    */
	    }
		result = Dialog::CANCEL;
	    	break;
	
	    default: break;
	}

	cursor.Show();
    }

    Audio::Mixer::Get().Enhance();
}

/* focus to heroes */
void Game::FocusToHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar)
{
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    selectCastles.Reset();

    Game::Focus & globalfocus = Game::Focus::Get();

    // hide old path
    if(Game::Focus::HEROES == globalfocus.Type()) globalfocus.GetHeroes().GetPath().Hide();

    globalfocus.Set(*hero);

    // show new path
    globalfocus.GetHeroes().GetPath().Show();
    //world.GetTiles((*heroes).GetCenter()).Redraw();

    // center
    areaMaps.Center(hero->GetCenter());
    radar.RedrawCursor();

    selectHeroes.SelectFromCenter(hero->GetCenter());
    selectHeroes.Redraw();
    selectCastles.Redraw();

    cursor.Show();
}

/* return changee cursor */
Cursor::themes_t Game::GetCursor(const Focus & focus, const Maps::Tiles & tile)
{
    switch(focus.Type())
    {
	case Focus::HEROES:
	{
	    const Heroes & from_hero = focus.GetHeroes();
	    
	    if(from_hero.isShipMaster())
	    {
		switch(tile.GetObject())
		{
		    case MP2::OBJN_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
    			    return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
    		    }
    		    break;

    		    case MP2::OBJ_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			    return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
        	    }
        	    break;

		    case MP2::OBJ_HEROES:
		    {
			const Heroes * to_hero = world.GetHeroes(tile.GetIndex());

    			if(NULL != to_hero && to_hero->isShipMaster())
    			{
			    if(to_hero->GetCenter() == from_hero.GetCenter())
				return Cursor::HEROES;
			    else
				return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE : Cursor::FIGHT;
			}
    		    }
    		    break;

    		    case MP2::OBJ_TREASURECHEST:
                	return Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT : Cursor::POINTER;

		    case MP2::OBJ_COAST:
			return Cursor::ANCHOR;

		    default:
                        // FIXME: select BOAT cursor or BOAT2 or BOAT3 or BOAT4
                        return MP2::isActionObject(tile.GetObject(), true) ? Cursor::REDBOAT : (tile.isPassable() ? Cursor::BOAT : Cursor::POINTER);
		}
	    }
	    else
	    {
		switch(tile.GetObject())
		{
    		    case MP2::OBJ_MONSTER:
    			return Cursor::FIGHT;

		    case MP2::OBJN_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
    			    return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::FIGHT;
    		    }
    		    break;

    		    case MP2::OBJ_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			    return from_hero.GetColor() == castle->GetColor() ? Cursor::ACTION : Cursor::FIGHT;
        	    }
        	    break;

		    case MP2::OBJ_HEROES:
		    {
			const Heroes * to_hero = world.GetHeroes(tile.GetIndex());

    			if(NULL != to_hero && !to_hero->isShipMaster())
    			{
			    if(to_hero->GetCenter() == from_hero.GetCenter())
				return Cursor::HEROES;
			    else
				return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE : Cursor::FIGHT;
			}
    		    }
    		    break;

    		    case MP2::OBJ_BOAT:
            		return Cursor::BOAT;

    		    case MP2::OBJ_TREASURECHEST:
                	return Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION;

		    default:
                    	// FIXME: select MOVE cursor or MOVE2 or MOVE3 or MOVE4
			return MP2::isActionObject(tile.GetObject(), false) ? Cursor::ACTION : (tile.isPassable() ? Cursor::MOVE : Cursor::POINTER);
		}
	    }
	}
	break;

	case Focus::CASTLE:
	{
	    const Castle & from_castle = focus.GetCastle();

	    switch(tile.GetObject())
	    {
    		case MP2::OBJN_CASTLE:
    		case MP2::OBJ_CASTLE:
    		{
    		    const Castle *to_castle = world.GetCastle(tile.GetIndex());

    		    if(NULL != to_castle)
    			return to_castle->GetColor() == from_castle.GetColor() ? Cursor::CASTLE : Cursor::POINTER;
		}
		break;

		case MP2::OBJ_HEROES:
    		{
    		    const Heroes *heroes = world.GetHeroes(tile.GetIndex());

		    if(NULL != heroes)
    			return heroes->GetColor() == from_castle.GetColor() ? Cursor::HEROES : Cursor::POINTER;
		}
		break;

		default:
		    return Cursor::POINTER;
	    }
	}
	break;

    	default:
    	break;
    }

    return Cursor::POINTER;
}

Game::menu_t Game::HumanTurn(GameArea & areaMaps, Radar & radar, StatusWindow & statusWindow, bool message)
{
    Game::Focus & global_focus = Focus::Get();

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Audio::Mixer & mixer = Audio::Mixer::Get();

    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);
    const Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollTop(BORDERWIDTH / 2, 0, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    LocalEvent & le = LocalEvent::GetLocalEvent();

    u32 ticket = 0;

    bool change_settings = false;

    Button &buttonScrollHeroesUp = *_buttonScrollHeroesUp;
    Button &buttonScrollCastleUp = *_buttonScrollCastleUp;
    Button &buttonNextHero = *_buttonNextHero;
    Button &buttonMovement = *_buttonMovement;
    Button &buttonKingdom = *_buttonKingdom;
    Button &buttonSpell = *_buttonSpell;
    Button &buttonEndTur = *_buttonEndTur;
    Button &buttonAdventure = *_buttonAdventure;
    Button &buttonFile = *_buttonFile;
    Button &buttonSystem = *_buttonSystem;
    Button &buttonScrollHeroesDown = *_buttonScrollHeroesDown;
    Button &buttonScrollCastleDown = *_buttonScrollCastleDown;

    Splitter &splitHeroes = *_splitHeroes;
    Splitter &splitCastles = *_splitCastles;

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();

    Game::SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    Game::SelectFocusHeroes  & selectHeroes = SelectFocusHeroes::Get();

    const std::vector<Rect> & coordsCastlesIcon = selectCastles.GetCoords();
    const std::vector<Rect> & coordsHeroesIcon = selectHeroes.GetCoords();

    splitHeroes.SetRange(0, myHeroes.size() > icon_count ? myHeroes.size() - icon_count : 0);
    splitCastles.SetRange(0, myCastles.size() > icon_count ? myCastles.size() - icon_count : 0);

    selectCastles.SetCastles(myCastles);
    selectCastles.Reset();

    selectHeroes.SetHeroes(myHeroes);
    selectHeroes.Reset();

    // focus set to castle
    if(myCastles.size())
    {
	global_focus.Set(*myCastles.front());
	selectCastles.Select(0);
    }
    else
    // focus set to heroes
    if(myHeroes.size())
    {
	global_focus.Set(*myHeroes.front());
	selectHeroes.Select(0);
    }

    // center to focus
    areaMaps.Center(global_focus.Center());
    radar.RedrawCursor();

    selectCastles.Redraw();
    selectHeroes.Redraw();

    if(message)
        Dialog::Message("", Color::String(myKingdom.GetColor()) + " player's turn", Font::BIG, Dialog::OK);

    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(le.KeyPress(KEY_ESCAPE))
	{
    	    // stop hero
    	    if(Game::Focus::HEROES == global_focus.Type() &&
    		global_focus.GetHeroes().isNeedMove() && 
    		global_focus.GetHeroes().isEnableMove())
    		    global_focus.GetHeroes().SetMove(false);
    	    else
    	    if(Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO)) return QUITGAME;
	}
	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft))
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_LEFT);
	    areaMaps.Scroll(GameArea::LEFT);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight))
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_RIGHT);
	    areaMaps.Scroll(GameArea::RIGHT);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop))
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_TOP);
	    areaMaps.Scroll(GameArea::TOP);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom)){
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_BOTTOM);
	    areaMaps.Scroll(GameArea::BOTTOM);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
	    const Point & mouse_coord = le.MouseCursor();
	    const u16 index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
	    const Maps::Tiles & tile = world.GetTiles(index_maps);
	    const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);

	    cursor.SetThemes(GetCursor(global_focus, tile));

	    if(le.MouseClickLeft(tile_pos))
	    {
		switch(global_focus.Type())
		{
		    case Focus::HEROES:
		    {
			Heroes & from_hero = global_focus.GetHeroes();
			
			switch(tile.GetObject())
			{
    			    // from hero to castle
    			    case MP2::OBJN_CASTLE:
    			    {
    				const Castle *to_castle = world.GetCastle(index_maps);

    				if(NULL != to_castle)
    				{
				    if(from_hero.GetColor() == to_castle->GetColor())
				    {
					if(selectHeroes.isSelected())
					{
					    selectHeroes.Reset();
					    selectHeroes.Redraw();
					    splitHeroes.Move(selectHeroes.GetTopIndex());
					}

					FocusToCastle(const_cast<Castle *>(to_castle), areaMaps, radar);
					splitCastles.Move(selectCastles.GetTopIndex());
					display.Flip();
				    }
    				}
			    }
			    break;

    			    // from hero to hero
    			    case MP2::OBJ_HEROES:
    			    {
			        const Heroes * to_hero = world.GetHeroes(index_maps);

			        if(NULL != to_hero)
			        {
			    	    if(from_hero.GetCenter() == to_hero->GetCenter())
				    {
					mixer.Reduce();
					OpenHeroes(&from_hero, areaMaps, radar);
					statusWindow.Redraw();
					display.Flip();
					mixer.Enhance();
				    }
				    else
					from_hero.ShowPathOrStartMove(index_maps);
    				}
    			    }
    			    break;

			    break;

			    default:
				if(tile.isPassable() || MP2::isActionObject(tile.GetObject(), from_hero.isShipMaster()))
				    from_hero.ShowPathOrStartMove(index_maps);
			    break;
			}
		    }
		    break;

		    case Focus::CASTLE:
		    {
			Castle & from_castle = global_focus.GetCastle();

			switch(tile.GetObject())
			{
			    // from castle to castle
			    case MP2::OBJN_CASTLE:
			    case MP2::OBJ_CASTLE:
			    {
    				const Castle *to_castle = world.GetCastle(index_maps);

    				if(NULL != to_castle &&
    				    from_castle.GetColor() == to_castle->GetColor())
    				{
				    // is selected open dialog
				    if(from_castle.GetCenter() == to_castle->GetCenter())
				    {
					mixer.Reduce();
					OpenCastle(&from_castle, areaMaps, radar);
					statusWindow.Redraw();
					mixer.Enhance();
				    }
				    // select other castle
				    else
				    {
					FocusToCastle(const_cast<Castle *>(to_castle), areaMaps, radar);
					splitCastles.Move(selectCastles.GetTopIndex());
				    }

				    selectCastles.Redraw();
				    display.Flip();
				}
			    }
			    break;

			    // from castle to heroes
			    case MP2::OBJ_HEROES:
			    {
    				const Heroes *to_hero = world.GetHeroes(index_maps);

				if(NULL != to_hero &&
    				    from_castle.GetColor() == to_hero->GetColor())
    				{
				    if(selectCastles.isSelected())
				    {
					selectCastles.Reset();
					selectCastles.Redraw();
					splitCastles.Move(selectCastles.GetTopIndex());
				    }

    				    FocusToHeroes(const_cast<Heroes *>(to_hero), areaMaps, radar);
				    splitHeroes.Move(selectHeroes.GetTopIndex());
				    display.Flip();
        			}
			    }
			    break;

			    default:
			    break;
			}
		    }
		    break;
		    
		    default:
		    break;
		}
	    }
	    else
	    if(le.MousePressRight(tile_pos))
	    {
		if(H2Config::Debug()) tile.DebugInfo(index_maps);

		switch(tile.GetObject())
		{
		    case MP2::OBJ_MONSTER:
			Dialog::QuickInfo(Army::String(Army::GetSize(Monster::GetSize(tile))) + " of " + Monster::String(Monster::Monster(tile)));
		    break;

		    case MP2::OBJN_CASTLE:
		    case MP2::OBJ_CASTLE:
    		    {
    		    	const Castle *castle = world.GetCastle(tile.GetIndex());
			if(castle) Dialog::QuickInfo(*castle);
		    }
		    break;

		    case MP2::OBJ_HEROES:
    		    {
    		    	const Heroes *heroes = world.GetHeroes(tile.GetIndex());
			if(heroes) Dialog::QuickInfo(*heroes);
		    }
		    break;

		    case MP2::OBJ_EVENT:
		    case MP2::OBJ_ZERO:
			Dialog::QuickInfo(Maps::Ground::String(tile.GetGround()));
		    break;

		    default:
			Dialog::QuickInfo(std::string(MP2::StringObject(tile.GetObject())));
		    break;
		}
	    }
	// end cursor over game area
	}
	else
	// cursor over left panel
	if(le.MouseCursor(areaLeftPanel))
	{
	    cursor.SetThemes(Cursor::POINTER);

	    // draw push buttons
	    le.MousePressLeft(buttonScrollHeroesUp) ? buttonScrollHeroesUp.PressDraw() : buttonScrollHeroesUp.ReleaseDraw();
	    le.MousePressLeft(buttonScrollCastleUp) ? buttonScrollCastleUp.PressDraw() : buttonScrollCastleUp.ReleaseDraw();
	    le.MousePressLeft(buttonNextHero) ? buttonNextHero.PressDraw() : buttonNextHero.ReleaseDraw();
	    le.MousePressLeft(buttonMovement) ? buttonMovement.PressDraw() : buttonMovement.ReleaseDraw();
	    le.MousePressLeft(buttonKingdom) ? buttonKingdom.PressDraw() : buttonKingdom.ReleaseDraw();
	    le.MousePressLeft(buttonSpell) ? buttonSpell.PressDraw() : buttonSpell.ReleaseDraw();
	    le.MousePressLeft(buttonEndTur) ? buttonEndTur.PressDraw() : buttonEndTur.ReleaseDraw();
	    le.MousePressLeft(buttonAdventure) ? buttonAdventure.PressDraw() : buttonAdventure.ReleaseDraw();
	    le.MousePressLeft(buttonFile) ? buttonFile.PressDraw() : buttonFile.ReleaseDraw();
	    le.MousePressLeft(buttonSystem) ? buttonSystem.PressDraw() : buttonSystem.ReleaseDraw();
	    le.MousePressLeft(buttonScrollHeroesDown) ? buttonScrollHeroesDown.PressDraw() : buttonScrollHeroesDown.ReleaseDraw();
	    le.MousePressLeft(buttonScrollCastleDown) ? buttonScrollCastleDown.PressDraw() : buttonScrollCastleDown.ReleaseDraw();

	    // point radar
	    if(le.MouseCursor(radar.GetRect()) &&
		(le.MouseClickLeft(radar.GetRect()) ||
		    le.MousePressLeft(radar.GetRect())))
	    {
		Rect prev(areaMaps.GetRect());
		areaMaps.CenterFromRadar(le.MouseCursor());
		if(prev != areaMaps.GetRect()){
		    cursor.Hide();
		    radar.RedrawCursor();
		    cursor.Show();
		    display.Flip();
		}
	    }
	
	    // click Scroll Heroes Up
	    if((le.MouseWheelUp(selectHeroes.GetMaxRect()) ||
		le.MouseWheelUp(splitHeroes.GetRect()) ||
		le.MouseClickLeft(buttonScrollHeroesUp))  && selectHeroes.Prev())
	    {
		cursor.Hide();
		selectHeroes.Redraw();
		splitHeroes.Backward();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Castle Up
	    if((le.MouseWheelUp(selectCastles.GetMaxRect()) ||
		le.MouseWheelUp(splitCastles.GetRect()) ||
		le.MouseClickLeft(buttonScrollCastleUp)) && selectCastles.Prev())
	    {
		cursor.Hide();
		selectCastles.Redraw();
		splitCastles.Backward();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Heroes Down
	    if((le.MouseWheelDn(selectHeroes.GetMaxRect()) ||
		le.MouseWheelDn(splitHeroes.GetRect()) ||
		le.MouseClickLeft(buttonScrollHeroesDown)) && 
		myHeroes.size() > selectHeroes.GetCoords().size() &&
		selectHeroes.Next())
	    {
		cursor.Hide();
		selectHeroes.Redraw();
		splitHeroes.Forward();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Castle Down
	    if((le.MouseWheelDn(selectCastles.GetMaxRect()) ||
		le.MouseWheelDn(splitCastles.GetRect())  ||
		le.MouseClickLeft(buttonScrollCastleDown)) &&
		myCastles.size() > selectCastles.GetCoords().size() &&
		selectCastles.Next())
	    {
		cursor.Hide();
		selectCastles.Redraw();
		splitCastles.Forward();
		cursor.Show();
		display.Flip();
	    }

    	    // move splitter cursor castle
    	    if(le.MousePressLeft(splitCastles.GetRect()) && myCastles.size() > icon_count && le.MouseCursor(splitCastles.GetRect()))
    	    {
    		u32 seek = (le.MouseCursor().y - splitCastles.GetRect().y) * 100 / splitCastles.GetStep();

        	if(seek > myCastles.size() - 1) seek = myCastles.size() - 1;

        	cursor.Hide();

        	splitCastles.Move(seek);
		selectCastles.SetTop(seek);
		selectCastles.Redraw();

        	cursor.Show();
        	display.Flip();
	    }
	    else
    	    // move splitter cursor heroes
    	    if(le.MousePressLeft(splitHeroes.GetRect()) && myHeroes.size() > icon_count && le.MouseCursor(splitHeroes.GetRect()))
    	    {
    		u32 seek = (le.MouseCursor().y - splitHeroes.GetRect().y) * 100 / splitHeroes.GetStep();

        	if(seek > myHeroes.size() - 1) seek = myHeroes.size() - 1;

        	cursor.Hide();

        	splitHeroes.Move(seek);
		selectHeroes.SetTop(seek);
		selectHeroes.Redraw();

        	cursor.Show();
        	display.Flip();
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

			    // open hero
			    if(hero && H2Config::MyColor() == (*hero).GetColor())
			    {
				if(selectCastles.isSelected())
				{
				    selectCastles.Reset();
				    selectCastles.Redraw();
				}

				OpenHeroes(hero, areaMaps, radar);

				statusWindow.Redraw();
				display.Flip();
			    }
			}
			else
			{
			    if(selectCastles.isSelected())
			    {
				selectCastles.Reset();
				selectCastles.Redraw();
			    }

			    const Point & center = selectHeroes.GetCenter(ii);
			    Heroes *hero = const_cast<Heroes *>(world.GetHeroes(center.x, center.y));

			    // open hero
			    if(hero && H2Config::MyColor() == (*hero).GetColor())
			    {
				FocusToHeroes(hero, areaMaps, radar);

				const Route::Path & path = global_focus.GetHeroes().GetPath();
				if(path.size())
				{
				    cursor.Hide();
				    path.Show();
				    world.GetTiles((*hero).GetCenter()).Redraw();
				    cursor.Show();
				}

				display.Flip();
			    }
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
			    cursor.Hide();
			    Dialog::QuickInfo(*hero);
			    cursor.Show();
			    display.Flip();
			}
		    }
		}
	    // end click Heroes Icons
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

			    // open castle
			    if(castle && H2Config::MyColor() == (*castle).GetColor())
			    {
				if(selectHeroes.isSelected())
				{
				    selectHeroes.Reset();
				    selectHeroes.Redraw();
				}

				OpenCastle(castle, areaMaps, radar);

				statusWindow.Redraw();
				selectCastles.Redraw();

				display.Flip();
			    }
			}
			else
			{
			    if(selectHeroes.isSelected())
			    {
				selectHeroes.Reset();
				selectHeroes.Redraw();
			    }

			    const Point & center = selectCastles.GetCenter(ii);
			    Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));

			    if(castle && H2Config::MyColor() == (*castle).GetColor())
			    {
				FocusToCastle(castle, areaMaps, radar);
				display.Flip();
			    }
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
			    cursor.Hide();
			    Dialog::QuickInfo(*castle);
			    cursor.Show();
			    display.Flip();
			}
	    	    }
		}
	    // end click Castle Icons
	    }

	    // click Next Hero
	    if(le.MouseClickLeft(buttonNextHero))
	    {
	    }
	    else
	    // click Continue Movement
	    if(le.MouseClickLeft(buttonMovement))
	    {
    		if(Game::Focus::HEROES == global_focus.Type() &&
    		    global_focus.GetHeroes().isNeedMove())
    		    global_focus.GetHeroes().isEnableMove() ? global_focus.GetHeroes().SetMove(false) : global_focus.GetHeroes().SetMove(true);
	    }
	    else
	    // click Kingdom Summary
	    if(le.MouseClickLeft(buttonKingdom))
	    {
	        Lua::DoString("alert()");
	        Lua::DoString("alert1('test')");
	        Lua::DoString("msg('header', 'text')");
	        Lua::DoString("message('header', 'text', 0, 2)");
	    }
	    else
	    // click Cast Spell
	    if(le.MouseClickLeft(buttonSpell))
	    {
	    }
	    else
	    // click End Turn
	    if(le.MouseClickLeft(buttonEndTur))
	    {
	        break;
	    }
	    else
    	    // click AdventureOptions
	    if(le.MouseClickLeft(buttonAdventure))
	    {
		mixer.Reduce();

		switch(Dialog::AdventureOptions())
		{
		    default:
			break;
		}

		mixer.Enhance();
    	    }
	    else
	    // click FileOptions
	    if(le.MouseClickLeft(buttonFile))
	    {
		mixer.Reduce();

		Game::menu_t result = Dialog::FileOptions();
	    
		switch(result)
		{
		    case Game::NEWGAME:
		    case Game::LOADGAME:
		    case Game::QUITGAME:
			return result;

		    case Game::SAVEGAME:
			break;

		    default:
			break;
		}

		mixer.Enhance();
	    }
	    else
	    // click SystemOptions
	    if(le.MouseClickLeft(buttonSystem))
	    {
		mixer.Reduce();

		// Change and save system settings
		change_settings = Dialog::SystemOptions();

		mixer.Enhance();
	    }
	    else
	    // click StatusWindow
	    if(le.MouseClickLeft(statusWindow.GetRect()))
	    {
		cursor.Hide();
		statusWindow.NextState();
		statusWindow.Redraw();
		cursor.Show();
		display.Flip();
	    }

	    // right info
	    if(le.MousePressRight(radar.GetRect())) Dialog::Message("World Map", "A miniature view of the known world. Left click to move viewing area.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonNextHero)) Dialog::Message("Next Hero", "Select the next Hero.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonMovement)) Dialog::Message("Continue Movement", "Continue the Hero's movement along the current path.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonKingdom)) Dialog::Message("Kingdom Summary", "View a Summary of your Kingdom.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonSpell)) Dialog::Message("Cast Spell", "Cast an adventure spell.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonEndTur)) Dialog::Message("End Turn", "End your turn and left the computer take its turn.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonAdventure)) Dialog::Message("Adventure Options", "Bring up the adventure options menu.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonFile)) Dialog::Message("File Options", "Bring up the file options menu, alloving you to load menu, save etc.", Font::BIG);
	    else
	    if(le.MousePressRight(buttonSystem)) Dialog::Message("System Options", "Bring up the system options menu, alloving you to customize your game.", Font::BIG);
	    else
	    if(le.MousePressRight(statusWindow.GetRect())) Dialog::Message("Status Window", "This window provides information on the status of your hero or kingdom, and shows the date. Left click here to cycle throungh these windows.", Font::BIG);

	// end cursor over left panel
	}

	// mix all sound from focus
	Game::EnvironmentSoundMixer(change_settings);

        // draw heroes movement (in focus)
    	if(Game::Focus::HEROES == global_focus.Type() &&
    	    global_focus.GetHeroes().isNeedMove() && 
    	    global_focus.GetHeroes().isEnableMove() &&
    	    !(ticket % ANIMATION_LOW))
    	    {
    		if(1 < global_focus.GetHeroes().GetPath().size()) global_focus.GetHeroes().PlayWalkSound();

		cursor.Hide();

    		if(global_focus.GetHeroes().Move())
		{
		    global_focus.Update();
    		    // action, and modify sound evironment
    		    global_focus.GetHeroes().Action();
		}
		else
		    global_focus.Update();

		selectHeroes.Redraw();
		areaMaps.Center(global_focus.Center());
		radar.RedrawCursor();

		cursor.Show();
		display.Flip();
	    }

        // animation
        if(!(ticket % ANIMATION_LOW))
        {
            cursor.Hide();
            areaMaps.RedrawAnimation();
            cursor.Show();
            display.Flip();
        }

        ++ticket;
    }

    return ENDTURN;
}
