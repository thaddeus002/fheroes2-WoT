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
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "background.h"
#include "dialog.h"

Dialog::answer_t Dialog::SystemOptions(void)
{
    Display & display = Display::Get();
    Settings & conf = Settings::Get();

    // preload
    const ICN::icn_t spanbkg = conf.EvilInterface() ? ICN::SPANBKGE : ICN::SPANBKG;
    const ICN::icn_t spanbtn = conf.EvilInterface() ? ICN::SPANBTNE : ICN::SPANBTN;

    AGG::PreloadObject(spanbkg);
    AGG::PreloadObject(spanbtn);
    AGG::PreloadObject(ICN::SPANEL);

    // cursor
    Cursor & cursor = Cursor::Get();
    const Cursor::themes_t oldcursor = cursor.Themes();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    // image box
    const Sprite &box = AGG::GetICN(spanbkg, 0);

    Rect rb((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    Background back(rb);
    back.Save();
    display.Blit(box, rb.x, rb.y);

    std::string text;

    // sound
    const Sprite & sprite1 = AGG::GetICN(ICN::SPANEL, conf.Sound() ? 1 : 0);
    const Rect rect1(rb.x + 36, rb.y + 47, sprite1.w(), sprite1.h());
    display.Blit(sprite1, rect1);
    text.clear();
    text = "sound ";
    text += conf.Sound() ? "on" : "off";
    Text(text, Font::SMALL, rect1.x + (rect1.w - Text::width(text, Font::SMALL)) / 2, rect1.y + rect1.h + 5);

    // music
    const Sprite & sprite2 = AGG::GetICN(ICN::SPANEL, conf.Music() ? 3 : 2);
    const Rect rect2(rb.x + 128, rb.y + 47, sprite2.w(), sprite2.h());
    display.Blit(sprite2, rect2);
    text.clear();
    text = "music ";
    text += conf.Music() ? "on" : "off";
    Text(text, Font::SMALL, rect2.x + (rect2.w - Text::width(text, Font::SMALL)) / 2, rect2.y + rect2.h + 5);

    // unused
    const Sprite & sprite3 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect3(rb.x + 220, rb.y + 47, sprite3.w(), sprite3.h());
    display.Blit(sprite3, rect3);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect3.x + (rect3.w - Text::width(text, Font::SMALL)) / 2, rect3.y + rect3.h + 5);

    // unused
    const Sprite & sprite4 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect4(rb.x + 36, rb.y + 157, sprite4.w(), sprite4.h());
    display.Blit(sprite4, rect4);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect4.x + (rect4.w - Text::width(text, Font::SMALL)) / 2, rect4.y + rect4.h + 5);

    // unused
    const Sprite & sprite5 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect5(rb.x + 128, rb.y + 157, sprite5.w(), sprite5.h());
    display.Blit(sprite5, rect5);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect5.x + (rect5.w - Text::width(text, Font::SMALL)) / 2, rect5.y + rect5.h + 5);

    // unused
    const Sprite & sprite6 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect6(rb.x + 220, rb.y + 157, sprite6.w(), sprite6.h());
    display.Blit(sprite6, rect6);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect6.x + (rect6.w - Text::width(text, Font::SMALL)) / 2, rect6.y + rect6.h + 5);

    // unused
    const Sprite & sprite7 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect7(rb.x + 36, rb.y + 267, sprite7.w(), sprite7.h());
    display.Blit(sprite7, rect7);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect7.x + (rect7.w - Text::width(text, Font::SMALL)) / 2, rect7.y + rect7.h + 5);

    // unused
    const Sprite & sprite8 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect8(rb.x + 128, rb.y + 267, sprite8.w(), sprite8.h());
    display.Blit(sprite8, rect8);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect8.x + (rect8.w - Text::width(text, Font::SMALL)) / 2, rect8.y + rect8.h + 5);

    // unused
    const Sprite & sprite9 = AGG::GetICN(ICN::SPANEL, 9);
    const Rect rect9(rb.x + 220, rb.y + 267, sprite9.w(), sprite9.h());
    display.Blit(sprite9, rect9);
    text.clear();
    text = "unused";
    Text(text, Font::SMALL, rect9.x + (rect9.w - Text::width(text, Font::SMALL)) / 2, rect9.y + rect9.h + 5);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button buttonOk(rb.x + 113, rb.y + 362, spanbtn, 0, 1);

    buttonOk.Draw();

    cursor.Show();
    display.Flip();

    Dialog::answer_t result = Dialog::ZERO;

    // dialog menu loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();

        if(le.MouseClickLeft(buttonOk)) { result = Dialog::OK; break; }
        if(le.KeyPress(KEY_ESCAPE)){ result = Dialog::CANCEL; break; }
    }

    // restore background
    cursor.Hide();
    back.Restore();
    cursor.SetThemes(oldcursor);
    cursor.Show();
    display.Flip();

    return result;
}
