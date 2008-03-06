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
#include "route.h"
#include "game_focus.h"

namespace Game
{
    void OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void OpenHeroes(Heroes *heroes, GameArea & areaMaps, Radar & radar);
    void FocusToCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void FocusToHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar);
};

Game::menu_t Game::StartGame(void)
{
    // Load maps
    world.LoadMaps(Settings::Get().FileInfo().FileMaps());

    GameArea areaMaps;
    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);

    Display & display = Display::Get();

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

    const Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    const Rect areaScrollTop(BORDERWIDTH / 2, 0, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    Point pt_stw, pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt ,pt_set, pt_shd, pt_scd;

    // coord button heroes scroll up
    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // coord button castle scroll up
    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    u8 icon_count = 0;

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

    Game::SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    Game::SelectFocusHeroes  & selectHeroes = SelectFocusHeroes::Get();

    selectCastles.SetCastles(myCastles);
    selectCastles.Reset();

    selectHeroes.SetHeroes(myHeroes);
    selectHeroes.Reset();

    const std::vector<Rect> & coordsCastlesIcon = selectCastles.GetCoords();
    const std::vector<Rect> & coordsHeroesIcon = selectHeroes.GetCoords();

    // splitter
    Splitter splitHeroes(AGG::GetICN(icnscroll, 4), Rect(pt_shu.x + 3, pt_shu.y + 18, 10, pt_shd.y - pt_shu.y - 20), Splitter::VERTICAL);
    Splitter splitCastles(AGG::GetICN(icnscroll, 4), Rect(pt_scu.x + 3, pt_scu.y + 18, 10, pt_scd.y - pt_scu.y - 20), Splitter::VERTICAL);

    splitHeroes.SetRange(0, myHeroes.size() > icon_count ? myHeroes.size() - icon_count : 0);
    splitCastles.SetRange(0, myCastles.size() > icon_count ? myCastles.size() - icon_count : 0);

    Game::Focus & global_focus = Focus::Get();

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
    // unknown focus
    else
    {
	Error::Warning("Game::StartGame: unknown game focus. exiting.");
	return QUITGAME;
    }

    selectCastles.Redraw();
    cursor.Hide();
    selectHeroes.Redraw();
    cursor.Hide();

    // center to focus
    areaMaps.Center(global_focus.Center());
    radar.RedrawCursor();
    cursor.Hide();

    // status window
    Game::StatusWindow statusWindow(pt_stw);
    statusWindow.Redraw();

    LocalEvent & le = LocalEvent::GetLocalEvent();

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

    cursor.Show();
    display.Flip();

    u32 ticket = 0;

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
    		    global_focus.GetHeroes().StopMove();
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
	    const Castle * castle = NULL;
	    const Heroes * heroes = NULL;
	    const u8 object = tile.GetObject();

	    switch(global_focus.Type())
	    {
		// focus from hero
		case Game::Focus::HEROES:
		{
		    Heroes & from_hero = global_focus.GetHeroes();

		    switch(object)
		    {
			// focus from hero to monster
    			case MP2::OBJ_MONSTER:
    			    cursor.SetThemes(Cursor::FIGHT);

			    if(le.MouseClickLeft(tile_pos))
			    {
				from_hero.ShowPathOrStartMove(index_maps);
			    }
			    else
			    if(le.MousePressRight(tile_pos))
			    {
				if(H2Config::Debug()) tile.DebugInfo(index_maps);

				Monster::monster_t monster = Monster::Monster(tile);

				if(Monster::UNKNOWN > monster) Dialog::QuickInfo(Army::String(Army::GetSize(Monster::GetSize(tile))) + " of " + Monster::String(monster));
			    }
			    break;

			// focus from hero to castle
    			case MP2::OBJN_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
    			    {
				if(H2Config::MyColor() == castle->GetColor())
				{
	    			    cursor.SetThemes(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					if(selectHeroes.isSelected())
					{
					    selectHeroes.Reset();
					    selectHeroes.Redraw();
					}

					// is selected open dialog
					if(selectCastles.isSelected() && castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    selectCastles.Redraw();

					    display.Flip();
					}
					// select other castle
					else
					{
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    splitCastles.Move(selectCastles.GetTopIndex());
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);

					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
	    			    cursor.SetThemes(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					from_hero.ShowPathOrStartMove(Maps::GetIndexFromAbsPoint(castle->GetCenter()));
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
    			    }
    			    break;

			// focus from hero to castle
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
			    {
				if(H2Config::MyColor() == castle->GetColor())
				{
        			    cursor.SetThemes(Cursor::ACTION);

				    if(le.MouseClickLeft(tile_pos))
				    {
					from_hero.ShowPathOrStartMove(index_maps);
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
				    cursor.SetThemes(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					from_hero.ShowPathOrStartMove(index_maps);
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
			    }
        		    break;

			// focus from hero to hero
			case MP2::OBJ_HEROES:
			{
			    const Heroes * heroes2 = world.GetHeroes(index_maps);

    			    if(NULL != heroes2)
    			    {
				if(H2Config::MyColor() == heroes2->GetColor())
				{
				    cursor.SetThemes(heroes2->GetCenter() == from_hero.GetCenter() ? Cursor::HEROES : Cursor::CHANGE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// same from_hero open dialog
					if(heroes2->GetCenter() == from_hero.GetCenter())
					{
					    OpenHeroes(&from_hero, areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// heroes dialog meeting
					else
					{
					    from_hero.ShowPathOrStartMove(index_maps);
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*heroes2);
				    }
				}
				// over enemy hero
				else
				{
				    cursor.SetThemes(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					from_hero.ShowPathOrStartMove(index_maps);
				    }
				    else
				    // show info enemy hero
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*heroes2);
				    }
				}
    			    }
    			}
    			    break;

			// focus from hero to boat
    			//case MP2::OBJ_BOAT:
    			//		cursor.SetThemes(Cursor::BOAT);
			//    break;

			// focus from hero to object
    			case MP2::OBJ_TREASURECHEST:
    			    if(Maps::Ground::WATER != tile.GetGround())
    			    {
    			        cursor.SetThemes(Cursor::ACTION);

				if(le.MouseClickLeft(tile_pos))
				{
				    from_hero.ShowPathOrStartMove(index_maps);
				}
    			    }
    			    break;

			// focus from hero to object
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
    			case MP2::OBJ_CRAKEDLAKE:
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

				cursor.SetThemes(Cursor::ACTION);

				if(le.MouseClickLeft(tile_pos))
				{
				    from_hero.ShowPathOrStartMove(index_maps);
                        	}
			    break;

			default:
                            if(Maps::Ground::WATER != tile.GetGround() && tile.isPassable())
                            {
                            	// FIXME: select MOVE cursor or MOVE2 or MOVE3 or MOVE4
                            	cursor.SetThemes(Cursor::MOVE);

				if(le.MouseClickLeft(tile_pos))
				    from_hero.ShowPathOrStartMove(index_maps);
                    	    }
                            else
                            {
                                cursor.SetThemes(Cursor::POINTER);
                            }
			    break;
		    }
		}
		break;

		// focus from boat
    		case Game::Focus::BOAT:
		{
    		    if(NULL == (heroes = world.GetHeroes(index_maps))) break;

		    switch(object)
		    {
			// focus from boat to castle
    			case MP2::OBJN_CASTLE:
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
			    {
				if((*castle).GetColor() == H2Config::MyColor())
				{
			    	    cursor.SetThemes(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// is selected open dialog
				        if(castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    selectCastles.Redraw();

					    display.Flip();
					}
					// select other castle
					else
					{
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    splitCastles.Move(selectCastles.GetTopIndex());
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
			    	    cursor.SetThemes(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
    			    }
    			    break;

			// focus from boat to hero
			//case MP2::OBJ_HEROES:
    			//    if(NULL != (heroes = world.GetHeroes(index_maps)) && (*heroes).GetColor() == H2Config::MyColor())
    			//			cursor.SetThemes(Cursor::HEROES);
        		//    break;

			// focus from boat to boat
    			case MP2::OBJ_BOAT:
    			    if(NULL != (heroes = world.GetHeroes(index_maps)))
					cursor.SetThemes((*heroes).GetColor() == H2Config::MyColor() ? Cursor::HEROES : Cursor::FIGHT);
    			    break;

			// focus from boat to object
    			case MP2::OBJ_TREASURECHEST:
			    if(Maps::Ground::WATER == tile.GetGround())
					cursor.SetThemes(Cursor::REDBOAT);
			    break;

			// focus from boat to object
    			case MP2::OBJ_SHIPWRECK:
    			case MP2::OBJ_WHIRLPOOL:
    			case MP2::OBJ_BUOY:
    			case MP2::OBJ_BOTTLE:
    			case MP2::OBJ_SHIPWRECKSURVIROR:
    			case MP2::OBJ_FLOTSAM:
    			case MP2::OBJ_MAGELLANMAPS:
					cursor.SetThemes(Cursor::REDBOAT);
			    break;

			// focus from boat to object
    			case MP2::OBJ_COAST:
					cursor.SetThemes(Cursor::ANCHOR);
			    break;

			default:
			    cursor.SetThemes(Maps::Ground::WATER == tile.GetGround() ? Cursor::BOAT : Cursor::POINTER);
			    break;
		    }
		}
		break;

		// focus from castle
		case Game::Focus::CASTLE:
		{
		    switch(object)
		    {
			// focus from castle to castle
    			case MP2::OBJN_CASTLE:
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
    			    {
    				if(castle->GetColor() == H2Config::MyColor())
    				{
    			    	    cursor.SetThemes(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// is selected open dialog
					if(castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    selectCastles.Redraw();
		    
					    display.Flip();
					}
					// select other castle
					else
					{
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    splitCastles.Move(selectCastles.GetTopIndex());
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
    			    	    cursor.SetThemes(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*castle);
				    }
				}
			    }
			    break;

			// focus from castle to heroes
			case MP2::OBJ_HEROES:
    			    if(NULL != (heroes = world.GetHeroes(index_maps)))
    			    {
    				if(heroes->GetColor() == H2Config::MyColor())
    				{
    			    	    cursor.SetThemes(Cursor::HEROES);

				    if(le.MouseClickLeft(tile_pos))
				    {
					if(selectCastles.isSelected())
					{
					    selectCastles.Reset();
					    selectCastles.Redraw();
					}

					// is selected open dialog
					if(selectHeroes.isSelected() && heroes->GetCenter() == selectHeroes.GetCenter(selectHeroes.GetSelectIndex()))
					{
					    OpenHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// select other hero
					else
					{
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);

					    const Route & path = global_focus.GetHeroes().GetPath();

					    if(path.size())
					    {
						cursor.Hide();
						path.Show();
						world.GetTiles((*heroes).GetCenter()).Redraw();
						cursor.Show();
					    }

					    display.Flip();
					}
        			    }
        			    else
				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*heroes);
				    }
        			}
        			else
        			{
    			    	    cursor.SetThemes(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					if(H2Config::Debug()) tile.DebugInfo(index_maps);
					
					Dialog::QuickInfo(*heroes);
				    }
        			}
        		    }
        		    break;

			// focus from castle to boat
    			//case MP2::OBJ_BOAT:
    			//    if(NULL != (heroes = world.GetHeroes(index_maps)) && (*heroes).GetColor() == H2Config::MyColor())
    			//			cursor.SetThemes(Cursor::HEROES);
        		//    break;

			default:
			    // default cursor
			    cursor.SetThemes(Cursor::POINTER);
			    break;
		    }
		}
		break;

    		case Game::Focus::UNSEL:
    		break;
	    }

	    if(le.MouseRight())
	    {
		if(H2Config::Debug()) tile.DebugInfo(index_maps);

		const std::string & info = (MP2::OBJ_ZERO == object || MP2::OBJ_EVENT == object ?
		    Maps::Ground::String(tile.GetGround()) : MP2::StringObject(object));

		Dialog::QuickInfo(info);
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

				const Route & path = global_focus.GetHeroes().GetPath();
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
	    }
	    else
	    // click End Turn
	    if(le.MouseClickLeft(buttonEndTur))
	    {
		cursor.Hide();
		world.NextDay();

		Color::color_t human = H2Config::MyColor();

		// AI move
		for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color != human)
		    world.GetKingdom(color).AITurns(statusWindow);

		statusWindow.SetState(Game::StatusWindow::DAY);
		statusWindow.Redraw();
	    
		selectCastles.Redraw();

		cursor.Show();
		display.Flip();
	    }
	    else
    	    // click AdventureOptions
	    if(le.MouseClickLeft(buttonAdventure))
	    {
		switch(Dialog::AdventureOptions())
		{
		    default:
			break;
		}
    	    }
	    else
	    // click FileOptions
	    if(le.MouseClickLeft(buttonFile))
	    {
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
	    }
	    else
	    // click SystemOptions
	    if(le.MouseClickLeft(buttonSystem)&& Dialog::OK == Dialog::SystemOptions())
	    {
		// Change and save system settings
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
	Game::EnvironmentSoundMixer();

        // draw heroes movement (in focus)
    	if(Game::Focus::HEROES == global_focus.Type() &&
    	    global_focus.GetHeroes().isNeedMove() && 
    	    global_focus.GetHeroes().isEnableMove() &&
    	    !(ticket % 100))	// FIXME: speed animation configurable
    	    {
    		global_focus.GetHeroes().Move();
		
		// center area map to hero
		cursor.Hide();
		areaMaps.Center(global_focus.Center());
		radar.RedrawCursor();
		cursor.Show();
		display.Flip();
	    }

        // animation
        if(!(ticket % 50)) 	// FIXME: speed animation configurable
        {
            cursor.Hide();
            areaMaps.RedrawAnimation();
            cursor.Show();
            display.Flip();
        }

        ++ticket;
    }

    return QUITGAME;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar)
{
    if(! castle) return;

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
    if(Game::Focus::HEROES == globalfocus.Type() || Game::Focus::BOAT == globalfocus.Type()) globalfocus.GetHeroes().GetPath().Hide();

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

    // hide path
    if(Game::Focus::HEROES == globalfocus.Type() || Game::Focus::BOAT == globalfocus.Type()) globalfocus.GetHeroes().GetPath().Hide();

    globalfocus.Set(*hero);

    areaMaps.Center(hero->GetCenter());
    radar.RedrawCursor();

    selectHeroes.SelectFromCenter(hero->GetCenter());
    selectHeroes.Redraw();
    selectCastles.Redraw();

    cursor.Show();
}
