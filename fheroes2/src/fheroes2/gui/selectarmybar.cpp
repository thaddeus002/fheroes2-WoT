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

#include "engine.h"
#include "agg.h"
#include "cursor.h"
#include "monster.h"
#include "text.h"
#include "world.h"
#include "payment.h"
#include "kingdom.h"
#include "army.h"
#include "tools.h"
#include "castle.h"
#include "race.h"
#include "editor_dialogs.h"
#include "selectarmybar.h"

void DialogRedistributeArmy(Army &, u8, Army &, u8);

enum
{
    FLAGS_READONLY	= 0x01,
    FLAGS_USEMONS32	= 0x02,
    FLAGS_SAVELAST	= 0x04,
    FLAGS_COUNT2SPRITE	= 0x08,
    FLAGS_CHANGEMODE	= 0x10
};

SelectArmyBar::SelectArmyBar() : army(NULL), interval(0), selected(-1), flags(0), background(NULL), castle(NULL)
{
}

const Rect & SelectArmyBar::GetArea(void) const
{
    return pos;
}

bool SelectArmyBar::isValid(void) const
{
    return background && army;
}

bool SelectArmyBar::isSelected(void) const
{
    return 0 <= selected;
}

s8 SelectArmyBar::Selected(void) const
{
    return selected;
}

s8 SelectArmyBar::GetIndexFromCoord(const Point & coord)
{
    if(!background || !army) return -1;

    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	const Rect rt(pos.x + ii * (background->w() + interval), pos.y, background->w(), background->h());
	if(rt & coord) return ii;
    }

    return -1;
}

void SelectArmyBar::SetArmy(Army& a)
{
    army = &a;
}

void SelectArmyBar::ResetArmy(void)
{
    army = NULL;
    flags &= ~FLAGS_SAVELAST;
}

void SelectArmyBar::SetPos(const Point & pt)
{
    SetPos(pt.x, pt.y);
}

void SelectArmyBar::SetPos(s16 cx, s16 cy)
    
{
    pos.x = cx;
    pos.y = cy;
    spritecursor.Save(cx, cy);
}

void SelectArmyBar::SetBackgroundSprite(const Surface & sf)
{
    background = &sf;
    pos.w = ARMYMAXTROOPS * sf.w() + (ARMYMAXTROOPS - 1) * interval;
    pos.h = background->h();

    offset.x = (background->w() - spritecursor.w()) / 2;
    offset.y = (background->h() - spritecursor.h()) / 2;
}

void SelectArmyBar::SetCursorSprite(const Surface & sf)
{
    spritecursor.SetSprite(sf);

    if(background)
    {
	offset.x = (background->w() - sf.w()) / 2;
	offset.y = (background->h() - sf.h()) / 2;
    }
}

void SelectArmyBar::SetInterval(const u8 it)
{
    interval = it;
    pos.w = (ARMYMAXTROOPS - 1) * interval;
    if(background) pos.w += ARMYMAXTROOPS * background->w();
}

void SelectArmyBar::SetSaveLastTroop(void)
{
    flags |= FLAGS_SAVELAST;
}

void SelectArmyBar::SetChangeMode(void)
{
    flags |= FLAGS_CHANGEMODE;
}

void SelectArmyBar::SetReadOnly(void)
{
    flags |= FLAGS_READONLY;
}

void SelectArmyBar::SetUseMons32Sprite(void)
{
    flags |= FLAGS_USEMONS32;
}

void SelectArmyBar::SetCount2Sprite(void)
{
    flags |= FLAGS_COUNT2SPRITE;
}

void SelectArmyBar::SetCastle(const Castle & c)
{
    castle = &c;
}

bool SelectArmyBar::SaveLastTroop(void) const
{
    return (flags & FLAGS_SAVELAST) && army && (1 == army->GetCount());
}

bool SelectArmyBar::ChangeMode(void) const
{
    return flags & FLAGS_CHANGEMODE;
}

bool SelectArmyBar::ReadOnly(void) const
{
    return flags & FLAGS_READONLY;
}

