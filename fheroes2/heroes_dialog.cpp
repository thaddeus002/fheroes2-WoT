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
#include "agg.h"
#include "localevent.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "config.h"
#include "payment.h"
#include "heroes.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "error.h"
#include "dialog.h"

Dialog::answer_t Heroes::OpenDialog(void)
{
    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Dialog::FrameBorder background;

    const Point & cur_pt(background.GetArea());
    Point dst_pt(cur_pt);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    display.Blit(AGG::GetICN("HEROBKG.ICN", 0), dst_pt);

    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? "HEROEXTE.ICN" : "HEROEXTG.ICN", 0), dst_pt);

    // portrait
    std::string message( heroes < 10 ? "PORT000" : "PORT00");
    String::AddInt(message, heroes);
    message += ".ICN";
    
    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 31;
    
    display.Blit(AGG::GetICN(message, 0), dst_pt);
    
    // name
    message = name + " the " + Race::String(race) + " ( Level ";
    String::AddInt(message, GetLevel());
    message += " )";
    dst_pt.x = cur_pt.x + 320;
    dst_pt.y = cur_pt.y + 1;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    u8 index_sprite = 0;

    // attack
    message = "Attack Skill";
    dst_pt.x = cur_pt.x + 196;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetAttack());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);
    
    // defense
    message = "Defense Skill";
    dst_pt.x = cur_pt.x + 284;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    // spell
    message = "Spell Power";
    dst_pt.x = cur_pt.x + 372;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    // knowledge
    message = "Knowledge";
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    // crest
    switch(color)
    {
	case Color::BLUE:	index_sprite = 0; break;
	case Color::GREEN:	index_sprite = 1; break;
	case Color::RED:	index_sprite = 2; break;
	case Color::YELLOW:	index_sprite = 3; break;
	case Color::ORANGE:	index_sprite = 4; break;
	case Color::PURPLE:	index_sprite = 5; break;
	default: Error::Warning("Heroes::OpenDialog: unknown race"); break;
    }

    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 130;

    display.Blit(AGG::GetICN("CREST.ICN", index_sprite), dst_pt);
    
    // monster
    dst_pt.x = cur_pt.x + 156;
    dst_pt.y = cur_pt.y + 130;

    Army::SelectBar selectHeroesTroops(dst_pt, army);
    selectHeroesTroops.Reset();
    selectHeroesTroops.Redraw();
    const std::vector<Rect> & coordsHeroesTroops = selectHeroesTroops.GetCoords();

    // secskill
    dst_pt.x = cur_pt.x + 3;
    dst_pt.y = cur_pt.y + 233;

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
	const Skill & skill = skills[ii];

	const Sprite & sprite_skill = AGG::GetICN("SECSKILL.ICN", Skill::NONE == skill.GetSkill() ? 0 : skill.GetSkill() + 1);

	display.Blit(sprite_skill, dst_pt);


	if(Skill::NONE != skill.GetSkill())
	{
	    // string skill
	    message = Skill::String(skill.GetSkill());
	    Text(dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) / 2, dst_pt.y + 3, message, Font::SMALL, true);

	    // string level
	    message = Skill::LevelString(skill.GetLevel());
	    Text(dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) / 2, dst_pt.y + 50, message, Font::SMALL, true);
	}

	dst_pt.x += sprite_skill.w() + 5;
    }
    
    // artifact
    for(u8 ii = 0; ii < HEROESMAXARTIFACT; ii++)
    {
        // sprite
        const Sprite & art = AGG::GetICN("ARTIFACT.ICN", Artifact::UNKNOWN == artifacts[ii] ? 0 : artifacts[ii] + 1);

        dst_pt.x = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.x + 51 + ii * (art.w() + 15) : cur_pt.x + 51 + (ii - HEROESMAXARTIFACT / 2) * (art.w() + 15));
        dst_pt.y = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.y + 308 : cur_pt.y + 387);

        display.Blit(art, dst_pt);
    }

    // bottom small bar
    Dialog::StatusBar statusBar(Point(cur_pt.x + 21, cur_pt.y + 460), AGG::GetICN("HSBTNS.ICN", 8), Font::BIG);
    statusBar.Clear();

    // button prev
    dst_pt.x = cur_pt.x + 1;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonPrevHero(dst_pt, "HSBTNS.ICN", 4, 5);

    // button next
    dst_pt.x = cur_pt.x + 640 - 23;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonNextHero(dst_pt, "HSBTNS.ICN", 6, 7);
    
    // button dismiss
    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 318;
    Button buttonDismiss(dst_pt, "HSBTNS.ICN", 0, 1);

    // button exit
    dst_pt.x = cur_pt.x + 603;
    dst_pt.y = cur_pt.y + 318;
    Button buttonExit(dst_pt, "HSBTNS.ICN", 2, 3);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    display.Flip();
    Cursor::Show();

    Dialog::answer_t result = Dialog::ZERO;
    bool exit = false;

    // dialog menu loop
    while(! exit)
    {
        le.HandleEvents();

	// heroes troops
	for(u8 ii = 0; ii < HEROESMAXARMY; ++ii)
	{
	    if(le.MouseClickLeft(coordsHeroesTroops[ii]))
	    {
		Cursor::Hide();

		// show dialog army info
		if(selectHeroesTroops.isSelected() && army[ii].Valid() && selectHeroesTroops.GetCursorIndex() == ii)
		{
		    Army::Troops & select_troops = army[ii];
		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();
		    Kingdom & kingdom = const_cast<Kingdom &>(world.GetMyKingdom());

		    switch(army[ii].ShowDialogInfo(this))
		    {
			case Dialog::UPGRADE:
			    select_troops.SetMonster(Monster::Upgrade(select_monster));
		            kingdom.OddFundsResource(PaymentConditions::UpgradeMonster(select_monster) * select_count);
			    break;

			case Dialog::DISMISS:
			    select_troops.SetMonster(Monster::UNKNOWN);
			    select_troops.SetCount(0);
			    break;

			default: break;
		    }
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Redraw();
		}
		else
		// change or combine army or move to empty
		if(selectHeroesTroops.isSelected())
		{
		    // from castle or heroes
		    Army::Troops & select_troops = army[selectHeroesTroops.GetCursorIndex()];
		    const Monster::monster_t select_monster = select_troops.GetMonster();
		    const u16 select_count = select_troops.GetCount();

		    // change or combine army
		    if(army[ii].Valid())
		    {
			// change
			if(army[ii].GetMonster() != select_monster)
			{
			    select_troops = army[ii];
			    army[ii].SetMonster(select_monster);
			    army[ii].SetCount(select_count);
			}
			// combine
			else
                        {
                            army[ii].SetCount(army[ii].GetCount() + select_count);
                            select_troops.SetMonster(Monster::UNKNOWN);
                            select_troops.SetCount(0);
                        }

			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		    // move to empty position
		    else
		    if(selectHeroesTroops.isSelected())
		    {
			army[ii] = select_troops;
			select_troops.SetMonster(Monster::UNKNOWN);
			select_troops.SetCount(0);

			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		}
		else
		// select army
		if(army[ii].Valid())
		{
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Select(ii);
		    selectHeroesTroops.Redraw();
		}

		display.Flip();
		Cursor::Show();
	    }
	    else
	    // right click empty troops - redistribute troops
	    if(le.MouseClickRight(coordsHeroesTroops[ii]) &&
		!army[ii].Valid() &&
		selectHeroesTroops.isSelected())
	    {
		Cursor::Hide();

		Army::Troops & select_troops = army[selectHeroesTroops.GetCursorIndex()];

		if(const u16 redistr_count = Dialog::SelectCount(select_troops.GetCount()))
		{
		    army[ii].SetMonster(select_troops.GetMonster());
		    army[ii].SetCount(redistr_count);
		
		    select_troops.SetCount(select_troops.GetCount() - redistr_count);
		}

		Cursor::Hide();

		selectHeroesTroops.Reset();
		selectHeroesTroops.Redraw();

		Cursor::Show();
	    }
	    else
	    // press right - show quick info
	    if(le.MousePressRight(coordsHeroesTroops[ii]) && army[ii].Valid())
	    {
		Cursor::Hide();

		army[ii].ShowDialogInfo(this, true);

		Cursor::Show();
	    }
	}

        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();
        le.MousePressLeft(buttonDismiss) ? buttonDismiss.Press() : buttonDismiss.Release();
        le.MousePressLeft(buttonPrevHero) ? buttonPrevHero.Press() : buttonPrevHero.Release();
        le.MousePressLeft(buttonNextHero) ? buttonNextHero.Press() : buttonNextHero.Release();
	
        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_ESCAPE)){ result = Dialog::CANCEL; exit = true; }
    }

    le.ResetKey();
    
    Cursor::Show();

    return result;
}
