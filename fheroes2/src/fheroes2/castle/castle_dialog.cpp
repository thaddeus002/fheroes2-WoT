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

#include <string>
#include <utility>
#include <algorithm>
#include "agg.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "settings.h"
#include "resource.h"
#include "castle.h"
#include "heroes.h"
#include "payment.h"
#include "profit.h"
#include "kingdom.h"
#include "tools.h"
#include "text.h"
#include "portrait.h"
#include "dialog.h"
#include "statusbar.h"
#include "selectarmybar.h"
#include "pocketpc.h"

void PackOrdersBuilding(const Castle & castle, std::vector<building_t> & orders_building)
{
    switch(castle.GetRace())
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
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
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
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
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
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
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
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
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
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(BUILD_WEL2);
	    break;
	case Race::NECR:
	    orders_building.push_back(BUILD_SPEC);
	    if(Settings::Get().PriceLoyaltyVersion()) orders_building.push_back(BUILD_TAVERN); // shrine
	    orders_building.push_back(BUILD_CASTLE);
	    orders_building.push_back(BUILD_CAPTAIN);
	    orders_building.push_back(BUILD_LEFTTURRET);
	    orders_building.push_back(BUILD_RIGHTTURRET);
	    orders_building.push_back(DWELLING_MONSTER6);
	    orders_building.push_back(BUILD_MOAT);
	    orders_building.push_back(DWELLING_MONSTER1);
	    orders_building.push_back(BUILD_SHIPYARD);
	    orders_building.push_back(BUILD_THIEVESGUILD);
	    orders_building.push_back(BUILD_TAVERN);
	    orders_building.push_back(DWELLING_MONSTER3);
	    orders_building.push_back(DWELLING_MONSTER5);
	    orders_building.push_back(DWELLING_MONSTER2);
	    orders_building.push_back(DWELLING_MONSTER4);
	    orders_building.push_back(BUILD_MAGEGUILD1);
	    orders_building.push_back(BUILD_MAGEGUILD2);
	    orders_building.push_back(BUILD_MAGEGUILD3);
	    orders_building.push_back(BUILD_MAGEGUILD4);
	    orders_building.push_back(BUILD_MAGEGUILD5);
	    orders_building.push_back(BUILD_WEL2);
	    orders_building.push_back(BUILD_MARKETPLACE);
	    orders_building.push_back(BUILD_STATUE);
	    orders_building.push_back(BUILD_WELL);
	    break;
	default: break;
    }
}

void RedrawIcons(const Castle & castle, const Heroes* hero1, const Heroes* hero2, const Point & pt)
{
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(ICN::STRIP, 0), pt.x, pt.y + 256);

    const Surface & sprite1 = hero1 ? Portrait::Hero(*hero1, Portrait::BIG) :
	AGG::GetICN(ICN::CREST, Color::GetIndex(castle.GetColor()));

    // icons 1
    display.Blit(sprite1, pt.x + 5, pt.y + 262);


    const Surface & sprite2 = hero2 ? Portrait::Hero(*hero2, Portrait::BIG) :
	(castle.isBuild(BUILD_CAPTAIN) && !hero1 ? Portrait::Captain(castle.GetRace(), Portrait::BIG) :
	    AGG::GetICN(ICN::STRIP, 3));

    // icons 2
    display.Blit(sprite2, pt.x + 5, pt.y + 361);

    // ext
    if(! hero2)
    display.Blit(AGG::GetICN(ICN::STRIP, 11), pt.x + 112, pt.y + 361);
}