void SelectArmyBar::Redraw(Surface & dst)
{
    if(!background || !army) return;

    spritecursor.Hide();
    Point pt(pos);

    for(u8 ii = 0; ii < army->Size(); ++ii)
    {
	const Troop* troop = army->GetTroop(ii);

	if(troop && troop->isValid())
	{
            // blit alt background
            if(flags & FLAGS_USEMONS32)
		background->Blit(pt, dst);
	    else
	    switch(troop->GetRace())
            {
                case Race::KNGT: AGG::GetICN(ICN::STRIP, 4).Blit(pt, dst);  break;
                case Race::BARB: AGG::GetICN(ICN::STRIP, 5).Blit(pt, dst);  break;
                case Race::SORC: AGG::GetICN(ICN::STRIP, 6).Blit(pt, dst);  break;
                case Race::WRLK: AGG::GetICN(ICN::STRIP, 7).Blit(pt, dst);  break;
                case Race::WZRD: AGG::GetICN(ICN::STRIP, 8).Blit(pt, dst);  break;
                case Race::NECR: AGG::GetICN(ICN::STRIP, 9).Blit(pt, dst);  break;
                default: AGG::GetICN(ICN::STRIP, 10).Blit(pt, dst); break;
            }

	    const Sprite & spmonh = AGG::GetICN(troop->ICNMonh(), 0);
	    const Sprite & mons32 = AGG::GetICN(ICN::MONS32, troop->GetSpriteIndex());

            if(flags & FLAGS_USEMONS32)
	    {
        	if(flags & FLAGS_COUNT2SPRITE)
	    	    mons32.Blit(pt.x + (background->w() - mons32.w()) / 2, pt.y + background->h() - mons32.h() - 1, dst);
	    	else
	    	    mons32.Blit(pt.x + (background->w() - mons32.w()) / 2, pt.y + background->h() - mons32.h() - 11, dst);
    
        	// draw count
        	Text text(GetString(troop->GetCount()), Font::SMALL);

        	if(flags & FLAGS_COUNT2SPRITE)
		    text.Blit(pt.x + background->w() - text.w() - 3, pt.y + background->h() - text.h() - 2, dst);
		else
		    text.Blit(pt.x + (background->w() - text.w()) / 2, pt.y + background->h() - 11, dst);
            }
            else
	    {
	    	spmonh.Blit(pt.x + spmonh.x(), pt.y + spmonh.y(), dst);
    
        	// draw count
        	Text text(GetString(troop->GetCount()), Font::SMALL);
		text.Blit(pt.x + background->w() - text.w() - 3, pt.y + background->h() - 13, dst);
	    }
	}
	else
	{
	    background->Blit(pt, dst);
	}
	pt.x += background->w() + interval;
    }

    if(0 <= selected) spritecursor.Show(pos.x + selected * (background->w() + interval) + offset.x, flags & FLAGS_USEMONS32 ? pos.y : pos.y + offset.y);
}

void SelectArmyBar::Reset(void)
{
    selected = -1;
    if(Cursor::Get().isVisible()) Cursor::Get().Hide();
    spritecursor.Hide();
}

void SelectArmyBar::Select(u8 index)
{
    if(army && background && index < ARMYMAXTROOPS)
    {
	selected = index;
	spritecursor.Hide();
	spritecursor.Show(pos.x + index * (background->w() + interval) + offset.x, flags & FLAGS_USEMONS32 ? pos.y : pos.y + offset.y);
    }
}

void SelectArmyBar::StatusMessageEvent2(u8 index1, u8 index2, const Troop & troop1, const Troop & troop2, bool different, std::string & res)
{
    if(!different && index1 == index2)
    {
	res = _("View %{name}");
	String::Replace(res, "%{name}", troop1.GetName());
    }
    else
    if(troop1.isValid() && troop2.isValid())
    {
	res = troop1() == troop2() ? _("Combine %{name} armies") : _("Exchange %{name2} with %{name}");
	String::Replace(res, "%{name}", troop1.GetName());
	String::Replace(res, "%{name2}", troop2.GetName());
    }
    else
    {
	res = _("Move and right click Redistribute %{name}");
	String::Replace(res, "%{name}", troop2.GetName());
    }
}

void SelectArmyBar::StatusMessageEvent1(const SelectArmyBar & bar, u8 index1, const Troop & troop1, std::string & res)
{
    if(bar.isSelected())
    {
	const s8 index2 = bar.Selected();
	Troop* troop2 = bar.army->GetTroop(index2);

	if(troop2)
	StatusMessageEvent2(index1, index2, troop1, *troop2, false, res);
    }
    else
    if(troop1.isValid())
    {
	res = _("Select %{name}");
	String::Replace(res, "%{name}", troop1.GetName());
    }
}

