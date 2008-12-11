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
#include <algorithm>

#include "agg.h"
#include "audio.h"
#include "engine.h"
#include "button.h"
#include "dialog.h"
#include "world.h"
#include "gamearea.h"
#include "cursor.h"
#include "radar.h"
#include "castle.h"
#include "heroes.h"
#include "splitter.h"
#include "maps_tiles.h"
#include "ground.h"
#include "gameevent.h"
#include "game_interface.h"
#include "game_statuswindow.h"
#include "game_selectobjbar.h"
#include "settings.h"
#include "route.h"
#include "game_focus.h"
#include "scripting.h"
#include "kingdom.h"

namespace Game
{
    Cursor::themes_t GetCursor(const Maps::Tiles & tile);
    void OpenCastle(Castle *castle);
    void OpenHeroes(Heroes *heroes);
    void ShowPathOrStartMoveHero(Heroes *hero, const u16 dst_index);
    Game::menu_t HumanTurn(void);
    bool CursorChangePosition(const u16 index);
    bool DiggingForArtifacts(const Heroes & hero);
    void DialogPlayersTurn(const Color::color_t);
};

void Game::DialogPlayersTurn(const Color::color_t color)
{
    const Sprite & border = AGG::GetICN(ICN::BRCREST, 6);

    Surface sign(border.w(), border.h());
    sign.Blit(border);

    switch(color)
    {
	    case Color::BLUE:	sign.Blit(AGG::GetICN(ICN::BRCREST, 0), 4, 4); break;
	    case Color::GREEN:	sign.Blit(AGG::GetICN(ICN::BRCREST, 1), 4, 4); break;
	    case Color::RED:	sign.Blit(AGG::GetICN(ICN::BRCREST, 2), 4, 4); break;
	    case Color::YELLOW:	sign.Blit(AGG::GetICN(ICN::BRCREST, 3), 4, 4); break;
	    case Color::ORANGE:	sign.Blit(AGG::GetICN(ICN::BRCREST, 4), 4, 4); break;
	    case Color::PURPLE:	sign.Blit(AGG::GetICN(ICN::BRCREST, 5), 4, 4); break;
   	    default: break;
    }

    Dialog::SpriteInfo("", Color::String(color) + " player's turn", sign);
}

bool Game::CursorChangePosition(const u16 index)
{
    static u16 old_pos = MAXU16;

    if(index != old_pos)
    {
	old_pos = index;

	return true;
    }

    return false;
}

