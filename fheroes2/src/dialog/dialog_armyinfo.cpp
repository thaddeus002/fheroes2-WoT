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
#include "animation.h"
#include "background.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "payment.h"
#include "world.h"
#include "kingdom.h"
#include "animation.h"
#include "localevent.h"
#include "monster.h"
#include "morale.h"
#include "luck.h"
#include "tools.h"
#include "text.h"
#include "army.h"
#include "skill.h"
#include "dialog.h"

Dialog::answer_t Dialog::ArmyInfo(const Army::Troops & army, const Skill::Primary *skills, bool dismiss, bool quickshow)
{
    const std::string viewarmy(H2Config::EvilInterface() ? "VIEWARME.ICN" : "VIEWARMY.ICN");

    const Surface & sprite_dialog = AGG::GetICN(viewarmy, 0);

    const Monster::stats_t monster = Monster::GetStats(army.monster);
    
    Rect pos_rt;

    pos_rt.x = (display.w() - sprite_dialog.w()) / 2;
    pos_rt.y = (display.h() - sprite_dialog.h()) / 2;
    pos_rt.w = sprite_dialog.w();
    pos_rt.h = sprite_dialog.h();

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
    String::AddInt(message, army.count);
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
	String::AddInt(message, monster.attack + (*skills).attack);
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
	String::AddInt(message, monster.defence + (*skills).defence);
	message += ")";
    }

    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // shot
    message = "Shots:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message.clear();
    String::AddInt(message, monster.shots);
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

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

    message = (skills ? Morale::String((*skills).morale) : Morale::String(Morale::NORMAL));
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // luck
    message = "Luck:";
    dst_pt.x = pos_rt.x + 400 - Text::width(message, Font::BIG);
    dst_pt.y += 18;
    Text(message, Font::BIG, dst_pt);

    message = (skills ? Luck::String((*skills).luck) : Luck::String(Luck::NORMAL));
    dst_pt.x = pos_rt.x + 420;
    Text(message, Font::BIG, dst_pt);

    // monster animation
    //Animation animeMonster(Point(pos_rt.x + 100, pos_rt.y + 180), monster.file, ?, ?, false, Animation::INFINITY | Animation::RING | Animation::LOW);

    // button upgrade
    Button *buttonUpgrade = NULL;
    bool upgrade = false;
    dst_pt.x = pos_rt.x + 284;
    dst_pt.y = pos_rt.y + 190;

    if(!quickshow && monster.monster != Monster::Upgrade(monster.monster))
    {
	buttonUpgrade = new Button(dst_pt, viewarmy, 5, 6);

	upgrade = PaymentConditions::payment_t(PaymentConditions::UpgradeMonster(monster.monster) * army.count) <= world.GetMyKingdom().GetFundsResource();

	if(!upgrade) (*buttonUpgrade).Press();
    }

    // button dismiss
    Button *buttonDismiss = NULL;
    dst_pt.x = pos_rt.x + 284;
    dst_pt.y = pos_rt.y + 222;
    if(!quickshow)
    {
	buttonDismiss = new Button(dst_pt, viewarmy, 1, 2);
	if(!dismiss) (*buttonDismiss).Press();
    }

    // button exit
    Button *buttonExit = NULL;
    dst_pt.x = pos_rt.x + 410;
    dst_pt.y = pos_rt.y + 222;
    if(!quickshow) buttonExit = new Button(dst_pt, viewarmy, 3, 4);

    LocalEvent & le = LocalEvent::GetLocalEvent();
    
    Cursor::Show();
    display.Flip();
    
    Dialog::answer_t result = Dialog::ZERO;
        
    // dialog menu loop
    while(le.HandleEvents())
    {
        if(quickshow)
	{
	    if(!le.MouseRight()) break;
        }
	else
	{
	    if(upgrade && buttonUpgrade) le.MousePressLeft(*buttonUpgrade) ? (*buttonUpgrade).Press() : (*buttonUpgrade).Release();
    	    if(dismiss && buttonDismiss) le.MousePressLeft(*buttonDismiss) ? (*buttonDismiss).Press() : (*buttonDismiss).Release();
    	    if(buttonExit) le.MousePressLeft(*buttonExit) ? (*buttonExit).Press() : (*buttonExit).Release();

	    // upgrade
	    if(upgrade && buttonUpgrade && le.MouseClickLeft(*buttonUpgrade)){ result = Dialog::UPGRADE; break; }

    	    // dismiss
	    if(dismiss && buttonDismiss && le.MouseClickLeft(*buttonDismiss)){ result = Dialog::DISMISS; break; }

    	    // exit
    	    if(buttonExit && (le.MouseClickLeft(*buttonExit) || le.KeyPress(SDLK_ESCAPE))){ result = Dialog::CANCEL; break; }
	}

	//animeMonster.DrawSprite();
    }

    if(buttonUpgrade) delete buttonUpgrade;
    if(buttonDismiss) delete buttonDismiss;
    if(buttonExit) delete buttonExit;

    Cursor::Hide();

    back.Restore();

    return result;
}
