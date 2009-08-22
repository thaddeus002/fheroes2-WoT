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
#include <bitset>
#include "agg.h"
#include "world.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "castle.h"
#include "dialog.h"
#include "heroes.h"
#include "text.h"
#include "statusbar.h"
#include "payment.h"
#include "kingdom.h"

void ShowBuildMessage(StatusBar & bar, bool isBuilt, const std::string & message, const Castle & castle, const u32 building)
{
    std::string str;
    
    if(isBuilt)
    {
        str = _("%{name} is already built");
        String::Replace(str, "%{name}", message);
    }
    else
    {
        const PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), static_cast<Castle::building_t>(building));

        if(!castle.AllowBuild())
	{
	    str = _("Cannot build. Already built here this turn.");
        }
        else
        if(castle.AllowBuild() && ! world.GetMyKingdom().AllowPayment(paymentBuild))
	{
	    str = _("Cannot afford %{name}");
    	    String::Replace(str, "%{name}", message);
        }
        else
        if(Castle::BUILD_SHIPYARD == building && !castle.HaveNearlySea())
        {
            str = _("Cannot build %{name} because castle is to far from water.");
    	    String::Replace(str, "%{name}", message);
        }
        else
        if(!castle.AllowBuyBuilding(static_cast<Castle::building_t>(building)))
        {
            str = _("Cannot build %{name}");
    	    String::Replace(str, "%{name}", message);
        }
        else
	{
	    str = _("Build %{name}");
    	    String::Replace(str, "%{name}", message);
        }
    }

    bar.ShowMessage(str);
}

Dialog::answer_t Castle::DialogBuyHero(const Heroes* hero)
{
    if(!hero) return Dialog::CANCEL;

    const ICN::icn_t system = (Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM);

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    Dialog::Box box(200, true);
    const Rect & box_rt = box.GetArea();
    LocalEvent & le = LocalEvent::GetLocalEvent();
    Point dst_pt;
    std::string str;

    str = _("Recruit Hero");
    Text text(str, Font::BIG);
    dst_pt.x = box_rt.x + (box_rt.w - text.w()) / 2;
    dst_pt.y = box_rt.y;
    text.Blit(dst_pt);

    //portrait and frame
    const Sprite & portrait_frame = AGG::GetICN(ICN::SURRENDR, 4);
    dst_pt.x = box_rt.x + (box_rt.w - portrait_frame.w()) / 2;
    dst_pt.y = dst_pt.y + text.h() + 10;
    display.Blit(portrait_frame, dst_pt);

    dst_pt.x = dst_pt.x + 5;
    dst_pt.y = dst_pt.y + 5;
    display.Blit(hero->GetPortrait101x93(), dst_pt);

    str = _("%{name} is a level %{value} %{race} with %{count} artifacts.");
    String::Replace(str, "%{name}", hero->GetName());
    String::Replace(str, "%{value}", hero->GetLevel());
    String::Replace(str, "%{race}", Race::String(hero->GetRace()));
    String::Replace(str, "%{count}", hero->GetCountArtifacts());

    TextBox box2(str, Font::BIG, box_rt.w);
    dst_pt.x = box_rt.x;
    dst_pt.y = dst_pt.y + portrait_frame.h() + 5;
    box2.Blit(dst_pt);

    Resource::funds_t paymentCosts(Resource::GOLD, RECRUIT_HEROES_GOLD);

    const Rect src_rt(box_rt.x, dst_pt.y + box2.h() + 10, box_rt.w, 0);
    Resource::AlignDraw(paymentCosts, src_rt);

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button button1(dst_pt, system, 1, 2);

    if(! AllowBuyHero())
    {
	button1.Press();
	button1.SetDisable(true);
    }

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button button2(dst_pt, system, 3, 4);

    button1.Draw();
    button2.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(button1.isEnable() &&
    	    (le.MouseClickLeft(button1) ||
    	    le.KeyPress(KEY_RETURN))) return Dialog::OK;

        if(le.MouseClickLeft(button2) ||
    	    le.KeyPress(KEY_ESCAPE)) break;
    }

    return Dialog::CANCEL;
}

