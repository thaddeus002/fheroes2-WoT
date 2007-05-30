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
#include "skill.h"
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
    
    const Rect rectAttackSkill(cur_pt.x + 156, cur_pt.y + 30, 80, 92);

    // defense
    message = "Defense Skill";
    dst_pt.x = cur_pt.x + 284;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    const Rect rectDefenseSkill(cur_pt.x + 156 + 88, cur_pt.y + 30, 80, 92);
    
    // spell
    message = "Spell Power";
    dst_pt.x = cur_pt.x + 372;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    const Rect rectSpellSkill(cur_pt.x + 156 + 2 * 88, cur_pt.y + 30, 80, 92);

    // knowledge
    message = "Knowledge";
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 34;
    Text(dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y, message, Font::SMALL, true);
    
    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.y += 70;
    Text(dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y, message, Font::BIG, true);

    const Rect rectKnowledgeSkill(cur_pt.x + 156 + 3 * 88, cur_pt.y + 30, 80, 92);

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
    
    std::vector<Rect> coordsSkill;

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

	coordsSkill.push_back(Rect(dst_pt.x, dst_pt.y, sprite_skill.w(), sprite_skill.h()));

	dst_pt.x += sprite_skill.w() + 5;
    }

    const Rect rectMaxCoordsSkill(coordsSkill);

    // artifact
    std::vector<Rect> coordsArtifact;

    for(u8 ii = 0; ii < HEROESMAXARTIFACT; ii++)
    {
        // sprite
        const Sprite & art = AGG::GetICN("ARTIFACT.ICN", ii < artifacts.size() && Artifact::UNKNOWN > artifacts[ii] ? artifacts[ii] + 1 : 0);

        dst_pt.x = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.x + 51 + ii * (art.w() + 15) : cur_pt.x + 51 + (ii - HEROESMAXARTIFACT / 2) * (art.w() + 15));
        dst_pt.y = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.y + 308 : cur_pt.y + 387);

        display.Blit(art, dst_pt);
	
	coordsArtifact.push_back(Rect(dst_pt.x, dst_pt.y, art.w(), art.h()));
    }
    const Rect rectMaxCoordsArtifact(coordsArtifact);

    // bottom small bar
    Dialog::StatusBar statusBar(Point(cur_pt.x + 22, cur_pt.y + 460), AGG::GetICN("HSBTNS.ICN", 8), Font::BIG);
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

        // button click
	le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();
        le.MousePressLeft(buttonDismiss) ? buttonDismiss.Press() : buttonDismiss.Release();
        le.MousePressLeft(buttonPrevHero) ? buttonPrevHero.Press() : buttonPrevHero.Release();
        le.MousePressLeft(buttonNextHero) ? buttonNextHero.Press() : buttonNextHero.Release();

        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_ESCAPE)){ result = Dialog::CANCEL; exit = true; }

	// left click info
        if(le.MouseClickLeft(rectAttackSkill)) Dialog::Message("Attack Skill", "Your attack skill is a bonus added to each creature's attack skill.", Font::BIG, Dialog::OK);
        if(le.MouseClickLeft(rectDefenseSkill)) Dialog::Message("Defense Skill", "Your defense skill is a bonus added to each creature's defense skill.", Font::BIG, Dialog::OK);
        if(le.MouseClickLeft(rectSpellSkill)) Dialog::Message("Spell Power", "Your spell power determines the length or power of a spell.", Font::BIG, Dialog::OK);
        if(le.MouseClickLeft(rectKnowledgeSkill)) Dialog::Message("Knowledge", "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.", Font::BIG, Dialog::OK);

	// left click skill
	for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MouseClickLeft(coordsSkill[ii]))
	{
	    const Skill::skill_t skill = skills[ii].GetSkill();
	    const Skill::level_t level = skills[ii].GetLevel();

	    if(Skill::NONE != skill) Dialog::Message(Skill::LevelString(level) + " " + Skill::String(skill), Skill::Description(skill, level), Font::BIG, Dialog::OK);
	}

	// left click artifact
	for(u8 ii = 0; ii < coordsArtifact.size(); ++ii) if(le.MouseClickLeft(coordsArtifact[ii]))
	{
	    const Artifact::artifact_t art = artifacts[ii];
	    if(Artifact::UNKNOWN != art) Dialog::Message(Artifact::String(art), Artifact::Description(art), Font::BIG, Dialog::OK);
	}

	// right info
        if(le.MousePressRight(rectAttackSkill)) Dialog::Message("Attack Skill", "Your attack skill is a bonus added to each creature's attack skill.", Font::BIG);
        if(le.MousePressRight(rectDefenseSkill)) Dialog::Message("Defense Skill", "Your defense skill is a bonus added to each creature's defense skill.", Font::BIG);
        if(le.MousePressRight(rectSpellSkill)) Dialog::Message("Spell Power", "Your spell power determines the length or power of a spell.", Font::BIG);
        if(le.MousePressRight(rectKnowledgeSkill)) Dialog::Message("Knowledge", "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.", Font::BIG);

	// right info skill
	for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MousePressRight(coordsSkill[ii]))
	{
	    const Skill::skill_t skill = skills[ii].GetSkill();
	    const Skill::level_t level = skills[ii].GetLevel();

	    if(Skill::NONE != skill) Dialog::Message(Skill::LevelString(level) + " " + Skill::String(skill), Skill::Description(skill, level), Font::BIG);
	}

	// right info artifact
	for(u8 ii = 0; ii < coordsArtifact.size(); ++ii) if(le.MousePressRight(coordsArtifact[ii]))
	{
	    const Artifact::artifact_t art = artifacts[ii];
	    if(Artifact::UNKNOWN != art) Dialog::Message(Artifact::String(art), Artifact::Description(art), Font::BIG);
	}

        // status message
	if(le.MouseCursor(rectAttackSkill)) statusBar.ShowMessage("View Attack Skill Info");
	else
	if(le.MouseCursor(rectDefenseSkill)) statusBar.ShowMessage("View Defense Skill Info");
	else
	if(le.MouseCursor(rectSpellSkill)) statusBar.ShowMessage("View Spell Power Info");
	else
	if(le.MouseCursor(rectKnowledgeSkill)) statusBar.ShowMessage("View Knowledge Info");
	else
        if(le.MouseCursor(buttonExit)) statusBar.ShowMessage("Exit hero");
        else
        if(le.MouseCursor(buttonDismiss)) statusBar.ShowMessage("Dismiss hero");
        else
        if(le.MouseCursor(buttonPrevHero)) statusBar.ShowMessage("Show prev heroes");
        else
        if(le.MouseCursor(buttonNextHero)) statusBar.ShowMessage("Show next heroes");
        else
	// status message over artifact
	if(le.MouseCursor(rectMaxCoordsArtifact))
	{
	    for(u8 ii = 0; ii < coordsArtifact.size(); ++ii) if(le.MouseCursor(coordsArtifact[ii]))
	    {
		if(ii < artifacts.size() && Artifact::UNKNOWN != artifacts[ii])
		    statusBar.ShowMessage("View " + Artifact::String(artifacts[ii]) + " Info");
		else
		    statusBar.Clear();
	    }
	}
	else
	// status message over skill
	if(le.MouseCursor(rectMaxCoordsSkill))
	{
	    for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MouseCursor(coordsSkill[ii]))
	    {
		const Skill::skill_t skill = skills[ii].GetSkill();
		const Skill::level_t level = skills[ii].GetLevel();

		if(Skill::NONE != skill)
		    statusBar.ShowMessage("View " + Skill::LevelString(level) + " " + Skill::String(skill) + " Info");
		else
		    statusBar.Clear();
	    }
	}
	else
	// status message over troops
	if(le.MouseCursor(coordsHeroesTroops[0]) ||
	   le.MouseCursor(coordsHeroesTroops[1]) ||
	   le.MouseCursor(coordsHeroesTroops[2]) ||
	   le.MouseCursor(coordsHeroesTroops[3]) ||
	   le.MouseCursor(coordsHeroesTroops[4]))
	{
	    const u8 heroes_select = selectHeroesTroops.GetCursorIndex();

	    for(u8 ii = 0; ii < coordsHeroesTroops.size(); ++ii) if(le.MouseCursor(coordsHeroesTroops[ii]))
	    {
		if(selectHeroesTroops.isSelected() && ii == heroes_select)
		    statusBar.ShowMessage("View " + Monster::String(army[ii].GetMonster()));
		else
		if(selectHeroesTroops.isSelected() && army[ii].Valid() && army[heroes_select].Valid())
		    army[heroes_select].GetMonster() == army[ii].GetMonster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army[ii].GetMonster()) + " armies") :
			statusBar.ShowMessage("Exchange " + Monster::String(army[ii].GetMonster()) + " with " + Monster::String(army[heroes_select].GetMonster()));
		else
		if(selectHeroesTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String(army[heroes_select].GetMonster()));
		else
		if(army[ii].Valid())
		    statusBar.ShowMessage("Select " + Monster::String(army[ii].GetMonster()));
		else
		    statusBar.ShowMessage("Empty");
	    }
	}
	else
        // clear all
        if(! statusBar.isEmpty()) statusBar.Clear();
    }

    le.ResetKey();
    
    Cursor::Show();

    return result;
}
