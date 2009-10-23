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
#include "network.h"

extern u16 DialogWithArtifact(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 buttons = Dialog::OK);

namespace Game
{
    Cursor::themes_t GetCursor(const Maps::Tiles & tile);
    void OpenCastle(Castle *castle);
    void OpenHeroes(Heroes *heroes);
    void ShowPathOrStartMoveHero(Heroes *hero, const u16 dst_index);
    menu_t HumanTurn(void);
    menu_t NetworkTurn(Kingdom &);
    bool DiggingForArtifacts(const Heroes & hero);
    void DialogPlayers(const Color::color_t, const std::string &);
    void MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t direct);
    bool CheckGameOver(Game::menu_t &);

    void MouseCursorAreaClickLeft(u16);
    void FocusHeroesClickLeftAction(Heroes &, u16);
    void FocusCastleClickLeftAction(Castle &, u16);
    void MouseCursorAreaPressRight(u16);

    void ButtonNextHero(void);
    void ButtonMovement(void);
    void ButtonKingdom(void);
    void ButtonSpell(void);
    void ButtonEndTurn(Game::menu_t &);
    void ButtonAdventure(Game::menu_t &);
    void ButtonFile(Game::menu_t &);
    void ButtonSystem(void);
    void StartNewGame(Game::menu_t &);

    void KeyPress_ESC(menu_t &);
    void KeyPress_e(menu_t &);
    void KeyPress_t(void);
    void KeyPress_s(void);
    void KeyPress_l(menu_t &);
    void KeyPress_p(void);
    void KeyPress_i(void);
    void KeyPress_d(menu_t &);
    void KeyPress_SPACE(void);
    void KeyPress_RETURN(void);
    void KeyPress_LEFT(void);
    void KeyPress_RIGHT(void);
    void KeyPress_TOP(void);
    void KeyPress_BOTTOM(void);
    void KeyPress_r(void);
    void KeyPress_b(void);
    void KeyPress_w(void);
    void KeyPress_c(void);
    void KeyPress_a(void);

    void NewWeekDialog(void);
    void ShowEventDay(void);
    void ShowWarningLostTowns(menu_t &);
};

bool Game::CheckGameOver(Game::menu_t & res)
{
    Settings & conf = Settings::Get();

    for(Color::color_t c = Color::BLUE; c != Color::GRAY; ++c)
        if(!world.GetKingdom(c).isPlay() && (c & conf.CurrentKingdomColors()))
    {
	std::string message(_("%{color} has been vanquished!"));
	String::Replace(message, "%{color}", Color::String(c));
	DialogPlayers(c, message);
	conf.SetCurrentKingdomColors(conf.CurrentKingdomColors() & ~c);
    }

    u16 cond = GameOver::COND_NONE;
    const Kingdom & myKingdom = world.GetMyKingdom();

    if(GameOver::COND_NONE != (cond = world.CheckKingdomLoss(myKingdom)))
    {
	conf.SetGameOverResult(cond);
	GameOver::DialogLoss(cond);
	res = MAINMENU;
	return true;
    }
    else
    if(GameOver::COND_NONE != (cond = world.CheckKingdomWins(myKingdom)))
    {
	conf.SetGameOverResult(cond);
    	GameOver::DialogWins(cond);
	res = HIGHSCORES;
	return true;
    }

    return false;
}

void Game::MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t direct)
{
    if(Maps::isValidDirection(hero.GetIndex(), direct))
    {
	const u16 dst = Maps::GetDirectionIndex(hero.GetIndex(), direct);
	const Maps::Tiles & tile = world.GetTiles(dst);

	switch(tile.GetObject())
	{
    	    case MP2::OBJN_CASTLE:
    	    {
    		const Castle *to_castle = world.GetCastle(dst);
		if(to_castle) ShowPathOrStartMoveHero(&hero, to_castle->GetIndex());
	    }

	    default:
		if(Cursor::POINTER != GetCursor(tile)) ShowPathOrStartMoveHero(&hero, dst);
	}
    }
}
                                   
void Game::DialogPlayers(const Color::color_t color, const std::string & str)
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

    Dialog::SpriteInfo("", str, sign);
}

