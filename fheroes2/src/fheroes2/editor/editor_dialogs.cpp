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
    SelectEnum(const Point & pt) : Interface::ListBox<int>(pt), ok(false)
    {
      RedrawBackground(pt);
      SetScrollButtonUp(ICN::LISTBOX, 3, 4, Point(pt.x + 256, pt.y + 25));
      SetScrollButtonDn(ICN::LISTBOX, 5, 6, Point(pt.x + 256, pt.y + 225));

      SetScrollSplitter(AGG::GetICN(ICN::LISTBOX, 10), Rect(pt.x + 261, pt.y + 48, 14, 174));
      SetAreaMaxItems(5);
      SetAreaItems(Rect(pt.x + 10, pt.y + 30, 250, 210));
    };
   
    void RedrawBackground(const Point & dst)
    {
      Display & display = Display::Get();

      // top line
      display.Blit(AGG::GetICN(ICN::LISTBOX, 0), dst.x + 2, dst.y + 25);

      // middle line
      for(u8 ii = 1; ii < 11; ++ii)
        display.Blit(AGG::GetICN(ICN::LISTBOX, 1), dst.x + 2, dst.y + 25 + (ii * 19));

      // bottom line
      display.Blit(AGG::GetICN(ICN::LISTBOX, 2), dst.x + 2, dst.y + 225);


      // scroll
      display.Blit(AGG::GetICN(ICN::LISTBOX, 7), dst.x + 256, dst.y + 45);

      for(u8 ii = 1; ii < 9; ++ii)
        display.Blit(AGG::GetICN(ICN::LISTBOX, 8), dst.x + 256, dst.y + 44 + (ii * 19));

      display.Blit(AGG::GetICN(ICN::LISTBOX, 9), dst.x + 256, dst.y + 206);
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

    bool ok;
};

class SelectEnumMonster : public SelectEnum
{
public:
    SelectEnumMonster(const Point & pt) : SelectEnum(pt) {};


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
      text.Blit(dst.x + 140 - text.w() / 2, dst.y);

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
    SelectEnumHeroes(const Point & pt) : SelectEnum(pt) {};

    void RedrawItem(const int & index, s16 dstx, s16 dsty, bool current)
    {
      Display & display = Display::Get();
      Heroes::heroes_t hero = Heroes::ConvertID(index);
      display.Blit(Heroes::GetPortrait30x22(hero), dstx + 5, dsty + 3);

      Text text(Heroes::GetName(hero), (current ? Font::YELLOW_BIG : Font::BIG));
      text.Blit(dstx + 50, dsty + 10);
    };

    void RedrawBackground(const Point & dst)
    {
      Text text("Select Hero:", Font::YELLOW_BIG);
      text.Blit(dst.x + 140 - text.w() / 2, dst.y);

      SelectEnum::RedrawBackground(dst);
    };
};




Monster::monster_t Dialog::SelectMonster(Monster::monster_t cur)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    std::vector<int> monsters(static_cast<int>(Monster::MONSTER_RND1) - 1, Monster::UNKNOWN);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);


    for(size_t ii = 0; ii < monsters.size(); ++ii) monsters[ii] = Monster::FromInt(ii + 1);

    const u16 window_w = 280;
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

    const u16 window_w = 280;
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
