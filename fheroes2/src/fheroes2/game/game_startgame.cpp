/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#ifdef AI
#undef AI
#endif

#include "ai.h"
#include "agg.h"
#include "engine.h"
#include "button.h"
#include "dialog.h"
#include "world.h"
#include "cursor.h"
#include "castle.h"
#include "heroes.h"
#include "splitter.h"
#include "maps_tiles.h"
#include "ground.h"
#include "gameevent.h"
#include "game_interface.h"
#include "game_io.h"
#include "settings.h"
#include "route.h"
#include "game_focus.h"
#include "kingdom.h"
#include "localclient.h"
#include "ai.h"

namespace Game
{
    Cursor::themes_t GetCursor(const s32);
    Cursor::themes_t GetCursorFocusCastle(const Castle &, const Maps::Tiles &);
    Cursor::themes_t GetCursorFocusHeroes(const Heroes &, const Maps::Tiles &);
    Cursor::themes_t GetCursorFocusShipmaster(const Heroes &, const Maps::Tiles &);

    void ShowPathOrStartMoveHero(Heroes *hero, const s32 dst_index);
    menu_t HumanTurn(bool);
    bool DiggingForArtifacts(Heroes & hero);
    void MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t direct);

    void MouseCursorAreaClickLeft(s32);
    void MouseCursorAreaPressRight(s32);

    void StartNewGame(menu_t &);

    void KeyArrowPress(Direction::vector_t);

    void NewWeekDialog(void);
    void ShowEventDay(void);
    void ShowWarningLostTowns(menu_t &);
}

void Game::MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t direct)
{
    if(Maps::isValidDirection(hero.GetIndex(), direct))
    {
	s32 dst = Maps::GetDirectionIndex(hero.GetIndex(), direct);
	const Maps::Tiles & tile = world.GetTiles(dst);
	bool allow = false;

	switch(tile.GetObject())
	{
    	    case MP2::OBJN_CASTLE:
    	    {
    		const Castle *to_castle = world.GetCastle(dst);
		if(to_castle)
		{
		    dst = to_castle->GetIndex();
		    allow = true;
		}
		break;
	    }

    	    case MP2::OBJ_BOAT:
    	    case MP2::OBJ_CASTLE:
    	    case MP2::OBJ_HEROES:
    	    case MP2::OBJ_MONSTER:
		allow = true;
		break;

	    default:
		allow = (tile.isPassable(&hero, Direction::CENTER, false) ||
				MP2::isActionObject(tile.GetObject(), hero.isShipMaster()));
		break;
	}

	if(allow) ShowPathOrStartMoveHero(&hero, dst);
    }
}
                                   
void Game::DialogPlayers(u8 color, std::string str)
{
    const Player* player = Settings::Get().GetPlayers().Get(color);
    String::Replace(str, "%{color}", (player ? player->name : Color::String(color)));

    const Sprite & border = AGG::GetICN(ICN::BRCREST, 6);

    Surface sign(border.w(), border.h());
    border.Blit(sign);

    switch(color)
    {
	    case Color::BLUE:	AGG::GetICN(ICN::BRCREST, 0).Blit(4, 4, sign); break;
	    case Color::GREEN:	AGG::GetICN(ICN::BRCREST, 1).Blit(4, 4, sign); break;
	    case Color::RED:	AGG::GetICN(ICN::BRCREST, 2).Blit(4, 4, sign); break;
	    case Color::YELLOW:	AGG::GetICN(ICN::BRCREST, 3).Blit(4, 4, sign); break;
	    case Color::ORANGE:	AGG::GetICN(ICN::BRCREST, 4).Blit(4, 4, sign); break;
	    case Color::PURPLE:	AGG::GetICN(ICN::BRCREST, 5).Blit(4, 4, sign); break;
   	    default: break;
    }

    Dialog::SpriteInfo("", str, sign);
}

Game::menu_t Game::StartGame(void)
{
    SetFixVideoMode();
    ::AI::Init();

    // cursor
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    if(! conf.LoadedGameVersion())
	GameOver::Result::Get().Reset();

    cursor.Hide();

    AGG::ICNRegistryFreeObjects();
    AGG::ICNRegistryEnable(false);

    AGG::ResetMixer();

    // draw interface
    Interface::Basic & I = Interface::Basic::Get();

    Interface::GameArea & areaMaps = I.gameArea;
    areaMaps.Build();

    // init focus: set first hero
    GameFocus::Reset(GameFocus::FIRSTHERO);

    Interface::Radar & radar = I.radar;
    Interface::StatusWindow& statusWin = I.statusWindow;

    I.iconsPanel.ResetIcons();

    radar.Build();

    I.Redraw(REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_BORDER);
    I.iconsPanel.HideIcons();

    bool skip_turns = conf.LoadedGameVersion();
    GameOver::Result & gameResult = GameOver::Result::Get();
    Game::menu_t m = ENDTURN;
    const Players & players = conf.GetPlayers();

    while(m == ENDTURN)
    {
	if(!skip_turns) world.NewDay();

	for(Players::const_iterator
	    it = players.begin(); it != players.end(); ++it) if(*it)
	{
	    const Player & player = (**it);
	    Kingdom & kingdom = world.GetKingdom(player.color);

	    if(!kingdom.isPlay() ||
		(skip_turns && player.color != conf.CurrentColor())) continue;

	    DEBUG(DBG_GAME, DBG_INFO, std::endl << world.DateString() << ", " << "color: " <<
		    Color::String(player.color) << ", resource: " << kingdom.GetFunds().String());

	    radar.SetHide(true);
	    I.SetRedraw(REDRAW_RADAR);
	    conf.SetCurrentColor(player.color);
	    world.ClearFog(player.color);
	    kingdom.ActionBeforeTurn();

	    switch(kingdom.GetControl())
	    {
		case CONTROL_HUMAN:
		    if(conf.GameType(Game::TYPE_HOTSEAT))
		    {
			cursor.Hide();
			I.iconsPanel.HideIcons();
			statusWin.Reset();
			I.SetRedraw(REDRAW_GAMEAREA | REDRAW_STATUS | REDRAW_ICONS);
			I.Redraw();
			display.Flip();
			DialogPlayers(player.color, _("%{color} player's turn"));
		    }
		    I.SetRedraw(REDRAW_ICONS);
		    I.iconsPanel.ShowIcons();
		    m = HumanTurn(skip_turns);
		    if(skip_turns) skip_turns = false;
		break;

		// CONTROL_AI turn
		default:
        	    if(m == ENDTURN)
		    {
			statusWin.Reset();
			statusWin.SetState(STATUS_AITURN);

			// for pocketpc: show status window
			if(conf.QVGA() && !conf.ShowStatus())
			{
			    conf.SetShowStatus(true);
			    I.SetRedraw(REDRAW_STATUS);
			}

			cursor.Hide();
			cursor.SetThemes(Cursor::WAIT);
			I.Redraw();
			cursor.Show();
			display.Flip();

			::AI::KingdomTurn(kingdom);
		    }
		break;
	    }

	    if(m != ENDTURN ||
		gameResult.LocalCheckGameOver(m)) break;
	}

	DELAY(10);
    }

    if(m == ENDTURN)
	display.Fill(0, 0, 0);
    else
	display.Fade();

    return m == ENDTURN ? QUITGAME : m;
}