Game::menu_t Game::StartGame(void)
{
    SetFixVideoMode();

    // cursor
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    conf.SetGameOverResult(GameOver::COND_NONE);
    conf.SetCurrentKingdomColors(conf.KingdomColors());

    cursor.Hide();

    AGG::FreeObject(ICN::HEROES);
    AGG::FreeObject(ICN::BTNSHNGL);
    AGG::FreeObject(ICN::SHNGANIM);
    AGG::FreeObject(ICN::BTNNEWGM);
    AGG::FreeObject(ICN::REDBACK);
    AGG::FreeObject(ICN::NGEXTRA);
    AGG::FreeObject(ICN::NGHSBKG);
    AGG::FreeObject(ICN::REQSBKG);
    AGG::FreeObject(ICN::REQUEST);
    AGG::FreeObject(ICN::REQUESTS);
    AGG::FreeObject(ICN::ESCROLL);
    AGG::FreeObject(ICN::HSBKG);
    AGG::FreeObject(ICN::HISCORE);

    if(Settings::Get().LowMemory())
    {
        AGG::ICNRegistryEnable(false);
        AGG::ICNRegistryFreeObjects();
    }

    // preload sounds
    Game::PreloadLOOPSounds();
    Mixer::Reset();

    // set controls type for players
    switch(conf.GameType())
    {
    	case Game::HOTSEAT:
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
        	world.GetKingdom(color).SetControl(LOCAL);
    	    break;

    	case Game::NETWORK:
	    conf.ResetModes(Settings::LOADGAME);
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
        	world.GetKingdom(color).SetControl(color == conf.MyColor() ? LOCAL : REMOTE);
    	    break;

    	default:
    	    world.GetKingdom(conf.MyColor()).SetControl(LOCAL);
    	    break;
    }

    // update starting resource
    if(!conf.Modes(Settings::LOADGAME))
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
    	    world.GetKingdom(color).UpdateStartingResource();

    // draw interface
    Interface::Basic & I = Interface::Basic::Get();

    Interface::GameArea & areaMaps = I.gameArea;
    areaMaps.Build();

    Game::Focus & global_focus = Focus::Get();
    global_focus.Reset();

    Interface::Radar & radar = I.radar;
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();
    Interface::StatusWindow& statusWin = I.statusWindow;
    heroesBar.Reset();
    castleBar.Reset();
    radar.Build();

    I.Redraw(REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_BORDER);

    Game::menu_t m = ENDTURN;

    while(m == ENDTURN)
    {
	// world new day (skip if load game)
	if(! conf.Modes(Settings::LOADGAME)) world.NewDay();

	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	{
	    // skip other player if loadgame
	    if(conf.Modes(Settings::LOADGAME) && color != conf.MyColor()) continue;

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
		    //Mixer::Enhance();
		    //Mixer::Reset();
		    if(Game::HOTSEAT == conf.GameType())
		    {
			conf.SetMyColor(Color::GRAY);
			castleBar.Hide();
			heroesBar.Hide();
			statusWin.SetState(STATUS_UNKNOWN);
			I.SetRedraw(REDRAW_GAMEAREA | REDRAW_STATUS);
			I.Redraw();
			display.Flip();
			std::string str = _("%{color} player's turn");
			String::Replace(str, "%{color}", Color::String(color));
			DialogPlayers(color, str);
		    }
		    conf.SetMyColor(color);
		    if(conf.Modes(Settings::LOADGAME)) conf.ResetModes(Settings::LOADGAME);
		    m = HumanTurn();
		    //Mixer::Reduce();
		    break;

	        case REMOTE:
		    m = NetworkTurn(kingdom);
		    break;

	        case AI:
            	    if(m == ENDTURN) kingdom.AITurns();
		    break;

		default:
		    Error::Verbose("Game::StartGame: unknown control, for player: " + Color::String(color));
		    break;
		}
	    }
	    if(m != ENDTURN) break;
	}
	DELAY(1);
    }

    return m == ENDTURN ? QUITGAME : m;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle)
{
    if(! castle) return;

    Mixer::Reduce();
    if(Settings::Get().LowMemory()) AGG::ICNRegistryEnable(true);

    Cursor & cursor = Cursor::Get();
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    Display & display = Display::Get();
    std::vector<Castle *>::const_iterator it = std::find(myCastles.begin(), myCastles.end(), castle);
    Game::Focus & globalfocus = Game::Focus::Get();
    Interface::StatusWindow::ResetTimer();
    bool show_position = !Settings::Get().PocketPC() && (640 != display.w() || 480 != display.h());
    bool need_fade = !show_position;
    
    if(it != myCastles.end())
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
	    if(show_position)
	    {
		globalfocus.Set(*it);
		globalfocus.SetRedraw();
		cursor.Hide();
		Interface::Basic::Get().Redraw();
		cursor.Show();
		display.Flip();
		DELAY(100);
	    }

	    result = (*it)->OpenDialog(need_fade);
	    if(need_fade) need_fade = false;

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

    if(it != myCastles.end()) globalfocus.Set(*it);
    if(Heroes *hero = const_cast<Heroes *>((*it)->GetHeroes())) globalfocus.Set(hero);
    globalfocus.SetRedraw();

    if(Settings::Get().LowMemory())
    {
        AGG::ICNRegistryEnable(false);
        AGG::ICNRegistryFreeObjects();
    }
    Mixer::Enhance();
}

