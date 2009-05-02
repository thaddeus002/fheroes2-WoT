/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <afletdinov@mail.dc.baikal.ru>*
 *                         Josh Matthews <josh@joshmatthews.net>           *
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
#include "button.h"
#include "cursor.h"
#include "world.h"
#include "settings.h"
#include "dialog.h"
#include "gamearea.h"
#include "radar.h"
#include "kingdom.h"

#define PUZZLE_WIDTH 6
#define PUZZLE_HEIGHT 8

void DrawPiece(const Point &cur_pt, u8 x, u8 y)
{
    const Sprite &piece = AGG::GetICN(ICN::PUZZLE, y * PUZZLE_WIDTH + x);
    Display::Get().Blit(piece, Point(piece.x(), piece.y()) + cur_pt);
}

void InitPuzzle(std::vector<Point> & revealOrder)
{
    revealOrder.push_back(Point(5, 3));
    revealOrder.push_back(Point(5, 0));
    revealOrder.push_back(Point(0, 4));
    revealOrder.push_back(Point(1, 0));
    revealOrder.push_back(Point(2, 7));
    revealOrder.push_back(Point(0, 6));
    revealOrder.push_back(Point(5, 1));
    revealOrder.push_back(Point(1, 1));
    revealOrder.push_back(Point(4, 2));
    revealOrder.push_back(Point(5, 7));
    revealOrder.push_back(Point(0, 5));
    revealOrder.push_back(Point(3, 6));
    revealOrder.push_back(Point(3, 7));
    revealOrder.push_back(Point(4, 0));
    revealOrder.push_back(Point(1, 5));
    revealOrder.push_back(Point(2, 0));
    revealOrder.push_back(Point(1, 7));
    revealOrder.push_back(Point(4, 6));
    revealOrder.push_back(Point(3, 0));
    revealOrder.push_back(Point(1, 6));
    revealOrder.push_back(Point(4, 7));
    revealOrder.push_back(Point(0, 2));
    revealOrder.push_back(Point(3, 2));
    revealOrder.push_back(Point(2, 2));
    revealOrder.push_back(Point(2, 6));
    revealOrder.push_back(Point(5, 2));
    revealOrder.push_back(Point(4, 5));
    revealOrder.push_back(Point(0, 0));
    revealOrder.push_back(Point(0, 7));
    revealOrder.push_back(Point(5, 6));
    revealOrder.push_back(Point(4, 4));
    revealOrder.push_back(Point(5, 5));
    revealOrder.push_back(Point(0, 1));
    revealOrder.push_back(Point(1, 3));
    revealOrder.push_back(Point(1, 4));
    revealOrder.push_back(Point(4, 3));
    revealOrder.push_back(Point(1, 2));
    revealOrder.push_back(Point(2, 3));
    revealOrder.push_back(Point(5, 4));
    revealOrder.push_back(Point(3, 1));
    revealOrder.push_back(Point(4, 1));
    revealOrder.push_back(Point(2, 5));
    revealOrder.push_back(Point(3, 4));
    revealOrder.push_back(Point(0, 3));
    revealOrder.push_back(Point(2, 1));
    revealOrder.push_back(Point(3, 5));
    revealOrder.push_back(Point(2, 4));
    revealOrder.push_back(Point(3, 3));
}

void Dialog::PuzzleMaps(void)
{
    std::vector<Point> revealOrder;
    revealOrder.reserve(50);

    InitPuzzle(revealOrder);

    Display & display = Display::Get();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);
    
    AGG::PlayMusic(MUS::PUZZLE);

    const Surface & sf = world.GetUltimateArtifactArea();
    const u8 visited_obelisks = world.GetKingdom(Settings::Get().MyColor()).CountVisitedObjects(MP2::OBJ_OBELISK);
    const u8 max_obelisk = world.CountObeliskOnMaps();

    Dialog::FrameBorder background(false, sf.w(), sf.h());

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    const Rect &radar = Radar::Get().GetRect();
    const Sprite &puzzleTitle = AGG::GetICN(ICN::VIEWPUZL, 0);
    display.Blit(puzzleTitle, radar);

    if(display.w() == 640)
        dst_pt.x += BORDERWIDTH;
    if(display.h() == 480)
        dst_pt.y += BORDERWIDTH;
    display.Blit(sf, dst_pt);
    
    // determine which puzzle pieces are visible
    char pieces[PUZZLE_HEIGHT][PUZZLE_WIDTH];
    memset(pieces, 1, sizeof(pieces));
    const u16 revealPerObelisk = PUZZLE_WIDTH * PUZZLE_HEIGHT / max_obelisk;
    const u16 extraPieces = PUZZLE_WIDTH * PUZZLE_HEIGHT % max_obelisk;
    u8 open_puzzle = visited_obelisks * revealPerObelisk + extraPieces / max_obelisk;
    if(visited_obelisks == max_obelisk)
        open_puzzle += extraPieces % max_obelisk;
    u16 revealIndex = 0;

    while(open_puzzle-- && revealIndex < revealOrder.size())
    {
        Point revealPt = revealOrder[revealIndex++];
        pieces[revealPt.y][revealPt.x] = 0;
    }
    
    // The exit button ICNs for the exit button are screwed up (about 5x as much vertical space
    // as it needed, so the size is hardcoded here)
    ICN::icn_t buttonICN = Settings::Get().EvilInterface() ? ICN::LGNDXTRE : ICN::LGNDXTRA;
    const Sprite &button = AGG::GetICN(buttonICN, 4);
    dst_pt.x = radar.x + (radar.w - button.w()) / 2;
    dst_pt.y = radar.y + radar.h - 27 - 10;
    Button buttonExit(dst_pt, buttonICN, 4, 5);

    buttonExit.Draw();

    if(Settings::Get().Debug()) Error::Verbose("Dialog::PuzzleMaps: debug mode, show all");
    
    s16 alpha = 250;
    while(!Settings::Get().Debug() && alpha >= 0)
    {
        Point border(BORDERWIDTH, BORDERWIDTH);
        Point adjusted(cur_pt - (display.w() != 640 ? border : Point(0, 0)));
        for(u8 y = 0; y < PUZZLE_HEIGHT; y++)
            for(u8 x = 0; x < PUZZLE_WIDTH; x++)
                if(!pieces[y][x])
                {
                    const Sprite &piece = AGG::GetICN(ICN::PUZZLE, y * PUZZLE_WIDTH + x);
                    Surface fadePiece(piece);
                    Point src(piece.x(), piece.y()), disp(src + cur_pt);
                    src -= border;
                    if(display.w() != 640)
                        disp -= border;
                    Rect area(src.x, src.y, piece.w(), piece.h());
                    display.Blit(sf, area, disp.x, disp.y);
                    fadePiece.SetAlpha(alpha);
                    display.Blit(fadePiece, disp.x, disp.y);

                    for(s8 i = -1; i <= 1; i++)
                        for(s8 j = -1; j <= 1; j++)
                            if((u8)(x + j) < PUZZLE_WIDTH && (u8)(y + i) < PUZZLE_HEIGHT && pieces[y + i][x + j])
                                DrawPiece(adjusted, x + j, y + i);
                }
                else DrawPiece(adjusted, x, y);
        alpha -= 25;
        display.Flip();
        DELAY(10);
    }

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
    }

    // In higher resolutions, the radar is not included in the FrameBorder,
    // so we need to make sure that it is explicitly redrawn.
    Radar::Get().RedrawArea();
}