Game::menu_t Game::StartGame(void)
{
    // cursor
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    SetFixVideoMode();

    cursor.Hide();
    display.Fade();

    // Load maps
    world.LoadMaps(conf.FileInfo().FileMaps());

    // preload sounds
    Game::PreloadLOOPSounds();

    // set controls type for players
    switch(conf.GameType())
    {
    	case Game::HOTSEAT:
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.Players())
        	world.GetKingdom(color).SetControl(LOCAL);
    	    break;

    	case Game::NETWORK:
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.Players())
        	world.GetKingdom(color).SetControl(color == conf.MyColor() ? LOCAL : REMOTE);
    	    break;

    	default:
    	    world.GetKingdom(conf.MyColor()).SetControl(LOCAL);
    	    break;
    }

    GameArea & areaMaps = GameArea::Get();
    areaMaps.Build();

    Mixer::Reset();

    // Create radar
    Radar & radar = Radar::Get();
    radar.Build();

    Game::Focus & global_focus = Focus::Get();
    global_focus.Reset();

    // draw interface
    Game::Interface::Get().Draw();

    Game::menu_t m = ENDTURN;

    while(1)
    {
	// AI move
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	{
	    Kingdom & kingdom = world.GetKingdom(color);
	    if(kingdom.isPlay())
	    {
		if(1 < conf.Debug()) kingdom.Dump();

		radar.HideArea();
		conf.SetCurrentColor(color);
		world.ClearFog(color);
                Mixer::PauseLoops();

		switch(kingdom.Control())
		{
	        case LOCAL:
		    Mixer::Enhance();
		    Mixer::Reset();
		    if(Game::HOTSEAT == conf.GameType())
		    {
			conf.SetMyColor(Color::GRAY);
			SelectBarCastle::Get().Hide();
			SelectBarHeroes::Get().Hide();
			StatusWindow::Get().SetState(StatusWindow::UNKNOWN);
			StatusWindow::Get().Redraw();
			areaMaps.Redraw();
			display.Flip();
			DialogPlayersTurn(color);
		    }
		    conf.SetMyColor(color);
		    m = HumanTurn();
		    Mixer::Reduce();
		    if(m != ENDTURN) goto OUTDOOR;
		    break;
	        case REMOTE:
		    cursor.SetThemes(Cursor::WAIT);
		    cursor.Show();
	            display.Flip();
		    Error::Verbose("Game::Turns: network player: " + Color::String(color));
		    cursor.Hide();
		    break;
	        case AI:
		    kingdom.AITurns();
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

    return m == ENDTURN ? QUITGAME : m;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle)
{
    if(! castle) return;

    Mixer::Reduce();

    Cursor & cursor = Cursor::Get();
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    Display & display = Display::Get();
    std::vector<Castle *>::const_iterator it = std::find(myCastles.begin(), myCastles.end(), castle);
    Game::Focus & globalfocus = Game::Focus::Get();

    if(it != myCastles.end())
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
	    cursor.Hide();
	    globalfocus.Set(*it);
	    globalfocus.Redraw();
	    cursor.Show();
	    display.Flip();

	    result = (*it)->OpenDialog();

	    if(Dialog::PREV == result)
	    {
		if(it == myCastles.begin()) it = myCastles.end();
		--it;
	    }
	    else
	    if(Dialog::NEXT == result)
	    {
		++it;
		if(it == myCastles.end()) it = myCastles.begin();
	    }
	}
    }

    cursor.Hide();
    Game::SelectBarCastle::Get().Redraw(*it);
    if(Heroes *hero = const_cast<Heroes *>((*it)->GetHeroes())) globalfocus.Set(hero);
    globalfocus.Redraw();
    cursor.Show();
    Display::Get().Flip();
    Mixer::Enhance();
}

/* open heroes wrapper */
void Game::OpenHeroes(Heroes *hero)
{
    if(! hero) return;

    Mixer::Reduce();

    Cursor & cursor = Cursor::Get();
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();
    Display & display = Display::Get();
    std::vector<Heroes *>::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), hero);
    Game::Focus & globalfocus = Game::Focus::Get();

    if(it != myHeroes.end())
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
	    cursor.Hide();
	    globalfocus.Set(*it);
	    globalfocus.Redraw();
	    cursor.Show();
	    display.Flip();

	    result = (*it)->OpenDialog();

	    switch(result)
	    {
		case Dialog::PREV:
	    	    if(it == myHeroes.begin()) it = myHeroes.end();
		    --it;
		    break;

		case Dialog::NEXT:
		    ++it;
		    if(it == myHeroes.end()) it = myHeroes.begin();
		    break;

		case Dialog::DISMISS:
		    AGG::PlaySound(M82::KILLFADE);

		    (*it)->GetPath().Hide();
		    GameArea::Get().Redraw();
		    Display::Get().Flip();

		    (*it)->FadeOut();
		    (*it)->SetFreeman(0);
		    myKingdom.RemoveHeroes(*it);

		    cursor.Hide();
    		    globalfocus.Reset(Game::Focus::HEROES);
    		    globalfocus.Redraw();

		    result = Dialog::CANCEL;
		    break;
	
		default: break;
	    }
	}
    }

    globalfocus.Redraw();
    cursor.Show();
    Display::Get().Flip();
    Mixer::Enhance();
}