/* open heroes wrapper */
void Game::OpenHeroes(Heroes *hero)
{
    if(! hero) return;

    Mixer::Reduce();
    if(Settings::Get().LowMemory()) AGG::ICNRegistryEnable(true);

    Cursor & cursor = Cursor::Get();
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();
    Display & display = Display::Get();
    std::vector<Heroes *>::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), hero);
    Game::Focus & globalfocus = Game::Focus::Get();
    Interface::StatusWindow::ResetTimer();
    Interface::Basic & I = Interface::Basic::Get();
    bool show_position = !Settings::Get().PocketPC() && (640 != display.w() || 480 != display.h());
    bool need_fade = !show_position;

    if(it != myHeroes.end())
    {
	Dialog::answer_t result = Dialog::ZERO;

	while(Dialog::CANCEL != result)
	{
	    if(show_position)
	    {
		globalfocus.Set(*it);
		globalfocus.SetRedraw();
		cursor.Hide();
		I.Redraw();
		cursor.Show();
		display.Flip();
		DELAY(100);
	    }

	    result = (*it)->OpenDialog(false, need_fade);
	    if(need_fade) need_fade = false;

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
		    myKingdom.RemoveHeroes(*it);
		    it = myHeroes.begin();
		    result = Dialog::CANCEL;
		    break;

		default: break;
	    }
	}
    }

    if(it != myHeroes.end())
	globalfocus.Set(*it);
    else
        globalfocus.Reset(Game::Focus::HEROES);
    globalfocus.SetRedraw();

    if(Settings::Get().LowMemory())
    {
        AGG::ICNRegistryEnable(false);
        AGG::ICNRegistryFreeObjects();
    }
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

	    if(from_hero.Modes(Heroes::ENABLEMOVE)) return Cursor::Get().Themes();

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
			    if(MP2::isWaterObject(tile.GetObject()))
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
			    if(castle->GetArmy().isValid())
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
				case 1:	return Cursor::FIGHT;
				case 2:	return Cursor::FIGHT2;
				case 3:	return Cursor::FIGHT3;
				default:return Cursor::FIGHT4;
			    }
			    else
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
				case 1:	return Cursor::ACTION;
				case 2:	return Cursor::ACTION2;
				case 3:	return Cursor::ACTION3;
				default:return Cursor::ACTION4;
			    }
			}
    		    }
    		    break;

    		    case MP2::OBJ_CASTLE:
    		    {
    			const Castle *castle = world.GetCastle(tile.GetIndex());

    			if(NULL != castle)
			{
			    if(from_hero.GetColor() != castle->GetColor() && castle->GetArmy().isValid())
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
				case 1:	return Cursor::FIGHT;
				case 2:	return Cursor::FIGHT2;
				case 3:	return Cursor::FIGHT3;
				default:return Cursor::FIGHT4;
			    }
			    else
			    switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
			    {
				case 0:	return Cursor::POINTER;
				case 1:	return Cursor::ACTION;
				case 2:	return Cursor::ACTION2;
				case 3:	return Cursor::ACTION3;
				default:return Cursor::ACTION4;
			    }
			    break;
			}
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

		    default:
			    if(MP2::isGroundObject(tile.GetObject()))
			    {
				const bool protection = (MP2::isPickupObject(tile.GetObject()) ? false :
					Maps::TileUnderProtection(tile.GetIndex()));

				switch(from_hero.GetRangeRouteDays(tile.GetIndex()))
				{
				    case 0:	return Cursor::POINTER;
				    case 1:	return protection ? Cursor::FIGHT : Cursor::ACTION;
				    case 2:	return protection ? Cursor::FIGHT2 : Cursor::ACTION2;
				    case 3:	return protection ? Cursor::FIGHT3 : Cursor::ACTION3;
				    default:	return protection ? Cursor::FIGHT4 : Cursor::ACTION4;
				}
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
    Interface::Basic & I = Interface::Basic::Get();

    // show path
    if(path.GetDestinationIndex() != dst_index)
    {
        hero->SetMove(false);
	path.Calculate(dst_index);
        if(1 < Settings::Get().Debug()) path.Dump();
        path.Show();
	I.SetRedraw(REDRAW_GAMEAREA);
    }
    // start move
    else
    if(path.isValid())
    {
        Game::Focus::Get().Set(hero);
        Game::Focus::Get().SetRedraw();
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

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();

    LocalEvent & le = LocalEvent::Get();
    
    u32 ticket = 0;
    Game::menu_t res = CANCEL;

    Interface::Basic & I = Interface::Basic::Get();

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();


    bool autohide_status = conf.PocketPC();

    // set focus
    if(Game::HOTSEAT == conf.GameType()) global_focus.Reset();

    if(conf.Original() && myHeroes.size())
	global_focus.Set(myHeroes.front());
    else
	global_focus.Reset(Focus::HEROES);
    if(Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().GetPath().isValid()) global_focus.GetHeroes().GetPath().Show();

    I.statusWindow.Reset();
    I.Redraw(REDRAW_GAMEAREA | REDRAW_RADAR | REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_STATUS | REDRAW_BORDER);

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(global_focus.Center()).GetGround()));
    Game::EnvironmentSoundMixer();

    cursor.Show();
    display.Flip();

    // new week dialog
    if(1 < world.CountWeek() && world.BeginWeek())
	NewWeekDialog();

    // show event day
    ShowEventDay();

    // check game over
    CheckGameOver(res);

    // warning lost all town
    if(myCastles.empty()) ShowWarningLostTowns(res);

    // startgame loop
    while(CANCEL == res && le.HandleEvents())
    {
	// for pocketpc: auto hide status if start turn
	if(autohide_status && conf.ShowStatus() && ticket > 300)
	{
	    KeyPress_w();
	    autohide_status = false;
	}

	// hot keys
	if(le.KeyPress()) switch(le.KeyValue())
	{
	    // exit
	    case KEY_ESCAPE:	KeyPress_ESC(res); break;
    	    // show/hide control panel
	    case KEY_a:		KeyPress_a(); break;
    	    // end turn
	    case KEY_e:		KeyPress_e(res); break;
    	    // next hero
	    case KEY_h:		ButtonNextHero(); break;
    	    // next town
	    case KEY_t:		KeyPress_t(); break;
	    // save game
	    case KEY_s:		KeyPress_s(); break;
	    // load game
	    case KEY_l:		KeyPress_l(res); break;
	    // puzzle map
	    case KEY_p:		KeyPress_p(); break;
	    // info game
	    case KEY_i:		KeyPress_i(); break;
	    // dig artifact
	    case KEY_d:		KeyPress_d(res); break;
	    // default action
	    case KEY_SPACE:	KeyPress_SPACE(); break;
	    // hero/town dialog
	    case KEY_RETURN:	KeyPress_RETURN(); break;
	    // hero movement
	    case KEY_m:		ButtonMovement(); break;
	    // system options
	    case KEY_o:		ButtonSystem(); break;
	    // new game
	    case KEY_n:		StartNewGame(res); break;
	    // scroll
	    case KEY_LEFT:	KeyPress_LEFT(); break;
	    case KEY_RIGHT:	KeyPress_RIGHT(); break;
	    case KEY_UP:	KeyPress_TOP(); break;
	    case KEY_DOWN:	KeyPress_BOTTOM(); break;

	    // hide/show radar
	    case KEY_r:		KeyPress_r(); break;
	    // hide/show buttons
	    case KEY_b:		KeyPress_b(); break;
	    // hide/show status window
	    case KEY_w:		KeyPress_w(); break;
	    // hide/show hero/town icons
	    case KEY_c:		KeyPress_c(); break;
	    
	    default: break;
	}

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
	else
	// cursor over radar
        if((!conf.HideInterface() || conf.ShowRadar()) &&
           le.MouseCursor(I.radar.GetArea()))
	{
	    if(Cursor::POINTER != cursor.Themes())
	    {
		cursor.SetThemes(Cursor::POINTER);
		I.SetRedraw(REDRAW_CURSOR);
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
		I.SetRedraw(REDRAW_CURSOR);
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
		I.SetRedraw(REDRAW_CURSOR);
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
		I.SetRedraw(REDRAW_CURSOR);
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
		I.SetRedraw(REDRAW_CURSOR);
	    }
	    I.controlPanel.QueueEventProcessing(res);
	}
	else
	// cursor over game area
	if(le.MouseCursor(I.gameArea.GetArea()))
	{
    	    I.gameArea.QueueEventProcessing();
	}

	
	// animation
        if(Game::ShouldAnimateInfrequent(ticket, 1))
        {
            if(I.gameArea.NeedScroll())
            {
    		cursor.SetThemes(I.gameArea.GetScrollCursor());
    		I.gameArea.Scroll();

    		// need stop hero
    		if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
    		    global_focus.GetHeroes().SetMove(false);

		I.SetRedraw(REDRAW_GAMEAREA | REDRAW_RADAR | REDRAW_CURSOR);
            }
	    else
    	    if(Game::Focus::HEROES == global_focus.Type())
	    {
		Heroes & hero = global_focus.GetHeroes();
		if(hero.isEnableMove())
		{
		    if(hero.Move())
		    {
            		I.gameArea.Center(global_focus.Center());
            		global_focus.Reset(Focus::HEROES);
            		global_focus.SetRedraw();

            		I.gameArea.SetUpdateCursor();
			I.SetRedraw(REDRAW_CURSOR);
		    }
		    else
		    {
			I.SetRedraw(REDRAW_GAMEAREA);
		    }

		    if(hero.isAction())
		    {
			// check game over
			CheckGameOver(res);
			hero.ResetAction();
		    }
		}
		else
		    hero.SetMove(false);
	    }
        }

	if(Game::ShouldAnimateInfrequent(ticket, 20))
	{
	    Maps::IncreaseAnimationTicket();
	    I.SetRedraw(REDRAW_GAMEAREA);
	}

	if(I.NeedRedraw())
	{
    	    cursor.Hide();
    	    I.Redraw();
    	    cursor.Show();
    	    display.Flip();
	}

        ++ticket;
    }

    if(ENDTURN == res)
    {
	// warning lost all town
	if(myHeroes.size() && myCastles.empty() && LOST_TOWN_DAYS < myKingdom.GetLostTownDays())
	{
	    std::string str = _("%{color} player, you have lost your last town. If you do not conquer another town in next week, you will be eliminated.");
	    String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	    DialogPlayers(conf.MyColor(), str);
	}

	if(Game::Focus::HEROES == global_focus.Type())
	{
	    global_focus.GetHeroes().ShowPath(false);
	    global_focus.SetRedraw();
	}

	if(conf.Modes(Settings::AUTOSAVE))
	{
	    std::string filename(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "save" + SEPARATOR +  "autosave.sav");
	    Game::Save(filename);
	}

	// for pocketpc: show status window if AI turn
	if(conf.PocketPC() && !conf.ShowStatus())
	{
	    conf.SetModes(Settings::SHOWSTATUS);
    	    I.SetRedraw(REDRAW_STATUS);
	}

	if(I.NeedRedraw())
	{
    	    cursor.Hide();
    	    I.Redraw();
    	    cursor.Show();
    	    display.Flip();
	}
    }
    else
    if(QUITGAME == res)
    {
#ifdef WITH_NET
        if(Game::NETWORK == conf.GameType()) Network::Logout();
#endif
    }

    // reset sound
    //Mixer::Reset();

    return res;
}

