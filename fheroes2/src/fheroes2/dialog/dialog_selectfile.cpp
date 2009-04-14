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

#include <bitset>
#include <algorithm>
#include <string>
#include "dir.h"
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "difficulty.h"
#include "splitter.h"
#include "settings.h"
#include "dialog.h"


extern char *basename(const char *path);
bool SelectFileListSimple(const std::string &, MapsFileInfoList &, std::string &);
void RedrawFileListSimple( const Rect &, const std::string &, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);
void RedrawMapsFileList(const Rect &, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);

bool Dialog::SelectFileSave(std::string & file)
{
    Dir dir;
    const std::string store_dir(Settings::Get().LocalDataPrefix() + SEPARATOR + "save");
    dir.Read(store_dir, ".sav", false);

    MapsFileInfoList lists(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii) if(!lists[ii].ReadXML(*itd)) --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);
    std::sort(lists.begin(), lists.end(), Maps::FileInfo::PredicateForSorting);

    return SelectFileListSimple(_("File to Save:"), lists, file);
}

bool Dialog::SelectFileLoad(std::string & file)
{
    Dir dir;
    const std::string store_dir(Settings::Get().LocalDataPrefix() + SEPARATOR + "save");
    dir.Read(store_dir, ".sav", false);

    MapsFileInfoList lists(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii) if(!lists[ii].ReadXML(*itd)) --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);
    std::sort(lists.begin(), lists.end(), Maps::FileInfo::PredicateForSorting);

    return SelectFileListSimple(_("File to Load:"), lists, file);
}

bool SelectFileListSimple(const std::string & header, MapsFileInfoList & lists, std::string & filename)
{
    // preload
    AGG::PreloadObject(ICN::REQBKG);
    AGG::PreloadObject(ICN::REQUEST);

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite & panel = AGG::GetICN(ICN::REQBKG, 0);
    Background back((display.w() - panel.w()) / 2, (display.h() - panel.h()) / 2, panel.w(), panel.h());
    const Rect & rt = back.GetRect();
    const Rect list_rt(rt.x + 40, rt.y + 55, 265, 215);

    Button buttonCancel(rt.x + 34, rt.y + 315, ICN::REQUEST, 3, 4);
    Button buttonOk(rt.x + 244, rt.y + 315, ICN::REQUEST, 1, 2);
    Button buttonPgUp(rt.x + 327, rt.y + 55, ICN::REQUEST, 5, 6);
    Button buttonPgDn(rt.x + 327, rt.y + 257, ICN::REQUEST, 7, 8);

    if(lists.empty()) buttonOk.SetDisable(true);

    const u8 max_items = 11;
    MapsFileInfoList::iterator top = lists.begin();
    MapsFileInfoList::iterator cur = lists.begin();

    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 330, rt.y + 73, 12, 180), Splitter::VERTICAL);
    split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));

    RedrawFileListSimple(rt, header, lists, top, cur, max_items);
    filename.clear();

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    split.Redraw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
    bool redraw = false;

    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) && buttonOk.isEnable() ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
        le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
        le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

        if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN)){ if(cur != lists.end()) filename = (*cur).FileMaps(); break; };
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE)){ filename.clear(); break; };

        if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > lists.begin()))
	{
	    top = (top - lists.begin() > max_items ? top - max_items : lists.begin());
	    split.Move(top - lists.begin());
	    redraw = true;
	}

        if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + max_items < lists.end()))
	{
	    top += max_items;
	    if(top + max_items > lists.end()) top = lists.end() - max_items;
	    split.Move(top - lists.begin());
	    redraw = true;
	}

        if(le.KeyPress(KEY_UP) && (cur > lists.begin()))
	{
	    --cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur;
		if(top + max_items > lists.end()) top = lists.end() - max_items;
	    }
	    split.Move(top - lists.begin());
	    redraw = true;
	}

        if(le.KeyPress(KEY_DOWN) && (cur < (lists.end() - 1)))
	{
	    ++cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur - max_items + 1;
		if(top + max_items > lists.end()) top = lists.end() - max_items;
		else
		if(top < lists.begin()) top = lists.begin();
	    }
	    split.Move(top - lists.begin());
	    redraw = true;
	}

	if((le.MouseWheelUp(list_rt) || le.MouseWheelUp(split.GetRect())) && (top > lists.begin()))
	{
	    --top;
	    split.Backward();
	    redraw = true;
	}

	if((le.MouseWheelDn(list_rt) || le.MouseWheelDn(split.GetRect())) && (top < (lists.end() - max_items)))
	{
	    ++top;
	    split.Forward();
	    redraw = true;
	}

	if(le.MousePressLeft(split.GetRect()) && (lists.size() > max_items))
	{
	    s16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();
	    top = lists.begin() + seek;
	    split.Move(seek);
	    redraw = true;
 	}

	if(le.KeyPress(KEY_DELETE) && cur != lists.end())
	{
	    std::string msg(_("Are you sure you want to delete file:"));
	    msg.append("\n \n");
	    msg.append(basename((*cur).FileMaps().c_str()));
	    if(Dialog::YES == Dialog::Message(_("Warning!"), msg, Font::BIG, Dialog::YES | Dialog::NO))
	    {
		unlink((*cur).FileMaps().c_str());
		lists.erase(cur);
		top = lists.begin();
		cur = top;
		split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));
		split.Move(top - lists.begin());
		redraw = true;
	    }
	}

	if(le.MouseClickLeft(list_rt) && lists.size())
	{
	    cur = top + static_cast<size_t>((le.MouseReleaseLeft().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur > (lists.end() - 1)) cur = lists.end() - 1;
	    redraw = true;
	}

	if(redraw)
	{
	    cursor.Hide();
	    RedrawFileListSimple(rt, header, lists, top, cur, max_items);
	    buttonOk.Draw();
	    buttonCancel.Draw();
	    buttonPgUp.Draw();
	    buttonPgDn.Draw();
	    split.Redraw();
	    cursor.Show();
	    display.Flip();
	    redraw = false;
	}
    }

    return filename.size();
}