Dialog::answer_t Castle::OpenDialog(bool readonly, bool fade)
{
    Settings & conf = Settings::Get();
    
    if(conf.QVGA()) return PocketPC::CastleOpenDialog(*this);

    const bool interface = conf.EvilInterface();
    if(conf.DynamicInterface())
    	conf.SetEvilInterface(GetRace() & (Race::BARB | Race::WRLK | Race::NECR));

    Display & display = Display::Get();

    Heroes* castle_guardians = world.GetHeroes(*this, true);
    Heroes* castle_heroes = world.GetHeroes(*this, false);

    if(castle_guardians && castle_heroes == castle_guardians)
	castle_heroes = NULL;

    // cursor
    Cursor & cursor = Cursor::Get();
    
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder background;
    background.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    background.Redraw();


    std::string msg_army, msg_date;

    // date string
    msg_date = _("Month: %{month}, Week: %{week}, Day: %{day}");
    String::Replace(msg_date, "%{month}", world.GetMonth());
    String::Replace(msg_date, "%{week}", world.GetWeek());
    String::Replace(msg_date, "%{day}", world.GetDay());


    // fade
    if(conf.ExtUseFade()) display.Fade();
        
    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    // button prev castle
    dst_pt.y += 480 - 19;
    Button buttonPrevCastle(dst_pt, ICN::SMALLBAR, 1, 2);

    // bottom small bar
    const Sprite & bar = AGG::GetICN(ICN::SMALLBAR, 0);
    dst_pt.x += buttonPrevCastle.w;
    display.Blit(bar, dst_pt);

    StatusBar statusBar;
    statusBar.SetFont(Font::BIG);
    statusBar.SetCenter(dst_pt.x + bar.w() / 2, dst_pt.y + 11);

    // button next castle
    dst_pt.x += bar.w();
    Button buttonNextCastle(dst_pt, ICN::SMALLBAR, 3, 4);

    // color crest
    const Sprite & crest = AGG::GetICN(ICN::CREST, Color::GetIndex(color));
    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 262;
    const Rect rectSign1(dst_pt, crest.w(), crest.h());

    RedrawIcons(*this, castle_guardians, castle_heroes, cur_pt);

    // castle troops selector
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 262;

    SelectArmyBar selectArmy1;
    if(castle_guardians)
    {
	selectArmy1.SetArmy(castle_guardians->GetArmy());
	selectArmy1.SetSaveLastTroop();
    }
    else
	selectArmy1.SetArmy(army);
    selectArmy1.SetPos(dst_pt);
    selectArmy1.SetInterval(6);
    selectArmy1.SetBackgroundSprite(AGG::GetICN(ICN::STRIP, 2));
    selectArmy1.SetCursorSprite(AGG::GetICN(ICN::STRIP, 1));
    selectArmy1.SetCastle(*this);
    selectArmy1.Redraw();

    // portrait heroes or captain or sign
    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 361;
    
    const Rect rectSign2(dst_pt.x, dst_pt.y, 100, 92);

    // castle_heroes troops background
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 361;

    SelectArmyBar selectArmy2;
    selectArmy2.SetPos(dst_pt);
    selectArmy2.SetInterval(6);
    selectArmy2.SetBackgroundSprite(AGG::GetICN(ICN::STRIP, 2));
    selectArmy2.SetCursorSprite(AGG::GetICN(ICN::STRIP, 1));
    selectArmy2.SetSaveLastTroop();
    selectArmy2.SetCastle(*this);

    if(castle_heroes)
    {
	castle_heroes->MovePointsScaleFixed();
        selectArmy2.SetArmy(castle_heroes->GetArmy());
        selectArmy2.Redraw();
    }
    
    // resource
    RedrawResourcePanel(cur_pt);

    // button exit
    dst_pt.x = cur_pt.x + 553;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, ICN::SWAPBTN, 0, 1);

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
    orders_building.reserve(25);

    PackOrdersBuilding(*this, orders_building);

    // update extra description
    payment_t profit;
    std::string description_well = GetDescriptionBuilding(BUILD_WELL, race);
    std::string description_wel2 = GetDescriptionBuilding(BUILD_WEL2, race);
    std::string description_castle = GetDescriptionBuilding(BUILD_CASTLE, race);
    std::string description_statue = GetDescriptionBuilding(BUILD_STATUE, race);
    std::string description_spec = GetDescriptionBuilding(BUILD_SPEC, race);
    String::Replace(description_well, "%{count}", grown_well);
    String::Replace(description_wel2, "%{count}", grown_wel2);
    profit = ProfitConditions::FromBuilding(BUILD_CASTLE, race);
    String::Replace(description_castle, "%{count}", profit.gold);
    profit = ProfitConditions::FromBuilding(BUILD_STATUE, race);
    String::Replace(description_statue, "%{count}", profit.gold);
    profit = ProfitConditions::FromBuilding(BUILD_SPEC, race);
    String::Replace(description_spec, "%{count}", profit.gold);

    // draw building
    RedrawAllBuilding(*this, cur_pt, orders_building);

    if(2 > world.GetMyKingdom().GetCastles().size() || readonly)
    {
	buttonPrevCastle.Press();
        buttonPrevCastle.SetDisable(true);

        buttonNextCastle.Press();
        buttonNextCastle.SetDisable(true);
    }

    buttonPrevCastle.Draw();
    buttonNextCastle.Draw();
    buttonExit.Draw();

    AGG::PlayMusic(MUS::FromRace(race));
    
    LocalEvent & le = LocalEvent::Get();

    cursor.Show();

    display.Flip();

    Dialog::answer_t result = Dialog::ZERO;

    bool army_redraw = false;

    // dialog menu loop
    while(le.HandleEvents())
    {
        // exit
	if(le.MouseClickLeft(buttonExit) || Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT)){ result = Dialog::CANCEL; break; }

        if(buttonPrevCastle.isEnable()) le.MousePressLeft(buttonPrevCastle) ? buttonPrevCastle.PressDraw() : buttonPrevCastle.ReleaseDraw();
        if(buttonNextCastle.isEnable()) le.MousePressLeft(buttonNextCastle) ? buttonNextCastle.PressDraw() : buttonNextCastle.ReleaseDraw();

        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

       // selector troops event
        if(castle_heroes && selectArmy2.isValid())
        {
            if(le.MouseCursor(selectArmy1.GetArea()) || le.MouseCursor(selectArmy2.GetArea()))
	    {
		msg_army = msg_date;

                if(SelectArmyBar::QueueEventProcessing(selectArmy1, selectArmy2, &msg_army))
		    RedrawResourcePanel(cur_pt);
    	    }
        }
	else
        {
            if(le.MouseCursor(selectArmy1.GetArea()))
	    {
		msg_army = msg_date;

                if(SelectArmyBar::QueueEventProcessing(selectArmy1, &msg_army))
		    RedrawResourcePanel(cur_pt);
	    }
	}

	// move hero to guardian
	if(conf.ExtAllowCastleGuardians() && !readonly && castle_heroes && !castle_guardians && le.MouseClickLeft(rectSign1))
	{
	    if(! castle_heroes->GetArmy().CanJoinArmy(army))
	    {
		// FIXME: correct message
		Dialog::Message("Join Error", "Army is full", Font::BIG, Dialog::OK);
	    }
	    else
	    {
		castle_heroes->SetModes(Heroes::GUARDIAN);
		castle_heroes->ResetModes(Heroes::SLEEPER);
		castle_guardians = castle_heroes;
		castle_guardians->GetPath().Reset();
		castle_guardians->GetArmy().JoinArmy(army);

		castle_heroes = NULL;

		world.GetTiles(center).SetObject(MP2::OBJ_CASTLE);
		castle_guardians->SaveUnderObject(MP2::OBJ_ZERO);

		// free position
		Point position(castle_guardians->GetCenter());
		position.y -= 1;
		castle_guardians->SetCenter(position);

		cursor.Hide();
        	if(selectArmy1.isSelected()) selectArmy1.Reset();
        	if(selectArmy2.isSelected()) selectArmy2.Reset();
		selectArmy2.ResetArmy();
		selectArmy1.SetArmy(castle_guardians->GetArmy());
		selectArmy1.SetSaveLastTroop();
		RedrawIcons(*this, castle_guardians, castle_heroes, cur_pt);
        	selectArmy2.Redraw();
        	selectArmy1.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}

	// view guardian
	if(!readonly && castle_guardians && le.MouseClickLeft(rectSign1))
	{
	    Game::OpenHeroesDialog(castle_guardians);

	    cursor.Hide();
            if(selectArmy1.isSelected()) selectArmy1.Reset();
            if(selectArmy2.isSelected()) selectArmy2.Reset();
            selectArmy2.Redraw();
	    cursor.Show();
	    display.Flip();
	}

	// move guardian to hero
	if(conf.ExtAllowCastleGuardians() && !readonly && !castle_heroes && castle_guardians && le.MouseClickLeft(rectSign2))
	{
	    castle_guardians->ResetModes(Heroes::GUARDIAN);
	    castle_heroes = castle_guardians;

	    castle_guardians = NULL;

	    // restore position
	    Point position(castle_heroes->GetCenter());
	    position.y += 1;
	    castle_heroes->SetCenter(position);

	    world.GetTiles(center).SetObject(MP2::OBJ_HEROES);
	    castle_heroes->SaveUnderObject(MP2::OBJ_CASTLE);

	    cursor.Hide();
    	    if(selectArmy1.isSelected()) selectArmy1.Reset();
    	    if(selectArmy2.isSelected()) selectArmy2.Reset();
	    selectArmy1.ResetArmy();
	    selectArmy1.SetArmy(army);
	    selectArmy2.SetArmy(castle_heroes->GetArmy());
	    selectArmy2.SetSaveLastTroop();
	    RedrawIcons(*this, castle_guardians, castle_heroes, cur_pt);
    	    selectArmy1.Redraw();
    	    selectArmy2.Redraw();
	    cursor.Show();
	    display.Flip();
	}

	// view hero
	if(!readonly && castle_heroes && le.MouseClickLeft(rectSign2))
	{
	    Game::OpenHeroesDialog(castle_heroes);

	    cursor.Hide();
            if(selectArmy1.isSelected()) selectArmy1.Reset();
            if(selectArmy2.isSelected()) selectArmy2.Reset();
            selectArmy2.Redraw();
	    cursor.Show();
	    display.Flip();
	}

        // prev castle
	if(buttonPrevCastle.isEnable() && le.MouseClickLeft(buttonPrevCastle)){ result = Dialog::PREV; break; }
	else
        // next castle
	if(buttonNextCastle.isEnable() && le.MouseClickLeft(buttonNextCastle)){ result = Dialog::NEXT; break; }
	else
	// left click building
	if((building & BUILD_THIEVESGUILD) && le.MouseClickLeft(coordBuildingThievesGuild))
	    Dialog::ThievesGuild(false);
	else
	if((building & BUILD_TAVERN) && le.MouseClickLeft(coordBuildingTavern))
	{
	    if(Race::NECR == race && conf.PriceLoyaltyVersion())
		Dialog::Message(GetStringBuilding(BUILD_TAVERN, race), GetDescriptionBuilding(BUILD_TAVERN, race), Font::BIG, Dialog::OK);
	    else
		OpenTavern();
	}
	else
	if(!readonly && (building & BUILD_SHIPYARD) && le.MouseClickLeft(coordBuildingShipyard))
	{
	    cursor.Hide();

	    if(Dialog::OK == Dialog::BuyBoat(AllowBuyBoat()))
	    {
		BuyBoat();
		RedrawResourcePanel(cur_pt);
    		// RedrawResourcePanel destroy sprite buttonExit
		if(buttonExit.isPressed()) buttonExit.Draw();
	    }
	    cursor.Show();
	    display.Flip();
	}
	else
	if((building & BUILD_WELL) && le.MouseClickLeft(coordBuildingWell))
	{
	    if(readonly)
		Dialog::Message(GetStringBuilding(BUILD_WELL, race), description_well, Font::BIG, Dialog::OK);
	    else
	    {
		OpenWell();
		army_redraw = true;
	    }
	}
	else
	if((building & BUILD_STATUE) && le.MouseClickLeft(coordBuildingStatue))
	    Dialog::Message(GetStringBuilding(BUILD_STATUE), description_statue, Font::BIG, Dialog::OK);
	else
	if((building & BUILD_MARKETPLACE) && le.MouseClickLeft(coordBuildingMarketplace))
	{
	    if(readonly)
		Dialog::Message(GetStringBuilding(BUILD_MARKETPLACE, race), GetDescriptionBuilding(BUILD_MARKETPLACE), Font::BIG, Dialog::OK);
	    else
	    {
		cursor.Hide();
		Dialog::Marketplace();
		cursor.Show();
		display.Flip();
		RedrawResourcePanel(cur_pt);
		// RedrawResourcePanel destroy sprite buttonExit
		if(buttonExit.isPressed()) buttonExit.Draw();
	    }
	}
	else
	if((building & BUILD_WEL2) && le.MouseClickLeft(coordBuildingWel2))
	    Dialog::Message(GetStringBuilding(BUILD_WEL2, race), description_wel2, Font::BIG, Dialog::OK);
	else
	if((building & BUILD_MOAT) && le.MouseClickLeft(coordBuildingMoat))
	    Dialog::Message(GetStringBuilding(BUILD_MOAT), GetDescriptionBuilding(BUILD_MOAT), Font::BIG, Dialog::OK);
	else
	if((building & BUILD_SPEC) && le.MouseClickLeft(coordBuildingSpec))
	    Dialog::Message(GetStringBuilding(BUILD_SPEC, race), description_spec, Font::BIG, Dialog::OK);
	else
	if(readonly &&
	    (((building & BUILD_CASTLE) && le.MouseClickLeft(coordBuildingCastle)) ||
	     (!(building & BUILD_CASTLE) && le.MouseClickLeft(coordBuildingTent))))
		Dialog::Message(GetStringBuilding(BUILD_CASTLE, race), description_castle, Font::BIG, Dialog::OK);
	else
	if(building & BUILD_CASTLE && le.MouseClickLeft(coordBuildingCastle))
	{
	    const Heroes * prev = castle_heroes;
	    const u32 build = OpenTown();
	    const bool buyhero = ((castle_heroes != prev) && (castle_heroes != NULL));

	    if(BUILD_NOTHING != build)
	    {
		cursor.Hide();

                if(selectArmy1.isSelected()) selectArmy1.Reset();

		// play sound
    		AGG::PlaySound(M82::BUILDTWN);

		RedrawAnimationBuilding(*this, cur_pt, orders_building, build);
		BuyBuilding(build);
		RedrawResourcePanel(cur_pt);

		if(BUILD_CAPTAIN == build && ! castle_heroes && ! castle_guardians)
		    display.Blit(Portrait::Captain(race, Portrait::BIG), cur_pt.x + 5, cur_pt.y + 361);

    		// RedrawResourcePanel destroy sprite buttonExit
		if(buttonExit.isPressed()) buttonExit.Draw();

		cursor.Show();
		display.Flip();
	    }

	    if(castle_heroes)
	    {
		cursor.Hide();

                if(selectArmy2.isSelected()) selectArmy2.Reset();

		if(buyhero)
		{
		    const Rect rt(0, 98, 552, 107);
		    Surface sf(rt.w, rt.h);
		    sf.SetColorKey();
            	    sf.Blit(AGG::GetICN(ICN::STRIP, 0), rt, 0, 0);
		    const Surface & port = Portrait::Hero((*castle_heroes), Portrait::BIG);
		    sf.Blit(port, 6, 6);
            	    selectArmy2.SetArmy(castle_heroes->GetArmy());
		    selectArmy2.SetPos(112, 5);
		    selectArmy2.Redraw(sf);
		    selectArmy2.SetPos(cur_pt.x + 112, cur_pt.y + 361);

		    AGG::PlaySound(M82::BUILDTWN);
		    RedrawResourcePanel(cur_pt);

		    LocalEvent & le = LocalEvent::Get();
		    u8 alpha = 0;

		    while(le.HandleEvents() && alpha < 250)
		    {
    			if(Game::AnimateInfrequent(Game::CASTLE_BUYHERO_DELAY))
    			{
    			    cursor.Hide();
        		    sf.SetAlpha(alpha);
        		    display.Blit(sf, cur_pt.x, cur_pt.y + 356);
    			    cursor.Show();
        		    display.Flip();
        		    alpha += 10;
    			}
		    }

		    cursor.Hide();
            	    display.Blit(AGG::GetICN(ICN::STRIP, 0), cur_pt.x, cur_pt.y + 256);
            	    display.Blit(Portrait::Hero((*castle_heroes), Portrait::BIG), cur_pt.x + 5, cur_pt.y + 361);
            	    selectArmy2.Redraw();
		}

		cursor.Show();
		display.Flip();
	    }
	}
	else
	// buy castle
	if(!(building & BUILD_CASTLE) && le.MouseClickLeft(coordBuildingTent))
	{
	    if(!Modes(ALLOWCASTLE))
	    {
		Dialog::Message(_("Town"), _("This town may not be upgraded to a castle."), Font::BIG, Dialog::OK);
	    }
	    else
	    if(Dialog::OK == DialogBuyCastle(true))
	    {
		cursor.Hide();

		// play sound
    		AGG::PlaySound(M82::BUILDTWN);

		RedrawAnimationBuilding(*this, cur_pt, orders_building, BUILD_CASTLE);
		BuyBuilding(BUILD_CASTLE);
		RedrawResourcePanel(cur_pt);

		// RedrawResourcePanel destroy sprite buttonExit
		if(buttonExit.isPressed()) buttonExit.Draw();

		cursor.Show();
		display.Flip();
	    }
	}
	else
	// captain
	if((building & BUILD_CAPTAIN) && le.MouseClickLeft(coordBuildingCaptain))
	    Dialog::Message(GetStringBuilding(BUILD_CAPTAIN), GetDescriptionBuilding(BUILD_CAPTAIN), Font::BIG, Dialog::OK);
	else
	// left click mage guild
	if((building & (BUILD_MAGEGUILD5 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD1)) &&
	    le.MouseClickLeft(GetCoordBuilding(BUILD_MAGEGUILD5, cur_pt)))
	{
		// buy spell book
		if(!castle_heroes || (*castle_heroes).HasArtifact(Artifact::MAGIC_BOOK))
		    OpenMageGuild();
		else
		if((*castle_heroes).BuySpellBook(&mageguild))
		    army_redraw = true;
	}
	else
	// left click dwelling monster
	if(!readonly && (building & DWELLING_MONSTER1) && le.MouseClickLeft(coordDwellingMonster1) &&
	    Castle::RecruitMonster(DWELLING_MONSTER1, Dialog::RecruitMonster(Monster(race, DWELLING_MONSTER1), dwelling[0])))
		army_redraw = true;
	else
	if(!readonly && (building & DWELLING_MONSTER2) && le.MouseClickLeft(coordDwellingMonster2) &&
	    Castle::RecruitMonster(DWELLING_MONSTER2, Dialog::RecruitMonster(Monster(race, GetActualDwelling(DWELLING_MONSTER2)), dwelling[1])))
		army_redraw = true;
	else
	if(!readonly && (building & DWELLING_MONSTER3) && le.MouseClickLeft(coordDwellingMonster3) &&
	    Castle::RecruitMonster(DWELLING_MONSTER3, Dialog::RecruitMonster(Monster(race, GetActualDwelling(DWELLING_MONSTER3)), dwelling[2])))
		army_redraw = true;
	else
	if(!readonly && (building & DWELLING_MONSTER4) && le.MouseClickLeft(coordDwellingMonster4) &&
	    Castle::RecruitMonster(DWELLING_MONSTER4, Dialog::RecruitMonster(Monster(race, GetActualDwelling(DWELLING_MONSTER4)), dwelling[3])))
		army_redraw = true;
	else
	if(!readonly && (building & DWELLING_MONSTER5) && le.MouseClickLeft(coordDwellingMonster5) &&
	    Castle::RecruitMonster(DWELLING_MONSTER5, Dialog::RecruitMonster(Monster(race, GetActualDwelling(DWELLING_MONSTER5)), dwelling[4])))
		army_redraw = true;
	else
	if(!readonly && (building & DWELLING_MONSTER6) && le.MouseClickLeft(coordDwellingMonster6) &&
	    Castle::RecruitMonster(DWELLING_MONSTER6, Dialog::RecruitMonster(Monster(race, GetActualDwelling(DWELLING_MONSTER6)), dwelling[5])))
		army_redraw = true;

	if(army_redraw)
	{
	    cursor.Hide();
	    selectArmy1.Redraw();
	    RedrawResourcePanel(cur_pt);
	    // RedrawResourcePanel destroy sprite buttonExit
	    if(buttonExit.isPressed()) buttonExit.Draw();
	    cursor.Show();
	    display.Flip();
	    army_redraw = false;
	}

	// right press building
	if(building & BUILD_THIEVESGUILD && le.MousePressRight(coordBuildingThievesGuild)) Dialog::Message(GetStringBuilding(BUILD_THIEVESGUILD), GetDescriptionBuilding(BUILD_THIEVESGUILD), Font::BIG);
	else
	if(building & BUILD_TAVERN && le.MousePressRight(coordBuildingTavern)) Dialog::Message(GetStringBuilding(BUILD_TAVERN, race), GetDescriptionBuilding(BUILD_TAVERN, race), Font::BIG);
	else
	if(building & BUILD_SHIPYARD && le.MousePressRight(coordBuildingShipyard)) Dialog::Message(GetStringBuilding(BUILD_SHIPYARD), GetDescriptionBuilding(BUILD_SHIPYARD), Font::BIG);
	else
	if(building & BUILD_WELL && le.MousePressRight(coordBuildingWell)) Dialog::Message(GetStringBuilding(BUILD_WELL), description_well, Font::BIG);
	else
	if(building & BUILD_STATUE && le.MousePressRight(coordBuildingStatue)) Dialog::Message(GetStringBuilding(BUILD_STATUE), description_statue, Font::BIG);
	else
	if(building & BUILD_MARKETPLACE && le.MousePressRight(coordBuildingMarketplace)) Dialog::Message(GetStringBuilding(BUILD_MARKETPLACE), GetDescriptionBuilding(BUILD_MARKETPLACE), Font::BIG);
	else
	if(building & BUILD_WEL2 && le.MousePressRight(coordBuildingWel2)) Dialog::Message(GetStringBuilding(BUILD_WEL2, race), description_wel2, Font::BIG);
	else
	if(building & BUILD_MOAT && le.MousePressRight(coordBuildingMoat)) Dialog::Message(GetStringBuilding(BUILD_MOAT), GetDescriptionBuilding(BUILD_MOAT), Font::BIG);
	else
	if(building & BUILD_SPEC && le.MousePressRight(coordBuildingSpec)) Dialog::Message(GetStringBuilding(BUILD_SPEC, race), description_spec, Font::BIG);
	else
	if(building & BUILD_CASTLE && le.MousePressRight(coordBuildingCastle)) Dialog::Message(GetStringBuilding(BUILD_CASTLE), description_castle, Font::BIG);
	else
	if(!(building & BUILD_CASTLE) && le.MousePressRight(coordBuildingTent)) Dialog::Message(GetStringBuilding(BUILD_TENT), GetDescriptionBuilding(BUILD_TENT), Font::BIG);
	else
	if(building & BUILD_CAPTAIN && le.MousePressRight(coordBuildingCaptain)) Dialog::Message(GetStringBuilding(BUILD_CAPTAIN), GetDescriptionBuilding(BUILD_CAPTAIN), Font::BIG);
	else
	// right press mage guild
	if(building & (BUILD_MAGEGUILD5 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD1) &&
	    le.MousePressRight(GetCoordBuilding(BUILD_MAGEGUILD5, cur_pt))) Dialog::Message(GetStringBuilding(BUILD_MAGEGUILD1), GetDescriptionBuilding(BUILD_MAGEGUILD1), Font::BIG);
	else
	// right press dwelling monster
	if(building & DWELLING_MONSTER1 && le.MousePressRight(coordDwellingMonster1))
	    Dialog::DwellingInfo(Monster(race, DWELLING_MONSTER1), dwelling[0]);
	else
	if(building & DWELLING_MONSTER2 && le.MousePressRight(coordDwellingMonster2))
	    Dialog::DwellingInfo(Monster(race, GetActualDwelling(DWELLING_MONSTER2)), dwelling[1]);
	else
	if(building & DWELLING_MONSTER3 && le.MousePressRight(coordDwellingMonster3))
	    Dialog::DwellingInfo(Monster(race, GetActualDwelling(DWELLING_MONSTER3)), dwelling[2]);
	else
	if(building & DWELLING_MONSTER4 && le.MousePressRight(coordDwellingMonster4))
	    Dialog::DwellingInfo(Monster(race, GetActualDwelling(DWELLING_MONSTER4)), dwelling[3]);
	else
	if(building & DWELLING_MONSTER5 && le.MousePressRight(coordDwellingMonster5))
	    Dialog::DwellingInfo(Monster(race, GetActualDwelling(DWELLING_MONSTER5)), dwelling[4]);
	else
	if(building & DWELLING_MONSTER6 && le.MousePressRight(coordDwellingMonster6))
	    Dialog::DwellingInfo(Monster(race, GetActualDwelling(DWELLING_MONSTER6)), dwelling[5]);

	// status armybar
        if((le.MouseCursor(selectArmy1.GetArea()) || le.MouseCursor(selectArmy2.GetArea())) &&
	    castle_heroes)
	{
	    statusBar.ShowMessage(msg_army);
	}
	else
        if(le.MouseCursor(selectArmy1.GetArea()))
	{
	    statusBar.ShowMessage(msg_army);
	}
	else
	// status message exit
	if(le.MouseCursor(buttonExit)) statusBar.ShowMessage(isCastle() ? _("Exit castle") : _("Exit town"));
	else
	// status message prev castle
	if(le.MouseCursor(buttonPrevCastle)) statusBar.ShowMessage(_("Show previous town"));
	else
	// status message next castle
	if(le.MouseCursor(buttonNextCastle)) statusBar.ShowMessage(_("Show next town"));
	else
	// status message over sign
	if(castle_guardians && le.MouseCursor(rectSign1)) statusBar.ShowMessage(_("View Hero"));
	else
	// status message view hero
	if(castle_heroes && le.MouseCursor(rectSign2)) statusBar.ShowMessage(_("View Hero"));
	else
	// building
	if(building & BUILD_THIEVESGUILD && le.MouseCursor(coordBuildingThievesGuild)) statusBar.ShowMessage(GetStringBuilding(BUILD_THIEVESGUILD));
	else
	if(building & BUILD_TAVERN && le.MouseCursor(coordBuildingTavern)) statusBar.ShowMessage(GetStringBuilding(BUILD_TAVERN, race));
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
	if(!(building & BUILD_CASTLE) && le.MouseCursor(coordBuildingTent)) statusBar.ShowMessage(GetStringBuilding(BUILD_TENT));
	else
	if(building & BUILD_CAPTAIN && le.MouseCursor(coordBuildingCaptain)) statusBar.ShowMessage(GetStringBuilding(BUILD_CAPTAIN));
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
	if(building & DWELLING_MONSTER1 && le.MouseCursor(coordDwellingMonster1)) statusBar.ShowMessage(Monster(race, DWELLING_MONSTER1).GetName());
	else
	if(building & DWELLING_MONSTER2 && le.MouseCursor(coordDwellingMonster2)) statusBar.ShowMessage(Monster(race, DWELLING_UPGRADE2 & building ? DWELLING_UPGRADE2 : DWELLING_MONSTER2).GetName());
	else
	if(building & DWELLING_MONSTER3 && le.MouseCursor(coordDwellingMonster3)) statusBar.ShowMessage(Monster(race, DWELLING_UPGRADE3 & building ? DWELLING_UPGRADE3 : DWELLING_MONSTER3).GetName());
	else
	if(building & DWELLING_MONSTER4 && le.MouseCursor(coordDwellingMonster4)) statusBar.ShowMessage(Monster(race, DWELLING_UPGRADE4 & building ? DWELLING_UPGRADE4 : DWELLING_MONSTER4).GetName());
	else
	if(building & DWELLING_MONSTER5 && le.MouseCursor(coordDwellingMonster5)) statusBar.ShowMessage(Monster(race, DWELLING_UPGRADE5 & building ? DWELLING_UPGRADE5 : DWELLING_MONSTER5).GetName());
	else
	if(building & DWELLING_MONSTER6 && le.MouseCursor(coordDwellingMonster6)) statusBar.ShowMessage(Monster(race, DWELLING_UPGRADE6 & building ? (DWELLING_UPGRADE7 & building ? DWELLING_UPGRADE7 : DWELLING_UPGRADE6) : DWELLING_MONSTER6).GetName());
	else
	{
	    statusBar.ShowMessage(msg_date);
	}

	// animation sprite
	if(Game::AnimateInfrequent(Game::CASTLE_AROUND_DELAY))
	{
	    cursor.Hide();
	    RedrawAllBuilding(*this, cur_pt, orders_building);
	    cursor.Show();
	    display.Flip();
	}
    }

    if(castle_heroes && conf.ExtHeroRecalculateMovement())
	castle_heroes->RecalculateMovePoints();

    if(conf.DynamicInterface())
	conf.SetEvilInterface(interface);

    return result;
}