bool Game::DiggingForArtifacts(const Heroes & hero)
{
    if(hero.GetMaxMovePoints() == hero.GetMovePoints())
    {
	if(0 != world.GetTiles(hero.GetCenter()).GetSize1())
	{
	    Dialog::Message("", _("Try searching on clear ground."), Font::BIG, Dialog::OK);
	    return false;
	}

	AGG::PlaySound(M82::DIGSOUND);

	const_cast<Heroes &>(hero).ResetMovePoints();
	const Artifact ultimate(world.GetUltimateArtifact());

	if(world.DiggingForUltimateArtifact(hero.GetCenter()) && ultimate != Artifact::UNKNOWN)
	{
	    AGG::PlaySound(M82::TREASURE);
	    // check returns
	    const_cast<Heroes &>(hero).PickupArtifact(ultimate());
	    DialogWithArtifact(_("Congratulations!"), _("After spending many hours digging here, you have uncovered the ") + ultimate.GetName(), ultimate());
	}
	else
	    Dialog::Message("", _("Nothing here. Where could it be?"), Font::BIG, Dialog::OK);

	Cursor::Get().Hide();
	Interface::IconsPanel::Get().GetHeroesBar().Redraw();
	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
	Dialog::Message("", _("Digging for artifacts requires a whole day, try again tomorrow."), Font::BIG, Dialog::OK);

    return false;
}

Game::menu_t Game::NetworkTurn(Kingdom & kingdom)
{
    Error::Verbose("Game::NetworkGame: player: " + Color::String(kingdom.GetColor()));

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Show();
    display.Flip();

    cursor.Hide();

    return ENDTURN;
}

void Game::MouseCursorAreaClickLeft(u16 index_maps)
{
    Game::Focus & global_focus = Focus::Get();
    switch(global_focus.Type())
    {
	case Focus::HEROES:	FocusHeroesClickLeftAction(global_focus.GetHeroes(), index_maps); break;
	case Focus::CASTLE:	FocusCastleClickLeftAction(global_focus.GetCastle(), index_maps); break;
	default: break;
    }
}

void Game::FocusHeroesClickLeftAction(Heroes & from_hero, u16 index_maps)
{
    Game::Focus & global_focus = Focus::Get();
    Maps::Tiles & tile = world.GetTiles(index_maps);

    switch(tile.GetObject())
    {
	// from hero to castle
	case MP2::OBJN_CASTLE:
	{
    	    const Castle *to_castle = world.GetCastle(index_maps);
	    if(NULL == to_castle) break;
	    else
	    if(from_hero.GetColor() == to_castle->GetColor())
	    {
		global_focus.Set(const_cast<Castle *>(to_castle));
		global_focus.SetRedraw();
	    }
	    else
		ShowPathOrStartMoveHero(&from_hero, Maps::GetIndexFromAbsPoint(to_castle->GetCenter()));
	}
	break;

    	// from hero to hero
    	case MP2::OBJ_HEROES:
	{
	    const Heroes * to_hero = world.GetHeroes(index_maps);
	    if(NULL == to_hero) break;
	    else
	    if(from_hero.GetCenter() == to_hero->GetCenter())
		OpenHeroes(&from_hero);
	    else
		ShowPathOrStartMoveHero(&from_hero, index_maps);
    	}
	break;

	default:
	    if(tile.isPassable(&from_hero) || MP2::isActionObject(tile.GetObject(), from_hero.isShipMaster()))
		ShowPathOrStartMoveHero(&from_hero, index_maps);
	    break;
    }
}

void Game::FocusCastleClickLeftAction(Castle & from_castle, u16 index_maps)
{
    Game::Focus & global_focus = Focus::Get();
    Maps::Tiles & tile = world.GetTiles(index_maps);

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
		    global_focus.Set(const_cast<Castle *>(to_castle));
		    global_focus.SetRedraw();
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
    		global_focus.Set(const_cast<Heroes *>(to_hero));
    		global_focus.SetRedraw();
    	    }
	}
	break;

	default: break;
    }
}

