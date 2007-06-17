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
#include <vector>
#include "agg.h"
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "castle.h"
#include "dialog.h"
#include "localevent.h"
#include "background.h"
#include "tools.h"
#include "text.h"

Dialog::answer_t Castle::DialogBuyBuilding(building_t build, bool buttons)
{
    const std::string &system = (H2Config::EvilInterface() ? "SYSTEME.ICN" : "SYSTEM.ICN");

    Cursor::Hide();

    Dialog::Box box(200, buttons);

    //Rect pos = box.GetArea();

    std::string icnsprite;

    switch(race)
    {
        case Race::KNGT: icnsprite = "CSTLKNGT.ICN"; break;
        case Race::BARB: icnsprite = "CSTLBARB.ICN"; break;
        case Race::SORC: icnsprite = "CSTLSORC.ICN"; break;
        case Race::WRLK: icnsprite = "CSTLWRLK.ICN"; break;
        case Race::WZRD: icnsprite = "CSTLWZRD.ICN"; break;
        case Race::NECR: icnsprite = "CSTLNECR.ICN"; break;
        default: return Dialog::CANCEL;
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button *button1 = NULL;
    Button *button2 = NULL;

    Point pt;
    if(buttons)
    {
	pt.x = box.GetArea().x;
        pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
        button1 = new Button(pt, system, 1, 2);
        pt.x = box.GetArea().x + box.GetArea().w - AGG::GetICN(system, 3).w();
        pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
        button2 = new Button(pt, system, 3, 4);
    }

    display.Flip();
    Cursor::Show();

    le.ResetKey();

    // message loop
    bool exit = false;
    Dialog::answer_t result = Dialog::ZERO;

    while(!exit)
    {
        le.HandleEvents();

        if(!buttons && !le.MouseRight()) exit = true;

        if(button1) le.MousePressLeft(*button1) ? button1->Press() : button1->Release();
        if(button2) le.MousePressLeft(*button2) ? button2->Press() : button2->Release();

        if(button1 && le.MouseClickLeft(*button1)){ exit = true; result = Dialog::OK; }
        if(button2 && le.MouseClickLeft(*button2)){ exit = true; result = Dialog::CANCEL; }

        if(le.KeyPress(SDLK_RETURN)){ exit = true; result = Dialog::OK; }

        if(le.KeyPress(SDLK_ESCAPE)){ exit = true; result = Dialog::CANCEL; }
    }
    
    le.ResetKey();

    Cursor::Hide();

    if(button1) delete button1;
    if(button2) delete button2;

    Cursor::Show();

    return result;
}

void Castle::OpenTown(void)
{
    // cursor
    Cursor::Hide();

    Dialog::FrameBorder background;

    const Point & cur_pt(background.GetArea());
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN("CASLWIND.ICN", 0), dst_pt);

    std::string message;
    
    switch(race)
    {
        case Race::BARB: message = "CSTLBARB.ICN"; break;
        case Race::KNGT: message = "CSTLKNGT.ICN"; break;
        case Race::NECR: message = "CSTLNECR.ICN"; break;
        case Race::SORC: message = "CSTLSORC.ICN"; break;
        case Race::WRLK: message = "CSTLWRLK.ICN"; break;
        case Race::WZRD: message = "CSTLWZRD.ICN"; break;
	default: return;
    }

    u8 index = 0;

    // dwelling 1
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 3;
    display.Blit(AGG::GetICN(message, 19), dst_pt);
    const Rect rectDwelling1(dst_pt, 135, 57);
    const std::string & stringDwelling1 = GetStringBuilding(DWELLING_MONSTER1, race);
    // status bar
    if(!(DWELLING_MONSTER1 & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER1) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringDwelling1, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 61;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling1, Font::SMALL, true);

    // dwelling 2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 3;
    index = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE2)) && building & DWELLING_MONSTER2 ? 25 : 20;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling2(dst_pt, 135, 57);
    bool allowUpgrade2 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER2));
    bool allowBuyBuildDwelling2 = (allowUpgrade2 && (DWELLING_MONSTER2 & building) ? AllowBuyBuilding(DWELLING_UPGRADE2) : AllowBuyBuilding(DWELLING_MONSTER2));
    const std::string & stringDwelling2 = GetStringBuilding(allowUpgrade2 && (DWELLING_MONSTER2 & building) ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, race);
    // status bar
    if(allowUpgrade2 && !(DWELLING_UPGRADE2 & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_UPGRADE2) ? 1 : 2), dst_pt);
    }
    else
    if(!(DWELLING_MONSTER2 & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER2) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringDwelling2, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 61;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling2, Font::SMALL, true);

    // dwelling 3
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 3;
    index = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE3)) && building & DWELLING_MONSTER3 ? 26 : 21;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling3(dst_pt, 135, 57);
    bool allowUpgrade3 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER3));
    bool allowBuyBuildDwelling3 = (allowUpgrade3 && (DWELLING_MONSTER3 & building) ? AllowBuyBuilding(DWELLING_UPGRADE3) : AllowBuyBuilding(DWELLING_MONSTER3));
    const std::string & stringDwelling3 = GetStringBuilding(allowUpgrade3 && (DWELLING_MONSTER3 & building) ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, race);
    // status bar
    if(allowUpgrade3 && !(DWELLING_UPGRADE3 & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_UPGRADE3) ? 1 : 2), dst_pt);
    }
    else
    if(!(DWELLING_MONSTER3 & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER3) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringDwelling3, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 61;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling3, Font::SMALL, true);

    // dwelling 4
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 78;
    index = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE4)) && building & DWELLING_MONSTER4 ? 27 : 22;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling4(dst_pt, 135, 57);
    bool allowUpgrade4 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER4));
    bool allowBuyBuildDwelling4 = (allowUpgrade4 && (DWELLING_MONSTER4 & building) ? AllowBuyBuilding(DWELLING_UPGRADE4) : AllowBuyBuilding(DWELLING_MONSTER4));
    const std::string & stringDwelling4 = GetStringBuilding(allowUpgrade4 && (DWELLING_MONSTER4 & building) ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, race);
    // status bar
    if(allowUpgrade4 && !(DWELLING_UPGRADE4 & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_UPGRADE4) ? 1 : 2), dst_pt);
    }
    else
    if(!(DWELLING_MONSTER4 & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER4) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringDwelling4, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling4, Font::SMALL, true);

    // dwelling 5
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 78;
    index = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE5)) && building & DWELLING_MONSTER5 ? 28 : 23;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling5(dst_pt, 135, 57);
    bool allowUpgrade5 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER5));
    bool allowBuyBuildDwelling5 = (allowUpgrade5 && (DWELLING_MONSTER5 & building) ? AllowBuyBuilding(DWELLING_UPGRADE5) : AllowBuyBuilding(DWELLING_MONSTER5));
    const std::string & stringDwelling5 = GetStringBuilding(allowUpgrade5 && (DWELLING_MONSTER5 & building) ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, race);
    // status bar
    if(allowUpgrade5 && !(DWELLING_UPGRADE5 & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_UPGRADE5) ? 1 : 2), dst_pt);
    }
    else
    if(!(DWELLING_MONSTER5 & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER5) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringDwelling5, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling5, Font::SMALL, true);

    // dwelling 6
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 78;
    index = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE7)) && building & DWELLING_UPGRADE6 ? 30 :
           (Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE6)) && building & DWELLING_MONSTER6 ? 29 : 24);
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling6(dst_pt, 135, 57);
    bool allowUpgrade6 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER6));
    bool allowUpgrade7 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE6));
    bool allowBuyBuildDwelling6 = (allowUpgrade7 && (DWELLING_UPGRADE6 & building) ?
	AllowBuyBuilding(DWELLING_UPGRADE7) :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? AllowBuyBuilding(DWELLING_UPGRADE6) :
	AllowBuyBuilding(DWELLING_MONSTER6)));
    const std::string & stringDwelling6 = GetStringBuilding(
	allowUpgrade7 && (DWELLING_UPGRADE6 & building) ? DWELLING_UPGRADE7 :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), race);
    // status bar
    if(allowUpgrade6 && !(DWELLING_UPGRADE6 & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_UPGRADE6) ? 1 : 2), dst_pt);
    }
    else
    if(!(DWELLING_MONSTER6 & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(DWELLING_MONSTER6) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringDwelling6, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text::Text(dst_pt.x, dst_pt.y, stringDwelling6, Font::SMALL, true);

    // mage guild
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(message, 0), dst_pt);
    const Rect rectMageGuild(dst_pt, 135, 57);
    std::string stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD1, race);
    bool allowBuyBuildMageGuild = false;
    // status bar
    if(!(BUILD_MAGEGUILD5 & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 215;

	switch(GetLevelMageGuild())
	{
	    case 0:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD1);
		stringMageGuild += ", Level 1";
		break;
	    case 1:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD2);
		stringMageGuild += ", Level 2";
		break;
	    case 2:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD3);
		stringMageGuild += ", Level 3";
		break;
	    case 3:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD4);
		stringMageGuild += ", Level 4";
		break;
	    case 4:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD5);
		stringMageGuild += ", Level 5";
		break;
	    default: break;
	}

	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildMageGuild ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringMageGuild, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 216;
    Text::Text(dst_pt.x, dst_pt.y, stringMageGuild, Font::SMALL, true);

    // tavern
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 158;
    const Rect rectTavern(dst_pt, 135, 57);
    const std::string & stringTavern = GetStringBuilding(BUILD_TAVERN, race);
    if(Race::NECR == race)
	display.FillRect(0, 0, 0, Rect(dst_pt, 135, 57));
    else
    {
	display.Blit(AGG::GetICN(message, 2), dst_pt);
	// status bar
	if(!(BUILD_TAVERN & building))
	{
	    dst_pt.x = cur_pt.x + 249;
	    dst_pt.y = cur_pt.y + 215;
	    display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_TAVERN) ? 1 : 2), dst_pt);
	}
	// name
	dst_pt.x = cur_pt.x + 216 - Text::width(stringTavern, Font::SMALL) / 2;
	dst_pt.y = cur_pt.y + 216;
        Text::Text(dst_pt.x, dst_pt.y, stringTavern, Font::SMALL, true);
    }

    // thieves guild
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(message, 1), dst_pt);
    const Rect rectThievesGuild(dst_pt, 135, 57);
    const std::string & stringThievesGuild = GetStringBuilding(BUILD_THIEVESGUILD, race);
    // status bar
    if(!(BUILD_THIEVESGUILD & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 215;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_THIEVESGUILD) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringThievesGuild, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 216;
    Text::Text(dst_pt.x, dst_pt.y, stringThievesGuild, Font::SMALL, true);

    // shipyard
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 3), dst_pt);
    const Rect rectShipyard(dst_pt, 135, 57);
    const std::string & stringShipyard = GetStringBuilding(BUILD_SHIPYARD, race);
    // status bar
    if(!(BUILD_SHIPYARD & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 290;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_SHIPYARD) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringShipyard, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 291;
    Text::Text(dst_pt.x, dst_pt.y, stringShipyard, Font::SMALL, true);

    // statue
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 7), dst_pt);
    const Rect rectStatue(dst_pt, 135, 57);
    const std::string & stringStatue = GetStringBuilding(BUILD_STATUE, race);
    // status bar
    if(!(BUILD_STATUE & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 290;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_STATUE) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringStatue, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 291;
    Text::Text(dst_pt.x, dst_pt.y, stringStatue, Font::SMALL, true);

    // marketplace
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 10), dst_pt);
    const Rect rectMarketplace(dst_pt, 135, 57);
    const std::string & stringMarketplace = GetStringBuilding(BUILD_MARKETPLACE, race);
    // status bar
    if(!(BUILD_MARKETPLACE & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 290;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_MARKETPLACE) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringMarketplace, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 291;
    Text::Text(dst_pt.x, dst_pt.y, stringMarketplace, Font::SMALL, true);

    // well
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 4), dst_pt);
    const Rect rectWell(dst_pt, 135, 57);
    const std::string & stringWell = GetStringBuilding(BUILD_WELL, race);
    // status bar
    if(!(BUILD_WELL & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 365;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_WELL) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringWell, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 366;
    Text::Text(dst_pt.x, dst_pt.y, stringWell, Font::SMALL, true);

    // wel2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 11), dst_pt);
    const Rect rectWel2(dst_pt, 135, 57);
    const std::string & stringWel2 = GetStringBuilding(BUILD_WEL2, race);
    // status bar
    if(!(BUILD_WEL2 & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 365;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_WEL2) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringWel2, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 366;
    Text::Text(dst_pt.x, dst_pt.y, stringWel2, Font::SMALL, true);

    // spec
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 13), dst_pt);
    const Rect rectSpec(dst_pt, 135, 57);
    const std::string & stringSpec = GetStringBuilding(BUILD_SPEC, race);
    // status bar
    if(!(BUILD_SPEC & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 365;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_SPEC) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringSpec, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 366;
    Text::Text(dst_pt.x, dst_pt.y, stringSpec, Font::SMALL, true);

    // left turret
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 8), dst_pt);
    const Rect rectLTurret(dst_pt, 135, 57);
    const std::string & stringLTurret = GetStringBuilding(BUILD_LEFTTURRET, race);
    // status bar
    if(!(BUILD_LEFTTURRET & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 445;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_LEFTTURRET) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringLTurret, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 446;
    Text::Text(dst_pt.x, dst_pt.y, stringLTurret, Font::SMALL, true);

    // right turret
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 9), dst_pt);
    const Rect rectRTurret(dst_pt, 135, 57);
    const std::string & stringRTurret = GetStringBuilding(BUILD_RIGHTTURRET, race);
    // status bar
    if(!(BUILD_RIGHTTURRET & building))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 445;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_RIGHTTURRET) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringRTurret, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 446;
    Text::Text(dst_pt.x, dst_pt.y, stringRTurret, Font::SMALL, true);

    // moat
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 12), dst_pt);
    const Rect rectMoat(dst_pt, 135, 57);
    const std::string & stringMoat = GetStringBuilding(BUILD_MOAT, race);
    // status bar
    if(!(BUILD_MOAT & building))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 445;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", AllowBuyBuilding(BUILD_MOAT) ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringMoat, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 446;
    Text::Text(dst_pt.x, dst_pt.y, stringMoat, Font::SMALL, true);

    // captain
    switch(race)
    {
        case Race::BARB: message = "CSTLCAPB.ICN"; break;
        case Race::KNGT: message = "CSTLCAPK.ICN"; break;
        case Race::NECR: message = "CSTLCAPN.ICN"; break;
        case Race::SORC: message = "CSTLCAPS.ICN"; break;
        case Race::WRLK: message = "CSTLCAPW.ICN"; break;
        case Race::WZRD: message = "CSTLCAPZ.ICN"; break;
	default: return;
    }
    dst_pt.x = cur_pt.x + 444;
    dst_pt.y = cur_pt.y + 165;
    index = building & BUILD_CAPTAIN ? 1 : 0;
    display.Blit(AGG::GetICN(message, index), dst_pt);

    // primary hero
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 260;
    display.FillRect(0, 0, 0, Rect(dst_pt, 102, 93));

    // secondary hero
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 362;
    display.FillRect(0, 0, 0, Rect(dst_pt, 102, 94));

    // bottom bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    Dialog::StatusBar statusBar(dst_pt, AGG::GetICN("CASLBAR.ICN", 0), Font::BIG);
    statusBar.Clear("Castle Options");

    // redraw resource panel
    RedrawResourcePanel();

    // button exit
    dst_pt.x = cur_pt.x + 554;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, "SWAPBTN.ICN", 0, 1);

    display.Flip();
    Cursor::Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    le.ResetKey();

    // message loop
    bool exit = false;

    while(!exit)
    {
        le.HandleEvents();

        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)){ exit = true; }

        if(le.MouseCursor(rectDwelling1))
        {
	    if(DWELLING_MONSTER1 & building)
		statusBar.ShowMessage(stringDwelling1 + " is already built");
	    else
	    if(AllowBuyBuilding(DWELLING_MONSTER1))
		statusBar.ShowMessage("Build " + stringDwelling1);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling1);
        }
        else
        if(le.MouseCursor(rectDwelling2))
        {
	    if((allowUpgrade2 && (DWELLING_UPGRADE2 & building)) ||
	       (!allowUpgrade2 && (DWELLING_MONSTER2 & building)))
		statusBar.ShowMessage(stringDwelling2 + " is already built");
	    else
	    if(allowBuyBuildDwelling2)
		statusBar.ShowMessage("Build " + stringDwelling2);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling2);
        }
        else
        if(le.MouseCursor(rectDwelling3))
        {
	    if((allowUpgrade3 && (DWELLING_UPGRADE3 & building)) ||
	       (!allowUpgrade3 && (DWELLING_MONSTER3 & building)))
		statusBar.ShowMessage(stringDwelling3 + " is already built");
	    else
	    if(allowBuyBuildDwelling3)
		statusBar.ShowMessage("Build " + stringDwelling3);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling3);
        }
        else
        if(le.MouseCursor(rectDwelling4))
        {
	    if((allowUpgrade4 && (DWELLING_UPGRADE4 & building)) ||
	       (!allowUpgrade4 && (DWELLING_MONSTER4 & building)))
		statusBar.ShowMessage(stringDwelling4 + " is already built");
	    else
	    if(allowBuyBuildDwelling4)
		statusBar.ShowMessage("Build " + stringDwelling4);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling4);
        }
        else
        if(le.MouseCursor(rectDwelling5))
        {
	    if((allowUpgrade5 && (DWELLING_UPGRADE5 & building)) ||
	       (!allowUpgrade5 && (DWELLING_MONSTER5 & building)))
		statusBar.ShowMessage(stringDwelling5 + " is already built");
	    else
	    if(allowBuyBuildDwelling5)
		statusBar.ShowMessage("Build " + stringDwelling5);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling5);
        }
        else
        if(le.MouseCursor(rectDwelling6))
        {
	    if((allowUpgrade7 && (DWELLING_UPGRADE7 & building)) ||
	       (!allowUpgrade7 && allowUpgrade6 && (DWELLING_UPGRADE6 & building)) ||
	       (!allowUpgrade6 && (DWELLING_MONSTER6 & building)))
		statusBar.ShowMessage(stringDwelling6 + " is already built");
	    else
	    if(allowBuyBuildDwelling6)
		statusBar.ShowMessage("Build " + stringDwelling6);
	    else
		statusBar.ShowMessage("Cannot build " + stringDwelling6);
        }
        else
        if(le.MouseCursor(rectMageGuild))
        {
	    if(BUILD_MAGEGUILD5 & building)
		statusBar.ShowMessage(stringMageGuild + " is already built");
	    else
	    if(allowBuyBuildMageGuild)
		statusBar.ShowMessage("Build " + stringMageGuild);
	    else
		statusBar.ShowMessage("Cannot build " + stringMageGuild);
        }
        else
        if(Race::NECR != race && le.MouseCursor(rectTavern))
        {
	    if(BUILD_TAVERN & building)
		statusBar.ShowMessage(stringTavern + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_TAVERN))
		statusBar.ShowMessage("Build " + stringTavern);
	    else
		statusBar.ShowMessage("Cannot build " + stringTavern);
        }
        else
        if(le.MouseCursor(rectThievesGuild))
        {
	    if(BUILD_THIEVESGUILD & building)
		statusBar.ShowMessage(stringThievesGuild + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_THIEVESGUILD))
		statusBar.ShowMessage("Build " + stringThievesGuild);
	    else
		statusBar.ShowMessage("Cannot build " + stringThievesGuild);
        }
        else
        if(le.MouseCursor(rectShipyard))
        {
	    if(BUILD_SHIPYARD & building)
		statusBar.ShowMessage(stringShipyard + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_SHIPYARD))
		statusBar.ShowMessage("Build " + stringShipyard);
	    else
		statusBar.ShowMessage("Cannot build " + stringShipyard);
        }
        else
        if(le.MouseCursor(rectStatue))
        {
	    if(BUILD_STATUE & building)
		statusBar.ShowMessage(stringStatue + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_STATUE))
		statusBar.ShowMessage("Build " + stringStatue);
	    else
		statusBar.ShowMessage("Cannot build " + stringStatue);
        }
        else
        if(le.MouseCursor(rectMarketplace))
        {
	    if(BUILD_MARKETPLACE & building)
		statusBar.ShowMessage(stringMarketplace + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_MARKETPLACE))
		statusBar.ShowMessage("Build " + stringMarketplace);
	    else
		statusBar.ShowMessage("Cannot build " + stringMarketplace);
        }
        else
        if(le.MouseCursor(rectWell))
        {
	    if(BUILD_WELL & building)
		statusBar.ShowMessage(stringWell + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_WELL))
		statusBar.ShowMessage("Build " + stringWell);
	    else
		statusBar.ShowMessage("Cannot build " + stringWell);
        }
        else
        if(le.MouseCursor(rectWel2))
        {
	    if(BUILD_WEL2 & building)
		statusBar.ShowMessage(stringWel2 + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_WEL2))
		statusBar.ShowMessage("Build " + stringWel2);
	    else
		statusBar.ShowMessage("Cannot build " + stringWel2);
        }
        else
        if(le.MouseCursor(rectSpec))
        {
	    if(BUILD_SPEC & building)
		statusBar.ShowMessage(stringSpec + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_SPEC))
		statusBar.ShowMessage("Build " + stringSpec);
	    else
		statusBar.ShowMessage("Cannot build " + stringSpec);
        }
        else
        if(le.MouseCursor(rectLTurret))
        {
	    if(BUILD_LEFTTURRET & building)
		statusBar.ShowMessage(stringLTurret + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_LEFTTURRET))
		statusBar.ShowMessage("Build " + stringLTurret);
	    else
		statusBar.ShowMessage("Cannot build " + stringLTurret);
        }
        else
        if(le.MouseCursor(rectRTurret))
        {
	    if(BUILD_RIGHTTURRET & building)
		statusBar.ShowMessage(stringRTurret + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_RIGHTTURRET))
		statusBar.ShowMessage("Build " + stringRTurret);
	    else
		statusBar.ShowMessage("Cannot build " + stringRTurret);
        }
        else
        if(le.MouseCursor(rectMoat))
        {
	    if(BUILD_MOAT & building)
		statusBar.ShowMessage(stringMoat + " is already built");
	    else
	    if(AllowBuyBuilding(BUILD_MOAT))
		statusBar.ShowMessage("Build " + stringMoat);
	    else
		statusBar.ShowMessage("Cannot build " + stringMoat);
        }
        else
        // clear all
        if(! statusBar.isEmpty()) statusBar.Clear("Castle Options");
    }

    le.ResetKey();

    Cursor::Show();
}