/* redraw resource info panel */
void Castle::RedrawResourcePanel(const Point & pt)
{
    Display & display = Display::Get();
    const Resource::funds_t & resource = world.GetMyKingdom().GetFundsResource();

    Point dst_pt = pt;

    Rect src_rt(dst_pt.x + 552, dst_pt.y + 262, 82, 192);
    display.FillRect(0, 0, 0, src_rt);

    std::string count;
    Text text;

    // sprite wood
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 10;
    const Sprite & wood = AGG::GetICN(ICN::RESOURCE, 0);
    display.Blit(wood, dst_pt);
    
    // count wood
    count.erase();
    String::AddInt(count, resource.wood);
    text.Set(count, Font::SMALL);
    dst_pt.y += 22;
    text.Blit(dst_pt.x + (wood.w() - text.w()) / 2, dst_pt.y);

    // sprite sulfur
    dst_pt.x = src_rt.x + 42;
    dst_pt.y = src_rt.y + 6;
    const Sprite & sulfur = AGG::GetICN(ICN::RESOURCE, 3);
    display.Blit(sulfur, dst_pt);
    
    // count sulfur
    count.erase();
    String::AddInt(count, resource.sulfur);
    text.Set(count);
    dst_pt.y += 26;
    text.Blit(dst_pt.x + (sulfur.w() - text.w()) / 2, dst_pt.y);

    // sprite crystal
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 45;
    const Sprite & crystal = AGG::GetICN(ICN::RESOURCE, 4);
    display.Blit(crystal, dst_pt);
    
    // count crystal
    count.erase();
    String::AddInt(count, resource.crystal);
    text.Set(count);
    dst_pt.y += 33;
    text.Blit(dst_pt.x + (crystal.w() - text.w()) / 2, dst_pt.y);

    // sprite mercury
    dst_pt.x = src_rt.x + 44;
    dst_pt.y = src_rt.y + 47;
    const Sprite & mercury = AGG::GetICN(ICN::RESOURCE, 1);
    display.Blit(mercury, dst_pt);
    
    // count mercury
    count.erase();
    String::AddInt(count, resource.mercury);
    text.Set(count);
    dst_pt.y += 34;
    text.Blit(dst_pt.x + (mercury.w() - text.w()) / 2, dst_pt.y);

    // sprite ore
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 92;
    const Sprite & ore = AGG::GetICN(ICN::RESOURCE, 2);
    display.Blit(ore, dst_pt);
    
    // count ore
    count.erase();
    String::AddInt(count, resource.ore);
    text.Set(count);
    dst_pt.y += 26;
    text.Blit(dst_pt.x + (ore.w() - text.w()) / 2, dst_pt.y);

    // sprite gems
    dst_pt.x = src_rt.x + 45;
    dst_pt.y = src_rt.y + 92;
    const Sprite & gems = AGG::GetICN(ICN::RESOURCE, 5);
    display.Blit(gems, dst_pt);
    
    // count gems
    count.erase();
    String::AddInt(count, resource.gems);
    text.Set(count);
    dst_pt.y += 26;
    text.Blit(dst_pt.x + (gems.w() - text.w()) / 2, dst_pt.y);

    // sprite gold
    dst_pt.x = src_rt.x + 6;
    dst_pt.y = src_rt.y + 130;
    const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
    display.Blit(gold, dst_pt);
    
    // count gold
    count.erase();
    String::AddInt(count, resource.gold);
    text.Set(count);
    dst_pt.y += 24;
    text.Blit(dst_pt.x + (gold.w() - text.w()) / 2, dst_pt.y);
    
    // sprite button exit
    dst_pt.x = src_rt.x + 1;
    dst_pt.y = src_rt.y + 166;
    const Sprite & exit = AGG::GetICN(ICN::SWAPBTN, 0);
    display.Blit(exit, dst_pt);
}