void Game::MouseCursorAreaPressRight(u16 index_maps)
{
    Settings & conf = Settings::Get();
    Maps::Tiles & tile = world.GetTiles(index_maps);

    if(conf.Debug()) tile.DebugInfo();

    if(!conf.Debug() && tile.isFog(conf.MyColor()))
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
	    const Heroes *heroes = world.GetHeroes(tile.GetIndex());
	    if(heroes) Dialog::QuickInfo(*heroes);
	}
	break;

	default:
	    Dialog::QuickInfo(tile);
	break;
    }
}

void Game::ButtonNextHero(void)
{
    Game::Focus & global_focus = Focus::Get();

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();

    if(myHeroes.empty()) return;

    if(Game::Focus::HEROES != global_focus.Type())
    {
	global_focus.Reset(Game::Focus::HEROES);
    }
    else
    {
	std::vector<Heroes *>::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), &global_focus.GetHeroes());
	++it;
	if(it == myHeroes.end()) it = myHeroes.begin();
	global_focus.Set(*it);
    }
    global_focus.SetRedraw();
}

void Game::ButtonMovement(void)
{
    Game::Focus & global_focus = Focus::Get();
    if(Game::Focus::HEROES == global_focus.Type() &&
	global_focus.GetHeroes().GetPath().isValid())
	global_focus.GetHeroes().SetMove(!global_focus.GetHeroes().isEnableMove());
}