/* open castle wrapper */
void Game::OpenCastleDialog(Castle *castle)
{
    if(! castle) return;

    Mixer::Pause();

    //Cursor & cursor = Cursor::Get();
    const Settings & conf = Settings::Get();
    Kingdom & myKingdom = world.GetKingdom(conf.CurrentColor());
    const KingdomCastles & myCastles = myKingdom.GetCastles();
    Display & display = Display::Get();
    KingdomCastles::const_iterator it = std::find(myCastles.begin(), myCastles.end(), castle);
    Interface::StatusWindow::ResetTimer();
    bool need_fade = conf.ExtUseFade() && 640 == display.w() && 480 == display.h();

    if(it != myCastles.end() || Players::isFriends(conf.CurrentColor(), castle->GetColor()))
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
/*
	    if(show_position)
	    {
		GameFocus::Set(*it);
		GameFocus::SetRedraw();
		cursor.Hide();
		Interface::Basic::Get().Redraw();
		cursor.Show();
		display.Flip();
		DELAY(100);
	    }
*/
	    if(Settings::Get().ExtLowMemory())
    		AGG::ICNRegistryEnable(true);

	    result = castle->OpenDialog((conf.CurrentColor() != castle->GetColor()), need_fade);
	    if(need_fade) need_fade = false;

	    if(Settings::Get().ExtLowMemory())
	    {
    		AGG::ICNRegistryEnable(false);
    		AGG::ICNRegistryFreeObjects();
	    }

	    if(it != myCastles.end())
	    {
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

		castle = (*it);
	    }
	}
    }

    Music::Reset();

    if(it != myCastles.end())
    {
	// focus priority: castle heroes
	Heroes* hero = (*it)->GetHeroes().Guest();

	if(hero && !hero->Modes(Heroes::GUARDIAN))
	    GameFocus::Set(hero);
	else
	    GameFocus::Set(*it);
    }

    GameFocus::SetRedraw();
}

/* open heroes wrapper */
void Game::OpenHeroesDialog(Heroes *hero)
{
    if(! hero) return;

    //Cursor & cursor = Cursor::Get();
    const Settings & conf = Settings::Get();
    Kingdom & myKingdom = world.GetKingdom(hero->GetColor());
    const KingdomHeroes & myHeroes = myKingdom.GetHeroes();
    Display & display = Display::Get();
    KingdomHeroes::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), hero);
    Interface::StatusWindow::ResetTimer();
    Interface::Basic & I = Interface::Basic::Get();
    bool need_fade = conf.ExtUseFade() && 640 == display.w() && 480 == display.h();

    if(it != myHeroes.end())
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
	    if(Settings::Get().ExtLowMemory())
		AGG::ICNRegistryEnable(true);

	    result = (*it)->OpenDialog(false, need_fade);
	    if(need_fade) need_fade = false;

	    if(Settings::Get().ExtLowMemory())
	    {
    		AGG::ICNRegistryEnable(false);
    		AGG::ICNRegistryFreeObjects();
	    }

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
		    I.SetRedraw(REDRAW_GAMEAREA);

		    (*it)->FadeOut();
		    (*it)->SetFreeman(0);
		    it = myHeroes.begin();
		    result = Dialog::CANCEL;
		    break;

		default: break;
	    }
	}
    }

    if(it != myHeroes.end())
	GameFocus::Set(*it);
    else
        GameFocus::Reset(GameFocus::HEROES);

    GameFocus::SetRedraw();
}

/* return changee cursor */
Cursor::themes_t Game::GetCursorFocusCastle(const Castle & from_castle, const Maps::Tiles & tile)
{
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
    	    const Heroes *heroes = tile.GetHeroes();

	    if(NULL != heroes)
    		return heroes->GetColor() == from_castle.GetColor() ? Cursor::HEROES : Cursor::POINTER;
	}
	break;

	default: break;
    }

    return Cursor::POINTER;
}

Cursor::themes_t Game::GetCursorFocusShipmaster(const Heroes & from_hero, const Maps::Tiles & tile)
{
    const Settings & conf = Settings::Get();
    const bool water = tile.isWater();

    switch(tile.GetObject())
    {
	case MP2::OBJ_MONSTER:
    	    return water ? Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(tile.GetIndex())) : Cursor::POINTER;

	case MP2::OBJ_BOAT:
	    return Cursor::POINTER;

	case MP2::OBJN_CASTLE:
    	case MP2::OBJ_CASTLE:
	{
    	    const Castle* castle = world.GetCastle(tile.GetIndex());

    	    if(castle)
    		return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
    	}
    	break;

	case MP2::OBJ_HEROES:
	{
	    const Heroes* to_hero = tile.GetHeroes();

    	    if(to_hero)
	    {
		if(! to_hero->isShipMaster())
		    return from_hero.GetColor() == to_hero->GetColor() ? Cursor::HEROES : Cursor::POINTER;
		else
		if(to_hero->GetCenter() == from_hero.GetCenter())
		    return Cursor::HEROES;
		else
		if(from_hero.GetColor() == to_hero->GetColor())
		    return Cursor::DistanceThemes(Cursor::CHANGE, from_hero.GetRangeRouteDays(tile.GetIndex()));
		else
		if(Players::isFriends(from_hero.GetColor(), to_hero->GetColor()))
		    return conf.ExtUnionsAllowHeroesMeetings() ? Cursor::CHANGE : Cursor::POINTER;
		else
		if(to_hero->AllowBattle(false))
		    return Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(tile.GetIndex()));
	    }
    	}
    	break;

	case MP2::OBJ_COAST:
	    return Cursor::DistanceThemes(Cursor::ANCHOR, from_hero.GetRangeRouteDays(tile.GetIndex()));

	default:
	    if(water)
	    {
		if(MP2::isWaterObject(tile.GetObject()))
		    return Cursor::DistanceThemes(Cursor::REDBOAT, from_hero.GetRangeRouteDays(tile.GetIndex()));
		else
		if(tile.isPassable(&from_hero, Direction::CENTER, false))
		    return Cursor::DistanceThemes(Cursor::BOAT, from_hero.GetRangeRouteDays(tile.GetIndex()));
	    }
	break;
    }

    return Cursor::POINTER;
}

