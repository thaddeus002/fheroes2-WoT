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
#include "cursor.h"
#include "settings.h"
#include "text.h"
#include "dialog.h"
#include "button.h"
#include "difficulty.h"
#include "world.h"
#include "pocketpc.h"

extern bool DialogSelectMapsFileList(MapsFileInfoList &, std::string &);

Game::menu_t PocketPC::SelectScenario(void)
{
    Game::SetFixVideoMode();
    AGG::PlayMusic(MUS::MAINMENU);

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    Settings & conf = Settings::Get();

    conf.SetGameType(Game::STANDARD);

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite = AGG::GetICN(ICN::HEROES, 0);
    Rect src_rt((sprite.w() - display.w()) / 2, 0, display.w(), display.h());
    display.Blit(sprite, src_rt, 0, 0);

    MapsFileInfoList lists;
    if(!PrepareMapsFileInfoList(lists))
    {
        Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
        return Game::MAINMENU;
    }

    std::string filemaps;
    PocketPC::DialogSelectMapsFileList(lists, filemaps);

    if(filemaps.size())
    {
	conf.LoadFileMaps(filemaps);
	conf.SetMyColor(conf.FirstAllowColor());
        conf.SetPlayersColors(conf.MyColor());
	conf.SetGameDifficulty(Difficulty::NORMAL);
    }

    return Game::SCENARIOINFO;
}

Game::menu_t PocketPC::ScenarioInfo(void)
{
    Game::SetFixVideoMode();
    AGG::PlayMusic(MUS::MAINMENU);

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite = AGG::GetICN(ICN::STONEBAK, 0);
    Rect src_rt((sprite.w() - display.w()) / 2, (sprite.h() - display.h()) / 2, display.w(), display.h());
    display.Blit(sprite, src_rt, 0, 0);

    Button buttonOk(display.w() / 2 - 140, display.h() - 35, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(display.w() / 2 + 40, display.h() - 35, ICN::NGEXTRA, 68, 69);
/*
    Text text;

    text.Set(_("Free Heroes II"), Font::BIG);
    text.Blit(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 10);

    text.Set(conf.BuildVersion(), Font::SMALL);
    text.Blit(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 148);

    text.Set(_("New Game"), Font::BIG);
    const Rect rectNewGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 40, text.w(), text.h());
    text.Blit(rectNewGame);

    text.Set(_("Load Game"));
    const Rect rectLoadGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 70, text.w(), text.h());
    text.Blit(rectLoadGame);

    text.Set(_("Quit"));
    const Rect rectQuitGame(dst_pt.x + (src_rt.w - text.w()) / 2, dst_pt.y + 100, text.w(), text.h());
    text.Blit(rectQuitGame);
*/
    buttonOk.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    // mainmenu loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

        // click cancel
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
        {
            Settings::Get().SetGameType(Game::UNKNOWN);
            return Game::MAINMENU;
            break;
        }
            else
	// click ok
        if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk))
        {
            if(Settings::Get().Debug()) Error::Verbose("select maps: " + conf.MapsFile());
            if(Settings::Get().Debug()) Error::Verbose("difficulty: " + Difficulty::String(conf.GameDifficulty()));
            if(Settings::Get().Debug()) Error::Verbose("select color: " + Color::String(conf.MyColor()));
            conf.FixKingdomRandomRace();
    	    Game::ShowLoadMapsText();
            world.LoadMaps(conf.MapsFile());
            return Game::STARTGAME;
            break;
        }
    }

    return Game::QUITGAME;
}
