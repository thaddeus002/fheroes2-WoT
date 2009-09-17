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
#include "pocketpc.h"

/* readonly: false, fade: false */
Dialog::answer_t Heroes::OpenDialog(bool readonly, bool fade)
{
    if(Settings::Get().PocketPC()) return PocketPC::HeroesOpenDialog(*this, readonly);

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    frameborder.Redraw();

    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    // fade
    if(fade && Settings::Get().Modes(Settings::FADE)) display.Fade();
    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    display.Blit(AGG::GetICN(ICN::HEROBKG, 0), dst_pt);

    display.Blit(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::HEROEXTE : ICN::HEROEXTG, 0), dst_pt);

    std::string message;

    // portrait
    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 31;
    display.Blit(GetPortrait101x93(), dst_pt);

    // name
    message = _("%{name} the %{race} ( Level %{level} )");
    String::Replace(message, "%{name}", name);
    String::Replace(message, "%{race}", Race::String(race));
    String::Replace(message, "%{level}", GetLevel());
    Text text(message, Font::BIG);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 1);

    // attack
    message = _("Attack Skill");
    text.Set(message, Font::SMALL);
    dst_pt.x = cur_pt.x + 196;
    dst_pt.y = cur_pt.y + 34;
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);

    message.clear();
    String::AddInt(message, GetAttack());
    text.Set(message, Font::BIG);
    dst_pt.y += 70;
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);
    
    const Rect rectAttackSkill(cur_pt.x + 156, cur_pt.y + 30, 80, 92);

    // defense
    message = _("Defense Skill");
    dst_pt.x = cur_pt.x + 284;
    dst_pt.y = cur_pt.y + 34;
    text.Set(message, Font::SMALL);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.y += 70;
    text.Set(message, Font::BIG);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);

    const Rect rectDefenseSkill(cur_pt.x + 156 + 88, cur_pt.y + 30, 80, 92);
    
    // spell
    message = _("Spell Power");
    dst_pt.x = cur_pt.x + 372;
    dst_pt.y = cur_pt.y + 34;
    text.Set(message, Font::SMALL);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.y += 70;
    text.Set(message, Font::BIG);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);

    const Rect rectSpellSkill(cur_pt.x + 156 + 2 * 88, cur_pt.y + 30, 80, 92);

    // knowledge
    message = _("Knowledge");
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 34;
    text.Set(message, Font::SMALL);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);
    
    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.y += 70;
    text.Set(message, Font::BIG);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);

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
    const std::string descriptionSpreadArmyFormat = _("'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army.");
    const Point army1_pt(dst_pt.x - 1, dst_pt.y - 1);
    
    // army format grouped
    dst_pt.x = cur_pt.x + 552;
    dst_pt.y = cur_pt.y + 63;
    const Sprite & sprite2 = AGG::GetICN(ICN::HSICONS, 10);
    display.Blit(sprite2, dst_pt);

    const Rect rectGroupedArmyFormat(dst_pt, sprite2.w(), sprite2.h());    
    const std::string descriptionGroupedArmyFormat = _("'Grouped' combat formation bunches your army together in the center of your side of the battlefield.");
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
    text.Set(message, Font::SMALL);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);
    
    std::string headerExperience = _("Level %{level}");
    String::Replace(headerExperience, "%{level}", GetLevel());
    
    std::string descriptionExperience = _("Current experience %{exp1} Next level %{exp2}.");
    String::Replace(descriptionExperience, "%{exp1}", GetExperience());
    String::Replace(descriptionExperience, "%{exp2}", GetExperienceFromLevel(GetLevelFromExperience(experience)));

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
    text.Set(message, Font::SMALL);
    text.Blit(dst_pt.x - text.w() / 2, dst_pt.y);

    std::string descriptionSpellPoints = _("%{name} currently has %{point} spell points out of a maximum of %{max}. The maximum number of spell points is 10 times your knowledge. It is occasionally possible to have more than your maximum spell points via special events.");
    String::Replace(descriptionSpellPoints, "%{name}", GetName());
    String::Replace(descriptionSpellPoints, "%{point}", GetSpellPoints());
    String::Replace(descriptionSpellPoints, "%{max}", GetMaxSpellPoints());

    // crest
    dst_pt.x = cur_pt.x + 49;
    dst_pt.y = cur_pt.y + 130;

    display.Blit(AGG::GetICN(ICN::CREST, Color::GetIndex(color)), dst_pt);
    
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
	    text.Set(message, Font::SMALL);
	    text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) / 2, dst_pt.y + 3);

	    // string level
	    message = Skill::Level::String(level);
	    text.Set(message);
	    text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) / 2, dst_pt.y + 50);
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

    // button prev
    dst_pt.x = cur_pt.x + 1;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonPrevHero(dst_pt, ICN::HSBTNS, 4, 5);

    // button next
    dst_pt.x = cur_pt.x + 640 - 23;
    dst_pt.y = cur_pt.y + 480 - 20;
    Button buttonNextHero(dst_pt, ICN::HSBTNS, 6, 7);
    
    // button dismiss
    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 318;
    Button buttonDismiss(dst_pt, ICN::HSBTNS, 0, 1);

    // button exit
    dst_pt.x = cur_pt.x + 603;
    dst_pt.y = cur_pt.y + 318;
    Button buttonExit(dst_pt, ICN::HSBTNS, 2, 3);

    LocalEvent & le = LocalEvent::Get();

    if(castle || readonly)
    {
	buttonDismiss.Press();
	buttonDismiss.SetDisable(true);
    }

    if(readonly || 2 > world.GetMyKingdom().GetHeroes().size())
    {
        buttonNextHero.Press();
        buttonPrevHero.Press();
        buttonNextHero.SetDisable(true);
        buttonPrevHero.SetDisable(true);
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
	if(buttonDismiss.isEnable()) le.MousePressLeft(buttonDismiss) ? buttonDismiss.PressDraw() : buttonDismiss.ReleaseDraw();
    	if(buttonPrevHero.isEnable()) le.MousePressLeft(buttonPrevHero) ? buttonPrevHero.PressDraw() : buttonPrevHero.ReleaseDraw();
    	if(buttonNextHero.isEnable()) le.MousePressLeft(buttonNextHero) ? buttonNextHero.PressDraw() : buttonNextHero.ReleaseDraw();

    	// prev hero
	if(buttonPrevHero.isEnable() && le.MouseClickLeft(buttonPrevHero)){ return Dialog::PREV; }

    	// next hero
    	if(buttonNextHero.isEnable() && le.MouseClickLeft(buttonNextHero)){ return Dialog::NEXT; }
    	    
    	// dismiss
    	if(buttonDismiss.isEnable() && le.MouseClickLeft(buttonDismiss) &&
    	      Dialog::YES == Dialog::Message(GetName(), _("Are you sure you want to dismiss this Hero?"), Font::BIG, Dialog::YES | Dialog::NO))
    	    { return Dialog::DISMISS; }

        if(le.MouseCursor(moraleIndicator.GetArea())) MoraleIndicator::QueueEventProcessing(moraleIndicator);
        else
        if(le.MouseCursor(luckIndicator.GetArea())) LuckIndicator::QueueEventProcessing(luckIndicator);

	// left click info
        if(le.MouseClickLeft(rectAttackSkill)) Dialog::Message(_("Attack Skill"), _("Your attack skill is a bonus added to each creature's attack skill."), Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectDefenseSkill)) Dialog::Message(_("Defense Skill"), _("Your defense skill is a bonus added to each creature's defense skill."), Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellSkill)) Dialog::Message(_("Spell Power"), _("Your spell power determines the length or power of a spell."), Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectKnowledgeSkill)) Dialog::Message(_("Knowledge"), _("Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge."), Font::BIG, Dialog::OK);
	else
        if(le.MouseClickLeft(rectExperienceInfo)) Dialog::Message(headerExperience, descriptionExperience, Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellPointsInfo)) Dialog::Message(_("Spell Points"), descriptionSpellPoints, Font::BIG, Dialog::OK);
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
        if(le.MousePressRight(rectAttackSkill)) Dialog::Message(_("Attack Skill"), _("Your attack skill is a bonus added to each creature's attack skill."), Font::BIG);
        else
        if(le.MousePressRight(rectDefenseSkill)) Dialog::Message(_("Defense Skill"), _("Your defense skill is a bonus added to each creature's defense skill."), Font::BIG);
        else
        if(le.MousePressRight(rectSpellSkill)) Dialog::Message(_("Spell Power"), _("Your spell power determines the length or power of a spell."), Font::BIG);
        else
        if(le.MousePressRight(rectKnowledgeSkill)) Dialog::Message(_("Knowledge"), _("Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge."), Font::BIG);
	else
        if(le.MousePressRight(rectExperienceInfo)) Dialog::Message(headerExperience, descriptionExperience, Font::BIG);
        else
        if(le.MousePressRight(rectSpellPointsInfo)) Dialog::Message(_("Spell Points"), descriptionSpellPoints, Font::BIG);
        else
        if(le.MousePressRight(rectSpreadArmyFormat)) Dialog::Message(_("Spread Formation"), descriptionSpreadArmyFormat, Font::BIG);
        else
        if(le.MousePressRight(rectGroupedArmyFormat)) Dialog::Message(_("Grouped Formation"), descriptionGroupedArmyFormat, Font::BIG);

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
	if(le.MouseCursor(rectAttackSkill)) statusBar.ShowMessage(_("View Attack Skill Info"));
	else
	if(le.MouseCursor(rectDefenseSkill)) statusBar.ShowMessage(_("View Defense Skill Info"));
	else
	if(le.MouseCursor(rectSpellSkill)) statusBar.ShowMessage(_("View Spell Power Info"));
	else
	if(le.MouseCursor(rectKnowledgeSkill)) statusBar.ShowMessage(_("View Knowledge Info"));
	else
	if(le.MouseCursor(moraleIndicator.GetArea())) statusBar.ShowMessage(_("View Morale Info"));
	else
	if(le.MouseCursor(luckIndicator.GetArea())) statusBar.ShowMessage(_("View Luck Info"));
	else
	if(le.MouseCursor(rectExperienceInfo)) statusBar.ShowMessage(_("View Experience Info"));
	else
	if(le.MouseCursor(rectSpellPointsInfo)) statusBar.ShowMessage(_("View Spell Points Info"));
	else
	if(le.MouseCursor(rectSpreadArmyFormat)) statusBar.ShowMessage(_("Set army combat formation to 'Spread'"));
	else
	if(le.MouseCursor(rectGroupedArmyFormat)) statusBar.ShowMessage(_("Set army combat formation to 'Grouped'"));
	else
        if(le.MouseCursor(buttonExit)) statusBar.ShowMessage(_("Exit hero"));
        else
        if(le.MouseCursor(buttonDismiss)) statusBar.ShowMessage(_("Dismiss hero"));
        else
        if(le.MouseCursor(buttonPrevHero)) statusBar.ShowMessage(_("Show prev heroes"));
        else
        if(le.MouseCursor(buttonNextHero)) statusBar.ShowMessage(_("Show next heroes"));
        else
	// status message over artifact
	if(le.MouseCursor(selectArtifacts.GetArea()))
	{
	    const s8 index = selectArtifacts.GetIndexFromCoord(le.GetMouseCursor());
	    if(0 <= index && index < HEROESMAXARTIFACT && artifacts[index] != Artifact::UNKNOWN)
	    {
		std::string str = _("View %{art} Info");
		String::Replace(str, "%{art}", artifacts[index].GetName());
		statusBar.ShowMessage(str);
	    }
	    else
		statusBar.ShowMessage(_("Hero Screen"));
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
		{
		    std::string str = _("View %{level} %{skill} Info");
		    String::Replace(str, "%{level}", Skill::Level::String(level));
		    String::Replace(str, "%{skill}", Skill::Secondary::String(skill));
		    statusBar.ShowMessage(str);
		}
		else
		    statusBar.ShowMessage(_("Hero Screen"));
	    }
	}
	else
        // status message over troops
        if(le.MouseCursor(selectArmy.GetArea()))
        {
            const s8 index1 = selectArmy.GetIndexFromCoord(le.GetMouseCursor());
            if(0 <= index1)
            {
                const Army::Troop & troop1 = army.At(index1);
                const std::string & monster1 = troop1.GetName();
                std::string str;

                if(selectArmy.isSelected())
                {
                    const u8 index2 = selectArmy.Selected();
                    const Army::Troop & troop2 = army.At(index2);
                    const std::string & monster2 = troop2.GetName();

                    if(index1 == index2)
            	    {
                	str = _("View %{monster}");
                	String::Replace(str, "%{monster}", monster1);
            	    }
                    else
                    if(troop1.isValid() && troop2.isValid())
                    {
                        str = troop1() == troop2() ? _("Combine %{monster1} armies") : _("Exchange %{monster2} with %{monster1}");
                	String::Replace(str, "%{monster1}", monster1);
                	String::Replace(str, "%{monster2}", monster2);
                    }
                    else
                    {
                        str = _("Move and right click Redistribute %{monster}");
                	String::Replace(str, "%{monster}", monster2);
                    }
                }
                else
                if(troop1.isValid())
                {
                    str = _("Select %{monster}");
                    String::Replace(str, "%{monster}", monster1);
                }
                else
                    str = _("Empty");

                statusBar.ShowMessage(str);
            }
	    else
		statusBar.ShowMessage(_("Hero Screen"));
        }
        else
        // clear all
        statusBar.ShowMessage(_("Hero Screen"));
    }

    return Dialog::ZERO;
}
