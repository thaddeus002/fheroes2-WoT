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

#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "monster.h"
#include "morale.h"
#include "speed.h"
#include "luck.h"
#include "army.h"
#include "skill.h"
#include "dialog.h"
#include "game.h"
#include "battle_stats.h"
#include "payment.h"
#include "pocketpc.h"

void DrawMonsterStats(const Point &, const Army::Troop &);
void DrawBattleStats(const Point &, const Battle2::Stats &);

Dialog::answer_t Dialog::ArmyInfo(const Army::Troop & troop, u16 flags)
{
    if(Settings::Get().QVGA()) return PocketPC::DialogArmyInfo(troop, flags);
    Display & display = Display::Get();

    const ICN::icn_t viewarmy = Settings::Get().EvilInterface() ? ICN::VIEWARME : ICN::VIEWARMY;
    const Surface & sprite_dialog = AGG::GetICN(viewarmy, 0);
    const Monster & mons = static_cast<Monster>(troop);
    const Battle2::Stats* battle = troop.GetBattleStats();
    Rect pos_rt;

    pos_rt.x = (display.w() - sprite_dialog.w()) / 2;
    pos_rt.y = (display.h() - sprite_dialog.h()) / 2;
    pos_rt.w = sprite_dialog.w();
    pos_rt.h = sprite_dialog.h();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Background back(pos_rt);
    back.Save();
    display.Blit(sprite_dialog, pos_rt.x, pos_rt.y);

    Point dst_pt;
    Text text;
    std::string message;

    dst_pt.x = pos_rt.x + 400;
    dst_pt.y = pos_rt.y + 40;

    DrawMonsterStats(dst_pt, troop);

    if(battle)
    {
	dst_pt.x = pos_rt.x + 400;
	dst_pt.y = pos_rt.y + 210;

	DrawBattleStats(dst_pt, *battle);
    }

    // name
    text.Set(troop.GetName(), Font::BIG);
    dst_pt.x = pos_rt.x + 140 - text.w() / 2;
    dst_pt.y = pos_rt.y + 40;
    text.Blit(dst_pt);
    
    // count
    String::AddInt(message, (battle ? battle->GetCount() : troop.GetCount()));
    text.Set(message);
    dst_pt.x = pos_rt.x + 140 - text.w() / 2;
    dst_pt.y = pos_rt.y + 225;
    text.Blit(dst_pt);

    const Sprite & frame = AGG::GetICN(troop.ICNMonh(), 0);
    display.Blit(frame, pos_rt.x + (pos_rt.w / 2 - frame.w()) / 2, pos_rt.y + 180 - frame.h());

    // button upgrade
    dst_pt.x = pos_rt.x + 284;
    dst_pt.y = pos_rt.y + 190;
    Button buttonUpgrade(dst_pt, viewarmy, 5, 6);

    // button dismiss
    dst_pt.x = pos_rt.x + 284;
    dst_pt.y = pos_rt.y + 222;
    Button buttonDismiss(dst_pt, viewarmy, 1, 2);

    // button exit
    dst_pt.x = pos_rt.x + 410;
    dst_pt.y = pos_rt.y + 222;
    Button buttonExit(dst_pt, viewarmy, 3, 4);

    if(READONLY & flags)
    {
        buttonDismiss.Press();
        buttonDismiss.SetDisable(true);
    }

    if(!battle && mons.isAllowUpgrade())
    {
        if(UPGRADE & flags)
        {
    	    if(UPGRADE_DISABLE & flags)
    	    {
        	buttonUpgrade.Press();
        	buttonUpgrade.SetDisable(true);
            }
	    else
        	buttonUpgrade.SetDisable(false);
	    buttonUpgrade.Draw();
        }
        else buttonUpgrade.SetDisable(true);
    }
    else buttonUpgrade.SetDisable(true);

    if(BUTTONS & flags)
    {
        if(!battle) buttonDismiss.Draw();
        buttonExit.Draw();
    }

    LocalEvent & le = LocalEvent::Get();
    Dialog::answer_t result = Dialog::ZERO;

    cursor.Show();
    display.Flip();
    
    // dialog menu loop
    while(le.HandleEvents())
    {
        if(flags & BUTTONS)
        {
            if(buttonUpgrade.isEnable()) le.MousePressLeft(buttonUpgrade) ? (buttonUpgrade).PressDraw() : (buttonUpgrade).ReleaseDraw();
    	    if(buttonDismiss.isEnable()) le.MousePressLeft(buttonDismiss) ? (buttonDismiss).PressDraw() : (buttonDismiss).ReleaseDraw();
    	    le.MousePressLeft(buttonExit) ? (buttonExit).PressDraw() : (buttonExit).ReleaseDraw();
            
            // upgrade
            if(buttonUpgrade.isEnable() && le.MouseClickLeft(buttonUpgrade))
            {
		std::string msg = 1.0f != Monster::GetUpgradeRatio() ?
		    _("Your troops can be upgraded, but it will cost you %{ratio} times the difference in cost for each troop, rounded up to next highest number. Do you wish to upgrade them?") :
		    _("Your troops can be upgraded, but it will cost you dearly. Do you wish to upgrade them?");
		String::Replace(msg, "%{ratio}", String::Double(Monster::GetUpgradeRatio(), 2));
        	if(Dialog::YES == Dialog::ResourceInfo("", msg,	troop.GetUpgradeCost(), Dialog::YES|Dialog::NO))
		{
		    result = Dialog::UPGRADE;
        	    break;
        	}
    	    }
    	    else
	    // dismiss
            if(buttonDismiss.isEnable() && le.MouseClickLeft(buttonDismiss) &&
        	Dialog::YES == Dialog::Message("", _("Are you sure you want to dismiss this army?"), Font::BIG, Dialog::YES | Dialog::NO))
            {
        	result = Dialog::DISMISS;
        	break;
    	    }
    	    else
	    // exit
    	    if(le.MouseClickLeft(buttonExit) || Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT)){ result = Dialog::CANCEL; break; }
        }
        else
        {
            if(!le.MousePressRight()) break;
        }
    }

    cursor.Hide();
    back.Restore();

    return result;
}

