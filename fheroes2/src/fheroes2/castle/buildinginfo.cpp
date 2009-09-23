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

#include <bitset>
#include "agg.h"
#include "monster.h"
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "payment.h"
#include "buildinginfo.h"

u8 GetIndexBuildingSprite(u32 build)
{
    switch(build)
    {
	case Castle::DWELLING_MONSTER1:	return 19;
	case Castle::DWELLING_MONSTER2: return 20;
	case Castle::DWELLING_MONSTER3: return 21;
	case Castle::DWELLING_MONSTER4: return 22;
	case Castle::DWELLING_MONSTER5: return 23; 
	case Castle::DWELLING_MONSTER6: return 24;
	case Castle::DWELLING_UPGRADE2: return 25;
	case Castle::DWELLING_UPGRADE3: return 26;
	case Castle::DWELLING_UPGRADE4: return 27;
	case Castle::DWELLING_UPGRADE5: return 28;
	case Castle::DWELLING_UPGRADE6: return 29;
	case Castle::DWELLING_UPGRADE7: return 30;
	case Castle::BUILD_MAGEGUILD1:
	case Castle::BUILD_MAGEGUILD2:
	case Castle::BUILD_MAGEGUILD3:
	case Castle::BUILD_MAGEGUILD4:
	case Castle::BUILD_MAGEGUILD5:	return 0;
	case Castle::BUILD_THIEVESGUILD:return 1;
	case Castle::BUILD_TAVERN:	return 2;
	case Castle::BUILD_SHIPYARD:	return 3;
	case Castle::BUILD_WELL:	return 4;
	case Castle::BUILD_CASTLE:	return 6;
	case Castle::BUILD_STATUE:	return 7;
	case Castle::BUILD_LEFTTURRET:	return 8;
	case Castle::BUILD_RIGHTTURRET:	return 9;
	case Castle::BUILD_MARKETPLACE:	return 10;
	case Castle::BUILD_WEL2:	return 11;
	case Castle::BUILD_MOAT:	return 12;
	case Castle::BUILD_SPEC:	return 13;
	case Castle::BUILD_CAPTAIN:	return 15;
	default: break;
    }

    return 0;
}

BuildingInfo::BuildingInfo(const Castle & c, Castle::building_t b) : castle(c), building(b), area(0, 0, 135, 57), disable(false)
{
    if(IsDwelling()) building = castle.GetActualDwelling(b);

    building = castle.isBuild(b) ? castle.GetUpgradeBuilding(b) : b;

    if(IsDwelling())
    {
	description = _("The %{building} produces %{monster}.");
        String::Replace(description, "%{building}", Castle::GetStringBuilding(building, castle.GetRace()));
        std::string name = Monster(castle.GetRace(), building).GetMultiName();
        String::Lower(name);
        String::Replace(description, "%{monster}", name);
    }
    else
	description = Castle::GetDescriptionBuilding(building, castle.GetRace());

    // necr and tavern check
    if(Race::NECR == castle.GetRace() && Castle::BUILD_TAVERN == building && !Settings::Get().Modes(Settings::PRICELOYALTY))
	disable = true;
}

u32 BuildingInfo::operator() (void) const
{
    return building;
}

void BuildingInfo::SetPos(s16 x, s16 y)
{
    area.x = x;
    area.y = y;
}

const Rect & BuildingInfo::GetArea(void) const
{
    return area;
}

bool BuildingInfo::AllowBuy(void) const
{
    return castle.AllowBuyBuilding(building);
}

bool BuildingInfo::IsDisable(void) const
{
    return disable;
}

bool BuildingInfo::IsDwelling(void) const
{
    switch(building)
    {
	case Castle::DWELLING_MONSTER1:
	case Castle::DWELLING_MONSTER2:
	case Castle::DWELLING_MONSTER3:
	case Castle::DWELLING_MONSTER4:
	case Castle::DWELLING_MONSTER5:
	case Castle::DWELLING_MONSTER6:
	case Castle::DWELLING_UPGRADE2:
	case Castle::DWELLING_UPGRADE3:
	case Castle::DWELLING_UPGRADE4:
	case Castle::DWELLING_UPGRADE5:
	case Castle::DWELLING_UPGRADE6:
	case Castle::DWELLING_UPGRADE7:
	    return true;
	default: break;
    }
    return false;
}

void BuildingInfo::Redraw(void)
{
    u8 index = GetIndexBuildingSprite(building);
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(ICN::BLDGXTRA, 0), area.x, area.y);

    if(disable)
    {
	display.FillRect(0, 0, 0, Rect(area.x + 1, area.y + 1, area.w, area.h));
	return;
    }

    switch(castle.GetRace())
    {
        case Race::BARB: display.Blit(AGG::GetICN(ICN::CSTLBARB, index), area.x + 1, area.y + 1); break;
        case Race::KNGT: display.Blit(AGG::GetICN(ICN::CSTLKNGT, index), area.x + 1, area.y + 1); break;
        case Race::NECR: display.Blit(AGG::GetICN(ICN::CSTLNECR, index), area.x + 1, area.y + 1); break;
        case Race::SORC: display.Blit(AGG::GetICN(ICN::CSTLSORC, index), area.x + 1, area.y + 1); break;
        case Race::WRLK: display.Blit(AGG::GetICN(ICN::CSTLWRLK, index), area.x + 1, area.y + 1); break;
        case Race::WZRD: display.Blit(AGG::GetICN(ICN::CSTLWZRD, index), area.x + 1, area.y + 1); break;
        default: break;
    }

    const Sprite & sprite_allow = AGG::GetICN(ICN::TOWNWIND, 11);
    const Sprite & sprite_deny  = AGG::GetICN(ICN::TOWNWIND, 12);
    const Sprite & sprite_money = AGG::GetICN(ICN::TOWNWIND, 13);
    Point dst_pt;

    bool allow_buy = AllowBuy();
    PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), building);

    // indicator
    dst_pt.x = area.x + 115;
    dst_pt.y = area.y + 40;
    if(castle.isBuild(building)) display.Blit(sprite_allow, dst_pt);
    else
    if(! allow_buy)
	(1 == paymentBuild.GetValidItems() && paymentBuild.gold && castle.AllowBuild()) ? display.Blit(sprite_money, dst_pt) : display.Blit(sprite_deny, dst_pt);

    // status bar
    if(!castle.isBuild(building))
    {
	dst_pt.x = area.x;
	dst_pt.y = area.y + 58;
	display.Blit(AGG::GetICN(ICN::CASLXTRA, allow_buy ? 1 : 2), dst_pt);
    }

    // name
    Text text(Castle::GetStringBuilding(building, castle.GetRace()), Font::SMALL);
    dst_pt.x = area.x + 68 - text.w() / 2;
    dst_pt.y = area.y + 59;
    text.Blit(dst_pt);
}


