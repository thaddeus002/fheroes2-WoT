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

#include "agg.h"
#include "background.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "payment.h"
#include "world.h"
#include "kingdom.h"
#include "localevent.h"
#include "monster.h"
#include "morale.h"
#include "luck.h"
#include "tools.h"
#include "text.h"
#include "army.h"
#include "skill.h"
#include "dialog.h"
#include "game.h"
#include "spell.h"
#include "SDL.h"

Dialog::answer_t Dialog::ArmyInfo(const Army::Troop & basicArmy, bool dismiss, bool quickshow, bool show_upgrade, bool battle)
{
    const Army::BattleTroop & army(basicArmy);

    Display & display = Display::Get();

    const ICN::icn_t viewarmy = H2Config::EvilInterface() ? ICN::VIEWARME : ICN::VIEWARMY;

    const Surface & sprite_dialog = AGG::GetICN(viewarmy, 0);

    const Monster::stats_t monster = Monster::GetStats(army.Monster());
    const Skill::Primary *skills = army.MasterSkill();

    Rect pos_rt;

    pos_rt.x = (display.w() - sprite_dialog.w()) / 2;
    pos_rt.y = (display.h() - sprite_dialog.h()) / 2;
    pos_rt.w = sprite_dialog.w();
    pos_rt.h = sprite_dialog.h();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Background back(pos_rt);
    
    back.Save();

    display.Blit(sprite_dialog, pos_rt.x, pos_rt.y);

    Point dst_pt(pos_rt.x, pos_rt.y);
    std::string message;

    // name
    dst_pt.x = pos_rt.x  + 140 - Text::width(monster.name, Font::BIG) / 2;
    dst_pt.y = pos_rt.y + 40;
    Text(monster.name, Font::BIG, dst_pt);
    
    // count
    String::AddInt(message, army.Count());
    dst_pt.x = pos_rt.x + 140 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = pos_rt.y + 225;
    Text(message, Font::BIG, dst_pt);
    
    // attack
    message = "Attack:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y = pos_rt.y + 40;
    Text(message, Font::BIG, dst_pt);

    message.clear();
    String::AddInt(message, monster.attack);

    if(skills)
    {
	message += " (";
	String::AddInt(message, monster.attack + (*skills).GetAttack());
	message += ")";
    }

    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // defense
    message = "Defense:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message.clear();
    String::AddInt(message, monster.defence);

    if(skills)
    {
	message += " (";
	String::AddInt(message, monster.defence + (*skills).GetDefense());
	message += ")";
    }

    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // shot
    if(monster.shots) {
	message = battle ? "Shots Left:" : "Shots:";
	dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
	dst_pt.y += 18;
	Text(message, Font::BIG, dst_pt);

	message.clear();
	String::AddInt(message, battle ? army.shots : monster.shots);
	dst_pt.x = pos_rt.x + 420;
	Text(message, Font::BIG, dst_pt);
    }

    // damage
    message = "Damage:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message.clear();
    String::AddInt(message, monster.damageMin);
    message += " - ";
    String::AddInt(message, monster.damageMax);
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // hp
    message = "Hit Points:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message.clear();
    String::AddInt(message, monster.hp);
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    if(battle) {
	message = "Hit Points Left:";
	dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
	dst_pt.y += 18;
	Text(message, Font::BIG, dst_pt);
	
	message.clear();
	String::AddInt(message, army.hp);
	dst_pt.x = pos_rt.x + 420;
	Text(message, Font::BIG, dst_pt);
    }

    // speed
    message = "Speed:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message = Speed::String(monster.speed);
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // morale
    message = "Morale:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message = (skills ? Morale::String((*skills).GetMorale()) : Morale::String(Morale::NORMAL));
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // luck
    message = "Luck:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message = (skills ? Luck::String((*skills).GetLuck()) : Luck::String(Luck::NORMAL));
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // monster animation
    //Animation animeMonster(Point(pos_rt.x + 100, pos_rt.y + 180), monster.file, ?, ?, false, Animation::INFINITY | Animation::RING | Animation::LOW);

    bool upgrade = false;

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

    if(!quickshow)
    {
	if(show_upgrade && monster.monster != Monster::Upgrade(monster.monster))
	{
	    upgrade = PaymentConditions::payment_t(PaymentConditions::UpgradeMonster(monster.monster) * army.Count()) <= world.GetMyKingdom().GetFundsResource();

	    if(!upgrade)
	    {
		buttonUpgrade.Press();
		buttonUpgrade.SetDisable(true);
	    }
	    
	    buttonUpgrade.Draw();
	}
	else
	    buttonUpgrade.SetDisable(true);

	if(!dismiss)
	{
	    buttonDismiss.Press();
	    buttonDismiss.SetDisable(true);
	}

	buttonDismiss.Draw();
	buttonExit.Draw();
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();
    
    cursor.Show();
    display.Flip();
    
    Dialog::answer_t result = Dialog::ZERO;
        
    u16 animat = 0;
    Point anim_rt(pos_rt.x + 100, pos_rt.y + 180);
    Army::BattleTroop troop(army.Monster(), 0);
    troop.astate = Monster::AS_NONE;
    troop.aframe = 0;
    troop.Blit(anim_rt);
    int animcount=0;
    // dialog menu loop
    while(le.HandleEvents())
    {
        if(quickshow)
	{
	    if(!le.MouseRight()) break;
        }
	else
	{
	    if(buttonUpgrade.isEnable()) le.MousePressLeft(buttonUpgrade) ? (buttonUpgrade).PressDraw() : (buttonUpgrade).ReleaseDraw();
    	    if(buttonDismiss.isEnable()) le.MousePressLeft(buttonDismiss) ? (buttonDismiss).PressDraw() : (buttonDismiss).ReleaseDraw();
    	    le.MousePressLeft(buttonExit) ? (buttonExit).PressDraw() : (buttonExit).ReleaseDraw();

	    // upgrade
	    if(buttonUpgrade.isEnable() && le.MouseClickLeft(buttonUpgrade)){ result = Dialog::UPGRADE; break; }

    	    // dismiss
	    if(buttonDismiss.isEnable() && le.MouseClickLeft(buttonDismiss)){ result = Dialog::DISMISS; break; }

    	    // exit
    	    if(le.MouseClickLeft(buttonExit) ||
    		le.KeyPress(KEY_ESCAPE)){ result = Dialog::CANCEL; break; }
	}
	if(Game::ShouldAnimateInfrequent(animat++, 3)) {
	    troop.Animate();
	    if(troop.astate == Monster::AS_NONE) {
		switch(animcount) {
		case 0: troop.Animate(Monster::AS_IDLE),  animcount++; break;
		case 1: troop.Animate(Monster::AS_WALK),  animcount++; break;
 		case 2: troop.Animate(Monster::AS_ATT1P), animcount++; break;
		case 3: troop.Animate(Monster::AS_WALK),  animcount++; break;
 		case 4: troop.Animate(Monster::AS_ATT2P), animcount++; break;
		case 5: troop.Animate(Monster::AS_WALK),  animcount++; break;
 		case 6: troop.Animate(Monster::AS_ATT3P), animcount++; break;
		case 7: troop.Animate(Monster::AS_PAIN),  animcount++; break;
 		case 8: troop.Animate(Monster::AS_PAIN),  animcount++; break;
 		case 9: troop.Animate(Monster::AS_DIE),   animcount=0; break;
		}
	    }
	    troop.BlitR(anim_rt);
	    display.Flip();
	}

	//animeMonster.DrawSprite();
    }

    cursor.Hide();

    back.Restore();

    return result;
}
