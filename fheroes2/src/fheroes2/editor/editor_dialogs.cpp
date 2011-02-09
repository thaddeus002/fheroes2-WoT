/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "text.h"
#include "army_troop.h"
#include "interface_list.h"
#include "editor_dialogs.h"

class SelectEnum : public Interface::ListBox<int>
{
public:
    SelectEnum(const Rect & rt) : Interface::ListBox<int>(rt), area(rt), ok(false)
    {
      RedrawBackground(rt);
      SetScrollButtonUp(ICN::LISTBOX, 3, 4, Point(rt.x + rt.w - 24, rt.y + 25));
      SetScrollButtonDn(ICN::LISTBOX, 5, 6, Point(rt.x + rt.w - 24, rt.y + rt.h - 55));

      SetScrollSplitter(AGG::GetICN(ICN::LISTBOX, 10), Rect(rt.x + rt.w - 19, rt.y + 48, 14, rt.h - 106));
      SetAreaMaxItems(5);
      SetAreaItems(Rect(rt.x + 10, rt.y + 30, rt.w - 30, rt.h - 70));
    };
   
    void RedrawBackground(const Point & dst)
    {
      Display & display = Display::Get();

      Dialog::FrameBorder::Redraw(Rect(dst.x, dst.y + 25, rtAreaItems.w + 5, rtAreaItems.h + 10),
							AGG::GetICN(ICN::CELLWIN, 1));
      // scroll
      display.Blit(AGG::GetICN(ICN::LISTBOX, 7), dst.x + area.w - 24, dst.y + 45);

      for(u8 ii = 1; ii < 9; ++ii)
        display.Blit(AGG::GetICN(ICN::LISTBOX, 8), dst.x + area.w - 24, dst.y + 44 + (ii * 19));

      display.Blit(AGG::GetICN(ICN::LISTBOX, 9), dst.x + area.w - 24, dst.y + area.h - 74);
    };

    void ActionListDoubleClick(int & index)
    {
      ok = true;
    };

    void RedrawItem(const int &, s16, s16, bool){};
    void ActionCurrentUp(void){};
    void ActionCurrentDn(void){};
    void ActionListSingleClick(int &){};
    void ActionListPressRight(int &){};

    Rect area;
    bool ok;
};

class SelectEnumMonster : public SelectEnum
{
public:
    SelectEnumMonster(const Rect & rt) : SelectEnum(rt) {};


    void RedrawItem(const int & index, s16 dstx, s16 dsty, bool current)
    {
      Display & display = Display::Get();
      Monster::monster_t mons = Monster::FromInt(index);
      display.Blit(AGG::GetICN(ICN::MONS32, Monster::GetSpriteIndex(mons)), dstx + 5, dsty + 3);

      Text text(Monster::GetName(mons), (current ? Font::YELLOW_BIG : Font::BIG));
      text.Blit(dstx + 50, dsty + 10);
    };

    void RedrawBackground(const Point & dst)
    {
      Text text("Select Monster:", Font::YELLOW_BIG);
      text.Blit(dst.x + (area.w - text.w()) / 2, dst.y);

      SelectEnum::RedrawBackground(dst);
    };

    void ActionListPressRight(int & index)
    {
      Army::Troop troop(Monster::FromInt(index), 1);
      Dialog::ArmyInfo(troop, 0);
    };
};

class SelectEnumHeroes : public SelectEnum
{
public:
    SelectEnumHeroes(const Rect & rt) : SelectEnum(rt) { SetAreaMaxItems(6); };

    void RedrawItem(const int & index, s16 dstx, s16 dsty, bool current)
    {
      Display & display = Display::Get();
      Heroes::heroes_t hero = Heroes::ConvertID(index);
      display.Blit(Heroes::GetPortrait30x22(hero), dstx + 5, dsty + 3);

      Text text(Heroes::GetName(hero), (current ? Font::YELLOW_BIG : Font::BIG));
      text.Blit(dstx + 50, dsty + 5);
    };

    void RedrawBackground(const Point & dst)
    {
      Text text("Select Hero:", Font::YELLOW_BIG);
      text.Blit(dst.x + (area.w - text.w()) / 2, dst.y);

      SelectEnum::RedrawBackground(dst);
    };
};

class SelectEnumArtifact : public SelectEnum
{
public:
    SelectEnumArtifact(const Rect & rt) : SelectEnum(rt) {};

    void RedrawItem(const int & index, s16 dstx, s16 dsty, bool current)
    {
      Display & display = Display::Get();
      Artifact art(index);
      display.Blit(AGG::GetICN(ICN::ARTFX, art.IndexSprite32()), dstx + 5, dsty + 3);

      Text text(art.GetName(), (current ? Font::YELLOW_BIG : Font::BIG));
      text.Blit(dstx + 50, dsty + 10);
    };

    void RedrawBackground(const Point & dst)
    {
      Text text("Select Artifact:", Font::YELLOW_BIG);
      text.Blit(dst.x + (area.w - text.w()) / 2, dst.y);

      SelectEnum::RedrawBackground(dst);
    };
};

class SelectEnumSpell : public SelectEnum
{
public:
    SelectEnumSpell(const Rect & rt) : SelectEnum(rt) { SetAreaMaxItems(4); };

    void RedrawItem(const int & index, s16 dstx, s16 dsty, bool current)
    {
      Display & display = Display::Get();
      Spell spell(index);
      display.Blit(AGG::GetICN(ICN::SPELLS, spell.IndexSprite()), dstx + 5, dsty + 3);

      Text text(spell.GetName(), (current ? Font::YELLOW_BIG : Font::BIG));
      text.Blit(dstx + 80, dsty + 10);
    };

