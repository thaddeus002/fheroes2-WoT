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
#include "button.h"
#include "cursor.h"
#include "splitter.h"
#include "difficulty.h"
#include "settings.h"
#include "maps.h"
#include "maps_fileinfo.h"
#include "text.h"
#include "tools.h"
#include "pocketpc.h"

void PocketPCRedrawMapsFileList(const Point &, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);


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

    MapsFileInfoList all;
    if(!PrepareMapsFileInfoList(all))
    {
        Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
        return Game::MAINMENU;
    }

    std::string filename;

    MapsFileInfoList small;
    MapsFileInfoList medium;
    MapsFileInfoList large;
    MapsFileInfoList xlarge;
    MapsFileInfoList *curlist = &all;

    small.reserve(all.size());
    medium.reserve(all.size());
    large.reserve(all.size());
    xlarge.reserve(all.size());

    MapsFileInfoList::iterator top = all.begin();
    MapsFileInfoList::iterator cur = all.begin();

    for(; cur != all.end(); ++ cur)
    {
	switch((*cur).size_w)
	{
    	    case Maps::SMALL:	small.push_back(*cur); break;
    	    case Maps::MEDIUM:	medium.push_back(*cur); break;
    	    case Maps::LARGE:	large.push_back(*cur); break;
    	    case Maps::XLARGE:	xlarge.push_back(*cur); break;
	    default: continue;
	}
    }

    const u16 window_w = 320;
    const u16 window_h = 224;
        
    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH, (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw();

    const Rect & dst_rt = frameborder.GetArea();
    const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
    display.Blit(background, Rect(0, 0, window_w, window_h), dst_rt);

    const Rect & rt = frameborder.GetArea();
    const Rect list_rt(rt.x + 17, rt.y + 37, 266, 156);

    const Rect countPlayers(rt.x + 45, rt.y + 55, 20, 175);
    const Rect sizeMaps(rt.x + 62, rt.y + 55, 20, 175);
    const Rect victoryCond(rt.x + 267, rt.y + 55, 20, 175);
    const Rect lossCond(rt.x + 287, rt.y + 55, 20, 175);

    Button buttonOk(rt.x + rt.w / 2 - 160, rt.y + rt.h - 30, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(rt.x + rt.w / 2 + 60, rt.y + rt.h - 30, ICN::NGEXTRA, 68, 69);

    display.Blit(AGG::GetICN(ICN::REQSBKG, 0), Rect(325, 70, 16, 100), rt.x + 283, rt.y + 55);
    display.Blit(AGG::GetICN(ICN::REQSBKG, 0), Rect(325, 167, 16, 50), rt.x + 283, rt.y + 125);
    Button buttonPgUp(rt.x + 285, rt.y + 40, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(rt.x + 285, rt.y + 175, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(rt.x + 7, rt.y + 12, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(rt.x + 69, rt.y + 12, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(rt.x + 131, rt.y + 12, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(rt.x + 193, rt.y + 12, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(rt.x + 255, rt.y + 12, ICN::REQUESTS, 17, 18);

    if(curlist->empty()) buttonOk.SetDisable(true);

    const u8 max_items = 8;
    top = curlist->begin();
    cur = top;

    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 288, rt.y + 58, 12, 114), Splitter::VERTICAL);
    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));

    PocketPCRedrawMapsFileList(rt, *curlist, top, cur, max_items);
    filename.clear();

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();

    if(small.empty()) buttonSelectSmall.SetDisable(true);
    if(medium.empty()) buttonSelectMedium.SetDisable(true);
    if(large.empty()) buttonSelectLarge.SetDisable(true);
    if(xlarge.empty()) buttonSelectXLarge.SetDisable(true);
    
    buttonSelectSmall.Draw();
    buttonSelectMedium.Draw();
    buttonSelectLarge.Draw();
    buttonSelectXLarge.Draw();
    buttonSelectAll.Draw();

    split.Redraw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();
    bool redraw = false;

    while(le.HandleEvents())
    {
        if(buttonOk.isEnable()) le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
        le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
        le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();
	le.MousePressLeft(buttonSelectSmall) && buttonSelectSmall.isEnable() ? buttonSelectSmall.PressDraw() : buttonSelectSmall.ReleaseDraw();
	le.MousePressLeft(buttonSelectMedium) && buttonSelectMedium.isEnable() ? buttonSelectMedium.PressDraw() : buttonSelectMedium.ReleaseDraw();
	le.MousePressLeft(buttonSelectLarge) && buttonSelectLarge.isEnable() ? buttonSelectLarge.PressDraw() : buttonSelectLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectXLarge) && buttonSelectXLarge.isEnable() ? buttonSelectXLarge.PressDraw() : buttonSelectXLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.PressDraw() : buttonSelectAll.ReleaseDraw();

        if((buttonOk.isEnable() && le.MouseClickLeft(buttonOk)) || le.KeyPress(KEY_RETURN)) break;
	else
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE)) return Game::MAINMENU;
	else
        if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > curlist->begin()))
	{
	    top = (top - curlist->begin() > max_items ? top - max_items : curlist->begin());
	    split.Move(top - curlist->begin());
	    redraw = true;
	}
	else
        if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + max_items < curlist->end()))
	{
	    top += max_items;
	    if(top + max_items > curlist->end()) top = curlist->end() - max_items;
	    split.Move(top - curlist->begin());
	    redraw = true;
	}
	else
        if(le.KeyPress(KEY_UP) && (cur > curlist->begin()))
	{
	    --cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur;
		if(top + max_items > curlist->end()) top = curlist->end() - max_items;
	    }
	    split.Move(top - curlist->begin());
	    redraw = true;
	}
	else
        if(le.KeyPress(KEY_DOWN) && (cur < (curlist->end() - 1)))
	{
	    ++cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur - max_items + 1;
		if(top + max_items > curlist->end()) top = curlist->end() - max_items;
		else
		if(top < curlist->begin()) top = curlist->begin();
	    }
	    split.Move(top - curlist->begin());
	    redraw = true;
	}
	else
	if((le.MouseWheelUp(list_rt) || le.MouseWheelUp(split.GetRect())) && (top > curlist->begin()))
	{
	    --top;
	    split.Backward();
	    redraw = true;
	}
	else
	if((le.MouseWheelDn(list_rt) || le.MouseWheelDn(split.GetRect())) && (top < (curlist->end() - max_items)))
	{
	    ++top;
	    split.Forward();
	    redraw = true;
	}
	else
	if(le.MousePressLeft(split.GetRect()) && (curlist->size() > max_items))
	{
	    s16 seek = (le.GetMouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();
	    top = curlist->begin() + seek;
	    split.Move(seek);
	    redraw = true;
 	}
	else
	if(le.MouseClickLeft(list_rt) && curlist->size())
	{
	    MapsFileInfoList::iterator cur2 = top + static_cast<size_t>((le.GetMouseCursor().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur2 > (curlist->end() - 1)) cur2 = curlist->end() - 1;
	    // double click
	    if(cur == cur2)
	    {
		break;
	    }
	    else
		cur = cur2;
	    redraw = true;
	}
	else
	if(((le.MouseClickLeft(buttonSelectSmall) || le.KeyPress(KEY_s)) && buttonSelectSmall.isEnable()) && buttonSelectSmall.isEnable())
	{
	    curlist = &small;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}
	else
	if(((le.MouseClickLeft(buttonSelectMedium) || le.KeyPress(KEY_m)) && buttonSelectMedium.isEnable()) && buttonSelectMedium.isEnable())
	{
	    curlist = &medium;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}
	else
	if(((le.MouseClickLeft(buttonSelectLarge) || le.KeyPress(KEY_l)) && buttonSelectLarge.isEnable()) && buttonSelectLarge.isEnable())
	{
	    curlist = &large;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}
	else
	if(((le.MouseClickLeft(buttonSelectXLarge) || le.KeyPress(KEY_x)) && buttonSelectXLarge.isEnable()) && buttonSelectXLarge.isEnable())
	{
	    curlist = &xlarge;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}
	else
	if(le.MouseClickLeft(buttonSelectAll) || le.KeyPress(KEY_a))
	{
	    curlist = &all;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	if(redraw)
	{
	    cursor.Hide();
	    PocketPCRedrawMapsFileList(rt, *curlist, top, cur, max_items);
	    cursor.Show();
	    display.Flip();
	    redraw = false;
	}
    }

    filename = (*cur).file;

    if(filename.size())
    {
        conf.LoadFileMaps(filename);
        conf.SetMyColor(conf.FirstAllowColor());
        conf.SetPlayersColors(conf.MyColor());
        conf.SetGameDifficulty(Difficulty::NORMAL);
    }

    return Game::SCENARIOINFO;
}

void PocketPCRedrawMapsFileList(const Point & dst, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
{
    Display & display = Display::Get();
    const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
    display.Blit(background, Rect(17, 37, 266, 156), dst.x + 15, dst.y + 35);

    Text text;
    u16 oy = 40;
    u8 index = 0;

    for(MapsFileInfoList::const_iterator ii = top; ii != lists.end() && (ii - top < max_items); ++ii)
    {
	const Maps::FileInfo & info = *ii;

	index = 19 + info.KingdomColorsCount();

	const Sprite & spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, dst.x + 19, dst.y + oy);

	switch(info.size_w)
	{
            case Maps::SMALL:	index = 26; break;
            case Maps::MEDIUM:	index = 27; break;
            case Maps::LARGE:	index = 28; break;
            case Maps::XLARGE:	index = 29; break;
            default:  		index = 30; break;
        }

	const Sprite & spriteSize = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteSize, dst.x + 19 + spriteCount.w() + 2, dst.y + oy);

	text.Set(info.name, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 74, dst.y + oy);

	index = 30 + info.conditions_wins;
	const Sprite & spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, dst.x + 244, dst.y + oy);

	index = 36 + info.conditions_loss;
	const Sprite & spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, dst.x + 244 + spriteWins.w() + 2, dst.y + oy);

	oy += text.h() + 2;
    }
}
