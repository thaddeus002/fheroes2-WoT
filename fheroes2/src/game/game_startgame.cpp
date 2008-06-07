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
    Cursor::themes_t GetCursor(const Focus & focus, const Maps::Tiles & tile, const u8 days = 0);
    void OpenCastle(Castle *castle);
    void OpenHeroes(Heroes *heroes);
    void FocusToCastle(Castle *castle);
    void FocusToHeroes(Heroes *hero);
    void ShowPathOrStartMoveHero(Heroes *hero, const u16 dst_index);
    Game::menu_t HumanTurn(StatusWindow & statusWindow, bool message);
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
    // cursor
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    Display & display = Display::Get();
    display.SetVideoMode(conf.VideoMode());

    cursor.Hide();
    display.Fade();

    // Load maps
    world.LoadMaps(conf.FileInfo().FileMaps());

    if(conf.HotSeat()) {
        for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) 
            if(color & conf.Players())
                world.GetKingdom(color).SetControl(Game::Human);
    } else {
        // single player
        world.GetKingdom(conf.MyColor()).SetControl(Game::Human);
    }
    conf.SetHotSeat(false);

    GameArea & areaMaps = GameArea::Get();
    areaMaps.Build();

    Audio::Mixer & mixer = Audio::Mixer::Get();

    Game::DrawInterface();

    // Create radar
    Radar & radar = Radar::Get();
    radar.Build();

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
    const u8 count_h = (display.h() - 480) / TILEWIDTH;
    icon_count = count_h > 3 ? 8 : ( count_h < 3 ? 4 : 7);
    const u16 last_coord = RADARWIDTH + BORDERWIDTH * 2 + icon_count * 32 + BORDERWIDTH;
    const u8 button_w = AGG::GetICN(icnbtn, 0).w();
    const u8 button_h = AGG::GetICN(icnbtn, 0).h();

    // coord status windows
    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_stw.y = last_coord + 2 * button_h;
    // coord button next hero
    pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_her.y = last_coord;
    // coord button action
    pt_act.x = pt_her.x + button_w;
    pt_act.y = last_coord;
    // coord button castle
    pt_cas.x = pt_act.x + button_w;
    pt_cas.y = last_coord;
    // coord button magic
    pt_mag.x = pt_cas.x + button_w;
    pt_mag.y = last_coord;
    // coord button end tur
    pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_end.y = last_coord + button_h;
    // coord button info
    pt_inf.x = pt_end.x + button_w;
    pt_inf.y = last_coord + button_h;
    // coord button option
    pt_opt.x = pt_inf.x + button_w;
    pt_opt.y = last_coord + button_h;
    // coord button settings
    pt_set.x = pt_opt.x + button_w;
    pt_set.y = last_coord + button_h;
    // coord button heroes scroll down
    pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
    // coord button castle scroll down
    pt_scd.x = display.w() - BORDERWIDTH - 15;
    pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;

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
    global_focus.Reset();

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

    Game::menu_t m = ENDTURN;
    int humans = 0;
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(world.GetKingdom(color).isPlay() && world.GetKingdom(color).Control() == Game::Human) 
	    humans ++;

    while(1)
    {
	// AI move
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	{
	    Kingdom & kingdom = world.GetKingdom(color);
	    if(kingdom.isPlay())
	    {
		world.ClearFog(color);

		switch(kingdom.Control())
		{
	        case Game::Human:
		    mixer.Enhance();
		    conf.SetMyColor(color);
		    radar.RedrawArea(color);
		    statusWindow.SetState(Game::StatusWindow::DAY);
		    statusWindow.Redraw();

		    m = HumanTurn(statusWindow, humans > 1);

		    cursor.Hide();
		    mixer.Reduce();

		    if(m != ENDTURN) goto OUTDOOR;
		    break;
	        case Game::Network:
		    cursor.SetThemes(Cursor::WAIT);
		    cursor.Show();
	            display.Flip();

		    kingdom.AITurns(statusWindow);
		    cursor.Hide();
	            // TODO network game
		    break;
	        case Game::AI:
		    cursor.SetThemes(Cursor::WAIT);
		    cursor.Show();
	            display.Flip();

		    kingdom.AITurns(statusWindow);
		    cursor.Hide();
		    break;
		default:
		    Dialog::Message(Color::String(color), "default", Font::BIG, Dialog::OK);
		    break;
		}
	    }
	}
	world.NextDay();
    }

OUTDOOR:

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

    delete _splitHeroes;
    delete _splitCastles;

    return m == ENDTURN ? QUITGAME : m;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle)
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
	FocusToCastle(castle);

	Display::Get().Flip();
	cursor.Hide();
	Display::Get().Fade();

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

    if(Heroes *hero = const_cast<Heroes *>(castle->GetHeroes())) FocusToHeroes(hero);

    Audio::Mixer::Get().Enhance();
}

