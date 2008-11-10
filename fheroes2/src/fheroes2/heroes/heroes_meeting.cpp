/****************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov <afletdinov@mail.dc.baikal.ru> *
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>            *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/

#include <string>
#include <algorithm>
#include "agg.h"
#include "engine.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "tools.h"
#include "text.h"
#include "army.h"
#include "heroes.h"
#include "portrait.h"
#include "game_interface.h"

namespace
{
    /** Encapsulate an object (artifact identifier or army member).
     *  Collects the selection bounding box, display bounding box,
     *  selection state, a reference to an object, and a reference
     *  to the parent list that the object will be returned to.
     *  All object swapping happens visually by manipulating pointers
     *  to the original objects inside these containers, and all objects
     *  are collected and put in their proper lists once the user exits
     *  the trading screen.
     */
    template<class T>
    struct SelectableRect
    {
        SelectableRect(const Rect &r, const Rect &r2, T *obj, std::vector<T> &p)
        : selectable(r), displayable(r2), selected(false), object(obj), parentList(&p) {}
        
        void toggle() { selected = !selected; }
        
        Rect selectable;  /**< Selection area for user interaction, and drawing the selection status */
        Rect displayable; /**< Display area for centering an object's #Sprite in the selection area */
        bool selected;    /**< Selection status */
        T *object;        /**< Reference to an object that needs displaying */
        std::vector<T> *parentList; /**< The list that the object will be returned to on cleanup */
    };

    template<class T>
    struct SelectableRectList
    {
      typedef std::vector<SelectableRect<T> > Type;
    };
    
    typedef SelectableRectList<Army::Troops>::Type ArmyList;
    typedef SelectableRectList<Artifact::artifact_t>::Type ArtifactList;
}

static void PrepareArmy(std::vector<Army::Troops> &army, const Point &p, ArmyList &coords);
static void PrepareArtifacts(std::vector<Artifact::artifact_t> &artifacts, const Point &p, ArtifactList &coords);
static void RedrawArmy(const ArmyList &coords);
static void RedrawArtifacts(const ArtifactList &coords);
static void RedrawItem(const Sprite *sprite, const Rect &display, const Rect &select, bool selected);
static void RedrawSecondarySkill(const Point & pt, const std::vector<Skill::Secondary> & skills);
template<class T> static void DeselectList(std::vector<SelectableRect<T> > *list);
template<class T> static SelectableRect<T> *GetSelectedListElement(std::vector<SelectableRect<T> > &list);
template<class T> static void PerformAction(SelectableRect<T> &picked, std::vector<SelectableRect<T> > *list);
template<class T> static std::vector<T> RecreateListsFromSelectable(const std::vector<SelectableRect<T> > &list, std::vector<T> &out);
template<class T> static bool IsActionValid(SelectableRect<T> &picked);
template<class T> static void RecalcDisplayable(SelectableRect<T> &item);