Dialog::answer_t Castle::DialogBuyBuilding(building_t build, bool buttons)
{
    Display & display = Display::Get();

    const ICN::icn_t system = (Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM);

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    std::string str;

    if((DWELLING_MONSTER1 |
	DWELLING_MONSTER2 |
	DWELLING_MONSTER3 |
	DWELLING_MONSTER4 |
	DWELLING_MONSTER5 |
	DWELLING_MONSTER6 |
	DWELLING_UPGRADE2 |
	DWELLING_UPGRADE3 |
	DWELLING_UPGRADE4 |
	DWELLING_UPGRADE5 |
	DWELLING_UPGRADE6 |
	DWELLING_UPGRADE7) & build)
    {
        str = _("The %{building} produces %{monster}.");
        String::Replace(str, "%{building}", GetStringBuilding(build, race));
        std::string name = Monster(race, build).GetMultiName();
        String::Lower(name);
        String::Replace(str, "%{monster}", name);
    }
    else
	str = GetDescriptionBuilding(build, race);

    TextBox box1(str, Font::BIG, BOXAREA_WIDTH);

    // prepare requires build string
    std::bitset<32> requires(Castle::GetBuildingRequires(build));
    std::bitset<32> building2(building);
    str.clear();	
    if(requires.any())
    {
	    u8 count = 0;

	    for(u8 pos = 0; pos < requires.size(); ++pos)
	    {
		if(requires.test(pos))
		{
		    u32 value = 1;
		    value <<= pos;
		    
		    ++count;

		    if(! (building & value))
		    {
			str += GetStringBuilding(static_cast<building_t>(value), race);

			if(count < requires.count()) str += ", ";
		    }
		}
	    }
    }

    bool requires_true = str.size();
    TextBox box2(str, Font::BIG, BOXAREA_WIDTH);

    PaymentConditions::BuyBuilding paymentBuild(race, build);
    const u8 extra = (4 > paymentBuild.GetValidItems() ? 50 : (6 > paymentBuild.GetValidItems() ? 100 : 160));
    const Sprite & window_icons = AGG::GetICN(ICN::BLDGXTRA, 0);
    
    Dialog::Box box(10 + window_icons.h() + 10 + box1.h() + (requires_true ? 10 + box2.h() : 0) + 10 + extra, buttons);

    const Rect & box_rt = box.GetArea();

    ICN::icn_t cstl_icn = ICN::UNKNOWN;

    switch(race)
    {
        case Race::KNGT: cstl_icn = ICN::CSTLKNGT; break;
        case Race::BARB: cstl_icn = ICN::CSTLBARB; break;
        case Race::SORC: cstl_icn = ICN::CSTLSORC; break;
        case Race::WRLK: cstl_icn = ICN::CSTLWRLK; break;
        case Race::WZRD: cstl_icn = ICN::CSTLWZRD; break;
        case Race::NECR: cstl_icn = ICN::CSTLNECR; break;
        default: return Dialog::CANCEL;
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point dst_pt;

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button button1(dst_pt, system, 1, 2);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button button2(dst_pt, system, 3, 4);

    u8 index = 0;

    // sprite
    switch(build)
    {
		case BUILD_MAGEGUILD1:
		case BUILD_MAGEGUILD2:
		case BUILD_MAGEGUILD3:
		case BUILD_MAGEGUILD4:
		case BUILD_MAGEGUILD5:	index = 0; break;
		case BUILD_THIEVESGUILD:index = 1; break;
		case BUILD_TAVERN:	index = 2; break;
		case BUILD_SHIPYARD:	index = 3; break;
		case BUILD_WELL:	index = 4; break;
		case BUILD_CASTLE:	index = 6; break;
		case BUILD_STATUE:	index = 7; break;
		case BUILD_LEFTTURRET:	index = 8; break;
		case BUILD_RIGHTTURRET:	index = 9; break;
		case BUILD_MARKETPLACE:	index = 10; break;
		case BUILD_WEL2:	index = 11; break;
		case BUILD_MOAT:	index = 12; break;
		case BUILD_SPEC:	index = 13; break;
		case BUILD_CAPTAIN:	index = 15; break;
		case DWELLING_MONSTER1: index = 19; break;
		case DWELLING_MONSTER2: index = 20; break;
		case DWELLING_MONSTER3: index = 21; break;
		case DWELLING_MONSTER4: index = 22; break;
		case DWELLING_MONSTER5: index = 23; break;
		case DWELLING_MONSTER6: index = 24; break;
		case DWELLING_UPGRADE2: index = 25; break;
		case DWELLING_UPGRADE3: index = 26; break;
		case DWELLING_UPGRADE4: index = 27; break;
		case DWELLING_UPGRADE5: index = 28; break;
		case DWELLING_UPGRADE6: index = 29; break;
		case DWELLING_UPGRADE7: index = 30; break;
		default: return Dialog::CANCEL;
    }

    // sprite window_icons
    dst_pt.x = box_rt.x + (box_rt.w - window_icons.w()) / 2;
    dst_pt.y = box_rt.y + 10;
    display.Blit(window_icons, dst_pt);

    const Sprite & building_icons = AGG::GetICN(cstl_icn, index);
    dst_pt.x = box_rt.x + (box_rt.w - building_icons.w()) / 2;
    dst_pt.y += 1;
    display.Blit(building_icons, dst_pt);

    Text text;

    text.Set(GetStringBuilding(build, race), Font::SMALL);
    dst_pt.x = box_rt.x + (box_rt.w - text.w()) / 2;
    dst_pt.y += 57;
    text.Blit(dst_pt);

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + 10 + window_icons.h() + 10;
    box1.Blit(dst_pt);

    dst_pt.y += box1.h() + 10;
    if(requires_true)
    {
	text.Set(_("Requires:"), Font::BIG);
	dst_pt.x = box_rt.x + (box_rt.w - text.w()) / 2;
	text.Blit(dst_pt);

	dst_pt.x = box_rt.x;
	dst_pt.y += text.h();
	box2.Blit(dst_pt);

	dst_pt.y += + 10;
    }

    Resource::AlignDraw(paymentBuild, Rect(box_rt.x, dst_pt.y, BOXAREA_WIDTH, 0));

    if(buttons)
    {
	if(!AllowBuyBuilding(build)) button1.SetDisable(true);

	button1.Draw();
	button2.Draw();
    }
	
    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
        if(!buttons && !le.MouseRight()) break;

        le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(button1.isEnable() && (le.KeyPress(KEY_RETURN) ||
    	    le.MouseClickLeft(button1))) return Dialog::OK;

        if(le.KeyPress(KEY_ESCAPE) ||
    	    le.MouseClickLeft(button2)) return Dialog::CANCEL;
    }

    return Dialog::ZERO;
}

void RedrawInfoDwelling(const Point & pt, const Castle & castle, const Castle::building_t & build)
{
    Display & display = Display::Get();

    const Sprite & sprite_allow = AGG::GetICN(ICN::TOWNWIND, 11);
    const Sprite & sprite_deny  = AGG::GetICN(ICN::TOWNWIND, 12);
    const Sprite & sprite_money = AGG::GetICN(ICN::TOWNWIND, 13);
    
    Point dst_pt(pt);

    bool allowBuyBuilding = castle.AllowBuyBuilding(build);

    PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), build);

    if(Castle::BUILD_CAPTAIN & build)
    {
	// indicator
	dst_pt.x = pt.x + 65;
	dst_pt.y = pt.y + 60;
	if(castle.isBuild(build)) display.Blit(sprite_allow, dst_pt);
	else
	if(! allowBuyBuilding)
	    (1 == paymentBuild.GetValidItems() && paymentBuild.gold && castle.AllowBuild()) ? display.Blit(sprite_money, dst_pt) : display.Blit(sprite_deny, dst_pt);
    }
    else
    {
	// indicator
	dst_pt.x = pt.x + 115;
	dst_pt.y = pt.y + 40;
	if(castle.isBuild(build)) display.Blit(sprite_allow, dst_pt);
	else
	if(! allowBuyBuilding)
	    (1 == paymentBuild.GetValidItems() && paymentBuild.gold && castle.AllowBuild()) ? display.Blit(sprite_money, dst_pt) : display.Blit(sprite_deny, dst_pt);

	// status bar
	if(!castle.isBuild(build))
	{
	    dst_pt.x = pt.x - 1;
	    dst_pt.y = pt.y + 57;
	    display.Blit(AGG::GetICN(ICN::CASLXTRA, allowBuyBuilding ? 1 : 2), dst_pt);
	}

	// name
	Text text(Castle::GetStringBuilding(build, castle.GetRace()), Font::SMALL);
        dst_pt.x = pt.x + 68 - text.w() / 2;
	dst_pt.y = pt.y + 58;
	text.Blit(dst_pt);
    }
}