/* focus to castle */
void Game::FocusToCastle(Castle *castle)
{
    if(! castle) return;

    Game::Focus & globalfocus = Game::Focus::Get();

    if(Game::Focus::HEROES == globalfocus.Type()) globalfocus.GetHeroes().ShowPath(false);

    globalfocus.Set(*castle);
    globalfocus.Redraw();
}

/* open heroes wrapper */
void Game::OpenHeroes(Heroes *hero)
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
	FocusToHeroes(hero);

	Display::Get().Flip();
	cursor.Hide();
	Display::Get().Fade();

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
void Game::FocusToHeroes(Heroes *hero)
{
    if(! hero) return;

    Game::Focus & globalfocus = Game::Focus::Get();

    if(Game::Focus::HEROES == globalfocus.Type()) globalfocus.GetHeroes().ShowPath(false);

    globalfocus.Set(*hero);
    globalfocus.GetHeroes().ShowPath(true);
    globalfocus.Redraw();
}

/* return changee cursor */
Cursor::themes_t Game::GetCursor(const Focus & focus, const Maps::Tiles & tile, const u8 days)
{
    if(0 == Settings::Get().Debug() && tile.isFog(Settings::Get().MyColor())) return Cursor::POINTER;

    switch(focus.Type())
    {
	case Focus::HEROES:
	{
	    const Heroes & from_hero = focus.GetHeroes();
	    
	    if(from_hero.isShipMaster())
	    {
		switch(tile.GetObject())
		{
		    case MP2::OBJ_BOAT:
    			    return Cursor::POINTER;

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
				switch(days)
				{
				    case 0:
				    case 1:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE : Cursor::FIGHT;
				    case 2:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE2 : Cursor::FIGHT2;
				    case 3:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE3 : Cursor::FIGHT3;
				    default:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE4 : Cursor::FIGHT4;
				}
			}
    		    }
    		    break;

    		    case MP2::OBJ_TREASURECHEST:
			switch(days)
			{
			    case 0:
			    case 1:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT : Cursor::POINTER;
			    case 2:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT2 : Cursor::POINTER;
			    case 3:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT3 : Cursor::POINTER;
			    default:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::REDBOAT4 : Cursor::POINTER;
			}
			break;

		    case MP2::OBJ_COAST:
			switch(days)
			{
			    case 0:
			    case 1:	return Cursor::ANCHOR;
			    case 2:	return Cursor::ANCHOR2;
			    case 3:	return Cursor::ANCHOR3;
			    default:	return Cursor::ANCHOR4;
			}
			break;

		    default:
			switch(days)
			{
			    case 0:
			    case 1:	return MP2::isActionObject(tile.GetObject(), true) ? Cursor::REDBOAT : (tile.isPassable() ? Cursor::BOAT : Cursor::POINTER);
			    case 2:	return MP2::isActionObject(tile.GetObject(), true) ? Cursor::REDBOAT2 : (tile.isPassable() ? Cursor::BOAT2 : Cursor::POINTER);
			    case 3:	return MP2::isActionObject(tile.GetObject(), true) ? Cursor::REDBOAT3 : (tile.isPassable() ? Cursor::BOAT3 : Cursor::POINTER);
			    default:	return MP2::isActionObject(tile.GetObject(), true) ? Cursor::REDBOAT4 : (tile.isPassable() ? Cursor::BOAT4 : Cursor::POINTER);
			}
			break;                        
		}
	    }
	    else
	    {
		switch(tile.GetObject())
		{
    		    case MP2::OBJ_MONSTER:
			switch(days)
			{
			    case 0:
			    case 1:	return Cursor::FIGHT;
			    case 2:	return Cursor::FIGHT2;
			    case 3:	return Cursor::FIGHT3;
			    default:	return Cursor::FIGHT4;
			}
			break;

		    case MP2::OBJN_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			    switch(days)
			    {
				case 0:
				case 1:	return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::FIGHT;
				case 2:	return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::FIGHT2;
				case 3:	return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::FIGHT3;
				default:return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::FIGHT4;
			    }
			    break;
    		    }
    		    break;

    		    case MP2::OBJ_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			    switch(days)
			    {
				case 0:
				case 1:	return from_hero.GetColor() == castle->GetColor() ? Cursor::ACTION : Cursor::FIGHT;
				case 2:	return from_hero.GetColor() == castle->GetColor() ? Cursor::ACTION2 : Cursor::FIGHT2;
				case 3:	return from_hero.GetColor() == castle->GetColor() ? Cursor::ACTION3 : Cursor::FIGHT3;
				default:return from_hero.GetColor() == castle->GetColor() ? Cursor::ACTION4 : Cursor::FIGHT4;
			    }
			    break;
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
				switch(days)
				{
				    case 0:
				    case 1:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE : Cursor::FIGHT;
				    case 2:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE2 : Cursor::FIGHT2;
				    case 3:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE3 : Cursor::FIGHT3;
				    default:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE4 : Cursor::FIGHT4;
				}
				break;
			}
    		    }
    		    break;

    		    case MP2::OBJ_BOAT:
			switch(days)
			{
			    case 0:
			    case 1:	return Cursor::BOAT;
			    case 2:	return Cursor::BOAT2;
			    case 3:	return Cursor::BOAT3;
			    default:	return Cursor::BOAT4;
			}
			break;

    		    case MP2::OBJ_TREASURECHEST:
			switch(days)
			{
			    case 0:
			    case 1:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION;
			    case 2:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION2;
			    case 3:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION3;
			    default:	return Maps::Ground::WATER == tile.GetGround() ? Cursor::POINTER : Cursor::ACTION4;
			}
			break;

		    default:
			switch(days)
			{
			    case 0:
			    case 1:	return MP2::isActionObject(tile.GetObject(), false) ? Cursor::ACTION : (tile.isPassable() ? Cursor::MOVE : Cursor::POINTER);
			    case 2:	return MP2::isActionObject(tile.GetObject(), false) ? Cursor::ACTION2 : (tile.isPassable() ? Cursor::MOVE2 : Cursor::POINTER);
			    case 3:	return MP2::isActionObject(tile.GetObject(), false) ? Cursor::ACTION3 : (tile.isPassable() ? Cursor::MOVE3 : Cursor::POINTER);
			    default:	return MP2::isActionObject(tile.GetObject(), false) ? Cursor::ACTION4 : (tile.isPassable() ? Cursor::MOVE4 : Cursor::POINTER);
			}
			break;
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