void Heroes::MeetingDialog(Heroes & heroes2)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);
    std::string message;

    display.Blit(AGG::GetICN(ICN::STONEBAK, 0), dst_pt);

    u8 w = 22;

    Rect src_rt(w, w, 640 - 2 * w, 480 - 2 * w);

    // background
    dst_pt.x = cur_pt.x + w;
    dst_pt.y = cur_pt.y + w;
    display.Blit(AGG::GetICN(ICN::SWAPWIN, 0), src_rt, dst_pt);
    
    Game::Interface::Get().DrawBorder(false, false);

    // header
    message = GetName() + " meets " + heroes2.GetName();
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 26;
    Text(message, Font::BIG, dst_pt);

    // portrait
    dst_pt.x = cur_pt.x + 93;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(GetHeroes(), Portrait::BIG), dst_pt);

    dst_pt.x = cur_pt.x + 447;
    dst_pt.y = cur_pt.y + 72;
    display.Blit(Portrait::Hero(heroes2.GetHeroes(), Portrait::BIG), dst_pt);

    // attack skill
    message.clear();
    message += "Attack Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetAttack());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetAttack());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 64;
    Text(message, Font::SMALL, dst_pt);

    // defense skill
    message.clear();
    message += "Defense Skill";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetDefense());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetDefense());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 96;
    Text(message, Font::SMALL, dst_pt);

    // spell power
    message.clear();
    message += "Spell Power";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetPower());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetPower());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 128;
    Text(message, Font::SMALL, dst_pt);

    // knowledge
    message.clear();
    message += "Knowledge";
    dst_pt.x = cur_pt.x + 320 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, GetKnowledge());
    dst_pt.x = cur_pt.x + 260 - Text::width(message, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, heroes2.GetKnowledge());
    dst_pt.x = cur_pt.x + 380 - Text::width(message, Font::SMALL);
    dst_pt.y = cur_pt.y + 160;
    Text(message, Font::SMALL, dst_pt);

    // secondary skill
    dst_pt.x = cur_pt.x + 23;
    dst_pt.y = cur_pt.y + 200;
    RedrawSecondarySkill(dst_pt, secondary_skills);

    dst_pt.x = cur_pt.x + 354;
    dst_pt.y = cur_pt.y + 200;
    RedrawSecondarySkill(dst_pt, heroes2.secondary_skills);

    // army
    ArmyList armyCoords[2];
    
    dst_pt.x = cur_pt.x + 36;
    dst_pt.y = cur_pt.y + 267;
    PrepareArmy(army, dst_pt, armyCoords[0]);
    RedrawArmy(armyCoords[0]);

    dst_pt.x = cur_pt.x + 381;
    dst_pt.y = cur_pt.y + 267;
    PrepareArmy(heroes2.army, dst_pt, armyCoords[1]);
    RedrawArmy(armyCoords[1]);

    // artifact
    ArtifactList artifactCoords[2];
    
    dst_pt.x = cur_pt.x + 23;
    dst_pt.y = cur_pt.y + 347;
    PrepareArtifacts(artifacts, dst_pt, artifactCoords[0]);
    RedrawArtifacts(artifactCoords[0]);

    dst_pt.x = cur_pt.x + 367;
    dst_pt.y = cur_pt.y + 347;
    PrepareArtifacts(heroes2.artifacts, dst_pt, artifactCoords[1]);
    RedrawArtifacts(artifactCoords[1]);

    // button exit
    dst_pt.x = cur_pt.x + 280;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, ICN::SWAPBTN, 0, 1);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
        
        bool updateScreen = false;
        
        for(int i = 0; i < 2; i++)
            for(u16 j = 0; j < armyCoords[i].size(); j++)
                if(le.MouseClickLeft(armyCoords[i][j].selectable))
                {
                    //TODO: Splitting troops
                    DeselectList<Artifact::artifact_t>(artifactCoords);
                    PerformAction<Army::Troops>(armyCoords[i][j], armyCoords);
                    updateScreen = true;
                    break;
                }
        
        for(int i = 0; i < 2; i++)
            for(u16 j = 0; j < artifactCoords[i].size(); j++)
                if(le.MouseClickLeft(artifactCoords[i][j].selectable))
                {
                    DeselectList<Army::Troops>(armyCoords);
                    PerformAction<Artifact::artifact_t>(artifactCoords[i][j], artifactCoords);
                    updateScreen = true;
                    break;
                }
        
        if(updateScreen)
        {
            cursor.Hide();
            RedrawArmy(armyCoords[0]);
            RedrawArmy(armyCoords[1]);
            RedrawArtifacts(artifactCoords[0]);
            RedrawArtifacts(artifactCoords[1]);
            cursor.Show();
            display.Flip();
        }
    }
    
    std::vector<Army::Troops> newArmy[2];
    std::vector<Artifact::artifact_t> newArtifacts[2];
    for(int i = 0; i < 2; i++)
    {
        RecreateListsFromSelectable<Army::Troops>(armyCoords[i], newArmy[i]);
        RecreateListsFromSelectable<Artifact::artifact_t>(artifactCoords[i], newArtifacts[i]);
    }
    army.swap(newArmy[0]);
    artifacts.swap(newArtifacts[0]);
    heroes2.army.swap(newArmy[1]);
    heroes2.artifacts.swap(newArtifacts[1]);
}

static void RedrawSecondarySkill(const Point & pt, const std::vector<Skill::Secondary> & skills)
{
    Display & display = Display::Get();

    Point dst_pt(pt);

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
        const Skill::Secondary::skill_t skill = ii < skills.size() ? skills[ii].Skill() : Skill::Secondary::UNKNOWN;
        const Skill::Level::type_t level = ii < skills.size() ? skills[ii].Level() : Skill::Level::NONE;

        if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
        {
    	    const Sprite & sprite_skill = AGG::GetICN(ICN::MINISS, Skill::Secondary::GetIndexSprite2(skill));
    	    display.Blit(sprite_skill, dst_pt);

            std::string message;
            String::AddInt(message, level);
            Text(message, Font::SMALL, dst_pt.x + (sprite_skill.w() - Text::width(message, Font::SMALL)) - 3, dst_pt.y + sprite_skill.h() - 12);

            dst_pt.x += sprite_skill.w() + 1;
        }
    }
}

/** Draw a selectable item in the given rectangle
 *  \param sprite       Sprite to blit over background
 *  \param displayable  Sprite's absolute blitting rectangle
 *  \param selectable   Absolute rectangle for which to draw a selection box
 *  \param selected     Draw the selection box or not
 */
