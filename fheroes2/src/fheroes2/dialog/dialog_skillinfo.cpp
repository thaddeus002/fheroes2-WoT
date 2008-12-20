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
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

void Dialog::SkillInfo(const Skill::Secondary::skill_t skill, const Skill::Level::type_t level, const bool ok_button)
{
    const std::string header(Skill::Level::String(level) + " " + Skill::Secondary::String(skill));
    const std::string description(Skill::Secondary::Description(skill, level));
    SkillInfo(header, description, skill, level, ok_button);
}

void Dialog::SkillInfo(const std::string &header, const std::string &message, const Skill::Secondary::skill_t skill, const Skill::Level::type_t level, const bool ok_button)
{
    Display & display = Display::Get();
    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const u16 height1 = Text::height(header, Font::BIG, BOXAREA_WIDTH);
    const u16 height2 = Text::height(message, Font::BIG, BOXAREA_WIDTH);

    const Sprite & sprite = AGG::GetICN(ICN::SECSKILL, Skill::Secondary::GetIndexSprite1(skill));

    Box box(height1 + 10 + height2 + 10 + sprite.h(), ok_button ? Dialog::OK : 0);

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

    // blit sprite
    const Sprite & border = AGG::GetICN(ICN::SECSKILL, 15);
    pos.x = box.GetArea().x + (pos.w - border.w()) / 2;
    display.Blit(border, pos.x, pos.y);
    pos.x = box.GetArea().x + (pos.w - sprite.w()) / 2;
    display.Blit(sprite, pos.x, pos.y + 3);

    Text text;

    // small text
    text.Set(Skill::Secondary::String(skill), Font::SMALL);
    pos.x = box.GetArea().x + (pos.w - text.w()) / 2;
    text.Blit(pos.x, pos.y + 3);

    text.Set(Skill::Level::String(level));
    pos.x = box.GetArea().x + (pos.w - text.w()) / 2;
    text.Blit(pos.x, pos.y + 55);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button *button = NULL;
    Point pt;

    if(ok_button)
    {
        pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
        pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
	button = new Button(pt, system, 1, 2);
    }

    if(button) (*button).Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
        if(!ok_button && !le.MouseRight()) break;

	if(button) le.MousePressLeft(*button) ? button->PressDraw() : button->ReleaseDraw();

        if(button && le.MouseClickLeft(*button)){ break; }

	if(le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)){ break; }
    }

    cursor.Hide();
    if(button) delete button;
}

void Dialog::SkillInfo(const std::string &header, const std::string &message, const Skill::Primary::skill_t skill)
{
    Display & display = Display::Get();
    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    u8 index = 0;
    std::string skill_name;

    switch(skill)
    {
	case Skill::Primary::ATTACK:
	    index = 0;
	    skill_name = Skill::Primary::String(skill) + " Skill";
	    break;

	case Skill::Primary::DEFENCE:
	    index = 1;
	    skill_name = Skill::Primary::String(skill) + " Skill";
	    break;

	case Skill::Primary::POWER:
	    index = 2;
	    skill_name = "Spell " + Skill::Primary::String(skill);
	    break;

	case Skill::Primary::KNOWLEDGE:
	    index = 3;
	    skill_name = Skill::Primary::String(skill);
	    break;

	default: break;
    }
    const Sprite & sprite = AGG::GetICN(ICN::PRIMSKIL, index);

    u16 height1 = Text::height(header, Font::BIG, BOXAREA_WIDTH);
    u16 height2 = Text::height(message, Font::BIG, BOXAREA_WIDTH);

    Box box(height1 + 20 + height2 + 10 + sprite.h(), Dialog::OK);

    Rect pos = box.GetArea();

    if(header.size())
    {
	TextBox(header, Font::BIG, pos);
        pos.y += height1 + 20;
    }

    if(message.size())
    {
        TextBox(message, Font::BIG, pos);
        pos.y += height2 + 20;
    }

    // blit sprite
    const Sprite & border = AGG::GetICN(ICN::PRIMSKIL, 4);
    pos.x = box.GetArea().x + (pos.w - border.w()) / 2;
    display.Blit(border, pos.x, pos.y);
    pos.x = box.GetArea().x + (pos.w - sprite.w()) / 2;
    display.Blit(sprite, pos.x, pos.y + 6);

    Text text;

    text.Set(skill_name, Font::SMALL);
    pos.x = box.GetArea().x + (pos.w - text.w()) / 2;
    text.Blit(pos.x, pos.y + 8);

    text.Set("+1", Font::BIG);
    pos.x = box.GetArea().x + (pos.w - text.w()) / 2;
    text.Blit(pos.x, pos.y + 80);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point pt;
    
    pt.x = box.GetArea().x + (box.GetArea().w - AGG::GetICN(system, 1).w()) / 2;
    pt.y = box.GetArea().y + box.GetArea().h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button button(pt, system, 1, 2);

    button.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(button) ? button.PressDraw() : button.ReleaseDraw();

        if(le.MouseClickLeft(button)){ break; }

	if(le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)){ break; }
    }

    cursor.Hide();
}