Castle::building_t Castle::OpenTown(void)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    frameborder.Redraw();
    
    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN(ICN::CASLWIND, 0), dst_pt);

    // hide captain options
    if(! (building & BUILD_CAPTAIN))
    {
	dst_pt.x = 530;
	dst_pt.y = 163;
	const Rect rect(dst_pt, 110, 84);
	dst_pt.x += cur_pt.x;
	dst_pt.y += cur_pt.y;
		
	display.Blit(AGG::GetICN(ICN::STONEBAK, 0), rect, dst_pt);
    }

    // draw castle sprite
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 0;
    DrawImageCastle(dst_pt);

    //
    ICN::icn_t icn = ICN::UNKNOWN;
    
    switch(race)
    {
        case Race::BARB: icn = ICN::CSTLBARB; break;
        case Race::KNGT: icn = ICN::CSTLKNGT; break;
        case Race::NECR: icn = ICN::CSTLNECR; break;
        case Race::SORC: icn = ICN::CSTLSORC; break;
        case Race::WRLK: icn = ICN::CSTLWRLK; break;
        case Race::WZRD: icn = ICN::CSTLWZRD; break;
	default: return BUILD_NOTHING;
    }

    u8 index = 0;

    // dwelling 1
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 3;
    display.Blit(AGG::GetICN(icn, 19), dst_pt);
    const Rect rectDwelling1(dst_pt, 135, 57);
    const std::string & stringDwelling1 = GetStringBuilding(DWELLING_MONSTER1, race);
    bool allowBuyBuildDwelling1 = AllowBuyBuilding(DWELLING_MONSTER1);
    RedrawInfoDwelling(dst_pt, *this, DWELLING_MONSTER1);

    // dwelling 2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 3;
    bool allowUpgrade2 = Monster(race, DWELLING_MONSTER2).isAllowUpgrade() && building & DWELLING_MONSTER2;
    index = allowUpgrade2  ? 25 : 20;
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectDwelling2(dst_pt, 135, 57);
    bool allowBuyBuildDwelling2 = (allowUpgrade2 && (DWELLING_MONSTER2 & building) ? AllowBuyBuilding(DWELLING_UPGRADE2) : AllowBuyBuilding(DWELLING_MONSTER2));
    const std::string & stringDwelling2 = GetStringBuilding(allowUpgrade2 && (DWELLING_MONSTER2 & building) ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, race);
    RedrawInfoDwelling(dst_pt, *this, allowUpgrade2 && (DWELLING_MONSTER2 & building) ? DWELLING_UPGRADE2 : DWELLING_MONSTER2);

    // dwelling 3
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 3;
    bool allowUpgrade3 = Monster(race, DWELLING_MONSTER3).isAllowUpgrade() && building & DWELLING_MONSTER3;
    index = allowUpgrade3 ? 26 : 21;
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectDwelling3(dst_pt, 135, 57);
    bool allowBuyBuildDwelling3 = (allowUpgrade3 && (DWELLING_MONSTER3 & building) ? AllowBuyBuilding(DWELLING_UPGRADE3) : AllowBuyBuilding(DWELLING_MONSTER3));
    const std::string & stringDwelling3 = GetStringBuilding(allowUpgrade3 && (DWELLING_MONSTER3 & building) ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, race);
    RedrawInfoDwelling(dst_pt, *this, allowUpgrade3 && (DWELLING_MONSTER3 & building) ? DWELLING_UPGRADE3 : DWELLING_MONSTER3);

    // dwelling 4
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade4 = Monster(race, DWELLING_MONSTER4).isAllowUpgrade() && building & DWELLING_MONSTER4;
    index = allowUpgrade4 ? 27 : 22;
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectDwelling4(dst_pt, 135, 57);
    bool allowBuyBuildDwelling4 = (allowUpgrade4 && (DWELLING_MONSTER4 & building) ? AllowBuyBuilding(DWELLING_UPGRADE4) : AllowBuyBuilding(DWELLING_MONSTER4));
    const std::string & stringDwelling4 = GetStringBuilding(allowUpgrade4 && (DWELLING_MONSTER4 & building) ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, race);
    RedrawInfoDwelling(dst_pt, *this, allowUpgrade4 && (DWELLING_MONSTER4 & building) ? DWELLING_UPGRADE4 : DWELLING_MONSTER4);

    // dwelling 5
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade5 = Monster(race, DWELLING_MONSTER5).isAllowUpgrade() && building & DWELLING_MONSTER5;
    index = allowUpgrade5 ? 28 : 23;
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectDwelling5(dst_pt, 135, 57);
    bool allowBuyBuildDwelling5 = (allowUpgrade5 && (DWELLING_MONSTER5 & building) ? AllowBuyBuilding(DWELLING_UPGRADE5) : AllowBuyBuilding(DWELLING_MONSTER5));
    const std::string & stringDwelling5 = GetStringBuilding(allowUpgrade5 && (DWELLING_MONSTER5 & building) ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, race);
    RedrawInfoDwelling(dst_pt, *this, allowUpgrade5 && (DWELLING_MONSTER5 & building) ? DWELLING_UPGRADE5 : DWELLING_MONSTER5);

    // dwelling 6
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade6 = Monster(race, DWELLING_MONSTER6).isAllowUpgrade() && building & DWELLING_MONSTER6;
    bool allowUpgrade7 = Monster(race, DWELLING_UPGRADE6).isAllowUpgrade() && building & DWELLING_UPGRADE6;
    index = allowUpgrade7 ? 30 :
           (allowUpgrade6 ? 29 : 24);
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectDwelling6(dst_pt, 135, 57);
    bool allowBuyBuildDwelling6 = (allowUpgrade7 && (DWELLING_UPGRADE6 & building) ?
	AllowBuyBuilding(DWELLING_UPGRADE7) :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? AllowBuyBuilding(DWELLING_UPGRADE6) :
	AllowBuyBuilding(DWELLING_MONSTER6)));
    const std::string & stringDwelling6 = GetStringBuilding(
	allowUpgrade7 && (DWELLING_UPGRADE6 & building) ? DWELLING_UPGRADE7 :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), race);
    RedrawInfoDwelling(dst_pt, *this, allowUpgrade7 && (DWELLING_UPGRADE6 & building) ? DWELLING_UPGRADE7 :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? DWELLING_UPGRADE6 : DWELLING_MONSTER6));

    // mage guild
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(icn, 0), dst_pt);
    const Rect rectMageGuild(dst_pt, 135, 57);
    std::string stringMageGuild;
    bool allowBuyBuildMageGuild = false;
    building_t nextLevelMageGuild = BUILD_MAGEGUILD1;
    // status bar
    switch(GetLevelMageGuild())
    {
	case 0:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD1);
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD1, race);
		nextLevelMageGuild = BUILD_MAGEGUILD1;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD1);
		break;
	case 1:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD2);
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD2, race);
		nextLevelMageGuild = BUILD_MAGEGUILD2;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD2);
		break;
	case 2:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD3);
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD3, race);
		nextLevelMageGuild = BUILD_MAGEGUILD3;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD3);
		break;
	case 3:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD4);
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD4, race);
		nextLevelMageGuild = BUILD_MAGEGUILD4;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD4);
		break;
	case 4:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD5);
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD5, race);
		nextLevelMageGuild = BUILD_MAGEGUILD5;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD5);
		break;
	case 5:
		allowBuyBuildMageGuild = false;
		stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD5, race);
		nextLevelMageGuild = BUILD_MAGEGUILD5;
		RedrawInfoDwelling(dst_pt, *this, BUILD_MAGEGUILD5);
		break;
	default: break;
    }

    // tavern
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 158;
    const Rect rectTavern(dst_pt, 135, 57);
    bool allowBuyBuildTavern = AllowBuyBuilding(BUILD_TAVERN);
    const std::string & stringTavern = GetStringBuilding(BUILD_TAVERN, race);
    if(Race::NECR == race && !Settings::Get().Modes(Settings::PRICELOYALTY))
	display.FillRect(0, 0, 0, Rect(dst_pt, 135, 57));
    else
    {
    	display.Blit(AGG::GetICN(icn, 2), dst_pt);
	RedrawInfoDwelling(dst_pt, *this, BUILD_TAVERN);
    }

    // thieves guild
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(icn, 1), dst_pt);
    const Rect rectThievesGuild(dst_pt, 135, 57);
    bool allowBuyBuildThievesGuild = AllowBuyBuilding(BUILD_THIEVESGUILD);
    const std::string & stringThievesGuild = GetStringBuilding(BUILD_THIEVESGUILD, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_THIEVESGUILD);

    // shipyard
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(icn, 3), dst_pt);
    const Rect rectShipyard(dst_pt, 135, 57);
    bool allowBuyBuildShipyard = AllowBuyBuilding(BUILD_SHIPYARD);
    const std::string & stringShipyard = GetStringBuilding(BUILD_SHIPYARD, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_SHIPYARD);

    // statue
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(icn, 7), dst_pt);
    const Rect rectStatue(dst_pt, 135, 57);
    bool allowBuyBuildStatue = AllowBuyBuilding(BUILD_STATUE);
    const std::string & stringStatue = GetStringBuilding(BUILD_STATUE, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_STATUE);

    // marketplace
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(icn, 10), dst_pt);
    const Rect rectMarketplace(dst_pt, 135, 57);
    bool allowBuyBuildMarketplace = AllowBuyBuilding(BUILD_MARKETPLACE);
    const std::string & stringMarketplace = GetStringBuilding(BUILD_MARKETPLACE, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_MARKETPLACE);

    // well
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(icn, 4), dst_pt);
    const Rect rectWell(dst_pt, 135, 57);
    bool allowBuyBuildWell = AllowBuyBuilding(BUILD_WELL);
    const std::string & stringWell = GetStringBuilding(BUILD_WELL, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_WELL);

    // wel2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(icn, 11), dst_pt);
    const Rect rectWel2(dst_pt, 135, 57);
    bool allowBuyBuildWel2 = AllowBuyBuilding(BUILD_WEL2);
    const std::string & stringWel2 = GetStringBuilding(BUILD_WEL2, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_WEL2);

    // spec
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(icn, 13), dst_pt);
    const Rect rectSpec(dst_pt, 135, 57);
    bool allowBuyBuildSpec = AllowBuyBuilding(BUILD_SPEC);
    const std::string & stringSpec = GetStringBuilding(BUILD_SPEC, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_SPEC);

    // left turret
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(icn, 8), dst_pt);
    const Rect rectLTurret(dst_pt, 135, 57);
    bool allowBuyBuildLTurret = AllowBuyBuilding(BUILD_LEFTTURRET);
    const std::string & stringLTurret = GetStringBuilding(BUILD_LEFTTURRET, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_LEFTTURRET);

    // right turret
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(icn, 9), dst_pt);
    const Rect rectRTurret(dst_pt, 135, 57);
    bool allowBuyBuildRTurret = AllowBuyBuilding(BUILD_RIGHTTURRET);
    const std::string & stringRTurret = GetStringBuilding(BUILD_RIGHTTURRET, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_RIGHTTURRET);

    // moat
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(icn, 12), dst_pt);
    const Rect rectMoat(dst_pt, 135, 57);
    bool allowBuyBuildMoat = AllowBuyBuilding(BUILD_MOAT);
    const std::string & stringMoat = GetStringBuilding(BUILD_MOAT, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_MOAT);

    // captain
    switch(race)
    {
        case Race::BARB: icn = ICN::CSTLCAPB; break;
        case Race::KNGT: icn = ICN::CSTLCAPK; break;
        case Race::NECR: icn = ICN::CSTLCAPN; break;
        case Race::SORC: icn = ICN::CSTLCAPS; break;
        case Race::WRLK: icn = ICN::CSTLCAPW; break;
        case Race::WZRD: icn = ICN::CSTLCAPZ; break;
	default: return BUILD_NOTHING;
    }
    dst_pt.x = cur_pt.x + 444;
    dst_pt.y = cur_pt.y + 165;
    index = building & BUILD_CAPTAIN ? 1 : 0;
    display.Blit(AGG::GetICN(icn, index), dst_pt);
    const Rect rectCaptain(dst_pt, AGG::GetICN(icn, index).w(), AGG::GetICN(icn, index).h());
    bool allowBuyBuildCaptain = AllowBuyBuilding(BUILD_CAPTAIN);
    const std::string & stringCaptain = GetStringBuilding(BUILD_CAPTAIN, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_CAPTAIN);

    const Sprite & spriteSpreadArmyFormat = AGG::GetICN(ICN::HSICONS, 9);
    const Sprite & spriteGroupedArmyFormat = AGG::GetICN(ICN::HSICONS, 10);
    const Rect rectSpreadArmyFormat(cur_pt.x + 550, cur_pt.y + 220, spriteSpreadArmyFormat.w(), spriteSpreadArmyFormat.h());
    const Rect rectGroupedArmyFormat(cur_pt.x + 585, cur_pt.y + 220, spriteGroupedArmyFormat.w(), spriteGroupedArmyFormat.h());
    const std::string descriptionSpreadArmyFormat(_("'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army."));
    const std::string descriptionGroupedArmyFormat(_("'Grouped' combat formation bunches your army toget her in the center of your side of the battlefield."));
    const Point pointSpreadArmyFormat(rectSpreadArmyFormat.x - 1, rectSpreadArmyFormat.y - 1);
    const Point pointGroupedArmyFormat(rectGroupedArmyFormat.x - 1, rectGroupedArmyFormat.y - 1);
    SpriteCursor cursorFormat(AGG::GetICN(ICN::HSICONS, 11), Modes(ARMYSPREAD) ? pointSpreadArmyFormat : pointGroupedArmyFormat);
    if(BUILD_CAPTAIN &building)
    {
	Text text(_("Attack Skill") + std::string(" "), Font::SMALL);
	dst_pt.x = cur_pt.x + 535;
	dst_pt.y = cur_pt.y + 168;
	text.Blit(dst_pt);

	std::string message;
	String::AddInt(message, captain.GetAttack());
	text.Set(message);
	dst_pt.x += 90;
	text.Blit(dst_pt);
	
	text.Set(_("Defense Skill") + std::string(" "));
	dst_pt.x = cur_pt.x + 535;
	dst_pt.y += 12;
	text.Blit(dst_pt);

	message.clear();
	String::AddInt(message, captain.GetDefense());
	text.Set(message);
	dst_pt.x += 90;
	text.Blit(dst_pt);

	text.Set(_("Spell Power") + std::string(" "));
	dst_pt.x = cur_pt.x + 535;
	dst_pt.y += 12;
	text.Blit(dst_pt);

	message.clear();
	String::AddInt(message, captain.GetPower());
	text.Set(message);
	dst_pt.x += 90;
	text.Blit(dst_pt);

	text.Set(_("Knowledge") + std::string(" "));
	dst_pt.x = cur_pt.x + 535;
	dst_pt.y += 12;
	text.Blit(dst_pt);

	message.clear();
	String::AddInt(message, captain.GetKnowledge());
	text.Set(message);
	dst_pt.x += 90;
	text.Blit(dst_pt);
	
	display.Blit(spriteSpreadArmyFormat, rectSpreadArmyFormat.x, rectSpreadArmyFormat.y);
	display.Blit(spriteGroupedArmyFormat, rectGroupedArmyFormat.x, rectGroupedArmyFormat.y);

	cursorFormat.Show(Modes(ARMYSPREAD) ? pointSpreadArmyFormat : pointGroupedArmyFormat);
    }

    Kingdom & kingdom = world.GetMyKingdom();

    const bool many_hero = kingdom.GetHeroes().size() == KINGDOMMAXHEROES;
    const bool allow_buy_hero = AllowBuyHero();

    Heroes* hero1 = kingdom.GetRecruits().GetHero1();
    Heroes* hero2 = kingdom.GetRecruits().GetHero2();

    // first hero
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 260;
    const Rect rectHero1(dst_pt, 102, 93);
    if(hero1)
    {
	display.Blit(hero1->GetPortrait101x93(), dst_pt);
    }
    else
	display.FillRect(0, 0, 0, rectHero1);
    // indicator
    if(many_hero || !allow_buy_hero)
    {
	dst_pt.x += 83;
	dst_pt.y += 75;
	display.Blit(AGG::GetICN(ICN::TOWNWIND, 12), dst_pt);
    }

    // second hero
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 362;
    const Rect rectHero2(dst_pt, 102, 94);
    if(hero2)
    {
	display.Blit(hero2->GetPortrait101x93(), dst_pt);
    }
    else
	display.FillRect(0, 0, 0, rectHero2);
    // indicator
    if(many_hero || !allow_buy_hero)
    {
	dst_pt.x += 83;
	dst_pt.y += 75;
	display.Blit(AGG::GetICN(ICN::TOWNWIND, 12), dst_pt);
    }

    // bottom bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    const Sprite & bar = AGG::GetICN(ICN::CASLBAR, 0);
    display.Blit(bar, dst_pt);

    StatusBar statusBar;
    statusBar.SetCenter(dst_pt.x + bar.w() / 2, dst_pt.y + 11);

    // redraw resource panel
    RedrawResourcePanel(cur_pt);

    // button exit
    dst_pt.x = cur_pt.x + 554;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, ICN::SWAPBTN, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;

	// click left
	if(!(DWELLING_MONSTER1 & building) && le.MouseClickLeft(rectDwelling1) && allowBuyBuildDwelling1 &&
		Dialog::OK == DialogBuyBuilding(DWELLING_MONSTER1, true))
	{
		return DWELLING_MONSTER1;
	}
	else
	if(!((allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2) & building) && le.MouseClickLeft(rectDwelling2) && allowBuyBuildDwelling2 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, true))
	{
		return allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2;
	}
	else
	if(!((allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3) & building) && le.MouseClickLeft(rectDwelling3) && allowBuyBuildDwelling3 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, true))
	{
		return allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3;
	}
	else
	if(!((allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4) & building) && le.MouseClickLeft(rectDwelling4) && allowBuyBuildDwelling4 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, true))
	{
		return allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4;
	}
	else
	if(!((allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5) & building) && le.MouseClickLeft(rectDwelling5) && allowBuyBuildDwelling5 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, true))
	{
		return allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5;
	}
	else
	if(!((allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6)) & building) && le.MouseClickLeft(rectDwelling6) && allowBuyBuildDwelling6 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), true))
	{
		return allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6);
	}
	else
	if(!(BUILD_MAGEGUILD5 & building) && le.MouseClickLeft(rectMageGuild) && allowBuyBuildMageGuild && Dialog::OK == DialogBuyBuilding(nextLevelMageGuild, true))
	{
		return nextLevelMageGuild;
	}
	else
	if(!(BUILD_TAVERN & building) && (Race::NECR != race || Settings::Get().Modes(Settings::PRICELOYALTY)) &&
	    le.MouseClickLeft(rectTavern) && allowBuyBuildTavern && Dialog::OK == DialogBuyBuilding(BUILD_TAVERN, true))
	{
		return BUILD_TAVERN;
	}
	else
	if(!(BUILD_THIEVESGUILD & building) && le.MouseClickLeft(rectThievesGuild) && allowBuyBuildThievesGuild && Dialog::OK == DialogBuyBuilding(BUILD_THIEVESGUILD, true))
	{
		return BUILD_THIEVESGUILD;
	}
	else
	if(!(BUILD_SHIPYARD & building) && le.MouseClickLeft(rectShipyard) && allowBuyBuildShipyard && Dialog::OK == DialogBuyBuilding(BUILD_SHIPYARD, true))
	{
		return BUILD_SHIPYARD;
	}
	else
	if(!(BUILD_STATUE & building) && le.MouseClickLeft(rectStatue) && allowBuyBuildStatue && Dialog::OK == DialogBuyBuilding(BUILD_STATUE, true))
	{
		return BUILD_STATUE;
	}
	else
	if(!(BUILD_MARKETPLACE & building) && le.MouseClickLeft(rectMarketplace) && allowBuyBuildMarketplace && Dialog::OK == DialogBuyBuilding(BUILD_MARKETPLACE, true))
	{
		return BUILD_MARKETPLACE;
	}
	else
	if(!(BUILD_WELL & building) && le.MouseClickLeft(rectWell) && allowBuyBuildWell && Dialog::OK == DialogBuyBuilding(BUILD_WELL, true))
	{
		return BUILD_WELL;
	}
	else
	if(!(BUILD_WEL2 & building) && le.MouseClickLeft(rectWel2) && allowBuyBuildWel2 && Dialog::OK == DialogBuyBuilding(BUILD_WEL2, true))
	{
		return BUILD_WEL2;
	}
	else
	if(!(BUILD_SPEC & building) && le.MouseClickLeft(rectSpec) && allowBuyBuildSpec && Dialog::OK == DialogBuyBuilding(BUILD_SPEC, true))
	{
		return BUILD_SPEC;
	}
	else
	if(!(BUILD_LEFTTURRET & building) && le.MouseClickLeft(rectLTurret) && allowBuyBuildLTurret && Dialog::OK == DialogBuyBuilding(BUILD_LEFTTURRET, true))
	{
		return BUILD_LEFTTURRET;
	}
	else
	if(!(BUILD_RIGHTTURRET & building) && le.MouseClickLeft(rectRTurret) && allowBuyBuildRTurret && Dialog::OK == DialogBuyBuilding(BUILD_RIGHTTURRET, true))
	{
		return BUILD_RIGHTTURRET;
	}
	else
	if(!(BUILD_MOAT & building) && le.MouseClickLeft(rectMoat) && allowBuyBuildMoat && Dialog::OK == DialogBuyBuilding(BUILD_MOAT, true))
	{
		return BUILD_MOAT;
	}
	else
	if(!(BUILD_CAPTAIN & building) && le.MouseClickLeft(rectCaptain) && allowBuyBuildCaptain && Dialog::OK == DialogBuyBuilding(BUILD_CAPTAIN, true))
	{
		return BUILD_CAPTAIN;
	}
        else
	if((BUILD_CAPTAIN && building) && le.MouseClickLeft(rectSpreadArmyFormat) && !Modes(ARMYSPREAD))
        {
            cursor.Hide();
            cursorFormat.Move(pointSpreadArmyFormat);
            cursor.Show();
            display.Flip();
            SetModes(ARMYSPREAD);
        }
	else
        if((BUILD_CAPTAIN && building) && le.MouseClickLeft(rectGroupedArmyFormat) && Modes(ARMYSPREAD))
        {
            cursor.Hide();
            cursorFormat.Move(pointGroupedArmyFormat);
            cursor.Show();
            display.Flip();
            ResetModes(ARMYSPREAD);
        }
	else
	if(hero1 && le.MouseClickLeft(rectHero1) &&
	    Dialog::OK == DialogBuyHero(hero1))
        {
    	    RecruitHero(hero1);

    	    return BUILD_NOTHING;
        }
	else
	if(hero2 && le.MouseClickLeft(rectHero2) &&
	    Dialog::OK == DialogBuyHero(hero2))
        {
    	    RecruitHero(hero2);

	    return BUILD_NOTHING;
        }


	// right
	if(le.MousePressRight(rectDwelling1)) PressRightAction(DWELLING_MONSTER1);
	else
	if(le.MousePressRight(rectDwelling2)) PressRightAction(allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2);
	else
	if(le.MousePressRight(rectDwelling3)) PressRightAction(allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3);
	else
	if(le.MousePressRight(rectDwelling4)) PressRightAction(allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4);
	else
	if(le.MousePressRight(rectDwelling5)) PressRightAction(allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5);
	else
	if(le.MousePressRight(rectDwelling6)) PressRightAction(allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6));
	else
	if(le.MousePressRight(rectMageGuild)) PressRightAction(nextLevelMageGuild);
	else
	if(le.MousePressRight(rectTavern)) PressRightAction(BUILD_TAVERN);
	else
	if(le.MousePressRight(rectThievesGuild)) PressRightAction(BUILD_THIEVESGUILD);
	else
	if(le.MousePressRight(rectShipyard)) PressRightAction(BUILD_SHIPYARD);
	else
	if(le.MousePressRight(rectStatue)) PressRightAction(BUILD_STATUE);
	else
	if(le.MousePressRight(rectMarketplace)) PressRightAction(BUILD_MARKETPLACE);
	else
	if(le.MousePressRight(rectWell)) PressRightAction(BUILD_WELL);
	else
	if(le.MousePressRight(rectWel2)) PressRightAction(BUILD_WEL2);
	else
	if(le.MousePressRight(rectSpec)) PressRightAction(BUILD_SPEC);
	else
	if(le.MousePressRight(rectLTurret)) PressRightAction(BUILD_LEFTTURRET);
	else
	if(le.MousePressRight(rectRTurret)) PressRightAction(BUILD_RIGHTTURRET);
	else
	if(le.MousePressRight(rectMoat)) PressRightAction(BUILD_MOAT);
	else
	if(le.MousePressRight(rectCaptain)) PressRightAction(BUILD_CAPTAIN);
        else
	if(le.MousePressRight(rectSpreadArmyFormat)) Dialog::Message(_("Spread Formation"), descriptionSpreadArmyFormat, Font::BIG);
        else
	if(le.MousePressRight(rectGroupedArmyFormat)) Dialog::Message(_("Grouped Formation"), descriptionGroupedArmyFormat, Font::BIG);
	else
	if(hero1 && le.MousePressRight(rectHero1)){ hero1->OpenDialog(true); cursor.Show(); display.Flip(); }
	else
	if(hero2 && le.MousePressRight(rectHero2)){ hero2->OpenDialog(true); cursor.Show(); display.Flip(); }

        // status info
	if(le.MouseCursor(rectDwelling1))
	    ShowBuildMessage(statusBar, DWELLING_MONSTER1 & building, stringDwelling1, *this, DWELLING_MONSTER1);
	else
	if(le.MouseCursor(rectDwelling2))
	{
	    if(isBuild(DWELLING_MONSTER2) && allowUpgrade2)
		ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE2), stringDwelling2, *this, DWELLING_UPGRADE2);
	    else
		ShowBuildMessage(statusBar, isBuild(DWELLING_MONSTER2), stringDwelling2, *this, DWELLING_MONSTER2);
	}
	else
	if(le.MouseCursor(rectDwelling3))
	{
	    if(isBuild(DWELLING_MONSTER3) && allowUpgrade3)
		ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE3), stringDwelling3, *this, DWELLING_UPGRADE3);
	    else
		ShowBuildMessage(statusBar, isBuild(DWELLING_MONSTER3), stringDwelling3, *this, DWELLING_MONSTER3);
	}
	else
	if(le.MouseCursor(rectDwelling4))
	{
	    if(isBuild(DWELLING_MONSTER4) && allowUpgrade4)
		ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE4), stringDwelling4, *this, DWELLING_UPGRADE4);
	    else
		ShowBuildMessage(statusBar, isBuild(DWELLING_MONSTER4), stringDwelling4, *this, DWELLING_MONSTER4);
	}
	else
	if(le.MouseCursor(rectDwelling5))
	{
	    if(isBuild(DWELLING_MONSTER5) && allowUpgrade5)
		ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE5), stringDwelling5, *this, DWELLING_UPGRADE5);
	    else
		ShowBuildMessage(statusBar, isBuild(DWELLING_MONSTER5), stringDwelling5, *this, DWELLING_MONSTER5);
	}
	else
	if(le.MouseCursor(rectDwelling6))
	{
	    if(isBuild(DWELLING_MONSTER6) && allowUpgrade6)
	    {
		if(isBuild(DWELLING_UPGRADE6) && allowUpgrade7)
		    ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE7), stringDwelling6, *this, DWELLING_UPGRADE7);
		else
		    ShowBuildMessage(statusBar, isBuild(DWELLING_UPGRADE6), stringDwelling6, *this, DWELLING_UPGRADE6);
	    }
	    else
		    ShowBuildMessage(statusBar, isBuild(DWELLING_MONSTER6), stringDwelling6, *this, DWELLING_MONSTER6);
	}
	else
	if(le.MouseCursor(rectMageGuild))
	{
	    switch(GetLevelMageGuild())
	    {
		case 0: ShowBuildMessage(statusBar, false, stringMageGuild, *this, BUILD_MAGEGUILD1); break;
		case 1: ShowBuildMessage(statusBar, false, stringMageGuild, *this, BUILD_MAGEGUILD2); break;
                case 2: ShowBuildMessage(statusBar, false, stringMageGuild, *this, BUILD_MAGEGUILD3); break;
                case 3: ShowBuildMessage(statusBar, false, stringMageGuild, *this, BUILD_MAGEGUILD4); break;
                case 4: ShowBuildMessage(statusBar, false, stringMageGuild, *this, BUILD_MAGEGUILD5); break;
                case 5: ShowBuildMessage(statusBar, true,  stringMageGuild, *this, BUILD_MAGEGUILD5); break;

		default: break;
	    }
	}
	else
	if((Race::NECR != race || Settings::Get().Modes(Settings::PRICELOYALTY)) && le.MouseCursor(rectTavern))
	    ShowBuildMessage(statusBar, BUILD_TAVERN & building, stringTavern, *this, BUILD_TAVERN);
	else
	if(le.MouseCursor(rectThievesGuild))
	    ShowBuildMessage(statusBar, BUILD_THIEVESGUILD & building, stringThievesGuild, *this, BUILD_THIEVESGUILD);
	else
	if(le.MouseCursor(rectShipyard))
	    ShowBuildMessage(statusBar, BUILD_SHIPYARD & building, stringShipyard, *this, BUILD_SHIPYARD);
	else
	if(le.MouseCursor(rectStatue))
	    ShowBuildMessage(statusBar, BUILD_STATUE & building, stringStatue, *this, BUILD_STATUE);
	else
	if(le.MouseCursor(rectMarketplace))
	    ShowBuildMessage(statusBar, BUILD_MARKETPLACE & building, stringMarketplace, *this, BUILD_MARKETPLACE);
	else
	if(le.MouseCursor(rectWell))
	    ShowBuildMessage(statusBar, BUILD_WELL & building, stringWell, *this, BUILD_WELL);
	else
	if(le.MouseCursor(rectWel2))
	    ShowBuildMessage(statusBar, BUILD_WEL2 & building, stringWel2, *this, BUILD_WEL2);
	else
	if(le.MouseCursor(rectSpec))
	    ShowBuildMessage(statusBar, BUILD_SPEC & building, stringSpec, *this, BUILD_SPEC);
	else
	if(le.MouseCursor(rectLTurret))
	    ShowBuildMessage(statusBar, BUILD_LEFTTURRET & building, stringLTurret, *this, BUILD_LEFTTURRET);
	else
	if(le.MouseCursor(rectRTurret))
	    ShowBuildMessage(statusBar, BUILD_RIGHTTURRET & building, stringRTurret, *this, BUILD_RIGHTTURRET);
	else
	if(le.MouseCursor(rectMoat))
	    ShowBuildMessage(statusBar, BUILD_MOAT & building, stringMoat, *this, BUILD_MOAT);
	else
	if(le.MouseCursor(rectCaptain))
	    ShowBuildMessage(statusBar, BUILD_CAPTAIN & building, stringCaptain, *this, BUILD_CAPTAIN);
	else
	if((hero1 && le.MouseCursor(rectHero1)) ||
	   (hero2 && le.MouseCursor(rectHero2)))
	{
	    if(many_hero)
		statusBar.ShowMessage(_("Cannot recruit - you have too many Heroes."));
	    else
	    if(castle_heroes)
		statusBar.ShowMessage(_("Cannot recruit - you already have a Hero in this town."));
	    else
	    if(! allow_buy_hero)
		statusBar.ShowMessage(_("Cannot afford a Hero"));
	    else
	    if(le.MouseCursor(rectHero1))
	    {
		std::string str = _("Recruit %{name} the %{race}");
		String::Replace(str, "%{name}", hero1->GetName());
		String::Replace(str, "%{race}", Race::String(hero1->GetRace()));
	    	statusBar.ShowMessage(str);
	    }
	    else
	    if(le.MouseCursor(rectHero2))
	    {
		std::string str = _("Recruit %{name} the %{race}");
		String::Replace(str, "%{name}", hero2->GetName());
		String::Replace(str, "%{race}", Race::String(hero2->GetRace()));
	    	statusBar.ShowMessage(str);
	    }
	}
	else
	if(le.MouseCursor(rectSpreadArmyFormat))
	    statusBar.ShowMessage(_("Set garrison combat formation to 'Spread'"));
	else
	if(le.MouseCursor(rectGroupedArmyFormat))
	    statusBar.ShowMessage(_("Set garrison combat formation to 'Grouped'"));
	else
	// clear all
	    statusBar.ShowMessage(_("Castle Options"));
    }

    return BUILD_NOTHING;
}