/* return changee cursor */
Cursor::themes_t Game::GetCursor(const Maps::Tiles & tile)
{
    if(0 == Settings::Get().Debug() && tile.isFog(Settings::Get().MyColor())) return Cursor::POINTER;

    const Game::Focus & focus = Game::Focus::Get();

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
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE : Cursor::FIGHT;
				    case 2:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE2 : Cursor::FIGHT2;
				    case 3:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE3 : Cursor::FIGHT3;
				    default:	return from_hero.GetColor() == to_hero->GetColor() ? Cursor::CHANGE4 : Cursor::FIGHT4;
				}
			}
    		    }
    		    break;

    		    case MP2::OBJ_WATERCHEST:
			switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			{
			    case 0:	return Cursor::POINTER;
			    case 1:	return Cursor::REDBOAT;
			    case 2:	return Cursor::REDBOAT2;
			    case 3:	return Cursor::REDBOAT3;
			    default:	return Cursor::REDBOAT4;
			}
			break;

		    case MP2::OBJ_COAST:
			switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			{
			    case 0:	return Cursor::POINTER;
			    case 1:	return Cursor::ANCHOR;
			    case 2:	return Cursor::ANCHOR2;
			    case 3:	return Cursor::ANCHOR3;
			    default:	return Cursor::ANCHOR4;
			}
			break;

		    default:
			    if(MP2::isActionObject(tile.GetObject(), true))
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return Cursor::REDBOAT;
				    case 2:	return Cursor::REDBOAT2;
				    case 3:	return Cursor::REDBOAT3;
				    default:	return Cursor::REDBOAT4;
				}
			    else
			    if(tile.isPassable(&from_hero))
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return Cursor::BOAT;
				    case 2:	return Cursor::BOAT2;
				    case 3:	return Cursor::BOAT3;
				    default:	return Cursor::BOAT4;
				}
			    else
				return Cursor::POINTER;
		}
	    }
	    else
	    {
		switch(tile.GetObject())
		{
    		    case MP2::OBJ_MONSTER:
			switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			{
			    case 0:	return Cursor::POINTER;
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
			{
			    if(from_hero.GetColor() == castle->GetColor()) return Cursor::CASTLE;
			    else
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
				case 1:	return Cursor::FIGHT;
				case 2:	return Cursor::FIGHT2;
				case 3:	return Cursor::FIGHT3;
				default:return Cursor::FIGHT4;
			    }
			}
    		    }
    		    break;

    		    case MP2::OBJ_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
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
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
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
			switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			{
			    case 0:	return Cursor::POINTER;
			    case 1:	return Cursor::BOAT;
			    case 2:	return Cursor::BOAT2;
			    case 3:	return Cursor::BOAT3;
			    default:	return Cursor::BOAT4;
			}
			break;

    		    case MP2::OBJ_TREASURECHEST:
			switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			{
			    case 0:	return Cursor::POINTER;
			    case 1:	return Cursor::ACTION;
			    case 2:	return Cursor::ACTION2;
			    case 3:	return Cursor::ACTION3;
			    default:	return Cursor::ACTION4;
			}
			break;

		    default:
			    if(MP2::isActionObject(tile.GetObject(), false))
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return Cursor::ACTION;
				    case 2:	return Cursor::ACTION2;
				    case 3:	return Cursor::ACTION3;
				    default:	return Cursor::ACTION4;
				}
			    else
			    if(tile.isPassable(&from_hero))
			    {
				const bool protection = Maps::TileUnderProtection(tile.GetIndex());
				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return protection ? Cursor::FIGHT : Cursor::MOVE;
				    case 2:	return protection ? Cursor::FIGHT2 : Cursor::MOVE2;
				    case 3:	return protection ? Cursor::FIGHT3 : Cursor::MOVE3;
				    default:	return protection ? Cursor::FIGHT4 : Cursor::MOVE4;
				}
			    }
			    else
				return Cursor::POINTER;
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
    Display & display = Display::Get();

    // show path
    if(path.GetDestinationIndex() != dst_index)
    {
        hero->SetMove(false);
	path.Calculate(dst_index);
        if(1 < Settings::Get().Debug()) path.Dump();
        path.Show();
    }
    // start move
    else
    if(path.isValid())
    {
	Cursor::Get().Hide();
        Game::Focus::Get().Set(hero);
        Game::Focus::Get().Redraw();
	Cursor::Get().Show();
	display.Flip();
        hero->SetMove(true);
    }
}