Cursor::themes_t Game::GetCursorFocusHeroes(const Heroes & from_hero, const Maps::Tiles & tile)
{
    const Settings & conf = Settings::Get();

    if(from_hero.Modes(Heroes::ENABLEMOVE))
	return Cursor::Get().Themes();
    else
    if(from_hero.isShipMaster())
	return GetCursorFocusShipmaster(from_hero, tile);

    switch(tile.GetObject())
    {
	case MP2::OBJ_MONSTER:
    	    if(from_hero.Modes(Heroes::GUARDIAN))
		return Cursor::POINTER;
	    else
		// for direct monster attack
		return Direction::UNKNOWN != Direction::Get(from_hero.GetIndex(), tile.GetIndex()) ?
			Cursor::FIGHT :
			Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(tile.GetIndex()));
	    break;

	case MP2::OBJN_CASTLE:
    	{
    	    const Castle *castle = world.GetCastle(tile.GetIndex());
    	    if(NULL != castle)
	    {
		if(from_hero.GetColor() == castle->GetColor())
		    return Cursor::CASTLE;
		else
		if(from_hero.Modes(Heroes::GUARDIAN))
		    return Cursor::POINTER;
		else
		if(Players::isFriends(from_hero.GetColor(), castle->GetColor()))
		    return conf.ExtUnionsAllowCastleVisiting() ? Cursor::ACTION : Cursor::POINTER;
		else
		if(castle->GetActualArmy().isValid())
		    return Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(castle->GetIndex()));
		else
		    return Cursor::DistanceThemes(Cursor::ACTION, from_hero.GetRangeRouteDays(castle->GetIndex()));
	    }
    	}
    	break;

    	case MP2::OBJ_CASTLE:
    	{
    	    const Castle *castle = world.GetCastle(tile.GetIndex());

    	    if(NULL != castle)
	    {
		if(from_hero.Modes(Heroes::GUARDIAN))
		    return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
		else
		if(from_hero.GetColor() == castle->GetColor())
		    return Cursor::DistanceThemes(Cursor::ACTION, from_hero.GetRangeRouteDays(tile.GetIndex()));
		else
		if(Players::isFriends(from_hero.GetColor(), castle->GetColor()))
		    return conf.ExtUnionsAllowCastleVisiting() ? Cursor::ACTION : Cursor::POINTER;
		else
		if(castle->GetActualArmy().isValid())
		    return Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(castle->GetIndex()));
		else
		    return Cursor::DistanceThemes(Cursor::ACTION, from_hero.GetRangeRouteDays(castle->GetIndex()));
	    }
        }
        break;

	case MP2::OBJ_HEROES:
	{
	    const Heroes* to_hero = tile.GetHeroes();

    	    if(NULL != to_hero)
	    {
		if(from_hero.Modes(Heroes::GUARDIAN))
		    return from_hero.GetColor() == to_hero->GetColor() ? Cursor::HEROES : Cursor::POINTER;
		else
		if(to_hero->GetCenter() == from_hero.GetCenter())
		    return Cursor::HEROES;
		else
		if(from_hero.GetColor() == to_hero->GetColor())
		{
		    Cursor::themes_t newcur = Cursor::DistanceThemes(Cursor::CHANGE, from_hero.GetRangeRouteDays(tile.GetIndex()));
		    return newcur != Cursor::POINTER ? newcur : Cursor::HEROES;
		}
		else
		if(Players::isFriends(from_hero.GetColor(), to_hero->GetColor()))
		{
		    Cursor::themes_t newcur = Cursor::DistanceThemes(Cursor::CHANGE, from_hero.GetRangeRouteDays(tile.GetIndex()));
		    return conf.ExtUnionsAllowHeroesMeetings() ? newcur: Cursor::POINTER;
		}
		else
		    return Cursor::DistanceThemes(Cursor::FIGHT, from_hero.GetRangeRouteDays(tile.GetIndex()));
	    }
    	}
    	break;

    	case MP2::OBJ_BOAT:
    		return from_hero.Modes(Heroes::GUARDIAN) ? Cursor::POINTER :
			Cursor::DistanceThemes(Cursor::BOAT, from_hero.GetRangeRouteDays(tile.GetIndex()));

	default:
	    if(from_hero.Modes(Heroes::GUARDIAN))
		return Cursor::POINTER;
	    else
	    if(MP2::isGroundObject(tile.GetObject()))
	    {
		bool protection = (MP2::isPickupObject(tile.GetObject()) ? false :
				(Maps::TileIsUnderProtection(tile.GetIndex()) ||
					(! Players::isFriends(from_hero.GetColor(), tile.QuantityColor()) &&
					    tile.CaptureObjectIsProtection())));

		return Cursor::DistanceThemes((protection ? Cursor::FIGHT : Cursor::ACTION),
						from_hero.GetRangeRouteDays(tile.GetIndex()));
	    }
	    else
	    if(tile.isPassable(&from_hero, Direction::CENTER, false))
	    {
		bool protection = Maps::TileIsUnderProtection(tile.GetIndex());

		return Cursor::DistanceThemes((protection ? Cursor::FIGHT : Cursor::MOVE),
	    					from_hero.GetRangeRouteDays(tile.GetIndex()));
	    }
	break;
    }

    return Cursor::POINTER;
}

Cursor::themes_t Game::GetCursor(const s32 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    if(tile.isFog(Settings::Get().CurrentColor())) return Cursor::POINTER;

    switch(GameFocus::Type())
    {
	case GameFocus::HEROES:
	    return GetCursorFocusHeroes(*GameFocus::GetHeroes(), tile);

	case GameFocus::CASTLE:
	    return GetCursorFocusCastle(*GameFocus::GetCastle(), tile);

    	default: break;
    }

    return Cursor::POINTER;
}

