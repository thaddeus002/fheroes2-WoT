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

#include <algorithm>
#include <string>
#include "dir.h"
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "difficulty.h"
#include "splitter.h"
#include "settings.h"
#include "maps.h"
#include "maps_fileinfo.h"
#include "dialog.h"
#include "string_util.h"


bool SelectFileListSimple(const std::string &, MapsFileInfoList &, std::string &, bool);
void RedrawFileListSimple(const Rect &, const std::string &, const std::string & filename, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);
void RedrawMapsFileList(const Rect &, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);
bool DialogSelectMapsFileList(MapsFileInfoList &, std::string &);
bool PrepareMapsFileInfoList(MapsFileInfoList &);

void ResizeToShortName(const std::string & str, std::string & res)
{
    res.assign(basename_internal(str.c_str()));
    size_t it = res.find('.');
    if(std::string::npos != it) res.resize(it);
}

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

    return SelectFileListSimple(_("File to Save:"), lists, file, true);
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

    return SelectFileListSimple(_("File to Load:"), lists, file, false);
}

bool SelectFileListSimple(const std::string & header, MapsFileInfoList & lists, std::string & result, bool editor)
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
    back.Save();

    const Rect & rt = back.GetRect();
    const Rect list_rt(rt.x + 40, rt.y + 55, 265, 215);
    const Rect enter_field(rt.x + 45, rt.y + 286, 260, 16);

    Button buttonCancel(rt.x + 34, rt.y + 315, ICN::REQUEST, 3, 4);
    Button buttonOk(rt.x + 244, rt.y + 315, ICN::REQUEST, 1, 2);
    Button buttonPgUp(rt.x + 327, rt.y + 55, ICN::REQUEST, 5, 6);
    Button buttonPgDn(rt.x + 327, rt.y + 257, ICN::REQUEST, 7, 8);

    const u8 max_items = 11;
    MapsFileInfoList::iterator top = lists.begin();
    MapsFileInfoList::iterator cur = lists.begin();

    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 330, rt.y + 73, 12, 180), Splitter::VERTICAL);
    split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));

    std::string filename;

    if(editor)
    {
	filename.assign("newgame");
	cur = lists.end();
    }
    else
    {
	if(lists.empty())
	    buttonOk.SetDisable(true);
	else
	    ResizeToShortName((*cur).file, filename);
    }

    RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    split.Redraw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();
    bool redraw = false;
    bool edit_mode = false;

    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) && buttonOk.isEnable() ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
        le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
        le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

        if((le.MouseClickLeft(buttonOk) && buttonOk.isEnable()) || le.KeyPress(KEY_RETURN))
        {
    	    if(editor && filename.size())
		result = Settings::Get().LocalDataPrefix() + SEPARATOR + "save" + SEPARATOR + filename + ".sav";
    	    else
    	    if(cur != lists.end())
    		result = (*cur).file;
    	    break;
    	}
    	else
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
        {
    	    result.clear();
    	    break;
	}
	else
        if(le.MouseClickLeft(enter_field) && editor)
	{
	    edit_mode = true;
	    redraw = true;
	}
	else
	if(edit_mode && le.KeyPress())
	{
	    String::AppendKey(filename, le.KeyValue(), le.KeyMod());

	    buttonOk.SetDisable(filename.empty());

	    redraw = true;
	}
	else
        if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > lists.begin()))
	{
	    top = (top - lists.begin() > max_items ? top - max_items : lists.begin());
	    split.Move(top - lists.begin());
	    redraw = true;
	}
	else
        if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + max_items < lists.end()))
	{
	    top += max_items;
	    if(top + max_items > lists.end()) top = lists.end() - max_items;
	    split.Move(top - lists.begin());
	    redraw = true;
	}
	else
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
	    edit_mode = false;
	}
	else
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
	    edit_mode = false;
	}
	else
	if((le.MouseWheelUp(list_rt) || le.MouseWheelUp(split.GetRect())) && (top > lists.begin()))
	{
	    --top;
	    split.Backward();
	    redraw = true;
	}
	else
	if((le.MouseWheelDn(list_rt) || le.MouseWheelDn(split.GetRect())) && (top < (lists.end() - max_items)))
	{
	    ++top;
	    split.Forward();
	    redraw = true;
	}
	else
	if(le.MousePressLeft(split.GetRect()) && (lists.size() > max_items))
	{
	    s16 seek = (le.GetMouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();
	    top = lists.begin() + seek;
	    split.Move(seek);
	    redraw = true;
 	}
	else
	if(le.KeyPress(KEY_DELETE) && cur != lists.end())
	{
	    std::string msg(_("Are you sure you want to delete file:"));
	    msg.append("\n \n");
	    msg.append(basename_internal((*cur).file.c_str()));
	    if(Dialog::YES == Dialog::Message(_("Warning!"), msg, Font::BIG, Dialog::YES | Dialog::NO))
	    {
		unlink((*cur).file.c_str());
		lists.erase(cur);
		if(lists.empty() || filename.empty()) buttonOk.SetDisable(true);
		top = lists.begin();
		cur = top;
		split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));
		split.Move(top - lists.begin());
		redraw = true;
	    }
	}

	if(le.MouseClickLeft(list_rt) && lists.size())
	{
	    MapsFileInfoList::iterator cur2 = top + static_cast<size_t>((le.GetMouseReleaseLeft().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur2 > (lists.end() - 1)) cur2 = lists.end() - 1;
	    // double click
	    if(cur2 == cur)
	    {
    		result = (*cur).file;
    		break;
	    }
	    else
		cur = cur2;
	    redraw = true;
	    edit_mode = false;
	}

	if(redraw)
	{
	    cursor.Hide();

	    if(edit_mode && editor)
	    {
		RedrawFileListSimple(rt, header, filename + "_", lists, top, cur, max_items);
	    }
	    else
	    if(cur != lists.end())
	    {
	    	ResizeToShortName((*cur).file, filename);
		RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);
	    }
	    else
		RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);

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

    cursor.Hide();
    back.Restore();

    return result.size();
}