void RedrawFileListSimple(const Rect & dst, const std::string & header, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
{
    Display & display = Display::Get();
    const Sprite & panel = AGG::GetICN(ICN::REQBKG, 0);
    display.Blit(panel, dst);

    Text text(header, Font::BIG);
    text.Blit(dst.x + 175 - text.w() / 2, dst.y + 30);

    u16 oy = 60;
    char short_date[15];
         short_date[14] = 0;

    for(MapsFileInfoList::const_iterator ii = top; ii != lists.end() && (ii - top < max_items); ++ii)
    {
	const Maps::FileInfo & info = *ii;

	const std::string name(basename(info.FileMaps().c_str()));

	std::strftime(short_date, 14, "%b %d, %H:%M", std::localtime(&info.Time()));

	text.Set(basename(info.FileMaps().c_str()), (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 45, dst.y + oy);

	text.Set(short_date, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 305 - text.w(), dst.y + oy);

	oy += text.h() + 2;
    }

    if(cur != lists.end())
    {
	text.Set(basename((*cur).FileMaps().c_str()), Font::BIG);
	text.Blit(dst.x + 175 - text.w() / 2, dst.y + 289);
    }
}

void Dialog::SelectMapsFileList(MapsFileInfoList & lists, std::string & filename)
{
    AGG::PreloadObject(ICN::REQSBKG);
    AGG::PreloadObject(ICN::REQUEST);

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    MapsFileInfoList & all = lists;
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
	switch((*cur).SizeMaps().w)
	{
    	    case Maps::SMALL:	small.push_back(*cur); break;
    	    case Maps::MEDIUM:	medium.push_back(*cur); break;
    	    case Maps::LARGE:	large.push_back(*cur); break;
    	    case Maps::XLARGE:	xlarge.push_back(*cur); break;
	    default: continue;
	}
    }
    
    const Sprite & panel = AGG::GetICN(ICN::REQSBKG, 0);
    Background back((display.w() - panel.w()) / 2, (display.h() - panel.h()) / 2, panel.w(), panel.h());
    const Rect & rt = back.GetRect();
    const Rect list_rt(rt.x + 55, rt.y + 55, 270, 175);

    const Rect countPlayers(rt.x + 45, rt.y + 55, 20, 175);
    const Rect sizeMaps(rt.x + 62, rt.y + 55, 20, 175);
    const Rect victoryCond(rt.x + 267, rt.y + 55, 20, 175);
    const Rect lossCond(rt.x + 287, rt.y + 55, 20, 175);

    Button buttonOk(rt.x + 140, rt.y + 410, ICN::REQUESTS, 1, 2);
    Button buttonPgUp(rt.x + 327, rt.y + 55, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(rt.x + 327, rt.y + 217, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(rt.x + 37, rt.y + 22, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(rt.x + 99, rt.y + 22, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(rt.x + 161, rt.y + 22, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(rt.x + 223, rt.y + 22, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(rt.x + 285, rt.y + 22, ICN::REQUESTS, 17, 18);

    if(curlist->empty()) buttonOk.SetDisable(true);

    const u8 max_items = 9;
    top = curlist->begin();
    cur = top;

    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 330, rt.y + 73, 12, 141), Splitter::VERTICAL);
    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));

    RedrawMapsFileList(rt, *curlist, top, cur, max_items);
    filename.clear();

    buttonOk.Draw();
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

    LocalEvent & le = LocalEvent::GetLocalEvent();
    bool redraw = false;

    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) && buttonOk.isEnable() ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
        le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();
	le.MousePressLeft(buttonSelectSmall) && buttonSelectSmall.isEnable() ? buttonSelectSmall.PressDraw() : buttonSelectSmall.ReleaseDraw();
	le.MousePressLeft(buttonSelectMedium) && buttonSelectMedium.isEnable() ? buttonSelectMedium.PressDraw() : buttonSelectMedium.ReleaseDraw();
	le.MousePressLeft(buttonSelectLarge) && buttonSelectLarge.isEnable() ? buttonSelectLarge.PressDraw() : buttonSelectLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectXLarge) && buttonSelectXLarge.isEnable() ? buttonSelectXLarge.PressDraw() : buttonSelectXLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.PressDraw() : buttonSelectAll.ReleaseDraw();

        if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;

        if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > curlist->begin()))
	{
	    top = (top - curlist->begin() > max_items ? top - max_items : curlist->begin());
	    split.Move(top - curlist->begin());
	    redraw = true;
	}

        if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + max_items < curlist->end()))
	{
	    top += max_items;
	    if(top + max_items > curlist->end()) top = curlist->end() - max_items;
	    split.Move(top - curlist->begin());
	    redraw = true;
	}

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

	if((le.MouseWheelUp(list_rt) || le.MouseWheelUp(split.GetRect())) && (top > curlist->begin()))
	{
	    --top;
	    split.Backward();
	    redraw = true;
	}

	if((le.MouseWheelDn(list_rt) || le.MouseWheelDn(split.GetRect())) && (top < (curlist->end() - max_items)))
	{
	    ++top;
	    split.Forward();
	    redraw = true;
	}

	if(le.MousePressLeft(split.GetRect()) && (curlist->size() > max_items))
	{
	    s16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();
	    top = curlist->begin() + seek;
	    split.Move(seek);
	    redraw = true;
 	}

	if(le.MouseClickLeft(list_rt) && curlist->size())
	{
	    cur = top + static_cast<size_t>((le.MouseReleaseLeft().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur > (curlist->end() - 1)) cur = curlist->end() - 1;
	    redraw = true;
	}

	if(le.MouseClickLeft(buttonSelectSmall) && buttonSelectSmall.isEnable())
	{
	    curlist = &small;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	if(le.MouseClickLeft(buttonSelectMedium) && buttonSelectMedium.isEnable())
	{
	    curlist = &medium;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	if(le.MouseClickLeft(buttonSelectLarge) && buttonSelectLarge.isEnable())
	{
	    curlist = &large;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	if(le.MouseClickLeft(buttonSelectXLarge) && buttonSelectXLarge.isEnable())
	{
	    curlist = &xlarge;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	if(le.MouseClickLeft(buttonSelectAll))
	{
	    curlist = &all;
	    cur = top = curlist->begin();
	    split.SetRange(0, (max_items < curlist->size() ? curlist->size() - max_items : 0));
	    redraw = true;
	}

	// right info
	if(le.MousePressRight(buttonSelectSmall)) Dialog::Message(_("Small Maps"), _("View only maps of size small (36x36)."), Font::BIG);
	if(le.MousePressRight(buttonSelectMedium)) Dialog::Message(_("Medium Maps"), _("View only maps of size medium (72x72)."), Font::BIG);
	if(le.MousePressRight(buttonSelectLarge)) Dialog::Message(_("Large Maps"), _("View only maps of size large (108x108)."), Font::BIG);
	if(le.MousePressRight(buttonSelectXLarge)) Dialog::Message(_("Extra Large Maps"), _("View only maps of size extra large (144x144)."), Font::BIG);
	if(le.MousePressRight(buttonSelectAll)) Dialog::Message(_("All Maps"), _("View all maps, regardless of size."), Font::BIG);
	if(le.MousePressRight(countPlayers)) Dialog::Message(_("Players Icon"), _("Indicates how many players total are in the EditScenario. Any positions not occupied by humans will be occupied by computer players."), Font::BIG);
	if(le.MousePressRight(sizeMaps)) Dialog::Message(_("Size Icon"), _("Indicates whether the maps is small (36x36), medium (72x72), large (108x108), or extra large (144x144)."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Name"), _("The name of the currently selected map."), Font::BIG);
	if(le.MousePressRight(victoryCond)) Dialog::Message(_("Victory Condition Icon"), _("There are 6 possiblities: FIXME."), Font::BIG);
	if(le.MousePressRight(lossCond)) Dialog::Message(_("Loss Condition Icon"), _("There are 4 possible loss conditions, as indicated by the following icons: FIXME."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Map Difficulty"), _("The map difficulty of the currently selected map.  The map difficulty is determined by the EditScenario designer. More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Description"), _("The description of the currently selected map."), Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message(_("OK"), _("Accept the choice made."), Font::BIG);

	if(redraw)
	{
	    cursor.Hide();
	    RedrawMapsFileList(rt, *curlist, top, cur, max_items);
	    buttonOk.Draw();
	    buttonPgUp.Draw();
	    buttonPgDn.Draw();
	    buttonSelectSmall.Draw();
	    buttonSelectMedium.Draw();
	    buttonSelectLarge.Draw();
	    buttonSelectXLarge.Draw();
	    buttonSelectAll.Draw();
	    split.Redraw();
	    cursor.Show();
	    display.Flip();
	    redraw = false;
	}
    }

    filename = (*cur).FileMaps();
}

void RedrawMapsFileList(const Rect & dst, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
{
    Display & display = Display::Get();
    const Sprite & panel = AGG::GetICN(ICN::REQSBKG, 0);
    display.Blit(panel, dst);

    Text text;
    u16 oy = 60;
    u8 index = 0;
    std::bitset<8> colors;

    for(MapsFileInfoList::const_iterator ii = top; ii != lists.end() && (ii - top < max_items); ++ii)
    {
	const Maps::FileInfo & info = *ii;

	colors = info.KingdomColors();
	index = 19 + colors.count();

	const Sprite & spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, dst.x + 45, dst.y + oy);

	switch(info.SizeMaps().w)
	{
            case Maps::SMALL:	index = 26; break;
            case Maps::MEDIUM:	index = 27; break;
            case Maps::LARGE:	index = 28; break;
            case Maps::XLARGE:	index = 29; break;
            default:  		index = 30; break;
        }

	const Sprite & spriteSize = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteSize, dst.x + 45 + spriteCount.w() + 2, dst.y + oy);

	text.Set(info.Name(), (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 100, dst.y + oy);

	index = 30 + info.ConditionsWins();
	const Sprite & spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, dst.x + 270, dst.y + oy);

	index = 36 + info.ConditionsLoss();
	const Sprite & spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, dst.x + 270 + spriteWins.w() + 2, dst.y + oy);

	oy += text.h() + 2;
    }

    colors = (*cur).KingdomColors();
    index = 19 + colors.count();

    const Sprite & spriteCount = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteCount, dst.x + 65, dst.y + 265);

    switch((*cur).SizeMaps().w)
    {
	case Maps::SMALL:	index = 26; break;
        case Maps::MEDIUM:	index = 27; break;
        case Maps::LARGE:	index = 28; break;
        case Maps::XLARGE:	index = 29; break;
        default:  		index = 30; break;
    }

    const Sprite & spriteSize = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteSize, dst.x + 65 + spriteCount.w() + 2, dst.y + 265);

    text.Set((*cur).Name(), Font::BIG);
    text.Blit(dst.x + 190 - text.w() / 2, dst.y + 265);

    index = 30 + (*cur).ConditionsWins();
    const Sprite & spriteWins = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteWins, dst.x + 275, dst.y + 265);

    index = 36 + (*cur).ConditionsLoss();
    const Sprite & spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteLoss, dst.x + 275 + spriteWins.w() + 2, dst.y + 265);

    text.Set(_("Maps Difficulty:"), Font::BIG);
    text.Blit(dst.x + 70, dst.y + 290);

    text.Set(Difficulty::String((*cur).Difficulty()));
    text.Blit(dst.x + 275 - text.w() / 2, dst.y + 290);

    TextBox box((*cur).Description(), Font::BIG, 290);
    box.Blit(dst.x + 45, dst.y + 320);
}

/*
// extended dialog
const Maps::FileInfo* Dialog::SelectFileInfo(const std::list<Maps::FileInfo *> & info_maps)
{
    // preload
    AGG::PreloadObject(ICN::REQSBKG);

    std::list<Maps::FileInfo *> smallmaps, mediummaps, largemaps, xlargemaps;

    const std::list<Maps::FileInfo *> & allmaps = info_maps;
    const std::list<Maps::FileInfo *> *curmaps = &allmaps;

    std::list<Maps::FileInfo *>::const_iterator it_list_head = allmaps.begin();
    std::list<Maps::FileInfo *>::const_iterator it_current = it_list_head;
    std::list<Maps::FileInfo *>::const_iterator it_end = allmaps.end();

    for(; it_current != it_end; ++it_current)
	switch((**it_current).SizeMaps().w)
	{
    	    case Maps::SMALL:	smallmaps.push_back(*it_current);break;
    	    case Maps::MEDIUM:	mediummaps.push_back(*it_current); break;
    	    case Maps::LARGE:	largemaps.push_back(*it_current); break;
    	    case Maps::XLARGE:	xlargemaps.push_back(*it_current); break;
	    default: continue;
	}
    it_current = it_list_head;

    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Background background(Rect(110, 0, 510, 480));
    background.Save();

    // image panel
    const Sprite &shadow = AGG::GetICN(ICN::REQSBKG, 1);
    display.Blit(shadow, 114, 21);
    const Sprite &panel = AGG::GetICN(ICN::REQSBKG, 0);
    display.Blit(panel, 130, 5);

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));

    const Rect countPlayers(175, 65, 17, 170);
    const Rect sizeMaps(195, 65, 17, 170);
    const Rect victoryCond(400, 65, 17, 170);
    const Rect lossCond(420, 65, 17, 170);
    
    // Splitter
    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < allmaps.size() ? allmaps.size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, ICN::REQUESTS, 1, 2);
    Button buttonPgUp(457, 60, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(457, 222, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(167, 28, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(229, 28, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(291, 28, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(353, 28, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(415, 28, ICN::REQUESTS, 17, 18);

    DrawList(it_list_head, it_current, (LISTMAXITEM > allmaps.size() ? allmaps.size() : LISTMAXITEM));
    DrawSelectInfo(**it_current);

    buttonOk.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    buttonSelectSmall.Draw();
    buttonSelectMedium.Draw();
    buttonSelectLarge.Draw();
    buttonSelectXLarge.Draw();
    buttonSelectAll.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
    size_t num_head = 0;
    size_t num_select = 0;

    // newstandard loop
    while(le.HandleEvents())
    {
	// press button
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

	le.MousePressLeft(buttonSelectSmall) ? buttonSelectSmall.PressDraw() : buttonSelectSmall.ReleaseDraw();
	le.MousePressLeft(buttonSelectMedium) ? buttonSelectMedium.PressDraw() : buttonSelectMedium.ReleaseDraw();
	le.MousePressLeft(buttonSelectLarge) ? buttonSelectLarge.PressDraw() : buttonSelectLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectXLarge) ? buttonSelectXLarge.PressDraw() : buttonSelectXLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.PressDraw() : buttonSelectAll.ReleaseDraw();

	// click small
	if((le.KeyPress(KEY_s) || le.MouseClickLeft(buttonSelectSmall)) && smallmaps.size())
	{
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click medium
	if((le.KeyPress(KEY_m) || le.MouseClickLeft(buttonSelectMedium)) && mediummaps.size())
	{
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click large
	if((le.KeyPress(KEY_l) || le.MouseClickLeft(buttonSelectLarge)) && largemaps.size())
	{
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click xlarge
	if((le.KeyPress(KEY_x) || le.MouseClickLeft(buttonSelectXLarge)) && xlargemaps.size())
	{
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click all
	if((le.KeyPress(KEY_a) || le.MouseClickLeft(buttonSelectAll)) && allmaps.size())
	{
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList) && curmaps->size())
	{
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num >= curmaps->size()) num = curmaps->size() - 1;
	    if(num_select == num) break;
	    num_select = num;
	    cursor.Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head;
	    while(num--) ++it_current;
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    DrawSelectInfo(**it_current);
	    cursor.Show();
	    display.Flip();
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp) ||
	    le.KeyPress(KEY_PAGEUP) || le.KeyPress(KEY_UP)) &&
	    curmaps->size() && num_head)
	{
	    cursor.Hide();
	    backgroundList.Restore();
	    backgroundInfo.Restore();
	    --it_list_head;
	    --num_head;
	    if(le.KeyPress(KEY_UP)) it_current = it_list_head;
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    DrawSelectInfo(**it_current);
	    split.Backward();
	    cursor.Show();
	    display.Flip();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn) ||
	    le.KeyPress(KEY_PAGEDOWN) || le.KeyPress(KEY_DOWN)) &&
	    LISTMAXITEM < curmaps->size() && num_head + LISTMAXITEM < curmaps->size())
	{
	    cursor.Hide();
	    backgroundList.Restore();
	    backgroundInfo.Restore();
	    ++it_list_head;
	    ++num_head;
	    if(le.KeyPress(KEY_DOWN)) it_current = it_list_head;
	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    DrawSelectInfo(**it_current);
	    split.Forward();
	    cursor.Show();
	    display.Flip();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM)
	{
	    s16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();

	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();

	    it_list_head = curmaps->begin();
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    split.Move(seek);
	    while(seek--){ ++it_list_head; ++num_head; }

	    DrawList(it_list_head, it_current, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    cursor.Show();
	    display.Flip();
 	}

	// right info
	if(le.MousePressRight(buttonSelectSmall)) Dialog::Message(_("Small Maps"), _("View only maps of size small (36x36)."), Font::BIG);
	if(le.MousePressRight(buttonSelectMedium)) Dialog::Message(_("Medium Maps"), _("View only maps of size medium (72x72)."), Font::BIG);
	if(le.MousePressRight(buttonSelectLarge)) Dialog::Message(_("Large Maps"), _("View only maps of size large (108x108)."), Font::BIG);
	if(le.MousePressRight(buttonSelectXLarge)) Dialog::Message(_("Extra Large Maps"), _("View only maps of size extra large (144x144)."), Font::BIG);
	if(le.MousePressRight(buttonSelectAll)) Dialog::Message(_("All Maps"), _("View all maps, regardless of size."), Font::BIG);
	if(le.MousePressRight(countPlayers)) Dialog::Message(_("Players Icon"), _("Indicates how many players total are in the EditScenario. Any positions not occupied by humans will be occupied by computer players."), Font::BIG);
	if(le.MousePressRight(sizeMaps)) Dialog::Message(_("Size Icon"), _("Indicates whether the maps is small (36x36), medium (72x72), large (108x108), or extra large (144x144)."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Name"), _("The name of the currently selected map."), Font::BIG);
	if(le.MousePressRight(victoryCond)) Dialog::Message(_("Victory Condition Icon"), _("There are 6 possiblities: FIXME."), Font::BIG);
	if(le.MousePressRight(lossCond)) Dialog::Message(_("Loss Condition Icon"), _("There are 4 possible loss conditions, as indicated by the following icons: FIXME."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Map Difficulty"), _("The map difficulty of the currently selected map.  The map difficulty is determined by the EditScenario designer. More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player."), Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message(_("Selected Description"), _("The description of the currently selected map."), Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message(_("OK"), _("Accept the choice made."), Font::BIG);

	// exit
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN)) break;
    }

    cursor.Hide();
    background.Restore();
    
    cursor.Show();

    return info_maps.size() ? *it_current : NULL;
}
*/