void Game::ShowPathOrStartMoveHero(Heroes *hero, const s32 dst_index)
{
    if(!hero || hero->Modes(Heroes::GUARDIAN)) return;

    Route::Path & path = hero->GetPath();
    Interface::Basic & I = Interface::Basic::Get();
    Cursor & cursor = Cursor::Get();

    // show path
    if(path.GetDestinedIndex() != dst_index &&
	    path.GetDestinationIndex() != dst_index)
    {
        hero->ResetModes(Heroes::SLEEPER);
        hero->SetMove(false);
	path.Calculate(dst_index);
        DEBUG(DBG_GAME, DBG_TRACE, hero->GetName() << ", route: " << path.String());
	I.SetRedraw(REDRAW_GAMEAREA);
	cursor.SetThemes(Game::GetCursor(dst_index));
    }
    // start move
    else
    if(path.isValid())
    {
        GameFocus::Set(hero);
        GameFocus::SetRedraw();

        hero->SetMove(true);
	cursor.SetThemes(Cursor::WAIT);
    }
}

Game::menu_t Game::HumanTurn(bool isload)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();

    LocalEvent & le = LocalEvent::Get();
    
    Game::menu_t res = CANCEL;

    cursor.Hide();
    Interface::Basic & I = Interface::Basic::Get();

    Kingdom & myKingdom = world.GetKingdom(conf.CurrentColor());
    const KingdomCastles & myCastles = myKingdom.GetCastles();
    const KingdomHeroes & myHeroes = myKingdom.GetHeroes();

    GameOver::Result & gameResult = GameOver::Result::Get();

    // set focus
    if(conf.ExtRememberLastFocus())
    {
	if(GameFocus::GetHeroes())
	    GameFocus::Reset(GameFocus::HEROES);
	else
	    GameFocus::Reset(GameFocus::CASTLE);
    }
    else
	GameFocus::Reset(GameFocus::FIRSTHERO);

    I.radar.SetHide(false);
    I.statusWindow.Reset();
    I.gameArea.SetUpdateCursor();
    I.Redraw(REDRAW_GAMEAREA | REDRAW_RADAR | REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_STATUS | REDRAW_BORDER);

    Game::EnvironmentSoundMixer();

    cursor.Show();
    display.Flip();

    if(!isload)
    {
	// new week dialog
	if(1 < world.CountWeek() && world.BeginWeek())
	    NewWeekDialog();

	// show event day
	 ShowEventDay();

	// autosave
	if(conf.ExtAutoSaveOn() && conf.ExtAutoSaveBeginOfDay())
	    Game::Save(conf.GetSaveDir() + SEPARATOR + "autosave.sav");
    }

    // check game over
    gameResult.LocalCheckGameOver(res);

    // warning lost all town
    if(myCastles.empty()) ShowWarningLostTowns(res);

    // check around actions (and skip for h2 orig, bug?)
    if(!conf.ExtOnlyFirstMonsterAttack()) myKingdom.HeroesActionNewPosition();

    // auto hide status
    bool autohide_status = conf.QVGA() && conf.ShowStatus();
    if(autohide_status) AnimateDelayReset(AUTOHIDE_STATUS_DELAY);

    // startgame loop
    while(CANCEL == res && le.HandleEvents())
    {
	// for pocketpc: auto hide status if start turn
	if(autohide_status && AnimateInfrequent(AUTOHIDE_STATUS_DELAY))
	{
	    EventSwitchShowStatus();
	    autohide_status = false;
	}

	// hot keys
	if(le.KeyPress())
	{
	    // exit dialog
	    if(HotKeyPress(EVENT_DEFAULT_EXIT)) EventExit(res);
	    else
    	    // end turn
	    if(HotKeyPress(EVENT_ENDTURN)) EventEndTurn(res);
	    else
    	    // next hero
	    if(HotKeyPress(EVENT_NEXTHERO)) EventNextHero();
	    else
    	    // next town
	    if(HotKeyPress(EVENT_NEXTTOWN)) EventNextTown();
	    else
	    // save game
	    if(HotKeyPress(EVENT_SAVEGAME)) EventSaveGame();
	    else
	    // load game
	    if(HotKeyPress(EVENT_LOADGAME))
	    {
		EventLoadGame(res);
		if(res != CANCEL) break;
	    }
	    else
	    // file options
	    if(HotKeyPress(EVENT_FILEOPTIONS)) EventFileDialog(res);
	    else
	    // system options
	    if(HotKeyPress(EVENT_SYSTEMOPTIONS)) EventSystemDialog();
	    else
	    // puzzle map
	    if(HotKeyPress(EVENT_PUZZLEMAPS)) EventPuzzleMaps();
	    else
	    // info game
	    if(HotKeyPress(EVENT_INFOGAME)) EventGameInfo();
	    else
	    // cast spell
	    if(HotKeyPress(EVENT_CASTSPELL)) EventCastSpell();
	    else
    	    // show/hide control panel
	    if(HotKeyPress(EVENT_CTRLPANEL)) EventSwitchShowControlPanel();
	    else
	    // hide/show radar
	    if(HotKeyPress(EVENT_SHOWRADAR)) EventSwitchShowRadar();
	    else
	    // hide/show buttons
	    if(HotKeyPress(EVENT_SHOWBUTTONS)) EventSwitchShowButtons();
	    else
	    // hide/show status window
	    if(HotKeyPress(EVENT_SHOWSTATUS)) EventSwitchShowStatus();
	    else
	    // hide/show hero/town icons
	    if(HotKeyPress(EVENT_SHOWICONS)) EventSwitchShowIcons();
	    else
	    	// hero movement
		if(HotKeyPress(EVENT_CONTINUE)) EventContinueMovement();
		else
		// dig artifact
		if(HotKeyPress(EVENT_DIGARTIFACT)) EventDigArtifact(res);
		else
		// sleep hero
		if(HotKeyPress(EVENT_SLEEPHERO)) EventSwitchHeroSleeping();
		else
		// move hero
		if(HotKeyPress(EVENT_MOVELEFT)) KeyArrowPress(Direction::LEFT);
		else
		if(HotKeyPress(EVENT_MOVERIGHT)) KeyArrowPress(Direction::RIGHT);
		else
		if(HotKeyPress(EVENT_MOVETOP)) KeyArrowPress(Direction::TOP);
		else
		if(HotKeyPress(EVENT_MOVEBOTTOM)) KeyArrowPress(Direction::BOTTOM);
		else
		if(HotKeyPress(EVENT_MOVETOPLEFT)) KeyArrowPress(Direction::TOP_LEFT);
		else
		if(HotKeyPress(EVENT_MOVETOPRIGHT)) KeyArrowPress(Direction::TOP_RIGHT);
		else
		if(HotKeyPress(EVENT_MOVEBOTTOMLEFT)) KeyArrowPress(Direction::BOTTOM_LEFT);
		else
		if(HotKeyPress(EVENT_MOVEBOTTOMRIGHT)) KeyArrowPress(Direction::BOTTOM_RIGHT);
	    else
	    // scroll maps
	    if(HotKeyPress(EVENT_SCROLLLEFT)) I.gameArea.SetScroll(SCROLL_LEFT);
	    else
	    if(HotKeyPress(EVENT_SCROLLRIGHT)) I.gameArea.SetScroll(SCROLL_RIGHT);
	    else
	    if(HotKeyPress(EVENT_SCROLLUP)) I.gameArea.SetScroll(SCROLL_TOP);
	    else
	    if(HotKeyPress(EVENT_SCROLLDOWN)) I.gameArea.SetScroll(SCROLL_BOTTOM);
	    // default action
	    else
	    if(HotKeyPress(EVENT_DEFAULTACTION)) EventDefaultAction();
	    // open focus
	    else
	    if(HotKeyPress(EVENT_OPENFOCUS)) EventOpenFocus();
	}

	if(conf.ExtTapMode())
	{
	    // scroll area maps left
	    if(le.MouseCursor(I.GetAreaScrollLeft()) && le.MousePressLeft()) I.gameArea.SetScroll(SCROLL_LEFT);
    	    else
	    // scroll area maps right
	    if(le.MouseCursor(I.GetAreaScrollRight()) && le.MousePressLeft()) I.gameArea.SetScroll(SCROLL_RIGHT);
	    else
	    // scroll area maps top
	    if(le.MouseCursor(I.GetAreaScrollTop()) && le.MousePressLeft()) I.gameArea.SetScroll(SCROLL_TOP);
	    else
	    // scroll area maps bottom
	    if(le.MouseCursor(I.GetAreaScrollBottom()) && le.MousePressLeft()) I.gameArea.SetScroll(SCROLL_BOTTOM);

	    // disable right click emulation
	    if(I.gameArea.NeedScroll()) le.SetTapMode(false);
	}
	else
	{
	    // scroll area maps left
	    if(le.MouseCursor(I.GetAreaScrollLeft())) I.gameArea.SetScroll(SCROLL_LEFT);
    	    else
	    // scroll area maps right
	    if(le.MouseCursor(I.GetAreaScrollRight())) I.gameArea.SetScroll(SCROLL_RIGHT);
	    else
	    // scroll area maps top
	    if(le.MouseCursor(I.GetAreaScrollTop())) I.gameArea.SetScroll(SCROLL_TOP);
	    else
	    // scroll area maps bottom
	    if(le.MouseCursor(I.GetAreaScrollBottom())) I.gameArea.SetScroll(SCROLL_BOTTOM);
	}

	// cursor over radar
        if((!conf.HideInterface() || conf.ShowRadar()) &&
           le.MouseCursor(I.radar.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
	    }
	    I.radar.QueueEventProcessing();
	}
	else
	// cursor over icons panel
        if((!conf.HideInterface() || conf.ShowIcons()) &&
           le.MouseCursor(I.iconsPanel.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
	    }
	    I.iconsPanel.QueueEventProcessing();
	}
	else
	// cursor over buttons area
        if((!conf.HideInterface() || conf.ShowButtons()) &&
           le.MouseCursor(I.buttonsArea.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
	    }
	    I.buttonsArea.QueueEventProcessing(res);
	}
	else
        // cursor over status area
        if((!conf.HideInterface() || conf.ShowStatus()) &&
           le.MouseCursor(I.statusWindow.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
	    }
	    I.statusWindow.QueueEventProcessing();
	}
	else
        // cursor over control panel
        if(conf.HideInterface() && conf.ShowControlPanel() &&
           le.MouseCursor(I.controlPanel.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
	    }
	    I.controlPanel.QueueEventProcessing(res);
	}
	else
	// cursor over game area
	if(le.MouseCursor(I.gameArea.GetArea()) && !I.gameArea.NeedScroll())
	{
    	    I.gameArea.QueueEventProcessing();
	}

        // fast scroll
	if(I.gameArea.NeedScroll() && AnimateInfrequent(SCROLL_DELAY))
        {
    	    cursor.Hide();

	    if(le.MouseCursor(I.GetAreaScrollLeft()) ||
		   le.MouseCursor(I.GetAreaScrollRight()) ||
		   le.MouseCursor(I.GetAreaScrollTop()) ||
		   le.MouseCursor(I.GetAreaScrollBottom()))
    		    cursor.SetThemes(I.gameArea.GetScrollCursor());

    	    I.gameArea.Scroll();

    	    // need stop hero
    	    if(GameFocus::GetHeroes() && GameFocus::GetHeroes()->isEnableMove())
    		    GameFocus::GetHeroes()->SetMove(false);

	    I.SetRedraw(REDRAW_GAMEAREA|REDRAW_RADAR);
    	    I.Redraw();
    	    cursor.Show();
    	    display.Flip();

            // enable right click emulation
	    if(conf.ExtTapMode())
        	le.SetTapMode(true);

	    continue;
        }

	// heroes move animation
        if(AnimateInfrequent(CURRENT_HERO_DELAY))
        {
    	    if(GameFocus::GetHeroes())
	    {
		Heroes* hero = GameFocus::GetHeroes();
		if(hero->isEnableMove())
		{
		    if(hero->Move(0 == conf.HeroesMoveSpeed()))
		    {
            		I.gameArea.SetCenter(hero->GetCenter());
            		GameFocus::Reset(GameFocus::HEROES);
            		GameFocus::SetRedraw();

            		I.gameArea.SetUpdateCursor();
		    }
		    else
		    {
			I.SetRedraw(REDRAW_GAMEAREA);
		    }

		    if(hero->isAction())
		    {
			// check game over
			gameResult.LocalCheckGameOver(res);
			hero->ResetAction();
		    }
		}
		else
		{
		    hero->SetMove(false);
		    if(Cursor::WAIT == cursor.Themes()) cursor.SetThemes(Cursor::POINTER);
		}
    	    }
	}

	// slow maps objects animation
        if(AnimateInfrequent(MAPS_DELAY))
	{
	    u32 & frame = Game::MapsAnimationFrame();
	    ++frame;
	    I.SetRedraw(REDRAW_GAMEAREA);
	}

	if(I.NeedRedraw())
	{
    	    cursor.Hide();
    	    I.Redraw();
    	    cursor.Show();
    	    display.Flip();
	}
	else
	if(!cursor.isVisible())
	{
    	    cursor.Show();
    	    display.Flip();
	}
    }

    if(ENDTURN == res)
    {
	// warning lost all town
	if(myHeroes.size() && myCastles.empty() && Game::GetLostTownDays() < myKingdom.GetLostTownDays())
	{
	    DialogPlayers(conf.CurrentColor(), _("%{color} player, you have lost your last town. If you do not conquer another town in next week, you will be eliminated."));
	}

	if(GameFocus::GetHeroes())
	{
	    GameFocus::GetHeroes()->ShowPath(false);
	    GameFocus::SetRedraw();
	}

	if(conf.ExtAutoSaveOn() && !conf.ExtAutoSaveBeginOfDay())
	    Game::Save(conf.GetSaveDir() + SEPARATOR + "autosave.sav");
    }

    return res;
}