void Game::ShowPathOrStartMoveHero(Heroes *hero, const u16 dst_index)
{
    if(!hero) return;

    Route::Path & path = hero->GetPath();

    // show path
    if(path.GetDestinationIndex() != dst_index)
    {
        Cursor & cursor = Cursor::Get();
        Display & display = Display::Get();

        hero->SetMove(false);

        cursor.Hide();

        if(path.isValid()) path.Hide();

        path.Calculate(dst_index);
        path.Show();
        cursor.Show();
        display.Flip();
    }
    // start move
    else
    if(path.EnableMove())
    {
        hero->SetMove(true);
    }
}

Game::menu_t Game::HumanTurn(StatusWindow & statusWindow, bool message)
{
    Game::Focus & global_focus = Focus::Get();
    Radar & radar = Radar::Get();
    GameArea & gamearea = GameArea::Get();

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Audio::Mixer & mixer = Audio::Mixer::Get();

    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, gamearea.GetRect().w * TILEWIDTH, gamearea.GetRect().h * TILEWIDTH);
    const Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollTop(BORDERWIDTH / 2, 0, (gamearea.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (gamearea.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
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

    switch(global_focus.Type())
    {
	// start game
	case Focus::UNSEL:
	    if(myCastles.size())
	    {
		global_focus.Set(*myCastles.front());
		selectCastles.Select(0);
	    }
	    else
	    if(myHeroes.size())
	    {
		global_focus.Set(*myHeroes.front());
		selectHeroes.Select(0);
	    }
	    break;

	case Focus::HEROES:
	    selectHeroes.Select(0);
	    FocusToHeroes(myHeroes.front());
	    break;

	case Focus::CASTLE:
	    selectCastles.Select(0);
	    FocusToCastle(myCastles.front());
	    break;
    }

    // center to focus
    gamearea.Center(global_focus.Center());
    radar.RedrawCursor();

    selectCastles.Redraw();
    selectHeroes.Redraw();

    display.Flip();
    cursor.Show();

    if(message)
        Dialog::Message("", Color::String(myKingdom.GetColor()) + " player's turn", Font::BIG, Dialog::OK);

    u8 route_days = 0;

    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(le.KeyPress(KEY_ESCAPE))
	{
    	    // stop hero
    	    if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
    	    	global_focus.GetHeroes().SetMove(false);
    	    else
    	    if(Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO)) return QUITGAME;
	    continue;
	}
	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft) && GameArea::x())
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_LEFT);
	    gamearea.Scroll(GameArea::LEFT);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight) && (world.w() - GameArea::w() > GameArea::x()))
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_RIGHT);
	    gamearea.Scroll(GameArea::RIGHT);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop) && GameArea::y())
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_TOP);
	    gamearea.Scroll(GameArea::TOP);
	    radar.RedrawCursor();
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom) && (world.h() - GameArea::h() > GameArea::y()))
	{
	    cursor.Hide();
	    cursor.SetThemes(Cursor::SCROLL_BOTTOM);
	    gamearea.Scroll(GameArea::BOTTOM);
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
	    const int index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
	    if(0 > index_maps) continue;

	    const Maps::Tiles & tile = world.GetTiles(index_maps);
	    const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);

	    cursor.SetThemes(GetCursor(global_focus, tile, route_days));

	    if(le.MouseClickLeft(tile_pos))
	    {
		if(Cursor::POINTER != cursor.Themes()) switch(global_focus.Type())
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

					FocusToCastle(const_cast<Castle *>(to_castle));
					splitCastles.Move(selectCastles.GetTopIndex());
					display.Flip();
				    }
				    else
				    {
					const u16 castle_center = Maps::GetIndexFromAbsPoint(to_castle->GetCenter());
					ShowPathOrStartMoveHero(&from_hero, castle_center);
					route_days = from_hero.GetRangeRouteDays();
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
					OpenHeroes(&from_hero);
					statusWindow.Redraw();
					display.Flip();
					mixer.Enhance();
				    }
				    else
				    {
					ShowPathOrStartMoveHero(&from_hero, index_maps);
					route_days = from_hero.GetRangeRouteDays();
				    }
    				}
    			    }
    			    break;

			    break;

			    default:
				if(tile.isPassable() || MP2::isActionObject(tile.GetObject(), from_hero.isShipMaster()))
				{
				    ShowPathOrStartMoveHero(&from_hero, index_maps);
				    route_days = from_hero.GetRangeRouteDays();
				}
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
					OpenCastle(&from_castle);
					statusWindow.Redraw();
					mixer.Enhance();
				    }
				    // select other castle
				    else
				    {
					FocusToCastle(const_cast<Castle *>(to_castle));
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

    				    FocusToHeroes(const_cast<Heroes *>(to_hero));
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

		    default:
			Dialog::QuickInfo(tile);
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
		const Point prev(gamearea.GetRect());
		gamearea.CenterFromRadar(le.MouseCursor());
		if(prev != gamearea.GetRect())
		{
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

				OpenHeroes(hero);

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
				FocusToHeroes(hero);
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

				OpenCastle(castle);

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
				FocusToCastle(castle);
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
    		    global_focus.GetHeroes().GetPath().isValid())
		    global_focus.GetHeroes().SetMove(!global_focus.GetHeroes().isEnableMove());
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
    		if(Game::Focus::HEROES == global_focus.Type()) {
		    Spell::spell_t spell = global_focus.GetHeroes().SpellBook().Open(Spell::Book::ADVN, true);
		    // TODO cast selected spell
		}
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

        // animation
        if(!(ticket % ANIMATION_MEDIUM))
        {
            cursor.Hide();

    	    if(Game::Focus::HEROES == global_focus.Type())
	    {
		Heroes & heroes = global_focus.GetHeroes();
		const Route::Path & path = heroes.GetPath();

		// if valid and enable move or anime sprite
		if(path.isValid() && (heroes.isEnableMove() || (heroes.GetSpriteIndex() < 45 && heroes.GetSpriteIndex() % 9) || heroes.GetSpriteIndex() >= 45))
		{
		    const u16 center = Maps::GetIndexFromAbsPoint(heroes.GetCenter());
		    const u16 front_index = path.GetFrontIndex();

		    // if need change through the circle
		    if(heroes.GetDirection() != Direction::Get(center, front_index))
		    {
			heroes.AngleStep(Direction::Get(center, front_index));
		    }
		    else
		    // move
		    {
			heroes.MoveStep();
		    }

		    if(heroes.GetCenter() != global_focus.Center())
		    {
			global_focus.Update();

			selectHeroes.Redraw();
			gamearea.Center(global_focus.Center());
			radar.RedrawArea(Settings::Get().MyColor());
			radar.RedrawCursor();
		    }
		}
		else
		{
		    heroes.SetMove(false);
		}
	    }

	    Maps::IncreaseAnimationTicket();

	    gamearea.Redraw();
            cursor.Show();
            display.Flip();
        }

        ++ticket;
    }

    Game::Focus & globalfocus = Game::Focus::Get();
    if(Game::Focus::HEROES == globalfocus.Type())
    {
	globalfocus.GetHeroes().ShowPath(false);
	globalfocus.Redraw();
        display.Flip();
    }

    return ENDTURN;
}
