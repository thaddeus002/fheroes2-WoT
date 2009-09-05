/****************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov <afletdinov@mail.dc.baikal.ru> *
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>            *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/

#include <string>
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "text.h"
#include "army.h"
#include "background.h"
#include "heroes.h"
#include "selectarmybar.h"
#include "selectartifactbar.h"
#include "heroes_indicator.h"
#include "game_interface.h"

void RedrawPrimarySkillInfo(const Point &, const Skill::Primary &, const Skill::Primary &);
void RedrawSecondarySkill(const Point &, const std::vector<Skill::Secondary> &);

void Heroes::MeetingDialog(Heroes & heroes2)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);
    
    const Sprite &backSprite = AGG::GetICN(ICN::SWAPWIN, 0);
    const Point cur_pt((display.w() - backSprite.w()) / 2, (display.h() - backSprite.h()) / 2);
    Background background(cur_pt, backSprite.w(), backSprite.h());
    background.Save();
    Point dst_pt(cur_pt);
    std::string message;

    Rect src_rt(0, 0, 640, 480);

    // background
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y;
    display.Blit(backSprite, src_rt, dst_pt);

    // header
    message = _("%{name1} meets %{name2}");
    String::Replace(message, "%{name1}", GetName());
    String::Replace(message, "%{name2}", heroes2.GetName());
    Text text(message, Font::BIG);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 26);

    // portrait
    dst_pt.x = cur_pt.x + 93;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(GetPortrait101x93(), dst_pt);

    dst_pt.x = cur_pt.x + 447;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(heroes2.GetPortrait101x93(), dst_pt);

    dst_pt.x = cur_pt.x + 34;
    dst_pt.y = cur_pt.y + 75;
    MoraleIndicator moraleIndicator1(*this);
    moraleIndicator1.SetPos(dst_pt);
    moraleIndicator1.Redraw();

    dst_pt.x = cur_pt.x + 34;
    dst_pt.y = cur_pt.y + 115;
    LuckIndicator luckIndicator1(*this);
    luckIndicator1.SetPos(dst_pt);
    luckIndicator1.Redraw();

    dst_pt.x = cur_pt.x + 566;
    dst_pt.y = cur_pt.y + 75;
    MoraleIndicator moraleIndicator2(heroes2);
    moraleIndicator2.SetPos(dst_pt);
    moraleIndicator2.Redraw();

    dst_pt.x = cur_pt.x + 566;
    dst_pt.y = cur_pt.y + 115;
    LuckIndicator luckIndicator2(heroes2);
    luckIndicator2.SetPos(dst_pt);
    luckIndicator2.Redraw();

    // primary skill
    Background backPrimary(cur_pt.x + 255, cur_pt.y + 50, 130, 135);
    backPrimary.Save();
    RedrawPrimarySkillInfo(cur_pt, *this, heroes2);

    // secondary skill
    dst_pt.x = cur_pt.x + 23;
    dst_pt.y = cur_pt.y + 200;
    RedrawSecondarySkill(dst_pt, secondary_skills);

    dst_pt.x = cur_pt.x + 354;
    dst_pt.y = cur_pt.y + 200;
    RedrawSecondarySkill(dst_pt, heroes2.secondary_skills);

    // army
    dst_pt.x = cur_pt.x + 36;
    dst_pt.y = cur_pt.y + 267;

    const Rect rt1(36, 267, 43, 53);
    Surface sfb1(rt1.w, rt1.h);
    sfb1.Blit(backSprite, rt1, 0, 0);
    Surface sfc1(rt1.w, rt1.h - 10);
    Cursor::DrawCursor(sfc1, 0x10, true);
    
    SelectArmyBar selectArmy1;
    selectArmy1.SetArmy(army);
    selectArmy1.SetPos(dst_pt);
    selectArmy1.SetInterval(2);
    selectArmy1.SetBackgroundSprite(sfb1);
    selectArmy1.SetCursorSprite(sfc1);
    selectArmy1.SetUseMons32Sprite();
    selectArmy1.SetSaveLastTroop();
    selectArmy1.Redraw();

    dst_pt.x = cur_pt.x + 381;
    dst_pt.y = cur_pt.y + 267;
    SelectArmyBar selectArmy2;
    selectArmy2.SetArmy(heroes2.GetArmy());
    selectArmy2.SetPos(dst_pt);
    selectArmy2.SetInterval(2);
    selectArmy2.SetBackgroundSprite(sfb1);
    selectArmy2.SetCursorSprite(sfc1);
    selectArmy2.SetUseMons32Sprite();
    selectArmy2.SetSaveLastTroop();
    selectArmy2.Redraw();

    // artifact
    dst_pt.x = cur_pt.x + 23;
    dst_pt.y = cur_pt.y + 347;

    const Rect rt2(23, 347, 34, 34);
    Surface sfb2(rt2.w, rt2.h);
    sfb2.Blit(backSprite, rt2, 0, 0);
    Surface sfc2(rt2.w, rt2.h);
    Cursor::DrawCursor(sfc2, 0x10, true);

    SelectArtifactsBar selectArtifacts1(*this);
    selectArtifacts1.SetPos(dst_pt);
    selectArtifacts1.SetInterval(2);
    selectArtifacts1.SetBackgroundSprite(sfb2);
    selectArtifacts1.SetCursorSprite(sfc2);
    selectArtifacts1.SetUseArts32Sprite();
    selectArtifacts1.Redraw();

    dst_pt.x = cur_pt.x + 367;
    dst_pt.y = cur_pt.y + 347;
    SelectArtifactsBar selectArtifacts2(heroes2);
    selectArtifacts2.SetPos(dst_pt);
    selectArtifacts2.SetInterval(2);
    selectArtifacts2.SetBackgroundSprite(sfb2);
    selectArtifacts2.SetCursorSprite(sfc2);
    selectArtifacts2.SetUseArts32Sprite();
    selectArtifacts2.Redraw();

    // button exit
    dst_pt.x = cur_pt.x + 280;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, ICN::SWAPBTN, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();
        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
        
	// selector troops event
	if(le.MouseCursor(selectArmy1.GetArea()) || le.MouseCursor(selectArmy2.GetArea()))
	{
	    if(selectArtifacts1.isSelected()) selectArtifacts1.Reset();
	    else
	    if(selectArtifacts2.isSelected()) selectArtifacts2.Reset();

    	    if(SelectArmyBar::QueueEventProcessing(selectArmy1, selectArmy2))
	    {
		cursor.Hide();
		moraleIndicator1.Redraw();
		moraleIndicator2.Redraw();
		luckIndicator1.Redraw();
		luckIndicator2.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}

	// selector artifacts event
	if(le.MouseCursor(selectArtifacts1.GetArea()) || le.MouseCursor(selectArtifacts2.GetArea()))
	{
	    if(selectArmy1.isSelected()) selectArmy1.Reset();
	    else
	    if(selectArmy2.isSelected()) selectArmy2.Reset();

    	    if(SelectArtifactsBar::QueueEventProcessing(selectArtifacts1, selectArtifacts2))
	    {
		cursor.Hide();
		backPrimary.Restore();
		RedrawPrimarySkillInfo(cur_pt, *this, heroes2);
		moraleIndicator1.Redraw();
		moraleIndicator2.Redraw();
		luckIndicator1.Redraw();
		luckIndicator2.Redraw();
		cursor.Show();
		display.Flip();
	    }
	}

        if(le.MouseCursor(moraleIndicator1.GetArea())) MoraleIndicator::QueueEventProcessing(moraleIndicator1);
        else
        if(le.MouseCursor(moraleIndicator2.GetArea())) MoraleIndicator::QueueEventProcessing(moraleIndicator2);
	else
        if(le.MouseCursor(luckIndicator1.GetArea())) LuckIndicator::QueueEventProcessing(luckIndicator1);
        else
        if(le.MouseCursor(luckIndicator2.GetArea())) LuckIndicator::QueueEventProcessing(luckIndicator2);
    }

    cursor.Hide();
    background.Restore();
    cursor.Show();
    display.Flip();
}

void RedrawPrimarySkillInfo(const Point & cur_pt, const Skill::Primary & p1, const Skill::Primary & p2)
{
    std::string message;

    // attack skill
    message = _("Attack Skill");
    Text text(message, Font::SMALL);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 64);

    message.clear();
    String::AddInt(message, p1.GetAttack());
    text.Set(message);
    text.Blit(cur_pt.x + 260 - text.w() / 2, cur_pt.y + 64);

    message.clear();
    String::AddInt(message, p2.GetAttack());
    text.Set(message);
    text.Blit(cur_pt.x + 380 - text.w(), cur_pt.y + 64);

    // defense skill
    message = _("Defense Skill");
    text.Set(message);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 96);

    message.clear();
    String::AddInt(message, p1.GetDefense());
    text.Set(message);
    text.Blit(cur_pt.x + 260 - text.w() / 2, cur_pt.y + 96);

    message.clear();
    String::AddInt(message, p2.GetDefense());
    text.Set(message);
    text.Blit(cur_pt.x + 380 - text.w(), cur_pt.y + 96);

    // spell power
    message = _("Spell Power");
    text.Set(message);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 128);

    message.clear();
    String::AddInt(message, p1.GetPower());
    text.Set(message);
    text.Blit(cur_pt.x + 260 - text.w() / 2, cur_pt.y + 128);

    message.clear();
    String::AddInt(message, p2.GetPower());
    text.Set(message);
    text.Blit(cur_pt.x + 380 - text.w(), cur_pt.y + 128);

    // knowledge
    message = _("Knowledge");
    text.Set(message);
    text.Blit(cur_pt.x + 320 - text.w() / 2, cur_pt.y + 160);

    message.clear();
    String::AddInt(message, p1.GetKnowledge());
    text.Set(message);
    text.Blit(cur_pt.x + 260 - text.w() / 2, cur_pt.y + 160);

    message.clear();
    String::AddInt(message, p2.GetKnowledge());
    text.Set(message);
    text.Blit(cur_pt.x + 380 - text.w(), cur_pt.y + 160);
}

void RedrawSecondarySkill(const Point & pt, const std::vector<Skill::Secondary> & skills)
{
    Display & display = Display::Get();

    Point dst_pt(pt);

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
        const Skill::Secondary::skill_t skill = ii < skills.size() ? skills[ii].Skill() : Skill::Secondary::UNKNOWN;
        const Skill::Level::type_t level = ii < skills.size() ? skills[ii].Level() : Skill::Level::NONE;

        if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
        {
    	    const Sprite & sprite_skill = AGG::GetICN(ICN::MINISS, Skill::Secondary::GetIndexSprite2(skill));
    	    display.Blit(sprite_skill, dst_pt);

            std::string message;
            String::AddInt(message, level);
            Text text(message, Font::SMALL);
            text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) - 3, dst_pt.y + sprite_skill.h() - 12);

            dst_pt.x += sprite_skill.w() + 1;
        }
    }
}