void Game::ButtonKingdom(void)
{
}

void Game::ButtonSpell(void)
{
    Game::Focus & global_focus = Focus::Get();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	Spell::spell_t spell = global_focus.GetHeroes().GetSpellBook().Open(SpellBook::ADVN, true);
	// TODO cast selected spell
	Error::Verbose("spell selected: " + std::string(Spell::GetName(spell)));
    }
}

void Game::ButtonEndTurn(Game::menu_t & ret)
{
    Game::Focus & global_focus = Focus::Get();
    const Kingdom & myKingdom = world.GetMyKingdom();

    if(Game::Focus::HEROES == global_focus.Type())
	global_focus.GetHeroes().SetMove(false);

    if(!myKingdom.HeroesMayStillMove() ||
	Dialog::YES == Dialog::Message("", _("One or more heroes may still move, are you sure you want to end your turn?"), Font::BIG, Dialog::YES | Dialog::NO))
	ret = ENDTURN;
}

void Game::ButtonAdventure(Game::menu_t & ret)
{
    Game::Focus & global_focus = Focus::Get();
    Mixer::Reduce();
    switch(Dialog::AdventureOptions(Game::Focus::HEROES == global_focus.Type()))
    {
	case Dialog::WORLD:
	    break;

	case Dialog::PUZZLE: 
	    KeyPress_p();
	    break;

	case Dialog::INFO:
	    KeyPress_i();
	    break;

	case Dialog::DIG:
	    KeyPress_d(ret);
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

void Game::ButtonFile(Game::menu_t & ret)
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
	    KeyPress_l(ret);
	    break;

	case SAVEGAME:
	    KeyPress_s();
	    break;

	default:
	break;
    }
}

void Game::ButtonSystem(void)
{
    // Change and save system settings
    const u8 changes = Dialog::SystemOptions();
		
    if(0x08 & changes)
    {
	Interface::Basic & I = Interface::Basic::Get();

        I.SetRedraw(REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_BORDER);
    }
}

void Game::KeyPress_ESC(menu_t & ret)
{
    Focus & global_focus = Focus::Get();

    // stop hero
    if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
	global_focus.GetHeroes().SetMove(false);
    else
    if(Dialog::YES & Dialog::Message("", _("Are you sure you want to quit?"), Font::BIG, Dialog::YES|Dialog::NO))
	ret = QUITGAME;
}