void Castle::PressRightAction(building_t b)
{
    bool complete = true;

    switch(b)
    {
	case BUILD_TAVERN:
	    if(Race::NECR == race && !Settings::Get().Modes(Settings::PRICELOYALTY)) return;
	    break;

	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	    complete = false;
	    break;

	default: break;
    }

    std::string building_description;
    
    if((DWELLING_MONSTER1 |
	DWELLING_MONSTER2 |
	DWELLING_MONSTER3 |
	DWELLING_MONSTER4 |
	DWELLING_MONSTER5 |
	DWELLING_MONSTER6 |
	DWELLING_UPGRADE2 |
	DWELLING_UPGRADE3 |
	DWELLING_UPGRADE4 |
	DWELLING_UPGRADE5 |
	DWELLING_UPGRADE6 |
	DWELLING_UPGRADE7) & b)
    {
	building_description = _("The %{building} produces %{monster}.");
	String::Replace(building_description, "%{building}", GetStringBuilding(b, race));
	String::Replace(building_description, "%{monster}", Monster(race, b).GetMultiName());
    }
    else
	building_description = GetDescriptionBuilding(b, race);

    if(b & building && complete)
	Dialog::Message(GetStringBuilding(b, race), building_description, Font::BIG);
    else
	DialogBuyBuilding(b, false);
}
