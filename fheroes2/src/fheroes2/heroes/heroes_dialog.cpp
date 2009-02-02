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
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "settings.h"
#include "payment.h"
#include "heroes.h"
#include "skill.h"
#include "kingdom.h"
#include "text.h"
#include "castle.h"
#include "portrait.h"
#include "dialog.h"
#include "heroes_indicator.h"
#include "selectarmybar.h"
#include "statusbar.h"
#include "selectartifactbar.h"

Dialog::answer_t Heroes::OpenDialog(bool readonly)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder frameborder;

    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    display.Blit(AGG::GetICN(ICN::HEROBKG, 0), dst_pt);

    display.Blit(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::HEROEXTE : ICN::HEROEXTG, 0), dst_pt);

    std::string message;

    // portrait
    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 31;
    display.Blit(Portrait::Hero(hid, Portrait::BIG), dst_pt);

    // name
    message = name + " the " + Race::String(race) + " ( Level ";
    String::AddInt(message, GetLevel());
    message += " )";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 1;
    Text(message, Font::BIG, dst_pt);

    u8 index_sprite = 0;

    // attack
    message = "Attack Skill";
    dst_pt.x = cur_pt.x + 196;
    dst_pt.y = cur_pt.y + 34;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);

    message.clear();
    String::AddInt(message, GetAttack());
    dst_pt.y += 70;
    Text(message, Font::BIG, dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y);
    
    const Rect rectAttackSkill(cur_pt.x + 156, cur_pt.y + 30, 80, 92);

    // defense
    message = "Defense Skill";
    dst_pt.x = cur_pt.x + 284;
    dst_pt.y = cur_pt.y + 34;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.y += 70;
    Text(message, Font::BIG, dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y);

    const Rect rectDefenseSkill(cur_pt.x + 156 + 88, cur_pt.y + 30, 80, 92);
    
    // spell
    message = "Spell Power";
    dst_pt.x = cur_pt.x + 372;
    dst_pt.y = cur_pt.y + 34;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.y += 70;
    Text(message, Font::BIG, dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y);

    const Rect rectSpellSkill(cur_pt.x + 156 + 2 * 88, cur_pt.y + 30, 80, 92);

    // knowledge
    message = "Knowledge";
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 34;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.y += 70;
    Text(message, Font::BIG, dst_pt.x - Text::width(message, Font::BIG) / 2, dst_pt.y);

    const Rect rectKnowledgeSkill(cur_pt.x + 156 + 3 * 88, cur_pt.y + 30, 80, 92);

    // morale
    dst_pt.x = cur_pt.x + 514;
    dst_pt.y = cur_pt.y + 35;

    MoraleIndicator moraleIndicator(*this);
    moraleIndicator.SetPos(dst_pt);
    moraleIndicator.Redraw();

    // luck
    dst_pt.x = cur_pt.x + 552;
    dst_pt.y = cur_pt.y + 35;

    LuckIndicator luckIndicator(*this);
    luckIndicator.SetPos(dst_pt);
    luckIndicator.Redraw();

    // army format spread
    dst_pt.x = cur_pt.x + 515;
    dst_pt.y = cur_pt.y + 63;
    const Sprite & sprite1 = AGG::GetICN(ICN::HSICONS, 9);
    display.Blit(sprite1, dst_pt);

    const Rect rectSpreadArmyFormat(dst_pt, sprite1.w(), sprite1.h());
    const std::string descriptionSpreadArmyFormat("'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army.");
    const Point army1_pt(dst_pt.x - 1, dst_pt.y - 1);
    
    // army format grouped
    dst_pt.x = cur_pt.x + 552;
    dst_pt.y = cur_pt.y + 63;
    const Sprite & sprite2 = AGG::GetICN(ICN::HSICONS, 10);
    display.Blit(sprite2, dst_pt);

    const Rect rectGroupedArmyFormat(dst_pt, sprite2.w(), sprite2.h());    
    const std::string descriptionGroupedArmyFormat("'Grouped' combat formation bunches your army together in the center of your side of the battlefield.");
    const Point army2_pt(dst_pt.x - 1, dst_pt.y - 1);

    // cursor format
    SpriteCursor cursorFormat(AGG::GetICN(ICN::HSICONS, 11), Modes(ARMYSPREAD) ? army1_pt : army2_pt);
    cursorFormat.Show(Modes(ARMYSPREAD) ? army1_pt : army2_pt);

    // experience
    dst_pt.x = cur_pt.x + 512;
    dst_pt.y = cur_pt.y + 85;
    const Sprite & sprite3 = AGG::GetICN(ICN::HSICONS, 1);
    const Rect rectExperienceInfo(dst_pt, sprite3.w(), sprite3.h());
    display.Blit(sprite3, dst_pt);

    message.clear();
    String::AddInt(message, GetExperience());
    dst_pt.x = cur_pt.x + 528;
    dst_pt.y = cur_pt.y + 107;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);
    
    std::string headerExperience("Level ");
    String::AddInt(headerExperience, GetLevel());
    
    std::string descriptionExperience("Current experience ");
    String::AddInt(descriptionExperience, GetExperience());
    descriptionExperience += " Next level ";
    String::AddInt(descriptionExperience, GetExperienceFromLevel(GetLevelFromExperience(experience)));
    descriptionExperience += ".";

    // spell points
    dst_pt.x = cur_pt.x + 550;
    dst_pt.y = cur_pt.y + 87;
    const Sprite & sprite4 = AGG::GetICN(ICN::HSICONS, 8);
    const Rect rectSpellPointsInfo(dst_pt, sprite4.w(), sprite4.h());
    display.Blit(sprite4, dst_pt);

    message.clear();
    String::AddInt(message, GetSpellPoints());
    message += "/";
    String::AddInt(message, GetMaxSpellPoints());
    dst_pt.x = cur_pt.x + 568;
    dst_pt.y = cur_pt.y + 107;
    Text(message, Font::SMALL, dst_pt.x - Text::width(message, Font::SMALL) / 2, dst_pt.y);

    std::string descriptionSpellPoints(name + " currently has ");
    String::AddInt(descriptionSpellPoints, GetSpellPoints());
    descriptionSpellPoints += " spell points out of a maximum of ";
    String::AddInt(descriptionSpellPoints, GetMaxSpellPoints());
    descriptionSpellPoints += ". The maximum number of spell points is 10 times your knowledge. It is occasionally possible to have more than your maximum spell points via special events.";

    // crest
    if(Color::GRAY == color)
	switch(GetColor())
	{
	    case Color::BLUE:	index_sprite = 0; break;
	    case Color::GREEN:	index_sprite = 1; break;
	    case Color::RED:	index_sprite = 2; break;
	    case Color::YELLOW:	index_sprite = 3; break;
	    case Color::ORANGE:	index_sprite = 4; break;
	    case Color::PURPLE:	index_sprite = 5; break;
	    default: break;
	}
    else    
	switch(color)
        {
	    case Color::BLUE:	index_sprite = 0; break;
	    case Color::GREEN:	index_sprite = 1; break;
	    case Color::RED:	index_sprite = 2; break;
	    case Color::YELLOW:	index_sprite = 3; break;
	    case Color::ORANGE:	index_sprite = 4; break;
	    case Color::PURPLE:	index_sprite = 5; break;
	    default: break;
	}

    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 130;

    display.Blit(AGG::GetICN(ICN::CREST, index_sprite), dst_pt);
    
    // monster
    dst_pt.x = cur_pt.x + 156;
    dst_pt.y = cur_pt.y + 130;

    SelectArmyBar selectArmy;
    selectArmy.SetArmy(army);
    selectArmy.SetPos(dst_pt);
    selectArmy.SetInterval(6);
    selectArmy.SetBackgroundSprite(AGG::GetICN(ICN::STRIP, 2));
    selectArmy.SetCursorSprite(AGG::GetICN(ICN::STRIP, 1));
    selectArmy.SetSaveLastTroop();
    if(readonly) selectArmy.SetReadOnly();
    const Castle* castle = inCastle();
    if(castle) selectArmy.SetCastle(*castle);
    selectArmy.Redraw();

    // secskill
    dst_pt.x = cur_pt.x + 3;
    dst_pt.y = cur_pt.y + 233;
    
    std::vector<Rect> coordsSkill;

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
	const Skill::Secondary::skill_t skill = ii < secondary_skills.size() ? secondary_skills[ii].Skill() : Skill::Secondary::UNKNOWN;
	const Skill::Level::type_t level = ii < secondary_skills.size() ? secondary_skills[ii].Level() : Skill::Level::NONE;

	const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(skill));

	display.Blit(sprite_skill, dst_pt);

	if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
	{
	    // string skill
	    message = Skill::Secondary::String(skill);
	    Text(message, Font::SMALL, dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) / 2, dst_pt.y + 3);

	    // string level
	    message = Skill::Level::String(level);
	    Text(message, Font::SMALL, dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) / 2, dst_pt.y + 50);
	}

	coordsSkill.push_back(Rect(dst_pt.x, dst_pt.y, sprite_skill.w(), sprite_skill.h()));

	dst_pt.x += sprite_skill.w() + 5;
    }

    const Rect rectMaxCoordsSkill(coordsSkill);

    dst_pt.x = cur_pt.x + 51;
    dst_pt.y = cur_pt.y + 308;

    SelectArtifactsBar selectArtifacts(*this);
    selectArtifacts.SetPos(dst_pt);
    selectArtifacts.SetInterval(15);
    selectArtifacts.SetBackgroundSprite(AGG::GetICN(ICN::ARTIFACT, 0));
    selectArtifacts.SetCursorSprite(AGG::GetICN(ICN::NGEXTRA, 62));
    if(readonly) selectArtifacts.SetReadOnly();
    selectArtifacts.Redraw();

    // bottom small bar
    dst_pt.x = cur_pt.x + 22;
    dst_pt.y = cur_pt.y + 460;
    const Sprite & bar = AGG::GetICN(ICN::HSBTNS, 8);
    display.Blit(bar, dst_pt);

    StatusBar statusBar;
    statusBar.SetCenter(dst_pt.x + bar.w() / 2, dst_pt.y + 11);

    u16 index1 = readonly ? 5 : 4;
    u16 index2 = 5;

    // button prev
    dst_pt.x = cur_pt.x + 1;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonPrevHero(dst_pt, ICN::HSBTNS, index1, index2);

    index1 = readonly ? 7 : 6;
    index2 = 7;

    // button next
    dst_pt.x = cur_pt.x + 640 - 23;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonNextHero(dst_pt, ICN::HSBTNS, index1, index2);
    
    index1 = readonly ? 1 : 0;
    index2 = 1;

    // button dismiss
    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 318;
    Button buttonDismiss(dst_pt, ICN::HSBTNS, index1, index2);

    // button exit
    dst_pt.x = cur_pt.x + 603;
    dst_pt.y = cur_pt.y + 318;
    Button buttonExit(dst_pt, ICN::HSBTNS, 2, 3);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    if(2 > world.GetMyKingdom().GetHeroes().size())
    {
	buttonPrevHero.Press();
	buttonPrevHero.SetDisable(true);

	buttonNextHero.Press();
	buttonNextHero.SetDisable(true);
    }

    if(inCastle())
    {
	buttonDismiss.Press();
	buttonDismiss.SetDisable(true);
    }

    buttonPrevHero.Draw();
    buttonNextHero.Draw();
    buttonDismiss.Draw();
    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    bool redrawMorale = false;
    bool redrawLuck = false;

    // dialog menu loop
    while(le.HandleEvents())
    {
	if(redrawMorale)
	{
	    cursor.Hide();
	    moraleIndicator.Redraw();
	    cursor.Show();
	    display.Flip();
	    redrawMorale = false;
	}

	if(redrawLuck)
	{
	    cursor.Hide();
	    luckIndicator.Redraw();
	    cursor.Show();
	    display.Flip();
	    redrawLuck = false;
	}

        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_ESCAPE)) return Dialog::CANCEL;

        // heroes troops
        if(le.MouseCursor(selectArmy.GetArea()))
        {
            if(SelectArmyBar::QueueEventProcessing(selectArmy))
            {
        	redrawMorale = true;
        	redrawLuck = true;
    	    }
	}

        if(le.MouseCursor(selectArtifacts.GetArea()))
        {
            SelectArtifactsBar::QueueEventProcessing(selectArtifacts);
            {
        	redrawMorale = true;
        	redrawLuck = true;
    	    }
	}

        // button click
	le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();
        if(!readonly)
	{
	    if(buttonDismiss.isEnable()) le.MousePressLeft(buttonDismiss) ? buttonDismiss.PressDraw() : buttonDismiss.ReleaseDraw();
    	    if(buttonPrevHero.isEnable()) le.MousePressLeft(buttonPrevHero) ? buttonPrevHero.PressDraw() : buttonPrevHero.ReleaseDraw();
    	    if(buttonNextHero.isEnable()) le.MousePressLeft(buttonNextHero) ? buttonNextHero.PressDraw() : buttonNextHero.ReleaseDraw();

    	    // prev hero
	    if(buttonPrevHero.isEnable() && le.MouseClickLeft(buttonPrevHero)){ return Dialog::PREV; }

    	    // next hero
    	    if(buttonNextHero.isEnable() && le.MouseClickLeft(buttonNextHero)){ return Dialog::NEXT; }
    	    
    	    // dismiss
    	    if(buttonDismiss.isEnable() && le.MouseClickLeft(buttonDismiss) &&
    	      Dialog::YES == Dialog::Message(GetName(), "Are you sure you want to dismiss this Hero?", Font::BIG, Dialog::YES | Dialog::NO))
    	    { return Dialog::DISMISS; }
	}

        if(le.MouseCursor(moraleIndicator.GetArea())) MoraleIndicator::QueueEventProcessing(moraleIndicator);
        else
        if(le.MouseCursor(luckIndicator.GetArea())) LuckIndicator::QueueEventProcessing(luckIndicator);

	// left click info
        if(le.MouseClickLeft(rectAttackSkill)) Dialog::Message("Attack Skill", "Your attack skill is a bonus added to each creature's attack skill.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectDefenseSkill)) Dialog::Message("Defense Skill", "Your defense skill is a bonus added to each creature's defense skill.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellSkill)) Dialog::Message("Spell Power", "Your spell power determines the length or power of a spell.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectKnowledgeSkill)) Dialog::Message("Knowledge", "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.", Font::BIG, Dialog::OK);
	else
        if(le.MouseClickLeft(rectExperienceInfo)) Dialog::Message(headerExperience, descriptionExperience, Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellPointsInfo)) Dialog::Message("Spell Points", descriptionSpellPoints, Font::BIG, Dialog::OK);
	else
        if(!readonly && le.MouseClickLeft(rectSpreadArmyFormat) && !Modes(ARMYSPREAD))
        {
	    cursor.Hide();
	    cursorFormat.Move(army1_pt);
	    cursor.Show();
	    display.Flip();
    	    SetModes(ARMYSPREAD);
        }
	else
        if(!readonly && le.MouseClickLeft(rectGroupedArmyFormat) && Modes(ARMYSPREAD))
        {
	    cursor.Hide();
	    cursorFormat.Move(army2_pt);
	    cursor.Show();
	    display.Flip();
    	    ResetModes(ARMYSPREAD);
        }

	// left click skill
	for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MouseClickLeft(coordsSkill[ii]))
	{
	    const Skill::Secondary::skill_t skill = ii < secondary_skills.size() ? secondary_skills[ii].Skill() : Skill::Secondary::UNKNOWN;
	    const Skill::Level::type_t level = ii < secondary_skills.size() ? secondary_skills[ii].Level() : Skill::Level::NONE;

	    if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
	    {
		cursor.Hide();
		Dialog::SkillInfo(skill, level, true);
		cursor.Show();
		display.Flip();
	    }
	}

	// right info
        if(le.MousePressRight(rectAttackSkill)) Dialog::Message("Attack Skill", "Your attack skill is a bonus added to each creature's attack skill.", Font::BIG);
        else
        if(le.MousePressRight(rectDefenseSkill)) Dialog::Message("Defense Skill", "Your defense skill is a bonus added to each creature's defense skill.", Font::BIG);
        else
        if(le.MousePressRight(rectSpellSkill)) Dialog::Message("Spell Power", "Your spell power determines the length or power of a spell.", Font::BIG);
        else
        if(le.MousePressRight(rectKnowledgeSkill)) Dialog::Message("Knowledge", "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.", Font::BIG);
	else
        if(le.MousePressRight(rectExperienceInfo)) Dialog::Message(headerExperience, descriptionExperience, Font::BIG);
        else
        if(le.MousePressRight(rectSpellPointsInfo)) Dialog::Message("Spell Points", descriptionSpellPoints, Font::BIG);
        else
        if(le.MousePressRight(rectSpreadArmyFormat)) Dialog::Message("Spread Formation", descriptionSpreadArmyFormat, Font::BIG);
        else
        if(le.MousePressRight(rectGroupedArmyFormat)) Dialog::Message("Grouped Formation", descriptionGroupedArmyFormat, Font::BIG);

	// right info skill
	for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MousePressRight(coordsSkill[ii]))
	{
	    const Skill::Secondary::skill_t skill = ii < secondary_skills.size() ? secondary_skills[ii].Skill() : Skill::Secondary::UNKNOWN;
	    const Skill::Level::type_t level = ii < secondary_skills.size() ? secondary_skills[ii].Level() : Skill::Level::NONE;

	    if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
	    {
		cursor.Hide();
		Dialog::SkillInfo(skill, level, false);
		cursor.Show();
		display.Flip();
	    }
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
	if(le.MouseCursor(moraleIndicator.GetArea())) statusBar.ShowMessage("View Morale Info");
	else
	if(le.MouseCursor(luckIndicator.GetArea())) statusBar.ShowMessage("View Luck Info");
	else
	if(le.MouseCursor(rectExperienceInfo)) statusBar.ShowMessage("View Experience Info");
	else
	if(le.MouseCursor(rectSpellPointsInfo)) statusBar.ShowMessage("View Spell Points Info");
	else
	if(le.MouseCursor(rectSpreadArmyFormat)) statusBar.ShowMessage("Set army combat formation to 'Spread'");
	else
	if(le.MouseCursor(rectGroupedArmyFormat)) statusBar.ShowMessage("Set army combat formation to 'Grouped'");
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
	if(le.MouseCursor(selectArtifacts.GetArea()))
	{
	    const s8 index = selectArtifacts.GetIndexFromCoord(le.MouseCursor());
	    if(0 <= index && index < HEROESMAXARTIFACT && Artifact::UNKNOWN != artifacts[index])
		statusBar.ShowMessage("View " + Artifact::String(artifacts[index]) + " Info");
	    else
		statusBar.ShowMessage("Hero Screen");
	}
	else
	// status message over skill
	if(le.MouseCursor(rectMaxCoordsSkill))
	{
	    for(u8 ii = 0; ii < coordsSkill.size(); ++ii) if(le.MouseCursor(coordsSkill[ii]))
	    {
		const Skill::Secondary::skill_t skill = ii < secondary_skills.size() ? secondary_skills[ii].Skill() : Skill::Secondary::UNKNOWN;
		const Skill::Level::type_t level = ii < secondary_skills.size() ? secondary_skills[ii].Level() : Skill::Level::NONE;

		if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
		    statusBar.ShowMessage("View " + Skill::Level::String(level) + " " + Skill::Secondary::String(skill) + " Info");
		else
		    statusBar.ShowMessage("Hero Screen");
	    }
	}
	else
        // status message over troops
        if(le.MouseCursor(selectArmy.GetArea()))
        {
            const s8 index1 = selectArmy.GetIndexFromCoord(le.MouseCursor());
            if(0 <= index1)
            {
                const Army::Troop & troop1 = army.At(index1);
                const std::string & monster1 = troop1.GetName();

                if(selectArmy.isSelected())
                {
                    const u8 index2 = selectArmy.Selected();
                    const Army::Troop & troop2 = army.At(index2);
                    const std::string & monster2 = troop2.GetName();

                    if(index1 == index2)
                        statusBar.ShowMessage("View " + monster1);
                    else
                    if(troop1.isValid() && troop2.isValid())
                        troop1() == troop2() ?
                        statusBar.ShowMessage("Combine " + monster1 + " armies") :
                        statusBar.ShowMessage("Exchange " + monster2 + " with " + monster1);
                    else
                        statusBar.ShowMessage("Move and right click Redistribute " + monster2);
                }
                else
                if(troop1.isValid())
                    statusBar.ShowMessage("Select " + monster1);
                else
                    statusBar.ShowMessage("Empty");
            }
        }
        else
        // clear all
        statusBar.ShowMessage("Hero Screen");
    }

    return Dialog::ZERO;
}