static void RedrawItem(const Sprite *sprite, const Rect &displayable, const Rect &selectable, bool selected)
{
    Display & display = Display::Get();

    const int ox = displayable.x;
    const int oy = displayable.y;
    Surface surf(selectable.w, selectable.h);
    Rect back(selectable.x, selectable.y, selectable.w, selectable.h);
    surf.Blit(AGG::GetICN(ICN::SWAPWIN, 0), back, 0, 0);
    if(sprite)
        surf.Blit(*sprite, ox - selectable.x, oy - selectable.y);
    
    if(selected)
    {
        int width = selectable.w;
        int height = selectable.h;
        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++)
            {
                if(x > 0 && x < width - 1
                && y > 0 && y < height - 1)
                    continue;
                surf.SetPixel(x, y, surf.MapRGB(255, 255, 255));
            }
    }
    
    display.Blit(surf, selectable.x, selectable.y);
}

/** Draw a list of SelectableRect items in their proper places
 *  \param army List of troops encapsulated in SelectableRect objects
 */
static void RedrawArmy(const ArmyList &army)
{
    for(u8 ii = 0; ii < army.size(); ++ii)
    {
        if(army[ii].object->isValid())
        {
            const Army::Troops &troop = *army[ii].object;
            const Sprite & sprite = AGG::GetICN(ICN::MONS32, troop.Monster());
            //Remove any remaining text underneath
            Rect adjustedSelect(army[ii].selectable);
            adjustedSelect.h += 20;
            RedrawItem(&sprite, army[ii].displayable, adjustedSelect, army[ii].selected);
            
            const Rect &select = army[ii].selectable;
            std::string message;
            String::AddInt(message, troop.Count());
            Text(message, Font::SMALL, select.x + (select.w - Text::width(message, Font::SMALL)) / 2, select.y + select.h + 3);
        }
        else
        {
            //Remove any remaining text underneath
            Rect adjustedSelect(army[ii].selectable);
            adjustedSelect.h += 20;
            RedrawItem(NULL, army[ii].displayable, adjustedSelect, army[ii].selected);
        }
    }
}

/** Draw a list of SelectableRect items in their proper places
 *  \param artifacts List of artifacts encapsulated in SelectableRect objects
 */
static void RedrawArtifacts(const ArtifactList &artifacts)
{
    for(u8 ii = 0; ii < artifacts.size(); ++ii)
    {
	if(artifacts[ii].object)
	{
            const Sprite & sprite = AGG::GetICN(ICN::ARTFX, *artifacts[ii].object);
            RedrawItem(&sprite, artifacts[ii].displayable, artifacts[ii].selectable, artifacts[ii].selected);
	}
        else RedrawItem(NULL, artifacts[ii].displayable, artifacts[ii].selectable, artifacts[ii].selected);
    }
}

/** Convert an army into a list of SelectableRect objects which encapsulate it
 *  \param[in]  army     Hero's army
 *  \param[in]  pt       Starting point at which to display the troop pictures
 *  \param[out] coords  List of SelectableRect objects encapsulating each troop object
 */
static void PrepareArmy(std::vector<Army::Troops> &army, const Point &pt, ArmyList &coords)
{
    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii)
    {
	if(army[ii].isValid())
	{
	    const Sprite & sprite = AGG::GetICN(ICN::MONS32, army[ii].Monster());
	    const u16 ox = pt.x + (44 - sprite.w()) / 2 + ii * 45;
	    const u16 oy = pt.y + 40 - sprite.h();
            Rect select(ox - (44 - sprite.w()) / 2, pt.y, 43, 43);
            Rect display(Point(ox, oy), sprite.w(), sprite.h());
            coords.push_back(SelectableRect<Army::Troops>(select, display, &army[ii], army));
	}
        else
        {
            Rect select(pt.x + ii * 45, pt.y, 43, 43);
            Rect display(select);
            coords.push_back(SelectableRect<Army::Troops>(select, display, &army[ii], army));
        }
    }
}

/** Convert a list of artifacts into a list of SelectableRect objects
 *  \param[in]  artifacts  List of artifacts
 *  \param[in]  pt         Starting point at which to display the artifact pictures
 *  \param[out] coords    List of SelectableRect objects encapsulating each artifact
 */