    void RedrawBackground(const Point & dst)
    {
      Text text("Select Spell:", Font::YELLOW_BIG);
      text.Blit(dst.x + (area.w - text.w()) / 2, dst.y);

      SelectEnum::RedrawBackground(dst);
    };
};

Spell Dialog::SelectSpell(u8 cur)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    std::vector<int> spells(static_cast<int>(Spell::STONE - 1), Spell::NONE);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);


    for(size_t ii = 0; ii < spells.size(); ++ii) spells[ii] = ii + 1;

    const u16 window_w = 340;
    const u16 window_h = 280;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH,
			    (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw(AGG::GetICN(ICN::TEXTBAK2, 0));

    const Rect & area = frameborder.GetArea();

    SelectEnumSpell listbox(area);

    listbox.SetListContent(spells);
    if(cur != Spell::NONE)
	listbox.SetCurrent(static_cast<int>(cur));
    listbox.Redraw();

    ButtonGroups btnGroups(area, Dialog::OK|Dialog::CANCEL);
    btnGroups.Draw();

    cursor.Show();
    display.Flip();

    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && ! listbox.ok && le.HandleEvents())
    {
        result = btnGroups.QueueEventProcessing();
        listbox.QueueEventProcessing();

        if(!cursor.isVisible())
        {
            listbox.Redraw();
            cursor.Show();
            display.Flip();
        }
    }

    return result == Dialog::OK || listbox.ok ?
	Spell(listbox.GetCurrent()) : Spell(Spell::NONE);
}


Artifact Dialog::SelectArtifact(u8 cur)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    std::vector<int> artifacts(static_cast<int>(Artifact::UNKNOWN), Artifact::UNKNOWN);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);


    for(size_t ii = 0; ii < artifacts.size(); ++ii) artifacts[ii] = ii;

    const u16 window_w = 370;
    const u16 window_h = 280;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH,
			    (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw(AGG::GetICN(ICN::TEXTBAK2, 0));

    const Rect & area = frameborder.GetArea();

    SelectEnumArtifact listbox(area);

    listbox.SetListContent(artifacts);
    if(cur != Artifact::UNKNOWN)
	listbox.SetCurrent(static_cast<int>(cur));
    listbox.Redraw();

    ButtonGroups btnGroups(area, Dialog::OK|Dialog::CANCEL);
    btnGroups.Draw();

    cursor.Show();
    display.Flip();

    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && ! listbox.ok && le.HandleEvents())
    {
        result = btnGroups.QueueEventProcessing();
        listbox.QueueEventProcessing();

        if(!cursor.isVisible())
        {
            listbox.Redraw();
            cursor.Show();
            display.Flip();
        }
    }

    return result == Dialog::OK || listbox.ok ?
	Artifact(listbox.GetCurrent()) : Artifact(Artifact::UNKNOWN);
}

Monster::monster_t Dialog::SelectMonster(Monster::monster_t cur)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    std::vector<int> monsters(static_cast<int>(Monster::MONSTER_RND1) - 1, Monster::UNKNOWN);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);


    for(size_t ii = 0; ii < monsters.size(); ++ii) monsters[ii] = Monster::FromInt(ii + 1);

    const u16 window_w = 260;
    const u16 window_h = 280;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH,
			    (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw(AGG::GetICN(ICN::TEXTBAK2, 0));

    const Rect & area = frameborder.GetArea();

    SelectEnumMonster listbox(area);

    listbox.SetListContent(monsters);
    if(cur != Monster::UNKNOWN)
	listbox.SetCurrent(static_cast<int>(cur));
    listbox.Redraw();

    ButtonGroups btnGroups(area, Dialog::OK|Dialog::CANCEL);
    btnGroups.Draw();

    cursor.Show();
    display.Flip();

    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && ! listbox.ok && le.HandleEvents())
    {
        result = btnGroups.QueueEventProcessing();
        listbox.QueueEventProcessing();

        if(!cursor.isVisible())
        {
            listbox.Redraw();
            cursor.Show();
            display.Flip();
        }
    }

    return result == Dialog::OK || listbox.ok ?
	Monster::FromInt(listbox.GetCurrent()) : Monster::UNKNOWN;
}

Heroes::heroes_t Dialog::SelectHeroes(Heroes::heroes_t cur)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    std::vector<int> heroes(static_cast<int>(Heroes::SANDYSANDY), Heroes::UNKNOWN);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);


    for(size_t ii = 0; ii < heroes.size(); ++ii) heroes[ii] = Heroes::ConvertID(ii);

    const u16 window_w = 240;
    const u16 window_h = 280;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH,
			    (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw(AGG::GetICN(ICN::TEXTBAK2, 0));

    const Rect & area = frameborder.GetArea();

    SelectEnumHeroes listbox(area);

    listbox.SetListContent(heroes);
    if(cur != Heroes::UNKNOWN)
	listbox.SetCurrent(static_cast<int>(cur));
    listbox.Redraw();

    ButtonGroups btnGroups(area, Dialog::OK|Dialog::CANCEL);
    btnGroups.Draw();

    cursor.Show();
    display.Flip();

    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && ! listbox.ok && le.HandleEvents())
    {
        result = btnGroups.QueueEventProcessing();
        listbox.QueueEventProcessing();

        if(!cursor.isVisible())
        {
            listbox.Redraw();
            cursor.Show();
            display.Flip();
        }
    }

    return result == Dialog::OK || listbox.ok ?
	Heroes::ConvertID(listbox.GetCurrent()) : Heroes::UNKNOWN;
}