bool SelectArmyBar::QueueEventProcessing(SelectArmyBar & bar, std::string* msg)
{
    LocalEvent & le = LocalEvent::Get();

    if(! bar.isValid()) return false;

    const s8 index1 = bar.GetIndexFromCoord(le.GetMouseCursor());
    Troop* troop1 = bar.army->GetTroop(index1);
    if(! troop1) return false;

    bool change = false;
    Cursor::Get().Hide();

    const s8 index_p = bar.GetIndexFromCoord(le.GetMousePressLeft());
    Troop* troop3 = bar.army->GetTroop(index_p);

    // drag drop - redistribute troops
    if(0 <= index_p && ARMYMAXTROOPS > index_p &&
       troop3 && troop3->isValid() &&
       !troop1->isValid())
    {
	while(le.HandleEvents() && le.MousePressLeft()){ Cursor::Get().Show(); Display::Get().Flip(); DELAY(1); };
	const s8 index_r = bar.GetIndexFromCoord(le.GetMouseReleaseLeft());
	troop3 = bar.army->GetTroop(index_r);

	if(troop3 && !troop3->isValid())
	{
	    DialogRedistributeArmy(*bar.army, index_p, *bar.army, index_r);
	    bar.Reset();
	    bar.Redraw();
	}
	le.ResetPressLeft();
    }
    else
    // left click
    if(le.MouseClickLeft(bar.GetArea()))
    {
	if(bar.isSelected())
	{
	    const s8 index2 = bar.Selected();
	    Troop* troop2 = bar.army->GetTroop(index2);

	    // dialog
	    if(index1 == index2)
	    {
		u16 flags = (bar.ReadOnly() || bar.SaveLastTroop() ? Dialog::READONLY | Dialog::BUTTONS : Dialog::BUTTONS);

		if(troop1->isAllowUpgrade() &&
		    bar.castle &&
		    bar.castle->GetRace() == troop1->GetRace() &&
		    bar.castle->isBuild(troop1->GetUpgrade().GetDwelling()))
		{
			flags |= Dialog::UPGRADE;

			if(! world.GetKingdom(bar.army->GetColor()).AllowPayment(troop1->GetUpgradeCost()))
			    flags |= Dialog::UPGRADE_DISABLE;
		}

		switch(Dialog::ArmyInfo(*troop1, flags))
		{
            	    case Dialog::UPGRADE:
            		world.GetKingdom(bar.army->GetColor()).OddFundsResource(troop1->GetUpgradeCost());
            		troop1->Upgrade();
			change = true;
            	    break;

        	    case Dialog::DISMISS:
                	troop1->Reset();
			change = true;
                	break;

		    default: break;
		}
	    }
	    else
	    // combine
	    if(troop1->GetID() == troop2->GetID())
	    {
		troop1->SetCount(troop1->GetCount() + troop2->GetCount());
		troop2->Reset();
		change = true;
	    }
	    // exchange
	    else
	    {
		Army::SwapTroops(*troop1, *troop2);
		change = true;
	    }

	    bar.Reset();
	    bar.Redraw();
	}
	else
	// select
	if(!bar.ReadOnly() && troop1->isValid())
	{
	    bar.Select(index1);
	    change = true;
	}
	// add any troop
	else
	if(bar.ChangeMode() && ! troop1->isValid())
	{
	    u8 cur = Monster::UNKNOWN;

	    if(bar.army->GetCommander())
	    switch(bar.army->GetCommander()->GetRace())
	    {
		case Race::KNGT: cur = Monster::PEASANT; break;
		case Race::BARB: cur = Monster::GOBLIN; break;
		case Race::SORC: cur = Monster::SPRITE; break;
		case Race::WRLK: cur = Monster::CENTAUR; break;
		case Race::WZRD: cur = Monster::HALFLING; break;
		case Race::NECR: cur = Monster::SKELETON; break;
		default: break;
	    }

	    Monster mons = Dialog::SelectMonster(cur);

	    if(mons.isValid())
	    {
		u32 count = 1;
		if(Dialog::SelectCount("Set Count", 1, 500000, count))
		{
		    troop1->Set(mons, count);
		    change = true;
		}
	    }
	}
    }
    else
    // press right
    if(le.MousePressRight(bar.GetArea()))
    {
	// show quick info
	if(troop1->isValid())
	{
	    bar.Reset();
	    if(bar.ChangeMode() && ! bar.SaveLastTroop())
	    {
		troop1->Reset();
		change = true;
	    }
	    else
		Dialog::ArmyInfo(*troop1, 0);
	}
	else
	// empty troops - redistribute troops
	if(bar.isSelected())
	{
	    const s8 index2 = bar.Selected();
	    //Troop & troop2 = bar.army->at(index2);

	    DialogRedistributeArmy(*bar.army, index2, *bar.army, index1);

	    bar.Reset();
	    bar.Redraw();
	    change = true;
	}
    }

    if(msg) StatusMessageEvent1(bar, index1, *troop1, *msg);

    Cursor::Get().Show();
    Display::Get().Flip();

    return change;
}