static void PrepareArtifacts(std::vector<Artifact::artifact_t> &artifacts, const Point &pt, ArtifactList &coords)
{
    u8 ii;
    for(ii = 0; ii < artifacts.size(); ++ii)
    {
        const Sprite & sprite = AGG::GetICN(ICN::ARTFX, artifacts[ii]);
        const u16 ox = pt.x + (34 - sprite.w()) / 2 + (ii % (HEROESMAXARTIFACT / 2)) * 36;
        const u16 oy = ii < HEROESMAXARTIFACT / 2 ? pt.y + 1 : pt.y + 37;
        Rect select(ox - (34 - sprite.w()) / 2, pt.y, 34, 34);
        Rect display(Point(ox, oy), sprite.w(), sprite.h());
        coords.push_back(SelectableRect<Artifact::artifact_t>(select, display, &artifacts[ii], artifacts));
    }
    for( ; ii < HEROESMAXARTIFACT; ii++)
    {
        const u16 ox = pt.x + 17 + (ii % (HEROESMAXARTIFACT / 2)) * 36;
        const u16 oy = ii < HEROESMAXARTIFACT / 2 ? pt.y + 1 : pt.y + 37;
        Rect select(ox - 17, oy - 1 , 34, 34);
        Rect display(select);
        coords.push_back(SelectableRect<Artifact::artifact_t>(select, display, NULL, artifacts));
    }
}

/** Deselect every item in an array of SelectableRect lists
 *  \param list Two element array of lists of SelectableRect objects
 */
template<class T>
static void DeselectList(std::vector<SelectableRect<T> > *list)
{
    for(int j = 0; j < 2; j++)
        for(u16 i = 0; i < list[j].size(); i++)
            list[j][i].selected = false;
}

/** Retrieve any existing, selected element in a list of SelectableRect objects
 *  \param list List of SelectableRect objects
 */
template<class T>
static SelectableRect<T> *GetSelectedListElement(std::vector<SelectableRect<T> > &list)
{
    for(u16 i = 0; i < list.size(); i++)
        if(list[i].selected)
            return &list[i];
    return NULL;
}

/** Check whether an action about to be performed on the given SelectableRect should continue
 *  \param picked Chosen object
 */
template <>
static bool IsActionValid<Army::Troops>(SelectableRect<Army::Troops> &picked)
{
    //TODO: Ensure there is still one stack left if troops are being transferred
    return true;
}

/** Check whether an action about to be performed on the given SelectableRect should continue
 *  \param picked Chosen object
 */
template <>
static bool IsActionValid<Artifact::artifact_t>(SelectableRect<Artifact::artifact_t> &picked)
{
    if(picked.object)
    {
        switch(*picked.object)
        {
            case Artifact::FIZBIN_MISFORTUNE:
            case Artifact::MAGIC_BOOK:
              Dialog::Message("", "This item cannot be traded.", Font::BIG, Dialog::OK);
              return false;
            
            default:
              return true;
        }
    }
    return true;
}

/** Act on the chosen SelectableRect, based on any previous selection
 *  \param picked User-picked item
 *  \param list   Two-element array of lists of SelectableRect objects, which are all the same type
 */
template<class T>
static void PerformAction(SelectableRect<T> &picked, std::vector<SelectableRect<T> > *list)
{
    if(!IsActionValid<T>(picked))
        return;

    for(int j = 0; j < 2; j++)
    {
        SelectableRect<T> *other = GetSelectedListElement(list[j]);
        if(other && other != &picked) //Perform a swap
        {
            std::swap(picked.object, other->object);
            RecalcDisplayable(picked);
            RecalcDisplayable(*other);
            other->toggle();
            return;
        }
    }
    if(picked.object) //Can't toggle empty spaces
        picked.toggle();
}

/** Perform any needed display calculations after swapping elements
 *  \param item Item containing newly-swapped object
 */
template<>
static void RecalcDisplayable<Artifact::artifact_t>(SelectableRect<Artifact::artifact_t> &item)
{
    if(item.object)
    {
        item.displayable.x = item.selectable.x + 1;
        item.displayable.y = item.selectable.y + 1;
    }
}

/** Perform any needed display calculations after swapping elements
 *  \param item Item containing newly-swapped object
 */
template<>
static void RecalcDisplayable<Army::Troops>(SelectableRect<Army::Troops> &item)
{
    if(item.object)
    {
        Army::Troops &troop = *item.object;
        const Sprite & sprite = AGG::GetICN(ICN::MONS32, troop.Monster());
        item.displayable = Rect(item.selectable.x + (44 - sprite.w()) / 2,
                                item.selectable.y + 40 - sprite.h(),
                                sprite.w(), sprite.h());
    }
}

/** Convert a list of SelectableRect objects back into the original item list, retaining modifications
 *  \param[in]  list List of SelectableRect objects
 *  \param[out] out  List to be filled up with the contents of \i list
 */
template<class T>
static std::vector<T> RecreateListsFromSelectable(const std::vector<SelectableRect<T> > &list, std::vector<T> &out)
{
    for(u16 j = 0; j < list.size(); j++)
        if(list[j].object)
            out.push_back(*list[j].object);
    return out;
}
