/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

Skill::Secondary::skill_t Dialog::LevelUpSelectSkill(const std::string & header, const Skill::Secondary & sec1, const Skill::Secondary & sec2)
{
    Display & display = Display::Get();
    const ICN::icn_t system = Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite_frame = AGG::GetICN(ICN::SECSKILL, 15);
    const Sprite & sprite_skill1 = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec1.Skill()));
    const Sprite & sprite_skill2 = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(sec2.Skill()));

    Point pt;
    std::string message = _("You may learn either %{level1} %{skill1} or %{level2} %{skill2}.");
    String::Replace(message, "%{level1}", Skill::Level::String(sec1.Level()));
    String::Replace(message, "%{skill1}", Skill::Secondary::String(sec1.Skill()));
    String::Replace(message, "%{level2}", Skill::Level::String(sec2.Level()));
    String::Replace(message, "%{skill2}", Skill::Secondary::String(sec2.Skill()));
    const u16 height1 = Text::height(header, Font::BIG, BOXAREA_WIDTH);
    const u16 height2 = Text::height(message, Font::BIG, BOXAREA_WIDTH);

    Box box(height1 + 10 + height2 + 10 + sprite_frame.h(), true);

    pt.x = box.GetArea().x + box.GetArea().w / 2 - AGG::GetICN(system, 9).w() - 20;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 9).h();
    Button button_learn1(pt, system, 9, 10);

    pt.x = box.GetArea().x + box.GetArea().w / 2 + 20;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 9).h();
    Button button_learn2(pt, system, 9, 10);

    Rect pos = box.GetArea();

    if(header.size())
    {
	TextBox(header, Font::BIG, pos);
        pos.y += height1 + 10;
    }

    if(message.size())
    {
        TextBox(message, Font::BIG, pos);
        pos.y += height2 + 10;
    }

    // sprite1
    pos.y += 20;
    pos.x = box.GetArea().x + box.GetArea().w / 2 - sprite_frame.w() - 20;
    display.Blit(sprite_frame, pos);
    pos.x += 3;
    Rect rect_image1(pos, sprite_skill1.w(), sprite_skill1.h());
    display.Blit(sprite_skill1, pos.x, pos.y + 3);

    Text text;

    // text
    text.Set(Skill::Secondary::String(sec1.Skill()), Font::SMALL);
    text.Blit(pos.x + (sprite_skill1.w() - text.w()) / 2, pos.y + 5);
    text.Set(Skill::Level::String(sec1.Level()), Font::SMALL);
    text.Blit(pos.x + (sprite_skill1.w() - text.w()) / 2, pos.y + sprite_skill1.h() - 12);

    // sprite2
    pos.x = box.GetArea().x + box.GetArea().w / 2 + 20;
    display.Blit(sprite_frame, pos);
    pos.x += 3;
    Rect rect_image2(pos, sprite_skill2.w(), sprite_skill2.h());
    display.Blit(sprite_skill2, pos.x, pos.y + 3);
    // text
    Text name_skill2(Skill::Secondary::String(sec2.Skill()), Font::SMALL);
    name_skill2.Blit(pos.x + (sprite_skill2.w() - name_skill2.w()) / 2, pos.y + 5);
    Text name_level2(Skill::Level::String(sec2.Level()), Font::SMALL);
    name_level2.Blit(pos.x + (sprite_skill2.w() - name_level2.w()) / 2, pos.y + sprite_skill2.h() - 12);

    button_learn1.Draw();
    button_learn2.Draw();

    cursor.Show();
    display.Flip();
    LocalEvent & le = LocalEvent::GetLocalEvent();
    Skill::Secondary::skill_t result = Skill::Secondary::UNKNOWN;

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(button_learn1) ? button_learn1.PressDraw() : button_learn1.ReleaseDraw();
	le.MousePressLeft(button_learn2) ? button_learn2.PressDraw() : button_learn2.ReleaseDraw();

        if(le.MouseClickLeft(button_learn1)){ result = sec1.Skill(); break; }
        if(le.MouseClickLeft(button_learn2)){ result = sec2.Skill(); break; }

	if(le.MouseClickLeft(rect_image1)){ cursor.Hide(); SkillInfo(sec1.Skill(), sec1.Level()); cursor.Show(); display.Flip(); }
	else
	if(le.MouseClickLeft(rect_image2)){ cursor.Hide(); SkillInfo(sec2.Skill(), sec2.Level()); cursor.Show(); display.Flip(); }

	if(le.MousePressRight(rect_image1)){ cursor.Hide(); SkillInfo(sec1.Skill(), sec1.Level(), false); cursor.Show(); display.Flip(); }
	else
	if(le.MousePressRight(rect_image2)){ cursor.Hide(); SkillInfo(sec2.Skill(), sec2.Level(), false); cursor.Show(); display.Flip(); }
    }

    cursor.Hide();

    return result;
}
