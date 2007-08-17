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

#include <string>
#include <utility>
#include "agg.h"
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "config.h"
#include "captain.h"
#include "resource.h"
#include "castle.h"
#include "heroes.h"
#include "payment.h"
#include "kingdom.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "error.h"
#include "portrait.h"
#include "dialog.h"

void Castle::RedrawNameTown(const Point & src_pt)
{
    const Sprite & ramka = AGG::GetICN("TOWNNAME.ICN", 0);
    Point dst_pt(src_pt.x + 320 - ramka.w() / 2, src_pt.y + 248);
    display.Blit(ramka, dst_pt);

    dst_pt.x = src_pt.x + 320 - Text::width(name, Font::SMALL) / 2;
    dst_pt.y = src_pt.y + 248;
    Text(name, Font::SMALL, dst_pt);
}

Dialog::answer_t Castle::OpenDialog(void)
{
    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Dialog::FrameBorder background;

    const Point & cur_pt(background.GetArea());
    Point dst_pt(cur_pt);

    std::string str_icn;

    // button prev castle
    dst_pt.y += 480 - 19;
    Button buttonPrevCastle(dst_pt, "SMALLBAR.ICN", 1, 2);

    // bottom small bar
    Dialog::StatusBar statusBar(Point(cur_pt.x + 21, cur_pt.y + 461), AGG::GetICN("SMALLBAR.ICN", 0), Font::BIG);
    statusBar.Clear();

    u8 index_sprite = 0;

    // button next castle
    dst_pt.x += 640 - 21;
    Button buttonNextCastle(dst_pt, "SMALLBAR.ICN", 3, 4);

    // strip grid
    str_icn = "STRIP.ICN";
    
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 256;

    display.Blit(AGG::GetICN(str_icn, 0), dst_pt);

    // color crest
    switch(color)
    {
	case Color::BLUE:	index_sprite = 0; break;
	case Color::GREEN:	index_sprite = 1; break;
	case Color::RED:	index_sprite = 2; break;
	case Color::YELLOW:	index_sprite = 3; break;
	case Color::ORANGE:	index_sprite = 4; break;
	case Color::PURPLE:	index_sprite = 5; break;
	default: Error::Warning("Dialog::OpenCastle: unknown color."); return Dialog::CANCEL;
    }

    const Sprite & crest = AGG::GetICN("CREST.ICN", index_sprite);

    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 262;

    display.Blit(crest, dst_pt);

    // castle troops selector
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 262;
    Army::SelectBar selectCastleTroops(dst_pt, army);
    selectCastleTroops.Reset();
    selectCastleTroops.Redraw();
    const std::vector<Rect> & coordsCastleTroops = selectCastleTroops.GetCoords();

    // portrait heroes or captain or sign
    const Heroes * heroes = isHeroesPresent();

    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 361;
    
    Rect rectHeroPortrait(dst_pt.x, dst_pt.y, 100, 92);

    if(heroes)
    {
	display.Blit(Portrait::Hero((*heroes).GetHeroes(), Portrait::BIG), dst_pt);
    }
    else
    if(isBuild(Castle::BUILD_CAPTAIN))
	display.Blit(Portrait::Captain(race, Portrait::BIG), dst_pt);
    else
    	display.Blit(AGG::GetICN("STRIP.ICN", 3), dst_pt);

    // heroes troops background
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 361;

    Army::SelectBar selectHeroesTroops(dst_pt, heroes ? (*heroes).GetArmy() : army);
    const std::vector<Rect> & coordsHeroesTroops = heroes ? selectHeroesTroops.GetCoords() : coordsCastleTroops;

    if(heroes)
    {
	selectHeroesTroops.Reset();
	selectHeroesTroops.Redraw();
    }
    else
    	display.Blit(AGG::GetICN("STRIP.ICN", 11), dst_pt);
    
    // resource
    RedrawResourcePanel();

    // button exit
    dst_pt.x = cur_pt.x + 553;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, "SWAPBTN.ICN", 0, 1);


    const Rect coordBuildingThievesGuild(GetCoordBuilding(BUILD_THIEVESGUILD, cur_pt));
    const Rect coordBuildingTavern(GetCoordBuilding(BUILD_TAVERN, cur_pt));
    const Rect coordBuildingShipyard(GetCoordBuilding(BUILD_SHIPYARD, cur_pt));
    const Rect coordBuildingWell(GetCoordBuilding(BUILD_WELL, cur_pt));
    const Rect coordBuildingStatue(GetCoordBuilding(BUILD_STATUE, cur_pt));
    const Rect coordBuildingMarketplace(GetCoordBuilding(BUILD_MARKETPLACE, cur_pt));
    const Rect coordBuildingWel2(GetCoordBuilding(BUILD_WEL2, cur_pt));
    const Rect coordBuildingMoat(GetCoordBuilding(BUILD_MOAT, cur_pt));
    const Rect coordBuildingSpec(GetCoordBuilding(BUILD_SPEC, cur_pt));
    const Rect coordBuildingCastle(GetCoordBuilding(BUILD_CASTLE, cur_pt));
    const Rect coordBuildingCaptain(GetCoordBuilding(BUILD_CAPTAIN, cur_pt));
    const Rect coordBuildingTent(GetCoordBuilding(BUILD_TENT, cur_pt));

    const Rect coordDwellingMonster1(GetCoordBuilding(DWELLING_MONSTER1, cur_pt));
    const Rect coordDwellingMonster2(GetCoordBuilding(DWELLING_MONSTER2, cur_pt));
    const Rect coordDwellingMonster3(GetCoordBuilding(DWELLING_MONSTER3, cur_pt));
    const Rect coordDwellingMonster4(GetCoordBuilding(DWELLING_MONSTER4, cur_pt));
    const Rect coordDwellingMonster5(GetCoordBuilding(DWELLING_MONSTER5, cur_pt));
    const Rect coordDwellingMonster6(GetCoordBuilding(DWELLING_MONSTER6, cur_pt));


    // orders draw building
    std::vector<building_t> orders_building;
    switch(race)
    {
	case Race::KNGT:
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(BUILD_WELL);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(BUILD_SHIPYARD);
	    break;
	case Race::BARB:
	    orders_building.push_back(BUILD_SPEC);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(BUILD_WELL);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(BUILD_SHIPYARD);
	    break;
	case Race::SORC:
	    orders_building.push_back(BUILD_SPEC);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(BUILD_SHIPYARD);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(BUILD_WELL);
	    orders_building.push_back(DWELLING_MONSTER5);
	    break;
	case Race::WRLK:
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(BUILD_SHIPYARD);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(BUILD_SPEC);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_WELL);
	    break;
	case Race::WZRD:
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(BUILD_SHIPYARD);
	    orders_building.push_back(BUILD_WELL);
	    orders_building.push_back(BUILD_SPEC);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(BUILD_WEL2);
	    break;
	case Race::NECR:
	    orders_building.push_back(BUILD_SPEC);
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(BUILD_SHIPYARD);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(BUILD_WELL);
	    break;
	default: break;
    }

    // draw building
    RedrawAllBuilding(cur_pt, orders_building);
    RedrawNameTown(cur_pt);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Cursor::Show();

    display.Flip();

    Dialog::answer_t result = Dialog::ZERO;
    bool exit = false;

    u32 ticket = 0;

    // dialog menu loop
    while(! exit)
    {
        le.HandleEvents();

        le.MousePressLeft(buttonPrevCastle) ? buttonPrevCastle.Press() : buttonPrevCastle.Release();
        le.MousePressLeft(buttonNextCastle) ? buttonNextCastle.Press() : buttonNextCastle.Release();
        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();
	
	// castle troops
	for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii)
	{
	    if(le.MouseClickLeft(coordsCastleTroops[ii]))
	    {
		Cursor::Hide();

		// show dialog army info
		if(selectCastleTroops.isSelected() && army[ii].Valid() && selectCastleTroops.GetCursorIndex() == ii)
		{
		    Army::Troops & select_troops = army[ii];
		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();
		    Kingdom & kingdom = const_cast<Kingdom &>(world.GetMyKingdom());

		    switch(army[ii].ShowDialogInfo())
		    {
			case Dialog::UPGRADE:
			    select_troops.SetMonster(Monster::Upgrade(select_monster));
		            kingdom.OddFundsResource(PaymentConditions::UpgradeMonster(select_monster) * select_count);
			    Castle::RedrawResourcePanel();
			    break;

			case Dialog::DISMISS:
			    select_troops.SetMonster(Monster::UNKNOWN);
			    select_troops.SetCount(0);
			    break;

			default: break;
		    }
		    selectCastleTroops.Reset();
		    selectCastleTroops.Redraw();
		}
		else
		// change or combine army or move to empty
		if(selectCastleTroops.isSelected() || (heroes && selectHeroesTroops.isSelected()))
		{
		    // from castle or heroes
		    Army::Troops & select_troops = ( selectCastleTroops.isSelected() ? army[selectCastleTroops.GetCursorIndex()] :
			const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy())[selectHeroesTroops.GetCursorIndex()] );

		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();

		    // change or combine army
		    if(army[ii].Valid())
		    {
			// change to castle
			if(army[ii].GetMonster() != select_monster)
			{
			    select_troops = army[ii];
			    army[ii].SetMonster(select_monster);
			    army[ii].SetCount(select_count);
			}
			else
			// combine to castle
			if(selectCastleTroops.isSelected() || heroes && 1 < (*heroes).GetCountArmy())
			{
			    army[ii].SetCount(army[ii].GetCount() + select_count);
			    select_troops.SetMonster(Monster::UNKNOWN);
			    select_troops.SetCount(0);
			}

			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();

			if(heroes && selectHeroesTroops.isSelected())
			{
			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}
		    }
		    // move to empty position
		    else
		    if(selectCastleTroops.isSelected() || heroes && 1 < (*heroes).GetCountArmy())
		    {
			army[ii] = select_troops;
			select_troops.SetMonster(Monster::UNKNOWN);
			select_troops.SetCount(0);

			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();

			if(heroes && selectHeroesTroops.isSelected())
			{
			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}
		    }
		}
		else
		// select army
		if(army[ii].Valid())
		{
		    selectCastleTroops.Reset();
		    selectCastleTroops.Select(ii);
		    selectCastleTroops.Redraw();
		}

		Cursor::Show();

		display.Flip();
	    }
	    else
	    // right click empty troops - redistribute troops
	    if(le.MouseClickRight(coordsCastleTroops[ii]) &&
		!army[ii].Valid() &&
		(selectCastleTroops.isSelected() || (heroes && selectHeroesTroops.isSelected() && 1 < (*heroes).GetCountArmy())))
	    {
		Cursor::Hide();

		Army::Troops & select_troops = ( selectCastleTroops.isSelected() ? army[selectCastleTroops.GetCursorIndex()] :
		    const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy())[selectHeroesTroops.GetCursorIndex()] );

		if(const u16 redistr_count = Dialog::SelectCount(select_troops.GetCount()))
		{
		    army[ii].SetMonster(select_troops.GetMonster());
		    army[ii].SetCount(redistr_count);
		
		    select_troops.SetCount(select_troops.GetCount() - redistr_count);
		}

		Cursor::Hide();

		selectCastleTroops.Reset();
		selectCastleTroops.Redraw();

		if(heroes && selectHeroesTroops.isSelected())
		{
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Redraw();
		}

		Cursor::Show();

		display.Flip();
	    }
	    else
	    // press right - show quick info
	    if(le.MousePressRight(coordsCastleTroops[ii]) && army[ii].Valid())
	    {
		Cursor::Hide();

		army[ii].ShowDialogInfo(NULL, true);

		Cursor::Show();

		display.Flip();
	    }
	}

	// heroes troops
	if(heroes)
	{
	    std::vector<Army::Troops> & army2 = const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy());

	    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii) if(le.MouseClickLeft(coordsHeroesTroops[ii]))
	    {
		Cursor::Hide();

		// show dialog army info
		if(selectHeroesTroops.isSelected() && army2[ii].Valid() && selectHeroesTroops.GetCursorIndex() == ii)
		{
		    Army::Troops & select_troops = army2[ii];
		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();
		    Kingdom & kingdom = const_cast<Kingdom &>(world.GetMyKingdom());

		    switch(army2[ii].ShowDialogInfo(heroes))
		    {
			case Dialog::UPGRADE:
			    select_troops.SetMonster(Monster::Upgrade(select_monster));
		            kingdom.OddFundsResource(PaymentConditions::UpgradeMonster(select_monster) * select_count);
			    Castle::RedrawResourcePanel();
			    break;

			case Dialog::DISMISS:
			    select_troops.SetMonster(Monster::UNKNOWN);
			    select_troops.SetCount(0);
			    break;

			default: break;
		    }
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Redraw();
		}
		else
		// change or combine army or move to empty
		if(selectHeroesTroops.isSelected() || selectCastleTroops.isSelected())
		{
		    // from castle or heroes
		    Army::Troops & select_troops = ( selectCastleTroops.isSelected() ? army[selectCastleTroops.GetCursorIndex()] :
			army2[selectHeroesTroops.GetCursorIndex()] );

		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();

		    // change or combine army
		    if(army2[ii].Valid())
		    {
			// change to heroes
			if(army2[ii].GetMonster() != select_monster)
			{
			    select_troops = army2[ii];
			    army2[ii].SetMonster(select_monster);
			    army2[ii].SetCount(select_count);
			}
			else
			// combine to heroes
			{
			    army2[ii].SetCount(army2[ii].GetCount() + select_count);
			    select_troops.SetMonster(Monster::UNKNOWN);
			    select_troops.SetCount(0);
			}

			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();

			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		    // move to empty position
		    else
		    {
			army2[ii] = select_troops;
			select_troops.SetMonster(Monster::UNKNOWN);
			select_troops.SetCount(0);

			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();

			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		}
		else
		// select army
		if(army2[ii].Valid())
		{
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Select(ii);
		    selectHeroesTroops.Redraw();
		}

		Cursor::Show();

		display.Flip();
	    }
	    else
	    // right empty click - redistribute troops
	    if(le.MouseClickRight(coordsHeroesTroops[ii]) &&
	       !army2[ii].Valid() &&
	       (selectHeroesTroops.isSelected() || selectCastleTroops.isSelected()))
	    {
		Cursor::Hide();

		Army::Troops & select_troops = ( selectCastleTroops.isSelected() ? army[selectCastleTroops.GetCursorIndex()] :
		    army2[selectHeroesTroops.GetCursorIndex()] );

		if(const u16 redistr_count = Dialog::SelectCount(select_troops.GetCount()))
		{
		    army2[ii].SetMonster(select_troops.GetMonster());
		    army2[ii].SetCount(redistr_count);
		
		    select_troops.SetCount(select_troops.GetCount() - redistr_count);
		}

		Cursor::Hide();

		selectCastleTroops.Reset();
		selectCastleTroops.Redraw();

		selectHeroesTroops.Reset();
		selectHeroesTroops.Redraw();

		Cursor::Show();

		display.Flip();
	    }
	    else
	    // press right - show quick info
	    if(le.MousePressRight(coordsHeroesTroops[ii]) && army2[ii].Valid())
	    {
		Cursor::Hide();

		army2[ii].ShowDialogInfo(heroes, true);

		Cursor::Show();

		display.Flip();
	    }
	}

	// view hero
	if(isHeroesPresent() && le.MouseClickLeft(rectHeroPortrait))
	{
	    (*const_cast<Heroes *>(heroes)).OpenDialog();

	    Cursor::Hide();
	    selectHeroesTroops.Reset();
	    selectHeroesTroops.Redraw();
	    Cursor::Show();
	    display.Flip();
	}

        // prev castle
	if(le.MouseClickLeft(buttonPrevCastle)){ return Dialog::PREV; }

        // next castle
	if(le.MouseClickLeft(buttonNextCastle)){ return Dialog::NEXT; }

        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_ESCAPE)){ result = Dialog::CANCEL; exit = true; }

	// left click building
	if(building & BUILD_THIEVESGUILD && le.MouseClickLeft(coordBuildingThievesGuild)) OpenThievesGuild();
	if(building & BUILD_TAVERN && le.MouseClickLeft(coordBuildingTavern)) OpenTavern();
	if(building & BUILD_SHIPYARD && le.MouseClickLeft(coordBuildingShipyard)); // FIXME dialog buy boat
	if(building & BUILD_WELL && le.MouseClickLeft(coordBuildingWell))
	{
	    OpenWell();
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	if(building & BUILD_STATUE && le.MouseClickLeft(coordBuildingStatue)) Dialog::Message(GetStringBuilding(BUILD_STATUE), GetDescriptionBuilding(BUILD_STATUE), Font::BIG, Dialog::OK);
	if(building & BUILD_MARKETPLACE && le.MouseClickLeft(coordBuildingMarketplace)){ Dialog::Marketplace(); RedrawResourcePanel(); }
	if(building & BUILD_WEL2 && le.MouseClickLeft(coordBuildingWel2)) Dialog::Message(GetStringBuilding(BUILD_WEL2, race), GetDescriptionBuilding(BUILD_WEL2, race), Font::BIG, Dialog::OK);
	if(building & BUILD_MOAT && le.MouseClickLeft(coordBuildingMoat)) Dialog::Message(GetStringBuilding(BUILD_MOAT), GetDescriptionBuilding(BUILD_MOAT), Font::BIG, Dialog::OK);
	if(building & BUILD_SPEC && le.MouseClickLeft(coordBuildingSpec)) Dialog::Message(GetStringBuilding(BUILD_SPEC, race), GetDescriptionBuilding(BUILD_SPEC, race), Font::BIG, Dialog::OK);
	if(building & BUILD_CASTLE && le.MouseClickLeft(coordBuildingCastle))
	{
	    const building_t build = OpenTown();
	    
	    if(Castle::BUILD_NOTHING != build)
	    {
		Cursor::Hide();
		BuyBuilding(build);
		RedrawResourcePanel();
	        RedrawAllBuilding(cur_pt, orders_building);
	        RedrawNameTown(cur_pt);
		Cursor::Show();
		display.Flip();
	    }
	}
	else
	// buy castle
	if(building & BUILD_TENT && le.MouseClickLeft(coordBuildingTent) && Dialog::OK == DialogBuyBuilding(BUILD_CASTLE, true))
	{
		Cursor::Hide();
		BuyBuilding(BUILD_CASTLE);
		RedrawResourcePanel();
	        RedrawAllBuilding(cur_pt, orders_building);
	        RedrawNameTown(cur_pt);
		Cursor::Show();
		display.Flip();
	}
	// captain
	if(building & BUILD_CAPTAIN && le.MouseClickLeft(coordBuildingCaptain)) Dialog::Message(GetStringBuilding(BUILD_CAPTAIN), GetDescriptionBuilding(BUILD_CAPTAIN), Font::BIG, Dialog::OK);

	// left click mage guild
	if(building & (BUILD_MAGEGUILD5 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD1) &&
	    le.MouseClickLeft(GetCoordBuilding(BUILD_MAGEGUILD5, cur_pt))) OpenMageGuild();

	// left click dwelling monster
	if(building & DWELLING_MONSTER1 && le.MouseClickLeft(coordDwellingMonster1) &&
	    Castle::RecrutMonster(DWELLING_MONSTER1, Dialog::RecrutMonster(
		Monster::Monster(race, DWELLING_MONSTER1), dwelling[0])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	else
	if(building & DWELLING_MONSTER2 && le.MouseClickLeft(coordDwellingMonster2) &&
	    Castle::RecrutMonster(DWELLING_MONSTER2, Dialog::RecrutMonster(
		Monster::Monster(race, building & DWELLING_UPGRADE2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2), dwelling[1])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	else
	if(building & DWELLING_MONSTER3 && le.MouseClickLeft(coordDwellingMonster3) &&
	    Castle::RecrutMonster(DWELLING_MONSTER3, Dialog::RecrutMonster(
		Monster::Monster(race, building & DWELLING_UPGRADE3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3), dwelling[2])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	else
	if(building & DWELLING_MONSTER4 && le.MouseClickLeft(coordDwellingMonster4) &&
	    Castle::RecrutMonster(DWELLING_MONSTER4, Dialog::RecrutMonster(
		Monster::Monster(race, building & DWELLING_UPGRADE4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4), dwelling[3])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	else
	if(building & DWELLING_MONSTER5 && le.MouseClickLeft(coordDwellingMonster5) &&
	    Castle::RecrutMonster(DWELLING_MONSTER5, Dialog::RecrutMonster(
		Monster::Monster(race, building & DWELLING_UPGRADE5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5), dwelling[4])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	else
	if(building & DWELLING_MONSTER6 && le.MouseClickLeft(coordDwellingMonster6) &&
	    Castle::RecrutMonster(DWELLING_MONSTER6, Dialog::RecrutMonster(
	    Monster::Monster(race, building & DWELLING_UPGRADE7 ? DWELLING_UPGRADE7 : (building & DWELLING_UPGRADE6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6)), dwelling[5])))
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}

	// right press building
	if(building & BUILD_THIEVESGUILD && le.MousePressRight(coordBuildingThievesGuild)) Dialog::Message(GetStringBuilding(BUILD_THIEVESGUILD), GetDescriptionBuilding(BUILD_THIEVESGUILD), Font::BIG);
	if(building & BUILD_TAVERN && le.MousePressRight(coordBuildingTavern)) Dialog::Message(GetStringBuilding(BUILD_TAVERN), GetDescriptionBuilding(BUILD_TAVERN), Font::BIG);
	if(building & BUILD_SHIPYARD && le.MousePressRight(coordBuildingShipyard)) Dialog::Message(GetStringBuilding(BUILD_SHIPYARD), GetDescriptionBuilding(BUILD_SHIPYARD), Font::BIG);
	if(building & BUILD_WELL && le.MousePressRight(coordBuildingWell)) Dialog::Message(GetStringBuilding(BUILD_WELL), GetDescriptionBuilding(BUILD_WELL), Font::BIG);
	{
	    Cursor::Hide();
	    selectCastleTroops.Redraw();
	    RedrawResourcePanel();
	    Cursor::Show();
	    display.Flip();
	}
	if(building & BUILD_STATUE && le.MousePressRight(coordBuildingStatue)) Dialog::Message(GetStringBuilding(BUILD_STATUE), GetDescriptionBuilding(BUILD_STATUE), Font::BIG);
	if(building & BUILD_MARKETPLACE && le.MousePressRight(coordBuildingMarketplace)) Dialog::Message(GetStringBuilding(BUILD_MARKETPLACE), GetDescriptionBuilding(BUILD_MARKETPLACE), Font::BIG);
	if(building & BUILD_WEL2 && le.MousePressRight(coordBuildingWel2)) Dialog::Message(GetStringBuilding(BUILD_WEL2, race), GetDescriptionBuilding(BUILD_WEL2, race), Font::BIG);
	if(building & BUILD_MOAT && le.MousePressRight(coordBuildingMoat)) Dialog::Message(GetStringBuilding(BUILD_MOAT), GetDescriptionBuilding(BUILD_MOAT), Font::BIG);
	if(building & BUILD_SPEC && le.MousePressRight(coordBuildingSpec)) Dialog::Message(GetStringBuilding(BUILD_SPEC, race), GetDescriptionBuilding(BUILD_SPEC, race), Font::BIG);
	if(building & BUILD_CASTLE && le.MousePressRight(coordBuildingCastle)) Dialog::Message(GetStringBuilding(BUILD_CASTLE), GetDescriptionBuilding(BUILD_CASTLE), Font::BIG);
	else
	if(building & BUILD_TENT && le.MousePressRight(coordBuildingTent)) Dialog::Message(GetStringBuilding(BUILD_TENT), GetDescriptionBuilding(BUILD_TENT), Font::BIG);
	if(building & BUILD_CAPTAIN && le.MousePressRight(coordBuildingCaptain)) Dialog::Message(GetStringBuilding(BUILD_CAPTAIN), GetDescriptionBuilding(BUILD_CAPTAIN), Font::BIG);

	// right press mage guild
	if(building & (BUILD_MAGEGUILD5 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD1) &&
	    le.MousePressRight(GetCoordBuilding(BUILD_MAGEGUILD5, cur_pt))) Dialog::Message(GetStringBuilding(BUILD_MAGEGUILD1), GetDescriptionBuilding(BUILD_MAGEGUILD1), Font::BIG);

	// right press dwelling monster
	if(building & DWELLING_MONSTER1 && le.MousePressRight(coordDwellingMonster1))
	    Dialog::DwellingInfo(Monster::Monster(race, DWELLING_MONSTER1), dwelling[0]);
	else
	if(building & DWELLING_MONSTER2 && le.MousePressRight(coordDwellingMonster2))
	    Dialog::DwellingInfo(Monster::Monster(race, building & DWELLING_UPGRADE2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2), dwelling[1]);
	else
	if(building & DWELLING_MONSTER3 && le.MousePressRight(coordDwellingMonster3))
	    Dialog::DwellingInfo(Monster::Monster(race, building & DWELLING_UPGRADE3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3), dwelling[2]);
	else
	if(building & DWELLING_MONSTER4 && le.MousePressRight(coordDwellingMonster4))
	    Dialog::DwellingInfo(Monster::Monster(race, building & DWELLING_UPGRADE4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4), dwelling[3]);
	else
	if(building & DWELLING_MONSTER5 && le.MousePressRight(coordDwellingMonster5))
	    Dialog::DwellingInfo(Monster::Monster(race, building & DWELLING_UPGRADE5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5), dwelling[4]);
	else
	if(building & DWELLING_MONSTER6 && le.MousePressRight(coordDwellingMonster6))
	    Dialog::DwellingInfo(Monster::Monster(race, building & DWELLING_UPGRADE7 ? DWELLING_UPGRADE7 : (building & DWELLING_UPGRADE6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6)), dwelling[5]);

	// status message exit
	if(le.MouseCursor(buttonExit)) statusBar.ShowMessage(isCastle() ? "Exit castle" : "Exit town");
	else
	// status message prev castle
	if(le.MouseCursor(buttonPrevCastle)) statusBar.ShowMessage("Show previous town");
	else
	// status message next castle
	if(le.MouseCursor(buttonNextCastle)) statusBar.ShowMessage("Show next town");
	else
	// status message view hero
	if(isHeroesPresent() && le.MouseCursor(rectHeroPortrait)) statusBar.ShowMessage("View Hero");
	else
	// building
	if(building & BUILD_THIEVESGUILD && le.MouseCursor(coordBuildingThievesGuild)) statusBar.ShowMessage(GetStringBuilding(BUILD_THIEVESGUILD));
	else
	if(building & BUILD_TAVERN && le.MouseCursor(coordBuildingTavern)) statusBar.ShowMessage(GetStringBuilding(BUILD_TAVERN));
	else
	if(building & BUILD_SHIPYARD && le.MouseCursor(coordBuildingShipyard)) statusBar.ShowMessage(GetStringBuilding(BUILD_SHIPYARD));
	else
	if(building & BUILD_WELL && le.MouseCursor(coordBuildingWell)) statusBar.ShowMessage(GetStringBuilding(BUILD_WELL));
	else
	if(building & BUILD_STATUE && le.MouseCursor(coordBuildingStatue)) statusBar.ShowMessage(GetStringBuilding(BUILD_STATUE));
	else
	if(building & BUILD_MARKETPLACE && le.MouseCursor(coordBuildingMarketplace)) statusBar.ShowMessage(GetStringBuilding(BUILD_MARKETPLACE));
	else
	if(building & BUILD_WEL2 && le.MouseCursor(coordBuildingWel2)) statusBar.ShowMessage(GetStringBuilding(BUILD_WEL2, race));
	else
	if(building & BUILD_MOAT && le.MouseCursor(coordBuildingMoat)) statusBar.ShowMessage(GetStringBuilding(BUILD_MOAT));
	else
	if(building & BUILD_SPEC && le.MouseCursor(coordBuildingSpec)) statusBar.ShowMessage(GetStringBuilding(BUILD_SPEC, race));
	else
	if(building & BUILD_CASTLE && le.MouseCursor(coordBuildingCastle)) statusBar.ShowMessage(GetStringBuilding(BUILD_CASTLE));
	else
	if(building & BUILD_CAPTAIN && le.MouseCursor(coordBuildingCaptain)) statusBar.ShowMessage(GetStringBuilding(BUILD_CAPTAIN));
	else
	if(building & BUILD_TENT && le.MouseCursor(coordBuildingTent)) statusBar.ShowMessage(GetStringBuilding(BUILD_TENT));
	else
	// mage guild
	if(building & BUILD_MAGEGUILD5 && le.MouseCursor(GetCoordBuilding(BUILD_MAGEGUILD5, cur_pt))) statusBar.ShowMessage(GetStringBuilding(BUILD_MAGEGUILD5));
	else
	if(building & BUILD_MAGEGUILD4 && le.MouseCursor(GetCoordBuilding(BUILD_MAGEGUILD4, cur_pt))) statusBar.ShowMessage(GetStringBuilding(BUILD_MAGEGUILD4));
	else
	if(building & BUILD_MAGEGUILD3 && le.MouseCursor(GetCoordBuilding(BUILD_MAGEGUILD3, cur_pt))) statusBar.ShowMessage(GetStringBuilding(BUILD_MAGEGUILD3));
	else
	if(building & BUILD_MAGEGUILD2 && le.MouseCursor(GetCoordBuilding(BUILD_MAGEGUILD2, cur_pt))) statusBar.ShowMessage(GetStringBuilding(BUILD_MAGEGUILD2));
	else
	if(building & BUILD_MAGEGUILD1 && le.MouseCursor(GetCoordBuilding(BUILD_MAGEGUILD1, cur_pt))) statusBar.ShowMessage(GetStringBuilding(BUILD_MAGEGUILD1));
	else
	// dwelling monster
	if(building & DWELLING_MONSTER1 && le.MouseCursor(coordDwellingMonster1)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_MONSTER1)));
	else
	if(building & DWELLING_MONSTER2 && le.MouseCursor(coordDwellingMonster2)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_UPGRADE2 & building ? DWELLING_UPGRADE2 : DWELLING_MONSTER2)));
	else
	if(building & DWELLING_MONSTER3 && le.MouseCursor(coordDwellingMonster3)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_UPGRADE3 & building ? DWELLING_UPGRADE3 : DWELLING_MONSTER3)));
	else
	if(building & DWELLING_MONSTER4 && le.MouseCursor(coordDwellingMonster4)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_UPGRADE4 & building ? DWELLING_UPGRADE4 : DWELLING_MONSTER4)));
	else
	if(building & DWELLING_MONSTER5 && le.MouseCursor(coordDwellingMonster5)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_UPGRADE5 & building ? DWELLING_UPGRADE5 : DWELLING_MONSTER5)));
	else
	if(building & DWELLING_MONSTER6 && le.MouseCursor(coordDwellingMonster6)) statusBar.ShowMessage(Monster::String(Monster::Monster(race, DWELLING_UPGRADE6 & building ? DWELLING_UPGRADE6 : DWELLING_MONSTER6)));
	else
	// status message over castle troops
	if(le.MouseCursor(coordsCastleTroops[0]) ||
	   le.MouseCursor(coordsCastleTroops[1]) ||
	   le.MouseCursor(coordsCastleTroops[2]) ||
	   le.MouseCursor(coordsCastleTroops[3]) ||
	   le.MouseCursor(coordsCastleTroops[4]))
	{
	    const u8 castle_select = selectCastleTroops.GetCursorIndex();

	    for(u8 ii = 0; ii < coordsCastleTroops.size(); ++ii) if(le.MouseCursor(coordsCastleTroops[ii]))
	    {
		if(selectCastleTroops.isSelected() && ii == castle_select)
		    statusBar.ShowMessage("View " + Monster::String(army[ii].GetMonster()));
		else
		if(selectCastleTroops.isSelected() && army[ii].Valid() && army[castle_select].Valid())
		    army[castle_select].GetMonster() == army[ii].GetMonster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army[ii].GetMonster()) + " armies") :
			statusBar.ShowMessage("Exchange " + Monster::String(army[ii].GetMonster()) + " with " + Monster::String(army[castle_select].GetMonster()));
		else
		if(heroes && selectHeroesTroops.isSelected() && 1 == (*heroes).GetCountArmy())
			statusBar.ShowMessage("Cannot move last army to garrison");
		else
		if(heroes && selectHeroesTroops.isSelected() && army[ii].Valid() && (*heroes).GetArmy().at(selectHeroesTroops.GetCursorIndex()).Valid())
		    (*heroes).GetArmy().at(selectHeroesTroops.GetCursorIndex()).GetMonster() == army[ii].GetMonster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army[ii].GetMonster()) + " armies") : 
			statusBar.ShowMessage("Exchange " + Monster::String(army[ii].GetMonster()) + " with " + Monster::String((*heroes).GetArmy().at(selectHeroesTroops.GetCursorIndex()).GetMonster()));
		else
		if(selectCastleTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String(army[castle_select].GetMonster()));
		else
		if(heroes && selectHeroesTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String((*heroes).GetArmy().at(selectHeroesTroops.GetCursorIndex()).GetMonster()));
		else
		if(army[ii].Valid())
		    statusBar.ShowMessage("Select " + Monster::String(army[ii].GetMonster()));
		else
		    statusBar.ShowMessage("Empty");
	    }
	}
	else
	// status message over heroes troops
	if(heroes &&
	   (le.MouseCursor(coordsHeroesTroops[0]) ||
	    le.MouseCursor(coordsHeroesTroops[1]) ||
	    le.MouseCursor(coordsHeroesTroops[2]) ||
	    le.MouseCursor(coordsHeroesTroops[3]) ||
	    le.MouseCursor(coordsHeroesTroops[4])))
	{
	    const u8 heroes_select = selectHeroesTroops.GetCursorIndex();
	    const std::vector<Army::Troops> & army2 = (*heroes).GetArmy();

	    for(u8 ii = 0; ii < coordsHeroesTroops.size(); ++ii) if(le.MouseCursor(coordsHeroesTroops[ii]))
	    {

		if(selectHeroesTroops.isSelected() && ii == heroes_select)
		    statusBar.ShowMessage("View " + Monster::String(army2[ii].GetMonster()));
		else
		if(selectHeroesTroops.isSelected() && army2[ii].Valid() && army2[heroes_select].Valid())
		    army2[heroes_select].GetMonster() == army2[ii].GetMonster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army2[ii].GetMonster()) + " armies") :
			statusBar.ShowMessage("Exchange " + Monster::String(army2[ii].GetMonster()) + " with " + Monster::String(army2[heroes_select].GetMonster()));
		else
		if(selectCastleTroops.isSelected() && army2[ii].Valid() && army[selectCastleTroops.GetCursorIndex()].Valid())
		    army[selectCastleTroops.GetCursorIndex()].GetMonster() == army2[ii].GetMonster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army2[ii].GetMonster()) + " armies") :
			statusBar.ShowMessage("Exchange " + Monster::String(army2[ii].GetMonster()) + " with " + Monster::String(army[selectCastleTroops.GetCursorIndex()].GetMonster()));
		else
		if(selectHeroesTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String(army2[heroes_select].GetMonster()));
		else
		if(selectCastleTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String(army[selectCastleTroops.GetCursorIndex()].GetMonster()));
		else
		if(army2[ii].Valid())
		    statusBar.ShowMessage("Select " + Monster::String(army2[ii].GetMonster()));
		else
		    statusBar.ShowMessage("Empty");
	    }
	}
	else
	// clear all
	if(! statusBar.isEmpty()) statusBar.Clear();

	// animation sprite
	if(!(++ticket % 50)) // FIXME: speed animation low
	{
	    Cursor::Hide();
	    RedrawAllBuilding(cur_pt, orders_building);
	    RedrawNameTown(cur_pt);
	    Cursor::Show();
	    display.Flip();
	}
    }

    return result;
}

