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
#include "spritecursor.h"
#include "skill.h"
#include "tools.h"
#include "text.h"
#include "error.h"
#include "castle.h"
#include "portrait.h"
#include "dialog.h"
#include "army.h"
#include "SDL.h"

void DrawLuckSprite(const Luck::luck_t luck, const Point & pt)
{
    Display & display = Display::Get();
    Point dst_pt(pt);

    switch(luck)
    {
	case Luck::CURSED:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 3);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::AWFUL:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 3);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 7;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::BAD:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 3);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::NORMAL:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 6);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::GOOD:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 2);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::GREAT:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 2);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 7;
	    display.Blit(sprite, dst_pt);
	} break;

	case Luck::IRISH:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 2);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	} break;

	default: break;
    }
}

void DrawMoraleSprite(const Morale::morale_t morale, const Point & pt)
{
    Display & display = Display::Get();
    Point dst_pt(pt);

    switch(morale)
    {
	case Morale::TREASON:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 5);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::AWFUL:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 5);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 7;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::POOR:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 5);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::NORMAL:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 7);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::GOOD:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 4);
	    dst_pt.x += (34 - sprite.w()) / 2;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::GREAT:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 4);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 7;
	    display.Blit(sprite, dst_pt);
	} break;

	case Morale::BLOOD:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, 4);
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	    dst_pt.x += 5;
	    display.Blit(sprite, dst_pt);
	} break;

	default: break;
    }
}

const char* MoraleString(const Morale::morale_t morale)
{
    switch(morale)
    {
	case Morale::TREASON:
	case Morale::AWFUL:
	case Morale::POOR:
	    return "Bad Morale";

	case Morale::NORMAL:
	    return "Neutral Morale";

	case Morale::GOOD:
	case Morale::GREAT:
	case Morale::BLOOD:
	    return "Good Morale";

	default: break;
    }
    return NULL;
}