void Game::KeyPress_e(menu_t & ret)
{
    ButtonEndTurn(ret);
}

void Game::KeyPress_t(void)
{
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<Castle *> & myCastles = myKingdom.GetCastles();

    if(myCastles.size())
    {
	Focus & global_focus = Focus::Get();

	if(Game::Focus::CASTLE != global_focus.Type())
	    global_focus.Reset(Game::Focus::CASTLE);
	else
	{
	    std::vector<Castle *>::const_iterator it = std::find(myCastles.begin(), myCastles.end(), &global_focus.GetCastle());
    	    ++it;
    	    if(it == myCastles.end()) it = myCastles.begin();
	    global_focus.Set(*it);
        }
        global_focus.SetRedraw();
    }
}

void Game::KeyPress_s(void)
{
    std::string filename;
    if(Dialog::SelectFileSave(filename) && filename.size())
    {
	Game::Save(filename);
	Dialog::Message("", _("Game saved successfully."), Font::BIG, Dialog::OK);
    }
}

void Game::KeyPress_l(menu_t & ret)
{
    if(Settings::Get().Original())
    {
	if(Dialog::YES == Dialog::Message("", _("Are you sure you want to load a new game? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
	ret = LOADGAME;
    }
    else
    {
	// fast load
	std::string filename;
	if(Dialog::SelectFileLoad(filename) && filename.size())
	    ret = Game::Load(filename) ? STARTGAME : MAINMENU;
    }
}

void Game::KeyPress_p(void)
{
    world.GetMyKingdom().PuzzleMaps().ShowMapsDialog();
}

void Game::KeyPress_i(void)
{
    Dialog::GameInfo();
}

void Game::KeyPress_d(menu_t & ret)
{
    Focus & global_focus = Focus::Get();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	DiggingForArtifacts(global_focus.GetHeroes());
	// check game over for ultimate artifact
	CheckGameOver(ret);
    }
}

void Game::KeyPress_SPACE(void)
{
    Focus & global_focus = Focus::Get();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	Heroes & hero = global_focus.GetHeroes();
	Interface::Basic & I = Interface::Basic::Get();
	hero.SetMove(false);
	hero.GetPath().Reset();
	I.SetRedraw(REDRAW_GAMEAREA);
	if(MP2::isActionObject(hero.GetUnderObject(), hero.isShipMaster()))
	    hero.Action(hero.GetIndex());
	if(MP2::OBJ_STONELIGHTS == hero.GetUnderObject() || MP2::OBJ_WHIRLPOOL == hero.GetUnderObject())
	{
	    hero.ApplyPenaltyMovement();
	    I.SetRedraw(REDRAW_HEROES);
	}
    }
}

void Game::KeyPress_RETURN(void)
{
    Focus & global_focus = Focus::Get();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	Heroes & hero = global_focus.GetHeroes();
	OpenHeroes(&hero);
    }
    else
    if(Game::Focus::CASTLE == global_focus.Type())
    {
	Castle & castl = global_focus.GetCastle();
	OpenCastle(&castl);
    }
}

void Game::KeyPress_LEFT(void)
{
    Focus & global_focus = Focus::Get();
    LocalEvent & le = LocalEvent::Get();
    // scroll map
    if((MOD_CTRL & le.KeyMod())) Interface::GameArea::Get().SetScroll(SCROLL_LEFT);
    else
    // move hero
    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::LEFT);
}

void Game::KeyPress_RIGHT(void)
{
    Focus & global_focus = Focus::Get();
    LocalEvent & le = LocalEvent::Get();
    // scroll map
    if((MOD_CTRL & le.KeyMod())) Interface::GameArea::Get().SetScroll(SCROLL_RIGHT);
    else
    // move hero
    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::RIGHT);
}

void Game::KeyPress_TOP(void)
{
    Focus & global_focus = Focus::Get();
    LocalEvent & le = LocalEvent::Get();
    // scroll map
    if((MOD_CTRL & le.KeyMod())) Interface::GameArea::Get().SetScroll(SCROLL_TOP);
    else
    // move hero
    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::TOP);
}

void Game::KeyPress_BOTTOM(void)
{
    Focus & global_focus = Focus::Get();
    LocalEvent & le = LocalEvent::Get();
    // scroll map
    if((MOD_CTRL & le.KeyMod())) Interface::GameArea::Get().SetScroll(SCROLL_BOTTOM);
    else
    // move hero
    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::BOTTOM);
}

void Game::NewWeekDialog(void)
{
    const Week::type_t name = world.GetWeekType();
    std::string message = world.BeginMonth() ? _("Astrologers proclaim month of the %{name}.") : _("Astrologers proclaim week of the %{name}.");
    AGG::PlayMusic(world.BeginMonth() ? MUS::WEEK2_MONTH1 : MUS::WEEK1, false);
    String::Replace(message, "%{name}", Week::GetString(name));
    message += "\n \n";
    message += (name == Week::PLAGUE ? _(" All populations are halved.") : _(" All dwellings increase population."));
    Dialog::Message("", message, Font::BIG, Dialog::OK);
}

