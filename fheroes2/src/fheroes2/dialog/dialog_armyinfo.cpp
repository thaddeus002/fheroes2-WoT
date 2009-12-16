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
#include "battle_troop.h"
#include "pocketpc.h"

void DrawMonsterStats(const Point &, const Army::BattleTroop &, bool);

Dialog::answer_t Dialog::ArmyInfo(const Army::Troop & troops, u16 flags)
{
    Army::BattleTroop battroop(troops);
    return ArmyInfo(battroop, flags);
}

Dialog::answer_t Dialog::ArmyInfo(const Army::BattleTroop & battroop, u16 flags)
{
    if(Settings::Get().PocketPC()) return PocketPC::DialogArmyInfo(battroop, flags);

    const bool battle = BATTLE & flags;
    Display & display = Display::Get();

    const ICN::icn_t viewarmy = Settings::Get().EvilInterface() ? ICN::VIEWARME : ICN::VIEWARMY;

    const Surface & sprite_dialog = AGG::GetICN(viewarmy, 0);

    const Monster & mons = battroop;

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
    DrawMonsterStats(dst_pt, battroop, flags & BATTLE);

    // name
    text.Set(mons.GetName(), Font::BIG);
    dst_pt.x = pos_rt.x + 140 - text.w() / 2;
    dst_pt.y = pos_rt.y + 40;
    text.Blit(dst_pt);
    
    // count
    String::AddInt(message, battroop.Count());
    text.Set(message);
    dst_pt.x = pos_rt.x + 140 - text.w() / 2;
    dst_pt.y = pos_rt.y + 225;
    text.Blit(dst_pt);

    const Sprite & frame = AGG::GetICN(mons.ICNMonh(), 0);
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
            buttonUpgrade.SetDisable(false);
            buttonUpgrade.Draw();
        }
        else if(READONLY & flags)
        {
            buttonUpgrade.Press();
            buttonUpgrade.SetDisable(true);
            buttonUpgrade.Draw();
        }
        else buttonUpgrade.SetDisable(true);
    }
    else buttonUpgrade.SetDisable(true);

    if(BUTTONS & flags)
    {
        if(!battle)
            buttonDismiss.Draw();
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
            if(buttonUpgrade.isEnable() && le.MouseClickLeft(buttonUpgrade)){ result = Dialog::UPGRADE; break; }
            
    	    // dismiss
            if(buttonDismiss.isEnable() && le.MouseClickLeft(buttonDismiss)){ result = Dialog::DISMISS; break; }
            
    	    // exit
    	    if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_ESCAPE)){ result = Dialog::CANCEL; break; }
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

void DrawMonsterStats(const Point & dst, const Army::BattleTroop & battroop, bool battle)
{
    Point dst_pt;
    std::string message;
    Text text;
    const Monster & mons = battroop;
    const Skill::Primary *skills = battroop.MasterSkill();

    // attack
    message = _("Attack");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y = dst.y;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetAttack());

    if(skills)
    {
	message += " (";
	String::AddInt(message, mons.GetAttack() + (*skills).GetAttack());
	message += ")";
    }

    text.Set(message);
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    // defense
    message = _("Defense");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x + - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetDefense());

    if(skills)
    {
	message += " (";
	String::AddInt(message, mons.GetDefense() + (*skills).GetDefense());
	message += ")";
    }

    text.Set(message);
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    // shot
    if(mons.isArchers())
    {
	message = _(battle ? "Shots Left" : "Shots");
	message += ":";
	text.Set(message);
	dst_pt.x = dst.x - text.w();
	dst_pt.y += 18;
	text.Blit(dst_pt);

	message.clear();
	String::AddInt(message, battle ? battroop.shots : mons.GetShots());
	text.Set(message);
	dst_pt.x = dst.x + 20;
	text.Blit(dst_pt);
    }

    // damage
    message = _("Damage");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetDamageMin());
    message += " - ";
    String::AddInt(message, mons.GetDamageMax());
    text.Set(message);
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    // hp
    message = _("Hit Points");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, mons.GetHitPoints());
    text.Set(message);
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    if(battle)
    {
	message = _("Hit Points Left");
	message += ":";
	text.Set(message);
	dst_pt.x = dst.x - text.w();
	dst_pt.y += 18;
	text.Blit(dst_pt);
	
	message.clear();
	String::AddInt(message, battroop.hp);
	text.Set(message);
	dst_pt.x = dst.x + 20;
	text.Blit(dst_pt);
    }

    // speed
    message = _("Speed");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    text.Set(Speed::String(mons.GetSpeed()));
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    // morale
    message = _("Morale");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    text.Set(skills ? Morale::String((*skills).GetMorale()) : Morale::String(Morale::NORMAL));
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);

    // luck
    message = _("Luck");
    message += ":";
    text.Set(message);
    dst_pt.x = dst.x - text.w();
    dst_pt.y += 18;
    text.Blit(dst_pt);

    text.Set(skills ? Luck::String((*skills).GetLuck()) : Luck::String(Luck::NORMAL));
    dst_pt.x = dst.x + 20;
    text.Blit(dst_pt);
}