void DrawMonsterStats(const Point & dst, const Army::Troop & troop)
{
    Point dst_pt;
    std::string message;
    Text text;
    const Monster & mons = static_cast<Monster>(troop);
    const Battle2::Stats* battle = troop.GetBattleStats();
    bool commander = troop.GetArmy() && troop.GetArmy()->GetCommander();
    bool pda = Settings::Get().QVGA();

    // attack
    message = _("Attack");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y = dst.y;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetAttack());

    if(commander && mons.GetAttack() != troop.GetAttack())
    {
	message += " (";
	String::AddInt(message, troop.GetAttack());
	message += ")";
    }
    else
    // added ext. battle info
    if(battle && mons.GetAttack() != battle->GetAttack())
    {
	message += " (";
	String::AddInt(message, battle->GetAttack());
	message += ")";
    }

    const u8 ox = 15;

    text.Set(message);
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    // defense
    message = _("Defense");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetDefense());

    if(commander && mons.GetDefense() != troop.GetDefense())
    {
	message += " (";
	String::AddInt(message, troop.GetDefense());
	message += ")";
    }
    else
    // added ext. battle info
    if(battle && mons.GetDefense() != battle->GetDefense())
    {
	message += " (";
	String::AddInt(message, battle->GetDefense());
	message += ")";
    }

    text.Set(message);
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    // shot
    if(mons.isArchers())
    {
	message = battle ? _("Shots Left") : _("Shots");
	message += ":";
	text.Set(message);
	dst_pt.x = dst.x - text.w();
	dst_pt.y += (pda ? 14 : 18);
	text.Blit(dst_pt);

	message.clear();
	String::AddInt(message, battle ? battle->GetShots() : mons.GetShots());
	text.Set(message);
	dst_pt.x = dst.x + ox;
	text.Blit(dst_pt);
    }

    // damage
    message = _("Damage");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetDamageMin());
    message += " - ";
    String::AddInt(message, mons.GetDamageMax());
    text.Set(message);
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    // hp
    message = _("Hit Points");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetHitPoints());
    text.Set(message);
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    if(battle && battle->isValid())
    {
	message = _("Hit Points Left");
	message += ":";
	text.Set(message);
	dst_pt.x = dst.x - text.w();
	dst_pt.y += (pda ? 14 : 18);
	text.Blit(dst_pt);
	
	message.clear();
	String::AddInt(message, battle->GetHitPoints() - (battle->GetCount() - 1) * mons.GetHitPoints());
	text.Set(message);
	dst_pt.x = dst.x + ox;
	text.Blit(dst_pt);
    }

    // speed
    message = _("Speed");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    message = Speed::String(battle ? battle->GetSpeed(true) : mons.GetSpeed());
    message += " (";
    String::AddInt(message, battle ? battle->GetSpeed(true) : mons.GetSpeed());
    message += ")";
    text.Set(message);
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    // morale
    message = _("Morale");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    text.Set(Morale::String(troop.GetMorale()));
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);

    // luck
    message = _("Luck");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += (pda ? 14 : 18);
    text.Blit(dst_pt);

    text.Set(Luck::String(troop.GetLuck()));
    dst_pt.x = dst.x + ox;
    text.Blit(dst_pt);
}

