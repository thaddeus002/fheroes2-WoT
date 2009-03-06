/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

void InfoSkillClear(const Rect &, const Rect &, const Rect &, const Rect &);
void InfoSkillSelect(Skill::Primary::skill_t, const Rect &, const Rect &, const Rect &, const Rect &);
Skill::Primary::skill_t InfoSkillNext(Skill::Primary::skill_t);
Skill::Primary::skill_t InfoSkillPrev(Skill::Primary::skill_t);

Skill::Primary::skill_t Dialog::SelectSkillFromArena(void)
{
    Display & display = Display::Get();
    const ICN::icn_t system = Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    TextBox textbox(_("You enter the arena and face a pack of vicious lions. You handily defeat them, to the wild cheers of the crowd.  Impressed by your skill, the aged trainer of gladiators agrees to train you in a skill of your choice."), Font::BIG, BOXAREA_WIDTH);

    Dialog::Box box(textbox.h() + 80, Dialog::OK);

    const Sprite & sprite = AGG::GetICN(ICN::XPRIMARY, 0);
    const Rect & box_rt = box.GetArea();
    Point dst_pt(box_rt.x, box_rt.y + 10);

    // text
    textbox.Blit(dst_pt);

    dst_pt.y += textbox.h() + 20;
    Skill::Primary::skill_t  res = Skill::Primary::ATTACK;

    const Rect rect1(dst_pt.x + box_rt.w / 2 - 2 * sprite.w() - 30, dst_pt.y, sprite.w(), sprite.h());
    const Rect rect2(dst_pt.x + box_rt.w / 2 - sprite.w() - 10, dst_pt.y, sprite.w(), sprite.h());
    const Rect rect3(dst_pt.x + box_rt.w / 2 + 10, dst_pt.y, sprite.w(), sprite.h());
    const Rect rect4(dst_pt.x + box_rt.w / 2 + sprite.w() + 30, dst_pt.y, sprite.w(), sprite.h());

    InfoSkillClear(rect1, rect2, rect3, rect4);
    InfoSkillSelect(res, rect1, rect2, rect3, rect4);

    // info texts
    Text text(Skill::Primary::String(Skill::Primary::ATTACK), Font::SMALL);
    dst_pt.x = rect1.x + (rect1.w - text.w()) / 2;
    dst_pt.y = rect1.y + rect1.h + 5;
    text.Blit(dst_pt);

    text.Set(Skill::Primary::String(Skill::Primary::DEFENCE));
    dst_pt.x = rect2.x + (rect2.w - text.w()) / 2;
    dst_pt.y = rect2.y + rect2.h + 5;
    text.Blit(dst_pt);

    text.Set(Skill::Primary::String(Skill::Primary::POWER));
    dst_pt.x = rect3.x + (rect3.w - text.w()) / 2;
    dst_pt.y = rect3.y + rect3.h + 5;
    text.Blit(dst_pt);

    text.Set(Skill::Primary::String(Skill::Primary::KNOWLEDGE));
    dst_pt.x = rect4.x + (rect4.w - text.w()) / 2;
    dst_pt.y = rect4.y + rect4.h + 5;
    text.Blit(dst_pt);

    // buttons
    dst_pt.x = box_rt.x + (box_rt.w - AGG::GetICN(system, 1).w()) / 2;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button buttonOk(dst_pt, system, 1, 2);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    buttonOk.Draw();
    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();

	if(le.KeyPress(KEY_LEFT) && Skill::Primary::UNKNOWN != InfoSkillPrev(res))
	{
	    cursor.Hide();
	    res = InfoSkillPrev(res);
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.KeyPress(KEY_RIGHT) && Skill::Primary::UNKNOWN != InfoSkillNext(res))
	{
	    cursor.Hide();
	    res = InfoSkillNext(res);
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(rect1))
	{
	    cursor.Hide();
	    res = Skill::Primary::ATTACK;
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(rect2))
	{
	    cursor.Hide();
	    res = Skill::Primary::DEFENCE;
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(rect3))
	{
	    cursor.Hide();
	    res = Skill::Primary::POWER;
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	else
	if(le.MouseClickLeft(rect4))
	{
	    cursor.Hide();
	    res = Skill::Primary::KNOWLEDGE;
	    InfoSkillClear(rect1, rect2, rect3, rect4);
	    InfoSkillSelect(res, rect1, rect2, rect3, rect4);
	    cursor.Show();
	    display.Flip();
	}
	
        if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk)) break;
    }

    cursor.Hide();
    return res;
}

void InfoSkillClear(const Rect & rect1, const Rect & rect2, const Rect & rect3, const Rect & rect4)
{
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(ICN::XPRIMARY, 0), rect1);
    display.Blit(AGG::GetICN(ICN::XPRIMARY, 1), rect2);
    display.Blit(AGG::GetICN(ICN::XPRIMARY, 2), rect3);
    display.Blit(AGG::GetICN(ICN::XPRIMARY, 3), rect4);
}

void InfoSkillSelect(Skill::Primary::skill_t skill, const Rect & rect1, const Rect & rect2, const Rect & rect3, const Rect & rect4)
{
    Display & display = Display::Get();

    switch(skill)
    {
	case Skill::Primary::ATTACK:	display.Blit(AGG::GetICN(ICN::XPRIMARY, 4), rect1); break;
	case Skill::Primary::DEFENCE:	display.Blit(AGG::GetICN(ICN::XPRIMARY, 5), rect2); break;
	case Skill::Primary::POWER:	display.Blit(AGG::GetICN(ICN::XPRIMARY, 6), rect3); break;
	case Skill::Primary::KNOWLEDGE:	display.Blit(AGG::GetICN(ICN::XPRIMARY, 7), rect4); break;
	default: break;
    }
}

Skill::Primary::skill_t InfoSkillNext(Skill::Primary::skill_t skill)
{
    switch(skill)
    {
	case Skill::Primary::ATTACK:	return Skill::Primary::DEFENCE;
	case Skill::Primary::DEFENCE:	return Skill::Primary::POWER;
	case Skill::Primary::POWER:	return Skill::Primary::KNOWLEDGE;
	default: break;
    }

    return Skill::Primary::UNKNOWN;
}

Skill::Primary::skill_t InfoSkillPrev(Skill::Primary::skill_t skill)
{
    switch(skill)
    {
	case Skill::Primary::DEFENCE:	return Skill::Primary::ATTACK;
	case Skill::Primary::POWER:	return Skill::Primary::DEFENCE;
	case Skill::Primary::KNOWLEDGE:	return Skill::Primary::POWER;
	default: break;
    }

    return Skill::Primary::UNKNOWN;
}
