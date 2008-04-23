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
#include "engine.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "tools.h"
#include "text.h"
#include "army.h"
#include "heroes.h"
#include "portrait.h"

void RedrawSecondarySkill(const Point & pt, const std::vector<Skill::Secondary> & skills);
void RedrawArmy(const Point & pt, const std::vector<Army::Troops> & army);
void RedrawArtifact(const Point & pt, const std::vector<Artifact::artifact_t> & artifacts);

void Heroes::MeetingDialog(Heroes & heroes2)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);

    Point dst_pt(cur_pt);

    std::string message;



    display.Blit(AGG::GetICN(ICN::STONEBAK, 0), dst_pt);

    u8 w = 22;

    Rect src_rt(w, w, 640 - 2 * w, 480 - 2 * w);

    // background
    dst_pt.x = cur_pt.x + w;
    dst_pt.y = cur_pt.y + w;
    display.Blit(AGG::GetICN(ICN::SWAPWIN, 0), src_rt, dst_pt);

    // header
    message = GetName() + " meets " + heroes2.GetName();
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 26;
    Text(message, Font::BIG, dst_pt);

    // portrait
    dst_pt.x = cur_pt.x + 93;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(GetHeroes(), Portrait::BIG), dst_pt);

    dst_pt.x = cur_pt.x + 447;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(heroes2.GetHeroes(), Portrait::BIG), dst_pt);

    // attack skill
    message.clear();
    message += "Attack Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetAttack());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetAttack());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    // defense skill
    message.clear();
    message += "Defense Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetDefense());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    // spell power
    message.clear();
    message += "Spell Power";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetPower());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    // knowledge
    message.clear();
    message += "Knowledge";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetKnowledge());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

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
    RedrawArmy(dst_pt, army);

    dst_pt.x = cur_pt.x + 381;
    dst_pt.y = cur_pt.y + 267;
    RedrawArmy(dst_pt, heroes2.army);

    // artifact
    dst_pt.x = cur_pt.x + 23;
    dst_pt.y = cur_pt.y + 347;
    RedrawArtifact(dst_pt, artifacts);

    dst_pt.x = cur_pt.x + 367;
    dst_pt.y = cur_pt.y + 347;
    RedrawArtifact(dst_pt, heroes2.artifacts);

    // button exit
    dst_pt.x = cur_pt.x + 280;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, ICN::SWAPBTN, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
    }
}

void RedrawSecondarySkill(const Point & pt, const std::vector<Skill::Secondary> & skills)
{
    Display & display = Display::Get();

    Point dst_pt(pt);

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
        const Skill::secondary_t skill = ii < skills.size() ? skills[ii].Skill() : Skill::UNKNOWN;
        const Skill::Level::type_t level = ii < skills.size() ? skills[ii].Level() : Skill::Level::NONE;

        if(Skill::UNKNOWN != skill && Skill::Level::NONE != level)
        {
    	    const Sprite & sprite_skill = AGG::GetICN(ICN::MINISS, Skill::Secondary::GetIndexSprite2(skill));
    	    display.Blit(sprite_skill, dst_pt);

            std::string message;
            String::AddInt(message, level);
            Text(message, Font::SMALL, dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) - 3, dst_pt.y + sprite_skill.h() - 12);

            dst_pt.x += sprite_skill.w() + 1;
        }
    }
}

void RedrawArmy(const Point & pt, const std::vector<Army::Troops> & army)
{
    Display & display = Display::Get();

    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii)
    {
	if(army[ii].isValid())
	{
	    const Sprite & sprite = AGG::GetICN(ICN::MONS32, army[ii].Monster());

	    const u16 ox = pt.x + (44 - sprite.w()) / 2 + ii * 45;
	    const u16 oy = pt.y + 40 - sprite.h();

	    display.Blit(sprite, ox, oy);
	}
    }
}

void RedrawArtifact(const Point & pt, const std::vector<Artifact::artifact_t> & artifacts)
{
    Display & display = Display::Get();

    for(u8 ii = 0; ii < HEROESMAXARTIFACT; ++ii)
    {
	if(ii < artifacts.size())
	{
	    const Sprite & sprite = AGG::GetICN(ICN::ARTFX, artifacts[ii]);

	    const u16 ox = pt.x + (34 - sprite.w()) / 2 + (ii % (HEROESMAXARTIFACT / 2)) * 36;
	    const u16 oy = ii < HEROESMAXARTIFACT / 2 ? pt.y + 1 : pt.y + 37;

	    display.Blit(sprite, ox, oy);
	}
    }
}