bool SelectArmyBar::QueueEventProcessing(SelectArmyBar & bar1, SelectArmyBar & bar2, std::string* msg)
{
    LocalEvent & le = LocalEvent::Get();

    if(!bar1.isValid() || !bar2.isValid()) return false;
    bool change = false;

    if((bar1.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar1.GetArea()))
	    return QueueEventProcessing(bar1, msg);
    else
    if((bar2.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar2.GetArea()))
	    return QueueEventProcessing(bar2, msg);
    else
    if(bar1.isSelected() && le.MouseCursor(bar2.GetArea()))
    {
	const s8 index1 = bar2.GetIndexFromCoord(le.GetMouseCursor());
	Troop* troop1 = bar2.army->GetTroop(index1);

	const s8 index2 = bar1.Selected();
	Troop* troop2 = bar1.army->GetTroop(index2);

	if(!troop1 || !troop2) return false;

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar2.GetArea()))
	{
	    // combine
	    if(!bar1.SaveLastTroop() && troop1->GetID() == troop2->GetID())
	    {
		troop1->SetCount(troop1->GetCount() + troop2->GetCount());
		troop2->Reset();
		change = true;
	    }
	    // exchange
	    else
	    if(!bar1.SaveLastTroop() || troop1->isValid())
	    {
	    	Army::SwapTroops(*troop1, *troop2);
		change = true;
	    }

	    bar1.Reset();
	    bar2.Reset();

	    bar1.Redraw();
	    bar2.Redraw();
	}
	else
	// press right
	if(le.MousePressRight(bar2.GetArea()))
	{
	    // show quick info
	    if(troop1->isValid())
	    {
		bar1.Reset();
		Dialog::ArmyInfo(*troop1, 0);
	    }
	    else
	    // empty troops - redistribute troops
	    {
		DialogRedistributeArmy(*bar1.army, index2, *bar2.army, index1);

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();

		change = true;
	    }
	}

	if(msg) StatusMessageEvent2(index1, index2, *troop1, *troop2, true, *msg);

	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
    if(bar2.isSelected() && le.MouseCursor(bar1.GetArea()))
    {
	const s8 index1 = bar1.GetIndexFromCoord(le.GetMouseCursor());
	Troop* troop1 = bar1.army->GetTroop(index1);

	const s8 index2 = bar2.Selected();
	Troop* troop2 = bar2.army->GetTroop(index2);

	if(!troop1 || !troop2) return false;

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar1.GetArea()))
	{
	    // combine
	    if(!bar2.SaveLastTroop() && troop1->GetID() == troop2->GetID())
	    {
		troop1->SetCount(troop1->GetCount() + troop2->GetCount());
		troop2->Reset();
		change = true;
	    }
	    // exchange
	    else
	    if(!bar2.SaveLastTroop() || troop1->isValid())
	    {
		Army::SwapTroops(*troop1, *troop2);
		change = true;
	    }

	    bar1.Reset();
	    bar2.Reset();

	    bar1.Redraw();
	    bar2.Redraw();
	}
	else
	// press right
	if(le.MousePressRight(bar1.GetArea()))
	{
	    // show quick info
	    if(troop1->isValid())
	    {
		bar2.Reset();
		Dialog::ArmyInfo(*troop1, 0);
	    }
	    else
	    // empty troops - redistribute troops
	    {
		DialogRedistributeArmy(*bar2.army, index2, *bar1.army, index1);

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();

		change = true;
	    }
	}

	if(msg) StatusMessageEvent2(index1, index2, *troop1, *troop2, true, *msg);

	Cursor::Get().Show();
	Display::Get().Flip();
    }


    return change;
}

void DialogRedistributeArmy(Army & army1, u8 index1, Army & army2, u8 index2)
{
    Troop* troop1 = army1.GetTroop(index1);
    Troop* troop2 = army2.GetTroop(index2);

    const bool last = (1 == army1.GetCount());

    if(2 > troop1->GetCount())
    {
	if(!last || troop2->isValid())
	    Army::SwapTroops(*troop1, *troop2);
    }
    else
    {
	u32 redistr_count = troop1->GetCount() / 2;
	u8 slots = Dialog::ArmySplitTroop((army2.GetTroop(index1) == troop1 ? 1 : 0) + army2.Size() - army2.GetCount(),
							(last ? troop1->GetCount() - 1 : troop1->GetCount()), redistr_count);

	switch(slots)
	{
	    case 3:
	    case 4:
	    case 5:
	    {
		const Troop troop(*troop1);
		troop1->Reset();
		army2.SplitTroopIntoFreeSlots(troop, slots);
	    }
		break;

	    case 2:
		troop2->Set(*troop1, redistr_count);
		troop1->SetCount(troop1->GetCount() - redistr_count);
		break;

	    default:
		break;
	}
    }
}
