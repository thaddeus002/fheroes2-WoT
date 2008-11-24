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
#include "game_statuswindow.h"
#include "game_selectobjbar.h"
#include "game_interface.h"

Game::Interface::Interface()
{
}

Game::Interface & Game::Interface::Get(void)
{
    static Interface interface;

    return interface;
}

u8 Game::Interface::CountIcons(void)
{
    return count_icons;
}

void Game::Interface::DrawBorder(bool drawMiddle, bool useAlt)
{
    Display & display = Display::Get();

    const bool evil = Settings::Get().EvilInterface();

    const u8 count_w = (display.w() - 640) / TILEWIDTH;
    const u8 count_h = (display.h() - 480) / TILEWIDTH;

    Rect srcrt;
    Point dstpt;
    const Sprite *border;
    if(useAlt)
      border = &AGG::GetICN(ICN::STONEBAK, 0);
    else
      border = &AGG::GetICN(evil ? ICN::ADVBORDE : ICN::ADVBORD, 0);

    const Sprite &icnadv = *border;

    // TOP BORDER
    srcrt.x = 0;
    srcrt.y = 0;
    srcrt.w = 223;
    srcrt.h = BORDERWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.x = 223;
    srcrt.w = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = 0;
    for(u8 ii = 0; ii < count_w + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.x += TILEWIDTH;
    }
    srcrt.x += TILEWIDTH;
    srcrt.w = icnadv.w() - srcrt.x;
    display.Blit(icnadv, srcrt, dstpt);

    // BOTTOM BORDER
    srcrt.x = 0;
    srcrt.y = icnadv.h() - BORDERWIDTH;
    srcrt.w = 223;
    srcrt.h = BORDERWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = display.h() - BORDERWIDTH;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.x = 223;
    srcrt.w = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = display.h() - BORDERWIDTH;
    for(u8 ii = 0; ii < count_w + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.x += TILEWIDTH;
    }
    srcrt.x += TILEWIDTH;
    srcrt.w = icnadv.w() - srcrt.x;
    display.Blit(icnadv, srcrt, dstpt);

    // LEFT BORDER
    srcrt.x = 0;
    srcrt.y = 0;
    srcrt.w = BORDERWIDTH;
    srcrt.h = 255;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = 255;
    srcrt.h = TILEWIDTH;
    dstpt.x = srcrt.x;
    dstpt.y = srcrt.y;
    for(u8 ii = 0; ii < count_h + 1; ++ii)
    {
        display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += TILEWIDTH;
    }
    srcrt.y += TILEWIDTH;
    srcrt.h = icnadv.h() - srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);

    if(drawMiddle)
    {
        // MIDDLE BORDER
        srcrt.x = icnadv.w() - RADARWIDTH - 2 * BORDERWIDTH;
        srcrt.y = 0;
        srcrt.w = BORDERWIDTH;
        srcrt.h = 255;
        dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
        dstpt.y = srcrt.y;
        display.Blit(icnadv, srcrt, dstpt);
        srcrt.y = 255;
        srcrt.h = TILEWIDTH;
        dstpt.x = display.w() - RADARWIDTH - 2 * BORDERWIDTH;
        dstpt.y = srcrt.y;
        for(u8 ii = 0; ii < count_h + 1; ++ii)
        {
            display.Blit(icnadv, srcrt, dstpt);
            dstpt.y += TILEWIDTH;
        }
        srcrt.y += TILEWIDTH;
        srcrt.h = icnadv.h() - srcrt.y;
        display.Blit(icnadv, srcrt, dstpt);

        // RIGHT BORDER
        srcrt.x = icnadv.w() - BORDERWIDTH;
        srcrt.y = 0;
        srcrt.w = BORDERWIDTH;
        srcrt.h = 255;
        dstpt.x = display.w() - BORDERWIDTH;
        dstpt.y = srcrt.y;
        display.Blit(icnadv, srcrt, dstpt);
        srcrt.y = 255;
        srcrt.h = TILEWIDTH;
        dstpt.x = display.w() - BORDERWIDTH;
        dstpt.y = srcrt.y;
        for(u8 ii = 0; ii < count_h + 1; ++ii)
        {
            display.Blit(icnadv, srcrt, dstpt);
            dstpt.y += TILEWIDTH;
        }
        srcrt.y += TILEWIDTH;
        srcrt.h = icnadv.h() - srcrt.y;
        display.Blit(icnadv, srcrt, dstpt);

        // ICON BORDER
        srcrt.x = icnadv.w() - RADARWIDTH - BORDERWIDTH;
        srcrt.y = RADARWIDTH + BORDERWIDTH;
        srcrt.w = RADARWIDTH;
        srcrt.h = BORDERWIDTH;
        dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
        dstpt.y = srcrt.y;
        display.Blit(icnadv, srcrt, dstpt);
        dstpt.y = srcrt.y + BORDERWIDTH + count_icons * 32;
        srcrt.y = srcrt.y + BORDERWIDTH + 4 * 32;
        display.Blit(icnadv, srcrt, dstpt);
    }
}

