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
#include "game_io.h"
#include "settings.h"
#include "route.h"
#include "game_focus.h"
#include "kingdom.h"

extern u16 DialogWithArtifact(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 buttons = Dialog::OK);

namespace Game
{
    Cursor::themes_t GetCursor(const Maps::Tiles & tile);
    void OpenCastle(Castle *castle);
    void OpenHeroes(Heroes *heroes);
    void ShowPathOrStartMoveHero(Heroes *hero, const u16 dst_index);
    menu_t HumanTurn(void);
    menu_t NetworkTurn(Kingdom &);
    bool CursorChangePosition(const u16 index);
    bool DiggingForArtifacts(const Heroes & hero);
    void DialogPlayers(const Color::color_t, const std::string &);
    void MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t direct);
    void CheckKingdomsLoss(void);
};

void Game::CheckKingdomsLoss(void)
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

    // preload sounds
    Game::PreloadLOOPSounds();

    // set controls type for players
    switch(conf.GameType())
    {
    	case Game::HOTSEAT:
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
        	world.GetKingdom(color).SetControl(LOCAL);
    	    break;

    	case Game::NETWORK:
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
        	world.GetKingdom(color).SetControl(color == conf.MyColor() ? LOCAL : REMOTE);
    	    break;

    	default:
    	    world.GetKingdom(conf.MyColor()).SetControl(LOCAL);
    	    break;
    }

    // update starting resource
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
        world.GetKingdom(color).UpdateStartingResource();

    GameArea & areaMaps = GameArea::Get();
    areaMaps.Build();

    Game::Focus & global_focus = Focus::Get();
    global_focus.Reset();

    // generate sprite map
    areaMaps.GenerateUltimateArtifactAreaSurface(world.GetUltimateArtifactIndex(), world.GetUltimateArtifactArea());
    world.GetUltimateArtifactArea().Sepia();

    Mixer::Reset();

    // Create radar
    Radar & radar = Radar::Get();
    radar.Build();

    // draw interface
    Game::Interface::Get().Draw();

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
			std::string str = _("%{color} player's turn");
			String::Replace(str, "%{color}", Color::String(color));
			DialogPlayers(color, str);
		    }
		    conf.SetMyColor(color);
		    if(conf.Modes(Settings::LOADGAME)) conf.ResetModes(Settings::LOADGAME);
		    m = HumanTurn();
		    Mixer::Reduce();
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
	}
	DELAY(1);
    }

    /* for testing: show score  */
    if(HIGHSCORES == m)
    {
	std::string str;
	String::AddInt(str, Game::GetGameOverScores());
	Dialog::Message("Your Score:", str, Font::BIG, Dialog::OK);
    }

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
    Game::StatusWindow::ResetTimer();

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
    Game::StatusWindow::ResetTimer();

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
    Display & display = Display::Get();

    Cursor::Get().Hide();
    // show path
    if(path.GetDestinationIndex() != dst_index)
    {
        hero->SetMove(false);
	path.Calculate(dst_index);
        if(1 < Settings::Get().Debug()) path.Dump();
        path.Show();
	GameArea::Get().Redraw();
    }
    // start move
    else
    if(path.isValid())
    {
        Game::Focus::Get().Set(hero);
        Game::Focus::Get().Redraw();
        hero->SetMove(true);
    }
    Cursor::Get().Show();
    display.Flip();
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
    Settings & conf = Settings::Get();

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

    global_focus.Reset();

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

    std::string filename;

    cursor.Show();
    display.Flip();

    // new week dialog
    if(1 < world.CountWeek() && world.BeginWeek())
    {
        const Week::type_t name = world.GetWeekType();
        std::string message = world.BeginMonth() ? _("Astrologers proclaim month of the %{name}.") : _("Astrologers proclaim week of the %{name}.");
        AGG::PlayMusic(world.BeginMonth() ? MUS::WEEK2_MONTH1 : MUS::WEEK1, false);
        String::Replace(message, "%{name}", Week::GetString(name));
        message += "\n \n";
        message += (name == Week::PLAGUE ? _(" All populations are halved.") : _(" All dwellings increase population."));
        Dialog::Message("", message, Font::BIG, Dialog::OK);
    }

    // show event day
    {
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

    // kingdom loss dialog
    CheckKingdomsLoss();

    Game::menu_t res = CANCEL;
    u16 cond = GameOver::COND_NONE;

    // game over conditions
    if(GameOver::COND_NONE != (cond = world.CheckKingdomLoss(myKingdom)))
    {
	conf.SetGameOverResult(cond);
	GameOver::DialogLoss(cond);
	res = MAINMENU;
    }
    else
    if(GameOver::COND_NONE != (cond = world.CheckKingdomWins(myKingdom)))
    {
	conf.SetGameOverResult(cond);
    	GameOver::DialogWins(cond);
	res = HIGHSCORES;
    }

    // warning lost all town
    if(myCastles.empty())
    {
	if(0 == myKingdom.GetLostTownDays())
	{
    	    AGG::PlayMusic(MUS::DEATH, false);
	    std::string str = _("%{color} player, your heroes abandon you, and you are banished from this land.");
	    String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	    DialogPlayers(conf.MyColor(), str);
	    conf.SetGameOverResult(GameOver::LOSS_ALL);
	    res = MAINMENU;
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
    
    // startgame loop
    while(!res && le.HandleEvents())
    {
	// Hot Keys:
	// ESC
	if(le.KeyPress(KEY_ESCAPE))
	{
    	    // stop hero
    	    if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
    	    	global_focus.GetHeroes().SetMove(false);
    	    else
    	    if(Dialog::YES & Dialog::Message("", _("Are you sure you want to quit?"), Font::BIG, Dialog::YES|Dialog::NO))
		res = QUITGAME;

    	    continue;
	}
        else
        // press End Turn
        if(le.KeyPress(KEY_e))
        {
            if(Game::Focus::HEROES == global_focus.Type())
                global_focus.GetHeroes().SetMove(false);

            if(!myKingdom.HeroesMayStillMove() ||
                Dialog::YES == Dialog::Message("", _("One or more heroes may still move, are you sure you want to end your turn?"), Font::BIG, Dialog::YES | Dialog::NO))
		res = ENDTURN;

    	    continue;
        }
        else
        // press Next Hero
        if(le.KeyPress(KEY_h) && myHeroes.size())
	{
	    if(Game::Focus::HEROES != global_focus.Type())
	    {
		cursor.Hide();
		global_focus.Reset(Game::Focus::HEROES);
		global_focus.Redraw();
		cursor.Show();
		display.Flip();
	    }
	    else
	    {
	        std::vector<Heroes *>::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), &global_focus.GetHeroes());
        	++it;
                if(it == myHeroes.end()) it = myHeroes.begin();
		cursor.Hide();
		global_focus.Set(*it);
		global_focus.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}
        else
        // press Next Tower
        if(le.KeyPress(KEY_t) && myCastles.size())
	{
	    if(Game::Focus::CASTLE != global_focus.Type())
	    {
		cursor.Hide();
		global_focus.Reset(Game::Focus::CASTLE);
		global_focus.Redraw();
		cursor.Show();
		display.Flip();
	    }
	    else
	    {
	        std::vector<Castle *>::const_iterator it = std::find(myCastles.begin(), myCastles.end(), &global_focus.GetCastle());
        	++it;
                if(it == myCastles.end()) it = myCastles.begin();
		cursor.Hide();
		global_focus.Set(*it);
		global_focus.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}
	else
	// save game
	if(le.KeyPress(KEY_s))
	{
	    if(Dialog::SelectFileSave(filename) && filename.size())
	    {
		Game::Save(filename);
		Dialog::Message("", _("Game saved successfully."), Font::BIG, Dialog::OK);
	    }
	}
	else
	// load game
	if(le.KeyPress(KEY_l))
	{
	    if(conf.Original())
	    {
		if(Dialog::YES == Dialog::Message("", _("Are you sure you want to load a new game? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
		return LOADGAME;
	    }
	    else
	    {
		// fast load
		if(Dialog::SelectFileLoad(filename) && filename.size()){ Game::Load(filename); return STARTGAME; }
	    }
	}
	else
	// puzzle maps
	if(le.KeyPress(KEY_p))
	{
	    Dialog::PuzzleMaps();
	}
	else
	// game info
	if(le.KeyPress(KEY_i))
	{
	    Dialog::GameInfo();
	}
	else
	// dig artifact
	if(le.KeyPress(KEY_d) && Game::Focus::HEROES == global_focus.Type())
	{
	    DiggingForArtifacts(global_focus.GetHeroes());
	    // check game over for ultimate artifact
	    if(GameOver::COND_NONE != (cond = world.CheckKingdomWins(myKingdom)))
	    {
		conf.SetGameOverResult(cond);
		GameOver::DialogWins(cond);
		res = HIGHSCORES;
	    }
	}
	else
	// space: action
	if(le.KeyPress(KEY_SPACE) && Game::Focus::HEROES == global_focus.Type())
	{
	    Heroes & hero = global_focus.GetHeroes();
	    cursor.Hide();
	    hero.SetMove(false);
	    hero.GetPath().Reset();
	    gamearea.Redraw();
	    if(MP2::isActionObject(hero.GetUnderObject(), hero.isShipMaster()))
		hero.Action(hero.GetIndex());
	    if(MP2::OBJ_STONELIGHTS == hero.GetUnderObject() || MP2::OBJ_WHIRLPOOL == hero.GetUnderObject())
	    {
		cursor.Hide();
		hero.ApplyPenaltyMovement();
		selectHeroes.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}
	else
	// return: open dialog
	if(le.KeyPress(KEY_RETURN))
	{
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
	else
	// key left
	if(le.KeyPress(KEY_LEFT))
	{
	    // scroll map
	    if((MOD_CTRL & le.KeyMod()) && gamearea.AllowScroll(GameArea::LEFT)) scrollDir |= GameArea::LEFT;
	    else
	    // move hero
	    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::LEFT);
	}
	else
	// key right
	if(le.KeyPress(KEY_RIGHT))
	{
	    // scroll map
	    if((MOD_CTRL & le.KeyMod()) && gamearea.AllowScroll(GameArea::RIGHT)) scrollDir |= GameArea::RIGHT;
	    else
	    // move hero
	    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::RIGHT);
	}
	else
	// key top
	if(le.KeyPress(KEY_UP))
	{
	    // scroll map
	    if((MOD_CTRL & le.KeyMod()) && gamearea.AllowScroll(GameArea::TOP)) scrollDir |= GameArea::TOP;
	    else
	    // move hero
	    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::TOP);
	}
	else
	// key bottom
	if(le.KeyPress(KEY_DOWN))
	{
	    // scroll map
	    if((MOD_CTRL & le.KeyMod()) && gamearea.AllowScroll(GameArea::BOTTOM)) scrollDir |= GameArea::BOTTOM;
	    else
	    // move hero
	    if(Focus::HEROES == global_focus.Type()) MoveHeroFromArrowKeys(global_focus.GetHeroes(), Direction::BOTTOM);
	}

	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft) && gamearea.AllowScroll(GameArea::LEFT)) scrollDir |= GameArea::LEFT;
        else
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight) && gamearea.AllowScroll(GameArea::RIGHT)) scrollDir |= GameArea::RIGHT;
	
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop) && gamearea.AllowScroll(GameArea::TOP)) scrollDir |= GameArea::TOP;
	else
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom) && gamearea.AllowScroll(GameArea::BOTTOM)) scrollDir |= GameArea::BOTTOM;

	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
	    const Point & mouse_coord = le.MouseCursor();
	    const int index_maps = gamearea.GetIndexFromMousePoint(mouse_coord);
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
		    gamearea.Redraw();
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
    		if(0 != splitter.GetStep())
    		{
    		    u32 seek = (le.MouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
        	    if(seek > myCastles.size() - I.CountIcons()) seek = myCastles.size() - I.CountIcons();

        	    cursor.Hide();
        	    splitter.Move(seek);
		    selectCastle.SetTop(seek);
		    selectCastle.Redraw();
        	    cursor.Show();
        	    display.Flip();
		}
	    }
	    else
    	    // move splitter cursor heroes
    	    if(le.MousePressLeft(selectHeroes.GetSplitter().GetRect()) &&
    		myHeroes.size() > I.CountIcons())
    	    {
    		Splitter & splitter = selectHeroes.GetSplitter();
    		if(0 != splitter.GetStep())
    		{
    		    u32 seek = (le.MouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
        	    if(seek > myHeroes.size() - I.CountIcons()) seek = myHeroes.size() - I.CountIcons();

        	    cursor.Hide();
        	    splitter.Move(seek);
		    selectHeroes.SetTop(seek);
		    selectHeroes.Redraw();
        	    cursor.Show();
        	    display.Flip();
		}
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
	    if(le.MouseClickLeft(buttonNextHero) && myHeroes.size())
	    {
		if(Game::Focus::HEROES != global_focus.Type())
		{
		    cursor.Hide();
		    global_focus.Reset(Game::Focus::HEROES);
		    global_focus.Redraw();
		    cursor.Show();
		    display.Flip();
		}
		else
		{
		    std::vector<Heroes *>::const_iterator it = std::find(myHeroes.begin(), myHeroes.end(), &global_focus.GetHeroes());
                    ++it;
                    if(it == myHeroes.end()) it = myHeroes.begin();
		    cursor.Hide();
		    global_focus.Set(*it);
		    global_focus.Redraw();
		    cursor.Show();
		    display.Flip();
		}
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
	    }
	    else
	    // click Cast Spell
	    if(le.MouseClickLeft(buttonSpell))
	    {
    		if(Game::Focus::HEROES == global_focus.Type()) {
		    Spell::spell_t spell = global_focus.GetHeroes().GetSpellBook().Open(SpellBook::ADVN, true);
		    // TODO cast selected spell
		    Error::Verbose("spell selected: " + std::string(Spell::GetName(spell)));
		}
	    }
	    else
	    // click End Turn
	    if(le.MouseClickLeft(buttonEndTur))
	    {
    		if(Game::Focus::HEROES == global_focus.Type())
		    global_focus.GetHeroes().SetMove(false);

		if(!myKingdom.HeroesMayStillMove() ||
		    Dialog::YES == Dialog::Message("", _("One or more heroes may still move, are you sure you want to end your turn?"), Font::BIG, Dialog::YES | Dialog::NO))
		    res = ENDTURN;

		continue;
	    }
	    else
    	    // click AdventureOptions
	    if(le.MouseClickLeft(buttonAdventure))
	    {
		Mixer::Reduce();

		switch(Dialog::AdventureOptions(Game::Focus::HEROES == global_focus.Type()))
		{
		    case Dialog::WORLD:	break;
		    case Dialog::PUZZLE:	Dialog::PuzzleMaps(); break;
		    case Dialog::INFO:	Dialog::GameInfo(); break;

		    case Dialog::DIG:
			DiggingForArtifacts(global_focus.GetHeroes());
			// check game over for ultimate artifact
			if(GameOver::COND_NONE != (cond = world.CheckKingdomWins(myKingdom)))
			{
			    conf.SetGameOverResult(cond);
			    GameOver::DialogWins(cond);
			    res = HIGHSCORES;
			}
			break;

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
		Mixer::Reset();

		switch(result)
		{
		    case NEWGAME:
			if(Dialog::YES == Dialog::Message("", _("Are you sure you want to restart? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
			    res = NEWGAME;
			break;

		    case QUITGAME:
			res = result;
			break;

		    case LOADGAME:
			if(Dialog::YES == Dialog::Message("", _("Are you sure you want to load a new game? (Your current game will be lost)"), Font::BIG, Dialog::YES|Dialog::NO))
			    res = LOADGAME;
			break;

		    case SAVEGAME:
			if(Dialog::SelectFileSave(filename) && filename.size()) Game::Save(filename);
			Dialog::Message("", _("Game saved successfully."), Font::BIG, Dialog::OK);
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
		    Mixer::Volume(-1, conf.SoundVolume());
		    Game::EnvironmentSoundMixer();
		}
		if(0x04 & changes)
		{
		    Music::Volume(conf.MusicVolume());
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
	    if(le.MousePressRight(radar.GetRect())) Dialog::Message(_("World Map"), _("A miniature view of the known world. Left click to move viewing area."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonNextHero)) Dialog::Message(_("Next Hero"), _("Select the next Hero."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonMovement)) Dialog::Message(_("Continue Movement"), _("Continue the Hero's movement along the current path."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonKingdom)) Dialog::Message(_("Kingdom Summary"), _("View a Summary of your Kingdom."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonSpell)) Dialog::Message(_("Cast Spell"), _("Cast an adventure spell."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonEndTur)) Dialog::Message(_("End Turn"), _("End your turn and left the computer take its turn."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonAdventure)) Dialog::Message(_("Adventure Options"), _("Bring up the adventure options menu."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonFile)) Dialog::Message(_("File Options"), _("Bring up the file options menu, alloving you to load menu, save etc."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonSystem)) Dialog::Message(_("System Options"), _("Bring up the system options menu, alloving you to customize your game."), Font::BIG);
	    else
	    if(le.MousePressRight(statusWindow.GetRect())) Dialog::Message(_("Status Window"), _("This window provides information on the status of your hero or kingdom, and shows the date. Left click here to cycle throungh these windows."), Font::BIG);

	// end cursor over left panel
	}

	// animation
        if(Game::ShouldAnimateInfrequent(ticket, 1))
        {
            if(scrollDir)
            {
        	cursor.Hide();
    		cursor.SetThemes(GameArea::ScrollToCursor(scrollDir));
    		gamearea.Scroll(scrollDir);

    		// need stop hero
    		if(Game::Focus::HEROES == global_focus.Type() && global_focus.GetHeroes().isEnableMove())
    		    global_focus.GetHeroes().SetMove(false);

        	scrollDir = GameArea::NONE;

		gamearea.Redraw();
        	radar.RedrawCursor();
        	cursor.Show();
        	display.Flip();
            }
	    else
    	    if(Game::Focus::HEROES == global_focus.Type())
	    {
		Heroes & hero = global_focus.GetHeroes();
		if(hero.isEnableMove())
		{
        	    cursor.Hide();
		    if(hero.Move())
		    {
            		gamearea.Center(global_focus.Center());
            		global_focus.Reset(Focus::HEROES);
            		global_focus.Redraw();

			cursor.SetThemes(GetCursor(world.GetTiles(gamearea.GetIndexFromMousePoint(le.MouseCursor()))));
		    }
		    gamearea.Redraw();
        	    cursor.Show();
        	    display.Flip();

		    // kingdom loss dialog
		    CheckKingdomsLoss();

		    // check game over
		    if(GameOver::COND_NONE != (cond = world.CheckKingdomLoss(myKingdom)))
		    {
			conf.SetGameOverResult(cond);
		    	GameOver::DialogLoss(cond);
			res = MAINMENU;
		    }
		    else
		    if(GameOver::COND_NONE != (cond = world.CheckKingdomWins(myKingdom)))
		    {
			conf.SetGameOverResult(cond);
		    	GameOver::DialogWins(cond);
			res = HIGHSCORES;
		    }
		}
		else
		    hero.SetMove(false);
	    }
        }

	if(Game::ShouldAnimateInfrequent(ticket, 20))
	{
    	    cursor.Hide();
	    Maps::IncreaseAnimationTicket();
	    gamearea.Redraw();
    	    cursor.Show();
    	    display.Flip();
	}

        ++ticket;
    }

    // warning lost all town
    if(ENDTURN == res && myHeroes.size() && myCastles.empty() && LOST_TOWN_DAYS < myKingdom.GetLostTownDays())
    {
	std::string str = _("%{color} player, you have lost your last town. If you do not conquer another town in next week, you will be eliminated.");
	String::Replace(str, "%{color}", Color::String(conf.MyColor()));
	DialogPlayers(conf.MyColor(), str);
    }

    cursor.Hide();
    if(Game::Focus::HEROES == global_focus.Type())
    {
	global_focus.GetHeroes().ShowPath(false);
	global_focus.Redraw();
        display.Flip();
    }

    if(ENDTURN == res && conf.Modes(Settings::AUTOSAVE))
    {
	filename = conf.LocalDataPrefix() + SEPARATOR + "save" + SEPARATOR +  "autosave.sav";
	Game::Save(filename);
    }

    // reset sound
    Mixer::Reset();

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
	Game::SelectBarHeroes::Get().Redraw(&hero);
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
    //const Settings & conf = Settings::Get();

    cursor.Show();
    display.Flip();

    cursor.Hide();

    return ENDTURN;
}