void Game::ShowEventDay(void)
{
    Kingdom & myKingdom = world.GetMyKingdom();
    std::vector<GameEvent::Day *> events;
    events.reserve(5);
    world.GetEventDay(myKingdom.GetColor(), events);
    std::vector<GameEvent::Day *>::const_iterator it1 = events.begin();
    std::vector<GameEvent::Day *>::const_iterator it2 = events.end();

    for(; it1 != it2; ++it1) if(*it1)
    {
    	AGG::PlayMusic(MUS::NEWS, false);
	if((*it1)->GetResource().GetValidItems())
	    Dialog::ResourceInfo("", (*it1)->GetMessage(), (*it1)->GetResource());
	else
	if((*it1)->GetMessage().size())
	    Dialog::Message("", (*it1)->GetMessage(), Font::BIG, Dialog::OK);
    }
}

void Game::ShowWarningLostTowns(menu_t & ret)
{
    const Kingdom & myKingdom = world.GetMyKingdom();
    Settings & conf = Settings::Get();
    if(0 == myKingdom.GetLostTownDays())
    {
    	    AGG::PlayMusic(MUS::DEATH, false);
	    std::string str = _("%{color} player, your heroes abandon you, and you are banished from this land.");
	    String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	    DialogPlayers(conf.MyColor(), str);
	    conf.SetGameOverResult(GameOver::LOSS_ALL);
	    ret = MAINMENU;
    }
    else
    if(1 == myKingdom.GetLostTownDays())
    {
	    std::string str = _("%{color} player, this is your last day to capture a town, or you will be banished from this land.");
	    String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	    DialogPlayers(conf.MyColor(), str);
    }
    else
    if(LOST_TOWN_DAYS >= myKingdom.GetLostTownDays())
    {
	    std::string str = _("%{color} player, you only have %{day} days left to capture a town, or you will be banished from this land.");
	    String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	    String::Replace(str, "%{day}", myKingdom.GetLostTownDays());
	    DialogPlayers(conf.MyColor(), str);
    }
}

void Game::KeyPress_r(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowRadar())
	{
	    conf.ResetModes(Settings::SHOWRADAR);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.PocketPC() && conf.Modes(Settings::SHOWICONS | Settings::SHOWSTATUS | Settings::SHOWBUTTONS))
	    {
		conf.ResetModes(Settings::SHOWICONS | Settings::SHOWSTATUS | Settings::SHOWBUTTONS);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetModes(Settings::SHOWRADAR);
	    Interface::Basic::Get().SetRedraw(REDRAW_RADAR);
	}
    }
}

void Game::KeyPress_b(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowButtons())
	{
	    conf.ResetModes(Settings::SHOWBUTTONS);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.PocketPC() && conf.Modes(Settings::SHOWRADAR | Settings::SHOWSTATUS | Settings::SHOWICONS))
	    {
		conf.ResetModes(Settings::SHOWRADAR | Settings::SHOWSTATUS | Settings::SHOWICONS);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetModes(Settings::SHOWBUTTONS);
	    Interface::Basic::Get().SetRedraw(REDRAW_BUTTONS);
	}
    }
}

void Game::KeyPress_w(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowStatus())
	{
	    conf.ResetModes(Settings::SHOWSTATUS);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.PocketPC() && conf.Modes(Settings::SHOWRADAR | Settings::SHOWICONS | Settings::SHOWBUTTONS))
	    {
		conf.ResetModes(Settings::SHOWRADAR | Settings::SHOWICONS | Settings::SHOWBUTTONS);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetModes(Settings::SHOWSTATUS);
	    Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
	}
    }
}

void Game::KeyPress_c(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowIcons())
	{
	    conf.ResetModes(Settings::SHOWICONS);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    if(conf.PocketPC() && conf.Modes(Settings::SHOWRADAR | Settings::SHOWSTATUS | Settings::SHOWBUTTONS))
	    {
		conf.ResetModes(Settings::SHOWRADAR | Settings::SHOWSTATUS | Settings::SHOWBUTTONS);
		Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	    }
	    conf.SetModes(Settings::SHOWICONS);
	    Interface::Basic::Get().SetRedraw(REDRAW_ICONS);
	}
    }
}

void Game::KeyPress_a(void)
{
    Settings & conf = Settings::Get();

    if(conf.HideInterface())
    {
	if(conf.ShowControlPanel())
	{
	    conf.ResetModes(Settings::SHOWCPANEL);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
	else
	{
	    conf.SetModes(Settings::SHOWCPANEL);
	    Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
	}
    }
}