bool Game::ShouldAnimate(u32 ticket)
{
    return Game::ShouldAnimateInfrequent(ticket, 1);
}

bool Game::ShouldAnimateInfrequent(u32 ticket, u32 modifier)
{
    //TODO: Use user-selected speed instead of medium by default
    return !(ticket % (1 < modifier ? (ANIMATION_SPEED - (2 * Settings::Get().Animation())) * modifier : ANIMATION_SPEED - (2 * Settings::Get().Animation())));
}

Game::menu_t Game::HumanTurn(void)
{
    Game::Focus & global_focus = Focus::Get();
    Radar & radar = Radar::Get();
    GameArea & gamearea = GameArea::Get();
    Game::StatusWindow & statusWindow = Game::StatusWindow::Get();

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    const Settings & conf = Settings::Get();

    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, gamearea.GetRect().w * TILEWIDTH, gamearea.GetRect().h * TILEWIDTH);
    const Rect areaScrollLeft(0, 0, BORDERWIDTH, display.h());
    const Rect areaScrollRight(display.w() - BORDERWIDTH, 0, BORDERWIDTH, display.h());
    const Rect areaScrollTop(0, 0, display.w() - RADARWIDTH, BORDERWIDTH);
    const Rect areaScrollBottom(0, display.h() - BORDERWIDTH, display.w(), BORDERWIDTH);
    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    u8 scrollDir = GameArea::NONE;
    LocalEvent & le = LocalEvent::GetLocalEvent();
    u32 ticket = 0;

    Game::Interface & I = Game::Interface::Get();

    Button &buttonScrollHeroesUp = I.buttonScrollHeroesUp;
    Button &buttonScrollCastleUp = I.buttonScrollCastleUp;
    Button &buttonNextHero = I.buttonNextHero;
    Button &buttonMovement = I.buttonMovement;
    Button &buttonKingdom = I.buttonKingdom;
    Button &buttonSpell = I.buttonSpell;
    Button &buttonEndTur = I.buttonEndTur;
    Button &buttonAdventure = I.buttonAdventure;
    Button &buttonFile = I.buttonFile;
    Button &buttonSystem = I.buttonSystem;
    Button &buttonScrollHeroesDown = I.buttonScrollHeroesDown;
    Button &buttonScrollCastleDown = I.buttonScrollCastleDown;

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();

    if(Game::HOTSEAT == conf.GameType() || conf.Original()) global_focus.Reset();

    Game::SelectBarCastle & selectCastle = Game::SelectBarCastle::Get();
    Game::SelectBarHeroes & selectHeroes = Game::SelectBarHeroes::Get();

    Splitter & splitCastle = selectCastle.GetSplitter();
    Splitter & splitHeroes = selectHeroes.GetSplitter();

    selectHeroes.Redraw();
    selectCastle.Redraw();


    switch(global_focus.Type())
    {
	case Focus::UNSEL:
    	    if(myHeroes.size())  global_focus.Set(myHeroes.front());
    	    else
	    if(myCastles.size()) global_focus.Set(myCastles.front());
	    break;
	case Focus::HEROES:
	    if(conf.Original()) global_focus.Set(myHeroes.front());
	    if(global_focus.GetHeroes().GetPath().isValid()) global_focus.GetHeroes().GetPath().Show();
	    break;
	case Focus::CASTLE:
	    if(conf.Original()) global_focus.Set(myCastles.front());
	    break;
    }

    //Override whatever the focus set the window to show
    statusWindow.SetState(StatusWindow::DAY);
    global_focus.Redraw();

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(global_focus.Center()).GetGround()));
    Game::EnvironmentSoundMixer();

    cursor.Show();
    display.Flip();

    // new week dialog
    if(1 < world.CountWeek() && world.BeginWeek())
    {
	const Week::type_t name = world.GetWeekType();
	const std::string message(name == Week::PLAGUE ? "All populations are halved." : "All dwellings increase population.");
	if(world.BeginMonth())
	    Dialog::Message("Astrologers proclaim month of the " + Week::GetString(name) + ".", message, Font::BIG, Dialog::OK);
	else
	    Dialog::Message("Astrologers proclaim week of the " + Week::GetString(name) + ".", message, Font::BIG, Dialog::OK);
    }

    // show event day
    {
	const GameEvent::Day* event_day = world.GetEventDay(conf.MyColor());
	if(event_day) Dialog::ResourceInfo(event_day->GetMessage(), "", event_day->GetResource());
    }

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
	if(le.MouseCursor(areaScrollLeft)) scrollDir |= GameArea::LEFT;
        else
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight)) scrollDir |= GameArea::RIGHT;
	
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop)) scrollDir |= GameArea::TOP;
	else
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom)) scrollDir |= GameArea::BOTTOM;

	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
	    const Point & mouse_coord = le.MouseCursor();
	    const int index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
	    if(0 > index_maps) continue;

	    const Maps::Tiles & tile = world.GetTiles(index_maps);
	    const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);

	    // change cusor if need
	    if(CursorChangePosition(index_maps)) cursor.SetThemes(GetCursor(tile));

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
					cursor.Hide();
					global_focus.Set(const_cast<Castle *>(to_castle));
					global_focus.Redraw();
					cursor.Show();
					display.Flip();
				    }
				    else
					ShowPathOrStartMoveHero(&from_hero, Maps::GetIndexFromAbsPoint(to_castle->GetCenter()));
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
					OpenHeroes(&from_hero);
				    else
					ShowPathOrStartMoveHero(&from_hero, index_maps);
    				}
    			    }
    			    break;

			    break;

			    default:
				if(tile.isPassable(&from_hero) || MP2::isActionObject(tile.GetObject(), from_hero.isShipMaster()))
				    ShowPathOrStartMoveHero(&from_hero, index_maps);
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
					OpenCastle(&from_castle);
				    // select other castle
				    else
				    {
					cursor.Hide();
					global_focus.Set(const_cast<Castle *>(to_castle));
					global_focus.Redraw();
					cursor.Show();
					display.Flip();
				    }
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
    				    cursor.Hide();
    				    global_focus.Set(const_cast<Heroes *>(to_hero));
    				    global_focus.Redraw();
    				    cursor.Show();
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
		if(conf.Debug()) tile.DebugInfo();

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
	    if((le.MouseWheelUp(selectHeroes.GetArea()) ||
		le.MouseWheelUp(splitHeroes.GetRect()) ||
		le.MouseClickLeft(buttonScrollHeroesUp)) && selectHeroes.Prev())
	    {
		cursor.Hide();
		selectHeroes.Redraw();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Castle Up
	    if((le.MouseWheelUp(selectCastle.GetArea()) ||
		le.MouseWheelUp(splitCastle.GetRect()) ||
		le.MouseClickLeft(buttonScrollCastleUp)) && selectCastle.Prev())
	    {
		cursor.Hide();
		selectCastle.Redraw();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Heroes Down
	    if((le.MouseWheelDn(selectHeroes.GetArea()) ||
		le.MouseWheelDn(splitHeroes.GetRect()) ||
		le.MouseClickLeft(buttonScrollHeroesDown)) && 
		selectHeroes.Next())
	    {
		cursor.Hide();
		selectHeroes.Redraw();
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Scroll Castle Down
	    if((le.MouseWheelDn(selectCastle.GetArea()) ||
		le.MouseWheelDn(splitCastle.GetRect())  ||
		le.MouseClickLeft(buttonScrollCastleDown)) &&
		selectCastle.Next())
	    {
		cursor.Hide();
		selectCastle.Redraw();
		cursor.Show();
		display.Flip();
	    }

    	    // move splitter cursor castle
    	    if(le.MousePressLeft(selectCastle.GetSplitter().GetRect()) &&
    		myCastles.size() > I.CountIcons())
    	    {
    		Splitter & splitter = selectCastle.GetSplitter();
    		u32 seek = (le.MouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
        	if(seek > myCastles.size() - I.CountIcons()) seek = myCastles.size() - I.CountIcons();

        	cursor.Hide();
        	splitter.Move(seek);
		selectCastle.SetTop(seek);
		selectCastle.Redraw();
        	cursor.Show();
        	display.Flip();
	    }
	    else
    	    // move splitter cursor heroes
    	    if(le.MousePressLeft(selectHeroes.GetSplitter().GetRect()) &&
    		myHeroes.size() > I.CountIcons())
    	    {
    		Splitter & splitter = selectHeroes.GetSplitter();
    		u32 seek = (le.MouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
        	if(seek > myHeroes.size() - I.CountIcons()) seek = myHeroes.size() - I.CountIcons();

        	cursor.Hide();
        	splitter.Move(seek);
		selectHeroes.SetTop(seek);
		selectHeroes.Redraw();
        	cursor.Show();
        	display.Flip();
	    }

	    // click Heroes Icons
	    if(const Heroes * hero = selectHeroes.MouseClickLeft())
	    {
		if(hero == selectHeroes.Selected())
		    OpenHeroes(const_cast<Heroes *>(hero));
		else
		{
		    cursor.Hide();
		    global_focus.Set(const_cast<Heroes *>(hero));
		    global_focus.Redraw();
		    cursor.Show();
		    display.Flip();
		}
	    }
	    else
	    if(const Heroes * hero = selectHeroes.MousePressRight())
	    {
		cursor.Hide();
		Dialog::QuickInfo(*hero);
		cursor.Show();
		display.Flip();
	    }
	    else
	    // click Castle Icons
	    if(const Castle * cstl = selectCastle.MouseClickLeft())
	    {
		if(cstl == selectCastle.Selected())
		    OpenCastle(const_cast<Castle *>(cstl));
		else
		{
		    cursor.Hide();
		    global_focus.Set(const_cast<Castle *>(cstl));
		    global_focus.Redraw();
		    cursor.Show();
		    display.Flip();
		}
	    }
	    else
	    if(const Castle * cstl = selectCastle.MousePressRight())
	    {
		cursor.Hide();
		Dialog::QuickInfo(*cstl);
		cursor.Show();
		display.Flip();
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
#ifdef WITH_LUA
	        Lua::DoString("alert()");
	        Lua::DoString("alert1('test')");
	        Lua::DoString("msg('header', 'text')");
	        Lua::DoString("message('header', 'text', 0, 2)");
#endif
	    }
	    else
	    // click Cast Spell
	    if(le.MouseClickLeft(buttonSpell))
	    {
    		if(Game::Focus::HEROES == global_focus.Type()) {
		    Spell::spell_t spell = global_focus.GetHeroes().SpellBook().Open(Spell::Book::ADVN, true);
		    // TODO cast selected spell
		    Error::Verbose("spell selected: " + Spell::String(spell));
		}
	    }
	    else
	    // click End Turn
	    if(le.MouseClickLeft(buttonEndTur))
	    {
		if(!myKingdom.HeroesMayStillMove() ||
		    Dialog::YES == Dialog::Message("", "One or more heroes may still move, are you sure you want to end your turn?", Font::BIG, Dialog::YES | Dialog::NO))
	    	    break;
	    }
	    else
    	    // click AdventureOptions
	    if(le.MouseClickLeft(buttonAdventure))
	    {
		Mixer::Reduce();

		switch(Dialog::AdventureOptions(Game::Focus::HEROES == global_focus.Type()))
		{
		    case Dialog::WORLD:	break;
		    case Dialog::PUZZLE:break;
		    case Dialog::INFO:	break;
		    case Dialog::DIG:	DiggingForArtifacts(global_focus.GetHeroes());

		    default: break;
		}

		Mixer::Enhance();
    	    }
	    else
	    // click FileOptions
	    if(le.MouseClickLeft(buttonFile))
	    {
		Mixer::Reduce();

		Game::menu_t result = Dialog::FileOptions();
	    
		switch(result)
		{
		    case Game::NEWGAME:
		    case Game::LOADGAME:
		    case Game::QUITGAME:
			Mixer::Reset();
			return result;

		    case Game::SAVEGAME:
			break;

		    default:
			break;
		}

		Mixer::Enhance();
	    }
	    else
	    // click SystemOptions
	    if(le.MouseClickLeft(buttonSystem))
	    {
		Mixer::Reduce();

		// Change and save system settings
		const u8 changes = Dialog::SystemOptions();
		
		if(0x02 & changes)
		{
		    const u16 vol1 = conf.SoundVolume() * MAXVOLUME / 10;
		    Mixer::Volume(-1, vol1);
		    Game::EnvironmentSoundMixer();
		}
		if(0x04 & changes)
		{
		    const u16 vol2 = conf.MusicVolume() * MAXVOLUME / 10;
		    Music::Volume(vol2);
		    Game::EnvironmentSoundMixer();
		}
		if(0x08 & changes)
		{
		    cursor.Hide();
		    if(selectHeroes.isSelected()) selectHeroes.Unselect();
		    if(selectCastle.isSelected()) selectCastle.Unselect();
		    I.Draw();
		    global_focus.Reset(Focus::HEROES);
		    global_focus.Redraw();
		    cursor.Show();
		    display.Flip();
		}

		Mixer::Enhance();
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

	// animation
        if(Game::ShouldAnimateInfrequent(ticket, 1))
        {
            cursor.Hide();

            if(scrollDir)
            {
    		cursor.SetThemes(GameArea::ScrollToCursor(scrollDir));
    		gamearea.Scroll(scrollDir);

    		// need stop hero
    		if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
    		    global_focus.GetHeroes().SetMove(false);

        	scrollDir = GameArea::NONE;
            }
	    else
    	    if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().Move())
	    {
		AGG::PlayMusic(MUS::FromGround(world.GetTiles(global_focus.Center()).GetGround()));
		Game::EnvironmentSoundMixer();

                statusWindow.Redraw();
                selectHeroes.Redraw(&global_focus.GetHeroes());
                gamearea.Center(global_focus.Center());
	    }

	    if(Game::ShouldAnimateInfrequent(ticket, 12)) Maps::IncreaseAnimationTicket();

	    gamearea.Redraw();
            radar.RedrawArea(conf.MyColor());
            radar.RedrawCursor();
            cursor.Show();
            display.Flip();
        }

        ++ticket;
    }

    cursor.Hide();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	global_focus.GetHeroes().ShowPath(false);
	global_focus.Redraw();
        display.Flip();
    }

    return ENDTURN;
}

bool Game::DiggingForArtifacts(const Heroes & hero)
{
    if(hero.GetMaxMovePoints() == hero.GetMovePoints())
    {
	if(0 != world.GetTiles(hero.GetCenter()).GetSize1())
	{
	    Dialog::Message("", "Try searching on clear ground.", Font::BIG, Dialog::OK);
	    return false;
	}

	AGG::PlaySound(M82::DIGSOUND);

	const_cast<Heroes &>(hero).ResetMovePoints();
	const Artifact::artifact_t ultimate = world.DiggingForUltimateArtifacts(hero.GetCenter());

	if(Artifact::UNKNOWN == ultimate)
	    Dialog::Message("", "Nothing here. Where could it be?", Font::BIG, Dialog::OK);
	else
	{
	    // TODO: congratulations!
	    // check returns
	    const_cast<Heroes &>(hero).PickupArtifact(ultimate);
	    Dialog::Message("Congratulations!!!", "After spending many hours digging here, you have uncovered the " + Artifact::String(ultimate), Font::BIG, Dialog::OK);
	}

	Cursor::Get().Hide();
	Game::SelectBarHeroes::Get().Redraw(&hero);
	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
	Dialog::Message("", "Digging for artifacts requires a whole day, try again tomorrow.", Font::BIG, Dialog::OK);

    return false;
}