/* redraw resource info panel */
void Castle::RedrawResourcePanel(void)
{
    const Resource::funds_t & resource = world.GetMyKingdom().GetFundsResource();

    Point dst_pt;
    dst_pt.x = Display::SMALL == display.w() ? 0 : (display.w() - (640 + SHADOWWIDTH + 2 * BORDERWIDTH)) / 2 + BORDERWIDTH + SHADOWWIDTH;
    dst_pt.y = Display::SMALL == display.w() ? 0 : (display.h() - (480 + SHADOWWIDTH + 2 * BORDERWIDTH)) / 2 + BORDERWIDTH;

    Rect src_rt(dst_pt.x + 552, dst_pt.y + 262, 82, 192);
    display.FillRect(0, 0, 0, src_rt);

    std::string count;

    // sprite wood
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 10;
    const Sprite & wood = AGG::GetICN("RESOURCE.ICN", 0);
    display.Blit(wood, dst_pt);
    
    // count wood
    count.erase();
    String::AddInt(count, resource.wood);
    dst_pt.y += 22;
    Text(count, Font::SMALL, dst_pt.x + (wood.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite sulfur
    dst_pt.x = src_rt.x + 42;
    dst_pt.y = src_rt.y + 6;
    const Sprite & sulfur = AGG::GetICN("RESOURCE.ICN", 3);
    display.Blit(sulfur, dst_pt);
    
    // count sulfur
    count.erase();
    String::AddInt(count, resource.sulfur);
    dst_pt.y += 26;
    Text(count, Font::SMALL, dst_pt.x + (sulfur.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite crystal
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 45;
    const Sprite & crystal = AGG::GetICN("RESOURCE.ICN", 4);
    display.Blit(crystal, dst_pt);
    
    // count crystal
    count.erase();
    String::AddInt(count, resource.crystal);
    dst_pt.y += 33;
    Text(count, Font::SMALL, dst_pt.x + (crystal.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite mercury
    dst_pt.x = src_rt.x + 44;
    dst_pt.y = src_rt.y + 47;
    const Sprite & mercury = AGG::GetICN("RESOURCE.ICN", 1);
    display.Blit(mercury, dst_pt);
    
    // count mercury
    count.erase();
    String::AddInt(count, resource.mercury);
    dst_pt.y += 34;
    Text(count, Font::SMALL, dst_pt.x + (mercury.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite ore
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 92;
    const Sprite & ore = AGG::GetICN("RESOURCE.ICN", 2);
    display.Blit(ore, dst_pt);
    
    // count ore
    count.erase();
    String::AddInt(count, resource.ore);
    dst_pt.y += 26;
    Text(count, Font::SMALL, dst_pt.x + (ore.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite gems
    dst_pt.x = src_rt.x + 45;
    dst_pt.y = src_rt.y + 92;
    const Sprite & gems = AGG::GetICN("RESOURCE.ICN", 5);
    display.Blit(gems, dst_pt);
    
    // count gems
    count.erase();
    String::AddInt(count, resource.gems);
    dst_pt.y += 26;
    Text(count, Font::SMALL, dst_pt.x + (gems.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);

    // sprite gold
    dst_pt.x = src_rt.x + 6;
    dst_pt.y = src_rt.y + 130;
    const Sprite & gold = AGG::GetICN("RESOURCE.ICN", 6);
    display.Blit(gold, dst_pt);
    
    // count gold
    count.erase();
    String::AddInt(count, resource.gold);
    dst_pt.y += 24;
    Text(count, Font::SMALL, dst_pt.x + (gold.w() - Text::width(count, Font::SMALL)) / 2, dst_pt.y);
    
    // sprite button exit
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 166;
    const Sprite & exit = AGG::GetICN("SWAPBTN.ICN", 0);
    display.Blit(exit, dst_pt);
}