bool Game::DiggingForArtifacts(Heroes & hero)
{
    if(hero.isShipMaster())
	Dialog::Message("", _("Try looking on land!!!"), Font::BIG, Dialog::OK);
    else
    if(hero.GetMaxMovePoints() <= hero.GetMovePoints())
    {
	if(! world.GetTiles(hero.GetCenter()).GoodForUltimateArtifact())
	{
	    Dialog::Message("", _("Try searching on clear ground."), Font::BIG, Dialog::OK);
	    return false;
	}

	AGG::PlaySound(M82::DIGSOUND);

	hero.ResetMovePoints();

	if(world.DiggingForUltimateArtifact(hero.GetCenter()))
	{
	    AGG::PlaySound(M82::TREASURE);
	    const Artifact & ultimate = world.GetUltimateArtifact().GetArtifact();
	    hero.PickupArtifact(ultimate);
	    std::string msg(_("After spending many hours digging here, you have uncovered the %{artifact}"));
	    String::Replace(msg, "%{artifact}", ultimate.GetName());
	    Dialog::ArtifactInfo(_("Congratulations!"), msg, ultimate());
	}
	else
	    Dialog::Message("", _("Nothing here. Where could it be?"), Font::BIG, Dialog::OK);

	Cursor::Get().Hide();
	Interface::IconsPanel::Get().RedrawIcons(ICON_HEROES);
	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
	Dialog::Message("", _("Digging for artifacts requires a whole day, try again tomorrow."), Font::BIG, Dialog::OK);

    return false;
}

void Game::MouseCursorAreaClickLeft(s32 index_maps)
{
    Castle* to_castle = NULL;
    Heroes* to_hero = NULL;
    Heroes* from_hero = GameFocus::GetHeroes();
    const Maps::Tiles & tile = world.GetTiles(index_maps);

    switch(Cursor::WithoutDistanceThemes(Cursor::Get().Themes()))
    {
	case Cursor::HEROES:
	    // focus change/open hero
	    if(NULL != (to_hero = tile.GetHeroes()))
	    {
		if(! from_hero ||
		    from_hero != to_hero)
		{
		    GameFocus::Set(to_hero);
		    GameFocus::SetRedraw();
		}
		else
		    OpenHeroesDialog(to_hero);
	    }
	    break;

        case Cursor::CASTLE:
	    // focus change/open castle
	    if(NULL != (to_castle = world.GetCastle(index_maps)))
	    {
		Castle* from_castle = GameFocus::GetCastle();

		if(! from_castle ||
		    from_castle != to_castle)
		{
		    GameFocus::Set(to_castle);
		    GameFocus::SetRedraw();
		}
		else
		    OpenCastleDialog(to_castle);
	    }
	    break;

        case Cursor::FIGHT:
	    if(NULL != (to_castle = world.GetCastle(index_maps)))
		ShowPathOrStartMoveHero(from_hero, to_castle->GetIndex());
	    else
		ShowPathOrStartMoveHero(from_hero, index_maps);
	    break;

        case Cursor::MOVE:
        case Cursor::BOAT:
        case Cursor::ANCHOR:
        case Cursor::CHANGE:
        case Cursor::ACTION:
        case Cursor::REDBOAT:
	    if(from_hero)
		ShowPathOrStartMoveHero(from_hero, index_maps);
	    break;

	default:
	    if(from_hero)
		from_hero->SetMove(false);
	    break;
    }
}

void Game::MouseCursorAreaPressRight(s32 index_maps)
{
    Heroes* hero = GameFocus::GetHeroes();

    // stop hero
    if(hero && hero->isEnableMove())
    {
	hero->SetMove(false);
	Cursor::Get().SetThemes(Game::GetCursor(index_maps));
    }
    else
    {
	Settings & conf = Settings::Get();
	Maps::Tiles & tile = world.GetTiles(index_maps);

	DEBUG(DBG_DEVEL, DBG_INFO, std::endl << tile.String());

	if(!IS_DEVEL() && tile.isFog(conf.CurrentColor()))
	    Dialog::QuickInfo(tile);
	else
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
		const Heroes *heroes = tile.GetHeroes();
		if(heroes) Dialog::QuickInfo(*heroes);
	    }
	    break;

	    default:
		Dialog::QuickInfo(tile);
	    break;
	}
    }
}