void RedrawFileListSimple(const Rect & dst, const std::string & header, const std::string & filename, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
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

	const std::string name(basename_internal(info.file.c_str()));

	std::strftime(short_date, 14, "%b %d, %H:%M", std::localtime(&info.localtime));

	text.Set(basename_internal(info.file.c_str()), (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 45, dst.y + oy);

	text.Set(short_date, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 305 - text.w(), dst.y + oy);

	oy += text.h() + 2;
    }

    if(filename.size())
    {
	text.Set(filename, Font::BIG);
	text.Blit(dst.x + 175 - text.w() / 2, dst.y + 289);
    }
}

bool Dialog::SelectMapsFile(std::string & filename)
{
    MapsFileInfoList lists;

    return PrepareMapsFileInfoList(lists) && DialogSelectMapsFileList(lists, filename);
}

bool PrepareMapsFileInfoList(MapsFileInfoList & lists)
{
    Settings & conf = Settings::Get();
    Dir dir;

    ListMapsDirectory::const_iterator it1 = conf.GetListMapsDirectory().begin();
    ListMapsDirectory::const_iterator it2 = conf.GetListMapsDirectory().end();

    for(; it1 != it2; ++it1)
    {
	dir.Read(*it1, ".mp2", false);
	// loyality version
	if(conf.Modes(Settings::PRICELOYALTY)) dir.Read(*it1, ".mx2", false);
    }

    if(dir.empty())
    {
        Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
        return false;
    }

    lists.resize(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;

    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii)
    if(lists[ii].ReadBIN(*itd))
    {
        if(conf.PreferablyCountPlayers() > lists[ii].AllowColorsCount()) --ii;
    }
    else --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);

    std::sort(lists.begin(), lists.end(), Maps::FileInfo::PredicateForSorting);

    return true;
}

bool DialogSelectMapsFileList(MapsFileInfoList & lists, std::string & filename)
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
	switch((*cur).size_w)
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
    back.Save();

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

    LocalEvent & le = LocalEvent::Get();
    bool redraw = false;
    bool res = false;

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

        if((le.MouseClickLeft(buttonOk) && buttonOk.isEnable()) || le.KeyPress(KEY_RETURN)){ res = true; break; }
	else
        if(le.KeyPress(KEY_ESCAPE)){ res = false; break; }
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
	    MapsFileInfoList::iterator cur2 = top + static_cast<size_t>((le.GetMouseReleaseLeft().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur2 > (curlist->end() - 1)) cur2 = curlist->end() - 1;
	    // double click
	    if(cur == cur2)
	    {
		res = true;
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

    filename = (*cur).file;

    cursor.Hide();
    back.Restore();
    
    return res;
}

void RedrawMapsFileList(const Rect & dst, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
{
    Display & display = Display::Get();
    const Sprite & panel = AGG::GetICN(ICN::REQSBKG, 0);
    display.Blit(panel, dst);

    Text text;
    u16 oy = 60;
    u8 index = 0;

    for(MapsFileInfoList::const_iterator ii = top; ii != lists.end() && (ii - top < max_items); ++ii)
    {
	const Maps::FileInfo & info = *ii;

	index = 19 + info.KingdomColorsCount();

	const Sprite & spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, dst.x + 45, dst.y + oy);

	switch(info.size_w)
	{
            case Maps::SMALL:	index = 26; break;
            case Maps::MEDIUM:	index = 27; break;
            case Maps::LARGE:	index = 28; break;
            case Maps::XLARGE:	index = 29; break;
            default:  		index = 30; break;
        }

	const Sprite & spriteSize = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteSize, dst.x + 45 + spriteCount.w() + 2, dst.y + oy);

	text.Set(info.name, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 100, dst.y + oy);

	index = 30 + info.conditions_wins;
	const Sprite & spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, dst.x + 270, dst.y + oy);

	index = 36 + info.conditions_loss;
	const Sprite & spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, dst.x + 270 + spriteWins.w() + 2, dst.y + oy);

	oy += text.h() + 2;
    }

    index = 19 + (*cur).KingdomColorsCount();

    const Sprite & spriteCount = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteCount, dst.x + 65, dst.y + 265);

    switch((*cur).size_w)
    {
	case Maps::SMALL:	index = 26; break;
        case Maps::MEDIUM:	index = 27; break;
        case Maps::LARGE:	index = 28; break;
        case Maps::XLARGE:	index = 29; break;
        default:  		index = 30; break;
    }

    const Sprite & spriteSize = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteSize, dst.x + 65 + spriteCount.w() + 2, dst.y + 265);

    text.Set((*cur).name, Font::BIG);
    text.Blit(dst.x + 190 - text.w() / 2, dst.y + 265);

    index = 30 + (*cur).conditions_wins;
    const Sprite & spriteWins = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteWins, dst.x + 275, dst.y + 265);

    index = 36 + (*cur).conditions_loss;
    const Sprite & spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteLoss, dst.x + 275 + spriteWins.w() + 2, dst.y + 265);

    text.Set(_("Maps Difficulty:"), Font::BIG);
    text.Blit(dst.x + 70, dst.y + 290);

    text.Set(Difficulty::String((*cur).difficulty));
    text.Blit(dst.x + 275 - text.w() / 2, dst.y + 290);

    TextBox box((*cur).description, Font::BIG, 290);
    box.Blit(dst.x + 45, dst.y + 320);
}