const char* LuckString(const Luck::luck_t luck)
{
    switch(luck)
    {
	case Luck::CURSED:
	case Luck::AWFUL:
	case Luck::BAD:
	    return "Bad Luck";

	case Luck::NORMAL:
	    return "Neutral Luck";

	case Luck::GOOD:
	case Luck::GREAT:
	case Luck::IRISH:
	    return "Good Luck";

	default: break;
    }
    return NULL;
}

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

    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? ICN::HEROEXTE : ICN::HEROEXTG, 0), dst_pt);

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
    dst_pt.x = cur_pt.x + 515;
    dst_pt.y = cur_pt.y + 35;

    const Rect rectMoraleInfo(dst_pt, 34, 26);
    Background backgroundMorale(rectMoraleInfo);
    std::list<std::string> moraleModificators;
    Morale::morale_t current_morale = GetMoraleWithModificators(&moraleModificators);
    if(moraleModificators.size()) moraleModificators.push_front("Current Morale Modifiers:");
    moraleModificators.push_front(" ");
    moraleModificators.push_front(Morale::Description(current_morale));
    std::string headerMoraleInfo(MoraleString(current_morale));

    backgroundMorale.Save();
    DrawMoraleSprite(current_morale, rectMoraleInfo);

    // luck
    dst_pt.x = cur_pt.x + 552;
    dst_pt.y = cur_pt.y + 35;

    const Rect rectLuckInfo(dst_pt, 34, 26);
    Background backgroundLuck(rectLuckInfo);
    std::list<std::string> luckModificators;
    Luck::luck_t current_luck = GetLuckWithModificators(&luckModificators);
    if(luckModificators.size()) luckModificators.push_front("Current Morale Modifiers:");
    luckModificators.push_front(" ");
    luckModificators.push_front(Luck::Description(current_luck));
    std::string headerLuckInfo(LuckString(current_luck));

    backgroundLuck.Save();
    DrawLuckSprite(current_luck, dst_pt);

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
    SpriteCursor cursorFormat(AGG::GetICN(ICN::HSICONS, 11), army_spread ? army1_pt : army2_pt);
    cursorFormat.Show(army_spread ? army1_pt : army2_pt);

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
	switch(Settings::Get().MyColor())
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

    // artifact
    for(u8 ii = 0; ii < HEROESMAXARTIFACT; ii++)
    {
        // sprite
        const Sprite & art = AGG::GetICN(ICN::ARTIFACT, 0);

        dst_pt.x = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.x + 51 + ii * (art.w() + 15) : cur_pt.x + 51 + (ii - HEROESMAXARTIFACT / 2) * (art.w() + 15));
        dst_pt.y = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.y + 308 : cur_pt.y + 387);

        display.Blit(art, dst_pt);
    }

    std::vector<Rect> coordsArtifact;

    for(u8 ii = 0; ii < artifacts.size(); ii++)
    {
        // sprite
        const Sprite & art = AGG::GetICN(ICN::ARTIFACT, artifacts[ii] + 1);

        dst_pt.x = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.x + 51 + ii * (art.w() + 15) : cur_pt.x + 51 + (ii - HEROESMAXARTIFACT / 2) * (art.w() + 15));
        dst_pt.y = (ii < HEROESMAXARTIFACT / 2 ? cur_pt.y + 308 : cur_pt.y + 387);

        display.Blit(art, dst_pt);
	
	coordsArtifact.push_back(Rect(dst_pt.x, dst_pt.y, art.w(), art.h()));
    }
    const Rect rectMaxCoordsArtifact(coordsArtifact);

    // bottom small bar
    Dialog::StatusBar statusBar(Point(cur_pt.x + 22, cur_pt.y + 460), AGG::GetICN(ICN::HSBTNS, 8), Font::BIG);
    statusBar.Clear("Hero Screen");

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
	    backgroundMorale.Restore();
	    moraleModificators.clear();
	    current_morale = GetMoraleWithModificators(&moraleModificators);
	    if(moraleModificators.size()) moraleModificators.push_front("Current Morale Modifiers:");
	    moraleModificators.push_front(" ");
	    moraleModificators.push_front(Morale::Description(current_morale));
	    DrawMoraleSprite(current_morale, rectMoraleInfo);
	    headerMoraleInfo = MoraleString(current_morale);
	    cursor.Show();
	    display.Flip();
	    redrawMorale = false;
	}

	if(redrawLuck)
	{
	    cursor.Hide();
	    backgroundLuck.Restore();
	    luckModificators.clear();
	    current_luck = GetLuckWithModificators(&luckModificators);
	    if(luckModificators.size()) luckModificators.push_front("Current Morale Modifiers:");
	    luckModificators.push_front(" ");
	    luckModificators.push_front(Luck::Description(current_luck));
	    DrawLuckSprite(current_luck, dst_pt);
	    headerLuckInfo = LuckString(current_luck);
	    cursor.Show();
	    display.Flip();
	    redrawLuck = false;
	}

        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_ESCAPE)) {
	    Display::Get().Fade();
	    return Dialog::CANCEL;
	}

	// heroes troops
	for(u8 ii = 0; ii < HEROESMAXARMY; ++ii)
	{
	    if(le.MouseClickLeft(coordsHeroesTroops[ii]))
	    {
		cursor.Hide();

		// show dialog army info
		if(selectHeroesTroops.isSelected() && Army::isValid(army[ii]) && selectHeroesTroops.GetCursorIndex() == ii)
		{
		    Army::Troop & select_troops = army[ii];
		    const Monster::monster_t select_monster = select_troops.Monster();
		    const u16 select_count = select_troops.Count();
		    Kingdom & kingdom = world.GetMyKingdom();

                    const Castle *castle = inCastle();
                    const bool show_upgrade = castle && Monster::AllowUpgrade(select_monster) &&
                    	    Monster::GetRace(select_monster) == castle->GetRace() &&
                            castle->isBuild(Castle::GetUpgradeBuilding(Monster::Dwelling(select_monster), castle->GetRace()));

                    switch(Dialog::ArmyInfo(army[ii], readonly? false : (1 == Army::GetCountTroops(army) ? false : true), false, show_upgrade))
		    {
			case Dialog::UPGRADE:
			    select_troops.SetMonster(Monster::Upgrade(select_monster));
		            kingdom.OddFundsResource(PaymentConditions::UpgradeMonster(select_monster) * select_count);
			    redrawMorale = true;
			    break;

			case Dialog::DISMISS:
			    select_troops.Set(Monster::UNKNOWN, 0);
			    redrawMorale = true;
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
		    if(! readonly)
		    {
			// from castle or heroes
			Army::Troop & select_troops = army[selectHeroesTroops.GetCursorIndex()];
			const Monster::monster_t select_monster = select_troops.Monster();
			const u16 select_count = select_troops.Count();

			// change or combine army
			if(Army::isValid(army[ii]))
			{
			    // change
			    if(army[ii].Monster() != select_monster)
			    {
				select_troops = army[ii];
				army[ii].Set(select_monster, select_count);
			    }
			    // combine
			    else
                    	    {
                        	army[ii].SetCount(army[ii].Count() + select_count);

                        	select_troops.Set(Monster::UNKNOWN, 0);
                    	    }

			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}
			// move to empty position
			else
			if(selectHeroesTroops.isSelected())
			{
			    army[ii] = select_troops;
			    select_troops.Set(Monster::UNKNOWN, 0);

			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}
		    }
		    else
		    {
			// select army
			if(Army::isValid(army[ii]))
			{
			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Select(ii);
			    selectHeroesTroops.Redraw();
			}

			cursor.Show();
			display.Flip();
		    }
		}
		else
		// select army
		if(Army::isValid(army[ii]))
		{
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Select(ii);
		    selectHeroesTroops.Redraw();
		}

		cursor.Show();
		display.Flip();
	    }
	    else
	    // right click empty troops - redistribute troops
	    if(le.MouseClickRight(coordsHeroesTroops[ii]) &&
		!Army::isValid(army[ii]) &&
		selectHeroesTroops.isSelected() &&
		!readonly)
	    {
		cursor.Hide();

		Army::Troop & select_troops = army[selectHeroesTroops.GetCursorIndex()];

		if(const u16 redistr_count = Dialog::SelectCount(select_troops.Count()))
		{
		    army[ii].Set(select_troops.Monster(), redistr_count);
		
		    select_troops.SetCount(select_troops.Count() - redistr_count);
		}

		cursor.Hide();

		selectHeroesTroops.Reset();
		selectHeroesTroops.Redraw();

		cursor.Show();
		display.Flip();
	    }
	    else
	    // press right - show quick info
	    if(le.MousePressRight(coordsHeroesTroops[ii]) && Army::isValid(army[ii]))
	    {
		cursor.Hide();

		Dialog::ArmyInfo(army[ii], readonly ? false : (1 == Army::GetCountTroops(army) ? false : true), true, false);

		cursor.Show();
		display.Flip();
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

	// left click info
        if(le.MouseClickLeft(rectAttackSkill)) Dialog::Message("Attack Skill", "Your attack skill is a bonus added to each creature's attack skill.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectDefenseSkill)) Dialog::Message("Defense Skill", "Your defense skill is a bonus added to each creature's defense skill.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellSkill)) Dialog::Message("Spell Power", "Your spell power determines the length or power of a spell.", Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectKnowledgeSkill)) Dialog::Message("Knowledge", "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.", Font::BIG, Dialog::OK);
	else
	if(le.MouseClickLeft(rectMoraleInfo)) Dialog::Message(headerMoraleInfo, moraleModificators, Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectLuckInfo)) Dialog::Message(headerLuckInfo, luckModificators, Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectExperienceInfo)) Dialog::Message(headerExperience, descriptionExperience, Font::BIG, Dialog::OK);
        else
        if(le.MouseClickLeft(rectSpellPointsInfo)) Dialog::Message("Spell Points", descriptionSpellPoints, Font::BIG, Dialog::OK);
	else
        if(!readonly && le.MouseClickLeft(rectSpreadArmyFormat) && !army_spread)
        {
	    cursor.Hide();
	    cursorFormat.Move(army1_pt);
	    cursor.Show();
	    display.Flip();
    	    army_spread = true;
        }
	else
        if(!readonly && le.MouseClickLeft(rectGroupedArmyFormat) && army_spread)
        {
	    cursor.Hide();
	    cursorFormat.Move(army2_pt);
	    cursor.Show();
	    display.Flip();
    	    army_spread = false;
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

	// left click artifact
	for(u8 ii = 0; ii < coordsArtifact.size(); ++ii) if(le.MouseClickLeft(coordsArtifact[ii]))
	{
	    const Artifact::artifact_t art = artifacts[ii];

	    if(Artifact::MAGIC_BOOK == art) spell_book.Open();
	    else
	    if(Artifact::UNKNOWN != art) Dialog::Message(Artifact::String(art), Artifact::Description(art), Font::BIG, Dialog::OK);
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
        if(le.MousePressRight(rectMoraleInfo)) Dialog::Message(headerMoraleInfo, moraleModificators, Font::BIG);
        else
        if(le.MousePressRight(rectLuckInfo)) Dialog::Message(headerLuckInfo, luckModificators, Font::BIG);
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
	if(le.MouseCursor(rectMoraleInfo)) statusBar.ShowMessage("View Morale Info");
	else
	if(le.MouseCursor(rectLuckInfo)) statusBar.ShowMessage("View Luck Info");
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
	if(le.MouseCursor(rectMaxCoordsArtifact))
	{
	    for(u8 ii = 0; ii < coordsArtifact.size(); ++ii) if(le.MouseCursor(coordsArtifact[ii]))
	    {
		if(ii < artifacts.size() && Artifact::UNKNOWN != artifacts[ii])
		    statusBar.ShowMessage("View " + Artifact::String(artifacts[ii]) + " Info");
		else
		    statusBar.Clear("Hero Screen");
	    }
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
		    statusBar.Clear("Hero Screen");
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
		    statusBar.ShowMessage("View " + Monster::String(army[ii].Monster()));
		else
		if(selectHeroesTroops.isSelected() && Army::isValid(army[ii]) && Army::isValid(army[heroes_select]))
		    army[heroes_select].Monster() == army[ii].Monster() ?
		    	statusBar.ShowMessage("Combine " + Monster::String(army[ii].Monster()) + " armies") :
			statusBar.ShowMessage("Exchange " + Monster::String(army[ii].Monster()) + " with " + Monster::String(army[heroes_select].Monster()));
		else
		if(selectHeroesTroops.isSelected())
		    statusBar.ShowMessage("Move and right click Redistribute " + Monster::String(army[heroes_select].Monster()));
		else
		if(Army::isValid(army[ii]))
		    statusBar.ShowMessage("Select " + Monster::String(army[ii].Monster()));
		else
		    statusBar.ShowMessage("Empty");
	    }
	}
	else
        // clear all
        if(! statusBar.isEmpty()) statusBar.Clear("Hero Screen");
    }

    return Dialog::ZERO;
}