const Sprite* GetModesSprite(u32 mod)
{
    switch(mod)
    {
	case Battle2::SP_BLOODLUST:	return &AGG::GetICN(ICN::SPELLINF, 9);
	case Battle2::SP_BLESS:		return &AGG::GetICN(ICN::SPELLINF, 3);
	case Battle2::SP_HASTE:		return &AGG::GetICN(ICN::SPELLINF, 0);
	case Battle2::SP_SHIELD:	return &AGG::GetICN(ICN::SPELLINF, 10);
	case Battle2::SP_STONESKIN:	return &AGG::GetICN(ICN::SPELLINF, 13);
	case Battle2::SP_DRAGONSLAYER:	return &AGG::GetICN(ICN::SPELLINF, 8);
	case Battle2::SP_STEELSKIN:	return &AGG::GetICN(ICN::SPELLINF, 14);
	case Battle2::SP_ANTIMAGIC:	return &AGG::GetICN(ICN::SPELLINF, 12);
	case Battle2::SP_CURSE:		return &AGG::GetICN(ICN::SPELLINF, 4);
	case Battle2::SP_SLOW:		return &AGG::GetICN(ICN::SPELLINF, 1);
	case Battle2::SP_BERSERKER:	return &AGG::GetICN(ICN::SPELLINF, 5);
	case Battle2::SP_HYPNOTIZE:	return &AGG::GetICN(ICN::SPELLINF, 7);
	case Battle2::SP_BLIND:		return &AGG::GetICN(ICN::SPELLINF, 2);
	case Battle2::SP_PARALYZE:	return &AGG::GetICN(ICN::SPELLINF, 6);
	case Battle2::SP_STONE:		return &AGG::GetICN(ICN::SPELLINF, 11);
	default: break;
    }
    return NULL;
}


void DrawBattleStats(const Point & dst, const Battle2::Stats & b)
{
    const u32 modes[] = {
	Battle2::SP_BLOODLUST, Battle2::SP_BLESS, Battle2::SP_HASTE, Battle2::SP_SHIELD, Battle2::SP_STONESKIN,
	Battle2::SP_DRAGONSLAYER, Battle2::SP_STEELSKIN, Battle2::SP_ANTIMAGIC, Battle2::SP_CURSE, Battle2::SP_SLOW,
	Battle2::SP_BERSERKER, Battle2::SP_HYPNOTIZE, Battle2::SP_BLIND, Battle2::SP_PARALYZE, Battle2::SP_STONE
    };

    // accumulate width
    u16 ow = 0;

    for(u8 ii = 0; ii < ARRAY_COUNT(modes); ++ii)
	if(b.Modes(modes[ii]))
	{
	    const Sprite* sprite = GetModesSprite(modes[ii]);
	    if(sprite)
		ow += sprite->w() + 4;
	}

    ow -= 4;
    ow = dst.x - ow / 2;

    Display & display = Display::Get();
    Text text;

    // blit centered
    for(u8 ii = 0; ii < ARRAY_COUNT(modes); ++ii)
	if(b.Modes(modes[ii]))
	{
	    const Sprite* sprite = GetModesSprite(modes[ii]);
	    if(sprite)
	    {
		display.Blit(*sprite, ow, dst.y);

		const u16 duration = b.affected.GetMode(modes[ii]);
		if(duration)
		{
		    std::ostringstream os;
		    os << duration;
		    text.Set(os.str(), Font::SMALL);
		    text.Blit(ow + (sprite->w() - text.w()) / 2, dst.y + sprite->h() + 1);
		}

		ow += sprite->w() + 4;
	    }
	}
}