const std::string & BuildingInfo::GetName(void) const
{
    return Castle::GetStringBuilding(building, castle.GetRace());
}

const std::string & BuildingInfo::GetDescription(void) const
{
    return description;
}

bool BuildingInfo::QueueEventProcessing(void)
{
    if(disable) return false;

    LocalEvent & le = LocalEvent::Get();
    if(le.MouseClickLeft(area))
    {
	return !castle.isBuild(building) && DialogBuyBuilding(true);
    }
    else
    if(le.MousePressRight(area))
    {
	if(castle.isBuild(building))
	    Dialog::Message(GetName(), GetDescription(), Font::BIG);
	else
	    DialogBuyBuilding(false);
    }
    return false;
}

bool BuildingInfo::DialogBuyBuilding(bool buttons) const
{
    Display & display = Display::Get();

    const ICN::icn_t system = (Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM);

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    TextBox box1(GetDescription(), Font::BIG, BOXAREA_WIDTH);

    // prepare requires build string
    std::string str;
    std::bitset<32> requires(castle.GetBuildingRequires(building));
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

		if(! castle.isBuild(value))
		{
		    str += Castle::GetStringBuilding(static_cast<Castle::building_t>(value), castle.GetRace());
		    if(count < requires.count()) str += ", ";
		}
	    }
	}
    }

    bool requires_true = str.size();
    Text requires_text(_("Requires:"), Font::BIG);
    TextBox box2(str, Font::BIG, BOXAREA_WIDTH);

    PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), building);
    Resource::BoxSprite rbs(paymentBuild, BOXAREA_WIDTH);

    const Sprite & window_icons = AGG::GetICN(ICN::BLDGXTRA, 0);
    const u8 space = Settings::Get().PocketPC() ? 5 : 10;
    Dialog::Box box(space + window_icons.h() + space + box1.h() + space + (requires_true ? requires_text.h() + box2.h() + space : 0) + rbs.GetArea().h, buttons);
    const Rect & box_rt = box.GetArea();
    ICN::icn_t cstl_icn = ICN::UNKNOWN;

    switch(castle.GetRace())
    {
        case Race::KNGT: cstl_icn = ICN::CSTLKNGT; break;
        case Race::BARB: cstl_icn = ICN::CSTLBARB; break;
        case Race::SORC: cstl_icn = ICN::CSTLSORC; break;
        case Race::WRLK: cstl_icn = ICN::CSTLWRLK; break;
        case Race::WZRD: cstl_icn = ICN::CSTLWZRD; break;
        case Race::NECR: cstl_icn = ICN::CSTLNECR; break;
        default: return Dialog::CANCEL;
    }

    LocalEvent & le = LocalEvent::Get();

    Point dst_pt;

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h - AGG::GetICN(system, 1).h();
    Button button1(dst_pt, system, 1, 2);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h - AGG::GetICN(system, 3).h();
    Button button2(dst_pt, system, 3, 4);

    dst_pt.x = box_rt.x + (box_rt.w - window_icons.w()) / 2;
    dst_pt.y = box_rt.y + space;
    display.Blit(window_icons, dst_pt);

    const Sprite & building_icons = AGG::GetICN(cstl_icn, GetIndexBuildingSprite(building));
    dst_pt.x = box_rt.x + (box_rt.w - building_icons.w()) / 2;
    dst_pt.y += 1;
    display.Blit(building_icons, dst_pt);

    Text text(GetName(), Font::SMALL);
    dst_pt.x = box_rt.x + (box_rt.w - text.w()) / 2;
    dst_pt.y += 57;
    text.Blit(dst_pt);

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + space + window_icons.h() + space;
    box1.Blit(dst_pt);

    dst_pt.y += box1.h() + space;
    if(requires_true)
    {
	dst_pt.x = box_rt.x + (box_rt.w - requires_text.w()) / 2;
	requires_text.Blit(dst_pt);

	dst_pt.x = box_rt.x;
	dst_pt.y += requires_text.h();
	box2.Blit(dst_pt);

	dst_pt.y += box2.h() + space;
    }

    rbs.SetPos(dst_pt.x, dst_pt.y);
    rbs.Redraw();

    if(buttons)
    {
	if(!AllowBuy()) button1.SetDisable(true);

	button1.Draw();
	button2.Draw();
    }

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
        if(!buttons && !le.MousePressRight()) break;

        le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(button1.isEnable() && (le.KeyPress(KEY_RETURN) ||
    	    le.MouseClickLeft(button1))) return true;

        if(le.KeyPress(KEY_ESCAPE) ||
    	    le.MouseClickLeft(button2)) break;
    }

    return false;
}