void Game::Interface::Draw(void)
{
    Display & display = Display::Get();
    const bool evil = Settings::Get().EvilInterface();
    const ICN::icn_t icnscroll = evil ? ICN::SCROLLE : ICN::SCROLL;
    const ICN::icn_t icnbtn = evil ? ICN::ADVEBTNS : ICN::ADVBTNS;

    Point pt_stw, pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt, pt_set, pt_shd, pt_scd;

    // coord button heroes scroll up
    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // coord button castle scroll up
    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // recalculate buttons coordinate
    const u8 count_h = (display.h() - 480) / TILEWIDTH;
    count_icons = count_h > 3 ? 8 : ( count_h < 3 ? 4 : 7);
    const u16 last_coord = RADARWIDTH + BORDERWIDTH * 2 + count_icons * 32 + BORDERWIDTH;
    const u8 button_w = AGG::GetICN(icnbtn, 0).w();
    const u8 button_h = AGG::GetICN(icnbtn, 0).h();

    // coord status window
    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_stw.y = RADARWIDTH + BORDERWIDTH * 2 + count_icons * 32 + BORDERWIDTH + 2 * button_h;
    // coord button next hero
    pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_her.y = last_coord;
    // coord button action
    pt_act.x = pt_her.x + button_w;
    pt_act.y = last_coord;
    // coord button castle
    pt_cas.x = pt_act.x + button_w;
    pt_cas.y = last_coord;
    // coord button magic
    pt_mag.x = pt_cas.x + button_w;
    pt_mag.y = last_coord;
    // coord button end tur
    pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
    pt_end.y = last_coord + button_h;
    // coord button info
    pt_inf.x = pt_end.x + button_w;
    pt_inf.y = last_coord + button_h;
    // coord button option
    pt_opt.x = pt_inf.x + button_w;
    pt_opt.y = last_coord + button_h;
    // coord button settings
    pt_set.x = pt_opt.x + button_w;
    pt_set.y = last_coord + button_h;
    // coord button heroes scroll down
    pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * count_icons - 15;
    // coord button castle scroll down
    pt_scd.x = display.w() - BORDERWIDTH - 15;
    pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * count_icons - 15;

    /**/
    //display.Fill(0x00, 0x00, 0x00);

    Rect srcrt;
    Point dstpt;

    // ICON PANEL
    const Sprite & icnadv = AGG::GetICN(evil ? ICN::ADVBORDE : ICN::ADVBORD, 0);
    srcrt.x = icnadv.w() - RADARWIDTH - BORDERWIDTH;
    srcrt.y = RADARWIDTH + 2 * BORDERWIDTH;
    srcrt.w = RADARWIDTH;
    srcrt.h = 32;
    dstpt.x = display.w() - RADARWIDTH - BORDERWIDTH;
    dstpt.y = srcrt.y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = srcrt.y + srcrt.h;
    dstpt.y = dstpt.y + srcrt.h;
    srcrt.h = 32;
    for(u8 ii = 0; ii < count_icons - 2; ++ii)
    {
	display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += srcrt.h;
    }
    srcrt.y = srcrt.y + 64;
    srcrt.h = 32;
    display.Blit(icnadv, srcrt, dstpt);

    Game::SelectBarCastle::Get().Redraw();
    Game::SelectBarHeroes::Get().Redraw();

    Game::StatusWindow::Get().SetPos(pt_stw);
    Game::StatusWindow::Get().Redraw();

    buttonScrollHeroesUp.SetSprite(icnscroll, 0, 1);
    buttonScrollCastleUp.SetSprite(icnscroll, 0, 1);
    buttonNextHero.SetSprite(icnbtn, 0, 1);
    buttonMovement.SetSprite(icnbtn, 2, 3);
    buttonKingdom.SetSprite(icnbtn, 4, 5);
    buttonSpell.SetSprite(icnbtn, 6, 7);
    buttonEndTur.SetSprite(icnbtn, 8, 9);
    buttonAdventure.SetSprite(icnbtn, 10, 11);
    buttonFile.SetSprite(icnbtn, 12, 13);
    buttonSystem.SetSprite(icnbtn, 14, 15);
    buttonScrollHeroesDown.SetSprite(icnscroll, 2, 3);
    buttonScrollCastleDown.SetSprite(icnscroll, 2, 3);

    buttonScrollHeroesUp.SetPos(pt_shu);
    buttonScrollCastleUp.SetPos(pt_scu);
    buttonNextHero.SetPos(pt_her);
    buttonMovement.SetPos(pt_act);
    buttonKingdom.SetPos(pt_cas);
    buttonSpell.SetPos(pt_mag);
    buttonEndTur.SetPos(pt_end);
    buttonAdventure.SetPos(pt_inf);
    buttonFile.SetPos(pt_opt);
    buttonSystem.SetPos(pt_set);
    buttonScrollHeroesDown.SetPos(pt_shd);
    buttonScrollCastleDown.SetPos(pt_scd);

    buttonScrollHeroesUp.Draw();
    buttonScrollCastleUp.Draw();
    buttonNextHero.Draw();
    buttonMovement.Draw();
    buttonKingdom.Draw();
    buttonSpell.Draw();
    buttonEndTur.Draw();
    buttonAdventure.Draw();
    buttonFile.Draw();
    buttonSystem.Draw();
    buttonScrollHeroesDown.Draw();
    buttonScrollCastleDown.Draw();

    DrawBorder(true, false);
}
