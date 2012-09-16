/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifdef WITH_EDITOR

#include "gamedefs.h"
#include "agg.h"
#include "dialog.h"
#include "cursor.h"
#include "button.h"
#include "settings.h"
#include "maps.h"
#include "game.h"
#include "world.h"

#include "error.h"

Game::menu_t Game::Editor::NewMaps(void)
{
    Display & display = Display::Get();
    display.Fill(0);

    // preload
    AGG::Cache::PreloadObject(ICN::BTNESIZE);
    AGG::Cache::PreloadObject(ICN::REDBACK);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    // image background
    const Sprite &back = AGG::GetICN(ICN::EDITOR, 0);
    const Point top((display.w() - back.w()) / 2, (display.h() - back.h()) / 2);
    back.Blit(top);

    const Sprite &panel = AGG::GetICN(ICN::REDBACK, 0);
    panel.Blit(top.x + 405, top.y + 5);

    LocalEvent & le = LocalEvent::Get();

    Button buttonSmall(top.x + 455, top.y + 45, ICN::BTNESIZE, 0, 1);
    Button buttonMedium(top.x + 455, top.y + 110, ICN::BTNESIZE, 2, 3);
    Button buttonLarge(top.x + 455, top.y + 175, ICN::BTNESIZE, 4, 5);
    Button buttonXLarge(top.x + 455, top.y + 240, ICN::BTNESIZE, 6, 7);
    Button buttonCancel(top.x + 455, top.y + 375, ICN::BTNESIZE, 8, 9);

    buttonSmall.Draw();
    buttonMedium.Draw();
    buttonLarge.Draw();
    buttonXLarge.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    // NewMap loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonSmall) ? buttonSmall.PressDraw() : buttonSmall.ReleaseDraw();
	le.MousePressLeft(buttonMedium) ? buttonMedium.PressDraw() : buttonMedium.ReleaseDraw();
	le.MousePressLeft(buttonLarge) ? buttonLarge.PressDraw() : buttonLarge.ReleaseDraw();
	le.MousePressLeft(buttonXLarge) ? buttonXLarge.PressDraw() : buttonXLarge.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	if(le.MouseClickLeft(buttonSmall))
	{
	    Game::ShowLoadMapsText();
    	    world.NewMaps(Maps::SMALL, Maps::SMALL);
	    return EDITSTART;
	}
	else
	if(le.MouseClickLeft(buttonMedium))
	{
	    Game::ShowLoadMapsText();
	    world.NewMaps(Maps::MEDIUM, Maps::MEDIUM);
	    return EDITSTART;
	}
	else
	if(le.MouseClickLeft(buttonLarge))
	{
	    Game::ShowLoadMapsText();
	    world.NewMaps(Maps::LARGE, Maps::LARGE);
	    return EDITSTART;
	}
	else
	if(le.MouseClickLeft(buttonXLarge))
	{
	    Game::ShowLoadMapsText();
	    world.NewMaps(Maps::XLARGE, Maps::XLARGE);
	    return EDITSTART;
	}
	if(le.MouseClickLeft(buttonCancel) || HotKeyPress(EVENT_DEFAULT_EXIT)) return EDITMAINMENU;

        // right info
	if(le.MousePressRight(buttonSmall)) 
	{
	    std::string str = _("Create a map that is %{size} squares wide by %{size} squares high.");
	    String::Replace(str, "%{size}", Maps::SMALL);
	    Dialog::Message("", str, Font::BIG);
	}
	else
	if(le.MousePressRight(buttonMedium))
	{
	    std::string str = _("Create a map that is %{size} squares wide by %{size} squares high.");
	    String::Replace(str, "%{size}", Maps::MEDIUM);
	    Dialog::Message("", str, Font::BIG);
	}
	else
	if(le.MousePressRight(buttonLarge))
	{
	    std::string str = _("Create a map that is %{size} squares wide by %{size} squares high.");
	    String::Replace(str, "%{size}", Maps::LARGE);
	    Dialog::Message("", str, Font::BIG);
	}
	else
	if(le.MousePressRight(buttonXLarge))
	{
	    std::string str = _("Create a map that is %{size} squares wide by %{size} squares high.");
	    String::Replace(str, "%{size}", Maps::XLARGE);
	    Dialog::Message("", str, Font::BIG);
	}
	else
	if(le.MousePressRight(buttonCancel)) Dialog::Message("", _("Cancel back to the main menu."), Font::BIG);
    }

    return EDITMAINMENU;
}

#endif
