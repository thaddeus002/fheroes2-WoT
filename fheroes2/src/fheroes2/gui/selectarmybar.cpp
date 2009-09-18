/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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
#include "selectarmybar.h"

void DialogRedistributeArmy(Army::Troop &, Army::Troop &);

enum
{
    FLAGS_READONLY	= 0x01,
    FLAGS_USEMONS32	= 0x02,
    FLAGS_SAVELAST	= 0x04,
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

void SelectArmyBar::SetArmy(Army::army_t & a)
{
    army = &a;
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

void SelectArmyBar::SetReadOnly(void)
{
    flags |= FLAGS_READONLY;
}

void SelectArmyBar::SetUseMons32Sprite(void)
{
    flags |= FLAGS_USEMONS32;
}

void SelectArmyBar::SetCastle(const Castle & c)
{
    castle = &c;
}

bool SelectArmyBar::SaveLastTroop(void) const
{
    return (flags & FLAGS_SAVELAST) && army && (1 == army->GetCount());
}

bool SelectArmyBar::ReadOnly(void) const
{
    return flags & FLAGS_READONLY;
}

void SelectArmyBar::Redraw(Surface & display)
{
    if(!background || !army) return;

    spritecursor.Hide();
    Point pt(pos);

    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	const Army::Troop & troop = army->At(ii);
	if(troop.isValid())
	{
            // blit alt background
            if(flags & FLAGS_USEMONS32)
		display.Blit(*background, pt);
	    else
	    switch(troop.GetRace())
            {
                case Race::KNGT: display.Blit(AGG::GetICN(ICN::STRIP, 4), pt);  break;
                case Race::BARB: display.Blit(AGG::GetICN(ICN::STRIP, 5), pt);  break;
                case Race::SORC: display.Blit(AGG::GetICN(ICN::STRIP, 6), pt);  break;
                case Race::WRLK: display.Blit(AGG::GetICN(ICN::STRIP, 7), pt);  break;
                case Race::WZRD: display.Blit(AGG::GetICN(ICN::STRIP, 8), pt);  break;
                case Race::NECR: display.Blit(AGG::GetICN(ICN::STRIP, 9), pt);  break;
                default: display.Blit(AGG::GetICN(ICN::STRIP, 10), pt); break;
            }

	    const Sprite & spmonh = AGG::GetICN(troop.ICNMonh(), 0);
	    const Sprite & mons32 = AGG::GetICN(ICN::MONS32, troop.GetSpriteIndex());

            if(flags & FLAGS_USEMONS32)
	    {
	    	display.Blit(mons32, pt.x + (background->w() - mons32.w()) / 2, pt.y + background->h() - mons32.h() - 11);
    
        	// draw count
        	std::string str;
        	String::AddInt(str, troop.Count());
        	Text text(str, Font::SMALL);
		text.Blit(pt.x + (background->w() - text.w()) / 2, pt.y + background->h() - 11);
            }
            else
	    {
	    	display.Blit(spmonh, pt.x + spmonh.x(), pt.y + spmonh.y());
    
        	// draw count
        	std::string str;
        	String::AddInt(str, troop.Count());
        	Text text(str, Font::SMALL);
		text.Blit(pt.x + background->w() - text.w() - 3, pt.y + background->h() - 13);
	    }
	}
	else
	{
	    display.Blit(*background, pt);
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

bool SelectArmyBar::QueueEventProcessing(SelectArmyBar & bar)
{
    LocalEvent & le = LocalEvent::Get();

    if(! bar.isValid()) return false;

    const s8 index1 = bar.GetIndexFromCoord(le.GetMouseCursor());
    if(0 > index1 || ARMYMAXTROOPS <= index1) return false;

    bool change = false;
    Army::Troop & troop1 = bar.army->At(index1);
    Cursor::Get().Hide();

    const s8 index_p = bar.GetIndexFromCoord(le.GetMousePressLeft());

    // drag drop - redistribute troops
    if(0 <= index_p && ARMYMAXTROOPS > index_p &&
       bar.army->At(index_p).isValid() && !troop1.isValid())
    {
	while(le.HandleEvents() && le.MousePressLeft()){ Cursor::Get().Show(); Display::Get().Flip(); DELAY(1); };
	const s8 index_r = bar.GetIndexFromCoord(le.GetMouseReleaseLeft());
	if(!bar.army->At(index_r).isValid())
	{
	    DialogRedistributeArmy(bar.army->At(index_p), bar.army->At(index_r));
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
	    Army::Troop & troop2 = bar.army->At(index2);

	    // dialog
	    if(index1 == index2)
	    {
		u16 flags = (bar.ReadOnly() || bar.SaveLastTroop() ? Dialog::READONLY | Dialog::BUTTONS : Dialog::BUTTONS);
		PaymentConditions::UpgradeMonster payment(troop1());
		payment *= troop1.Count();

		if(troop1.isAllowUpgrade() &&
		    bar.castle &&
		    bar.castle->GetRace() == troop1.GetRace() &&
		    bar.castle->isBuild(Monster::GetDwelling(Monster::Upgrade(troop1))) &&
		    payment <= world.GetMyKingdom().GetFundsResource())
		    flags |= Dialog::UPGRADE;

		switch(Dialog::ArmyInfo(troop1, flags))
		{
            	    case Dialog::UPGRADE:
            		world.GetMyKingdom().OddFundsResource(payment);
            		troop1.Upgrade();
			change = true;
            	    break;

        	    case Dialog::DISMISS:
                	troop1.Reset();
			change = true;
                	break;

		    default: break;
		}
	    }
	    else
	    // combine
	    if(troop1() == troop2())
	    {
		troop1.SetCount(troop1.Count() + troop2.Count());
		troop2.Reset();
	    }
	    // exchange
	    else
		Army::SwapTroops(troop1, troop2);

	    bar.Reset();
	    bar.Redraw();
	}
	else
	// select
	if(!bar.ReadOnly() && troop1.isValid())
	{
	    bar.Select(index1);
	}
    }
    else
    // press right
    if(le.MousePressRight(bar.GetArea()))
    {
	// show quick info
	if(troop1.isValid())
	{
	    bar.Reset();
	    Dialog::ArmyInfo(troop1, 0);
	}
	else
	// empty troops - redistribute troops
	if(bar.isSelected())
	{
	    const s8 index2 = bar.Selected();
	    Army::Troop & troop2 = bar.army->At(index2);

	    DialogRedistributeArmy(troop2, troop1);

	    bar.Reset();
	    bar.Redraw();
	}
    }

    Cursor::Get().Show();
    Display::Get().Flip();

    return change;
}

bool SelectArmyBar::QueueEventProcessing(SelectArmyBar & bar1, SelectArmyBar & bar2)
{
    LocalEvent & le = LocalEvent::Get();

    if(!bar1.isValid() || !bar2.isValid()) return false;
    bool change = false;

    if((bar1.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar1.GetArea()))
	    QueueEventProcessing(bar1);
    else
    if((bar2.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar2.GetArea()))
	    QueueEventProcessing(bar2);
    else
    if(bar1.isSelected() && le.MouseCursor(bar2.GetArea()))
    {
	const s8 index1 = bar2.GetIndexFromCoord(le.GetMouseCursor());
	if(0 > index1 || ARMYMAXTROOPS <= index1) return false;
	const s8 index2 = bar1.Selected();

	Army::Troop & troop1 = bar2.army->At(index1);
	Army::Troop & troop2 = bar1.army->At(index2);

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar2.GetArea()))
	{
	    // combine
	    if(!bar1.SaveLastTroop() && troop1() == troop2())
	    {
		troop1.SetCount(troop1.Count() + troop2.Count());
		troop2.Reset();
		change = true;
	    }
	    // exchange
	    else
	    if(!bar1.SaveLastTroop() || troop1.isValid())
	    {
	    	Army::SwapTroops(troop1, troop2);
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
	    if(troop1.isValid())
	    {
		bar1.Reset();
		Dialog::ArmyInfo(troop1, 0);
	    }
	    else
	    // empty troops - redistribute troops
	    {
		DialogRedistributeArmy(troop2, troop1);

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();
	    }
	}

	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
    if(bar2.isSelected() && le.MouseCursor(bar1.GetArea()))
    {
	const s8 index1 = bar1.GetIndexFromCoord(le.GetMouseCursor());
	if(0 > index1 || ARMYMAXTROOPS <= index1) return false;
	const s8 index2 = bar2.Selected();

	Army::Troop & troop1 = bar1.army->At(index1);
	Army::Troop & troop2 = bar2.army->At(index2);

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar1.GetArea()))
	{
	    // combine
	    if(!bar2.SaveLastTroop() && troop1() == troop2())
	    {
		troop1.SetCount(troop1.Count() + troop2.Count());
		troop2.Reset();
		change = true;
	    }
	    // exchange
	    else
	    if(!bar2.SaveLastTroop() || troop1.isValid())
	    {
		Army::SwapTroops(troop1, troop2);
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
	    if(troop1.isValid())
	    {
		bar2.Reset();
		Dialog::ArmyInfo(troop1, 0);
	    }
	    else
	    // empty troops - redistribute troops
	    {
		DialogRedistributeArmy(troop2, troop1);

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();
	    }
	}

	Cursor::Get().Show();
	Display::Get().Flip();
    }

    return change;
}

void DialogRedistributeArmy(Army::Troop & troop1, Army::Troop & troop2)
{
    u16 redistr_count = troop1.Count() / 2;
    if(Dialog::SelectCount(_("Move how many troops?"), 1, troop1.Count(), redistr_count))
    {
	troop2.Set(troop1, redistr_count);
	troop1.SetCount(troop1.Count() - redistr_count);
    }
}