void Game::EventNextHero(void)
{
    const Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());
    const KingdomHeroes & myHeroes = myKingdom.GetHeroes();

    if(myHeroes.empty()) return;

    if(GameFocus::GetHeroes())
    {
	KingdomHeroes::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(),
								GameFocus::GetHeroes());
	++it;
	if(it == myHeroes.end()) it = myHeroes.begin();
	GameFocus::Set(*it);
    }
    else
    {
	GameFocus::Reset(GameFocus::HEROES);
    }
    GameFocus::SetRedraw();
}

void Game::EventContinueMovement(void)
{
    Heroes* hero = GameFocus::GetHeroes();

    if(hero && hero->GetPath().isValid())
	hero->SetMove(! hero->isEnableMove());
}

void Game::EventKingdomInfo(void)
{
}

void Game::EventCastSpell(void)
{
    Interface::Basic & I = Interface::Basic::Get();
    Heroes* hero = GameFocus::GetHeroes();

    if(hero)
    {
	const Spell spell = hero->OpenSpellBook(SpellBook::ADVN, true);
	// apply cast spell
	if(spell.isValid())
	{
	    hero->ActionSpellCast(spell);
	    I.SetRedraw(REDRAW_ICONS);
	}
    }
}

void Game::EventEndTurn(Game::menu_t & ret)
{
    const Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());

    if(GameFocus::GetHeroes())
	GameFocus::GetHeroes()->SetMove(false);

    if(!myKingdom.HeroesMayStillMove() ||
	Dialog::YES == Dialog::Message("", _("One or more heroes may still move, are you sure you want to end your turn?"), Font::BIG, Dialog::YES | Dialog::NO))
	ret = ENDTURN;
}

void Game::EventAdventureDialog(Game::menu_t & ret)
{
    Mixer::Reduce();
    switch(Dialog::AdventureOptions(GameFocus::HEROES == GameFocus::Type()))
    {
	case Dialog::WORLD:
	    break;

	case Dialog::PUZZLE: 
	    EventPuzzleMaps();
	    break;

	case Dialog::INFO:
	    EventGameInfo();
	    break;

	case Dialog::DIG:
	    EventDigArtifact(ret);
	    break;

	default: break;
    }
    Mixer::Enhance();
}

void Game::StartNewGame(Game::menu_t & ret)
{
    if(Dialog::YES == Dialog::Message("", _("Are you sure you want to restart? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
	ret = NEWGAME;
}

void Game::EventFileDialog(Game::menu_t & ret)
{
    switch(Dialog::FileOptions())
    {
	case NEWGAME:
	    StartNewGame(ret);
	    break;

	case QUITGAME:
	    ret = QUITGAME;
	    break;

	case LOADGAME:
	    EventLoadGame(ret);
	    break;

	case SAVEGAME:
	    EventSaveGame();
	    break;

	default:
	break;
    }
}

void Game::EventSystemDialog(void)
{
    // Change and save system settings
    const u8 changes = Dialog::SystemOptions();
    Interface::Basic & I = Interface::Basic::Get();

    // change scroll
    if(0x10 & changes)
    {
	// hardcore reset pos
	I.gameArea.SetCenter(0, 0);
	if(GameFocus::Type() != GameFocus::UNSEL)
	    I.gameArea.SetCenter(GameFocus::GetCenter());
        I.SetRedraw(REDRAW_GAMEAREA);

	if(Settings::Get().HideInterface())
	    I.controlPanel.ResetTheme();
    }

    if(0x08 & changes)
        I.SetRedraw(REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_STATUS | REDRAW_BORDER);
}

void Game::EventExit(menu_t & ret)
{
    Heroes* hero = GameFocus::GetHeroes();

    // stop hero
    if(hero && hero->isEnableMove())
	hero->SetMove(false);
    else
    if(Dialog::YES & Dialog::Message("", _("Are you sure you want to quit?"), Font::BIG, Dialog::YES|Dialog::NO))
	ret = QUITGAME;
}

void Game::EventNextTown(void)
{
    Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());
    KingdomCastles & myCastles = myKingdom.GetCastles();

    if(myCastles.size())
    {
	if(GameFocus::GetCastle())
	{
	    KingdomCastles::const_iterator it = std::find(myCastles.begin(), myCastles.end(),
						    GameFocus::GetCastle());
    	    ++it;
    	    if(it == myCastles.end()) it = myCastles.begin();
	    GameFocus::Set(*it);
        }
	else
	    GameFocus::Reset(GameFocus::CASTLE);

        GameFocus::SetRedraw();
    }
}

void Game::EventSaveGame(void)
{
    std::string filename;
    if(Dialog::SelectFileSave(filename) && filename.size() && Game::Save(filename))
	Dialog::Message("", _("Game saved successfully."), Font::BIG, Dialog::OK);
}

void Game::EventLoadGame(menu_t & ret)
{
    if(Dialog::YES == Dialog::Message("", _("Are you sure you want to load a new game? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
    ret = LOADGAME;
}

void Game::EventPuzzleMaps(void)
{
    world.GetKingdom(Settings::Get().CurrentColor()).PuzzleMaps().ShowMapsDialog();
}

void Game::EventGameInfo(void)
{
    Dialog::GameInfo();
}

void Game::EventSwitchHeroSleeping(void)
{
    Heroes* hero = GameFocus::GetHeroes();

    if(hero)
    {
	Interface::Basic & I = Interface::Basic::Get();

	if(hero->Modes(Heroes::SLEEPER))
	    hero->ResetModes(Heroes::SLEEPER);
	else
	{
	    hero->SetModes(Heroes::SLEEPER);
	    hero->GetPath().Reset();
	}

	I.SetRedraw(REDRAW_HEROES);
    }
}

void Game::EventDigArtifact(menu_t & ret)
{
    Heroes* hero = GameFocus::GetHeroes();

    if(hero)
    {
	DiggingForArtifacts(*hero);
	// check game over for ultimate artifact
	GameOver::Result::Get().LocalCheckGameOver(ret);
    }
}

void Game::EventDefaultAction(void)
{
    Heroes* hero = GameFocus::GetHeroes();

    if(hero)
    {
	Interface::Basic & I = Interface::Basic::Get();
	const Maps::Tiles & tile = world.GetTiles(hero->GetIndex());

	// 1. action object
	if(MP2::isActionObject(hero->GetMapsObject(), hero->isShipMaster()) &&
	    (! MP2::isMoveObject(hero->GetMapsObject()) || hero->CanMove()))
	{
	    hero->Action(hero->GetIndex());
	    if(MP2::OBJ_STONELIGHTS == tile.GetObject(false) || MP2::OBJ_WHIRLPOOL == tile.GetObject(false))
		I.SetRedraw(REDRAW_HEROES);
	    I.SetRedraw(REDRAW_GAMEAREA);
	}
	else
	// 2. continue
        if(hero->GetPath().isValid())
    	    hero->SetMove(true);
	else
	// 3. hero dialog
	    OpenHeroesDialog(hero);
    }
    else
    // 4. town dialog
    if(GameFocus::GetCastle())
    {
	OpenCastleDialog(GameFocus::GetCastle());
    }
}

void Game::EventOpenFocus(void)
{
    if(GameFocus::GetHeroes())
	OpenHeroesDialog(GameFocus::GetHeroes());
    else
    if(GameFocus::GetCastle())
	OpenCastleDialog(GameFocus::GetCastle());
}

void Game::KeyArrowPress(Direction::vector_t dir)
{
    Interface::GameArea & area = Interface::GameArea::Get();
    Heroes* hero = GameFocus::GetHeroes();

    // move hero
    if(hero) MoveHeroFromArrowKeys(*hero, dir);
    else
    // scroll map
    switch(dir)
    {
	case Direction::TOP_LEFT:	area.SetScroll(SCROLL_TOP); area.SetScroll(SCROLL_LEFT); break;
	case Direction::TOP:		area.SetScroll(SCROLL_TOP); break;
        case Direction::TOP_RIGHT:	area.SetScroll(SCROLL_TOP); area.SetScroll(SCROLL_RIGHT); break;
        case Direction::RIGHT:		area.SetScroll(SCROLL_RIGHT); break;
        case Direction::BOTTOM_RIGHT:	area.SetScroll(SCROLL_BOTTOM); area.SetScroll(SCROLL_RIGHT); break;
        case Direction::BOTTOM:		area.SetScroll(SCROLL_BOTTOM); break;
        case Direction::BOTTOM_LEFT:	area.SetScroll(SCROLL_BOTTOM); area.SetScroll(SCROLL_LEFT); break;
        case Direction::LEFT:		area.SetScroll(SCROLL_LEFT); break;
	default: break;
    }
}

void Game::NewWeekDialog(void)
{
    const Week & week = world.GetWeekType();

    // head
    std::string message = world.BeginMonth() ? _("Astrologers proclaim Month of the %{name}.") : _("Astrologers proclaim Week of the %{name}.");
    AGG::PlayMusic(world.BeginMonth() ?
	(week.GetType() == Week::MONSTERS ? MUS::MONTH2 : MUS::WEEK2_MONTH1) : MUS::WEEK1, false);
    String::Replace(message, "%{name}", week.GetName());
    message += "\n \n";

    if(week.GetType() == Week::MONSTERS)
    {
	const Monster monster(week.GetMonster());
	const u8 count = world.BeginMonth() ? Castle::GetGrownMonthOf() : Castle::GetGrownWeekOf(monster);

	if(monster.isValid() && count)
	{
	    if(world.BeginMonth())
		message += 100 == Castle::GetGrownMonthOf() ? _("After regular growth, population of %{monster} is doubled!") :
								    ngettext("After regular growth, population of %{monter} increase on %{count} percent!",
										"After regular growth, population of %{monter} increase on %{count} percent!", count);
	    else
		message += ngettext("%{monster} population increases by +%{count}.", "%{monster} population increases by +%{count}.", count);
	    String::Replace(message, "%{monster}", monster.GetMultiName());
	    String::Replace(message, "%{count}", count);
	    message += "\n";
	}
    }

    if(week.GetType() == Week::PLAGUE)
	message += _(" All populations are halved.");
    else
	message += _(" All dwellings increase population.");

    Dialog::Message("", message, Font::BIG, Dialog::OK);
}

void Game::ShowEventDay(void)
{
    Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());
    EventsDate events = world.GetEventsDate(myKingdom.GetColor());

    for(EventsDate::const_iterator
	it = events.begin(); it != events.end(); ++it)
    {
    	AGG::PlayMusic(MUS::NEWS, false);
	if((*it).resource.GetValidItems())
	    Dialog::ResourceInfo("", (*it).message, (*it).resource);
	else
	if((*it).message.size())
	    Dialog::Message("", (*it).message, Font::BIG, Dialog::OK);
    }
}

void Game::ShowWarningLostTowns(menu_t & ret)
{
    const Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());
    if(0 == myKingdom.GetLostTownDays())
    {
    	    AGG::PlayMusic(MUS::DEATH, false);
	    DialogPlayers(myKingdom.GetColor(), _("%{color} player, your heroes abandon you, and you are banished from this land."));
	    GameOver::Result::Get().SetResult(GameOver::LOSS_ALL);
	    ret = MAINMENU;
    }
    else
    if(1 == myKingdom.GetLostTownDays())
    {
	    DialogPlayers(myKingdom.GetColor(), _("%{color} player, this is your last day to capture a town, or you will be banished from this land."));
    }
    else
    if(Game::GetLostTownDays() >= myKingdom.GetLostTownDays())
    {
	    std::string str = _("%{color} player, you only have %{day} days left to capture a town, or you will be banished from this land.");
	    String::Replace(str, "%{day}", myKingdom.GetLostTownDays());
	    DialogPlayers(myKingdom.GetColor(), str);
    }
}

void Game::EventSwitchShowRadar(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowRadar())
	{
	    conf.SetShowRadar(false);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.QVGA() && (conf.ShowIcons() || conf.ShowStatus() || conf.ShowButtons()))
	    {
		conf.SetShowIcons(false);
		conf.SetShowStatus(false);
		conf.SetShowButtons(false);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetShowRadar(true);
	    Interface::Basic::Get().SetRedraw(REDRAW_RADAR);
	}
    }
}

void Game::EventSwitchShowButtons(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowButtons())
	{
	    conf.SetShowButtons(false);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.QVGA() && (conf.ShowRadar() || conf.ShowStatus() || conf.ShowIcons()))
	    {
		conf.SetShowIcons(false);
		conf.SetShowStatus(false);
		conf.SetShowRadar(false);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetShowButtons(true);
	    Interface::Basic::Get().SetRedraw(REDRAW_BUTTONS);
	}
    }
}

void Game::EventSwitchShowStatus(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowStatus())
	{
	    conf.SetShowStatus(false);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.QVGA() && (conf.ShowRadar() || conf.ShowIcons() || conf.ShowButtons()))
	    {
		conf.SetShowIcons(false);
		conf.SetShowButtons(false);
		conf.SetShowRadar(false);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetShowStatus(true);
	    Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
	}
    }
}

void Game::EventSwitchShowIcons(void)
{
    Settings & conf = Settings::Get();
    Interface::Basic & I = Interface::Basic::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowIcons())
	{
	    conf.SetShowIcons(false);
	    I.SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.QVGA() && (conf.ShowRadar() || conf.ShowStatus() || conf.ShowButtons()))
	    {
		conf.SetShowButtons(false);
		conf.SetShowRadar(false);
		conf.SetShowStatus(false);
		I.SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetShowIcons(true);
	    I.iconsPanel.SetCurrentVisible();
	    I.SetRedraw(REDRAW_ICONS);
	}
    }
}

void Game::EventSwitchShowControlPanel(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	conf.SetShowPanel(!conf.ShowControlPanel());
	Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
    }
}
