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
#include "mp2.h"
#include "world.h"
#include "settings.h"
#include "castle.h"
#include "monster.h"
#include "heroes.h"
#include "battle.h"
#include "gamearea.h"
#include "game_focus.h"
#include "game_selectobjbar.h"
#include "kingdom.h"
#include "cursor.h"
#include "algorithm.h"
#include "gameevent.h"

#define PlaySoundWarning	AGG::PlaySound(M82::EXPERNCE)
#define PlaySoundSuccess	AGG::PlaySound(M82::TREASURE)
#define PlaySoundFailure	AGG::PlaySound(M82::H2MINE)
#define PlaySoundVisited	AGG::PlaySound(M82::RSBRYFZL)

void ActionToCastle(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToHeroes(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToMonster(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToBoat(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToCoast(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToWagon(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToSkeleton(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToResource(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToPickupResource(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToFlotSam(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToArtifact(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToShrine(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToWitchsHut(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToGoodLuckObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToPoorLuckObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToSign(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToMagicWell(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToTradingPost(Heroes &hero);
void ActionToPrimarySkillObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToPoorMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToGoodMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToExperienceObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToTreasureChest(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToAncientLamp(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToTeleports(Heroes &hero, const u16 dst_index);
void ActionToWhirlpools(Heroes &hero, const u16 dst_index);
void ActionToObservationTower(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToCaptureObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToDwellingJoinMonster(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToDwellingRecruitMonster(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToDwellingBattleMonster(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToArtesianSpring(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToAbandoneMine(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToXanadu(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToUpgradeArmyObject(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToMagellanMaps(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToEvent(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToObelisk(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToTreeKnowledge(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToOracle(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToDaemonCave(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToAlchemistsTower(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToStables(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToArena(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToSirens(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToJail(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToHutMagi(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToEyeMagi(Heroes &hero, const u8 obj, const u16 dst_index);
void ActionToShinx(Heroes &hero, const u8 obj, const u16 dst_index);

u16 DialogGoldWithExp(const std::string & hdr, const std::string & msg, const u16 count, const u16 exp, const u16 buttons = Dialog::OK)
{
    const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
    Surface image(sprite.w() + gold.w() + 50, sprite.h() + 12);
    image.SetColorKey();
    image.Blit(gold, 0, image.h() - gold.h() - 12);
    image.Blit(sprite, gold.w() + 50, 0);
    std::string str;
    String::AddInt(str, count);
    Text text(str, Font::SMALL);
    text.Blit((gold.w() - text.w()) / 2, image.h() - 12, image);
    str.clear();
    String::AddInt(str, exp);
    text.Set(str);
    text.Blit(gold.w() + 50 + (sprite.w() - text.w()) / 2, image.h() - 12, image);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogArtifactWithExp(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 exp, const u16 buttons = Dialog::OK)
{
    std::string str;
    String::AddInt(str, exp);
    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
    const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
    const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, Artifact::IndexSprite64(art));
    Surface image(sprite.w() + border.w() + 50, border.h());
    image.SetColorKey();
    image.Blit(border);
    image.Blit(artifact, 5, 5);
    image.Blit(sprite, border.w() + 50, (border.h() - sprite.h()) / 2);
    Text text(str, Font::SMALL);
    text.Blit(border.w() + 50 + (sprite.w() - text.w()) / 2, image.h() - 12, image);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithExp(const std::string & hdr, const std::string & msg, const u16 exp, const u16 buttons = Dialog::OK)
{
    std::string str;
    String::AddInt(str, exp);
    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
    Surface image(sprite.w(), sprite.h() + 12);
    image.SetColorKey();
    image.Blit(sprite);
    Text text(str, Font::SMALL);
    text.Blit((sprite.w() - text.w()) / 2, sprite.h(), image);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithArtifactAndGold(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 count, const u16 buttons = Dialog::OK)
{
    std::string str;
    String::AddInt(str, count);
    const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
    const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
    const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, Artifact::IndexSprite64(art));
    Surface image(gold.w() + border.w() + 50, border.h());
    image.SetColorKey();
    image.Blit(border);
    image.Blit(artifact, 5, 5);
    image.Blit(gold, border.w() + 50, (border.h() - gold.h()) / 2);
    Text text(str, Font::SMALL);
    text.Blit(border.w() + 50 + (gold.w() - text.w()) / 2, border.h() - 25, image);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithGold(const std::string & hdr, const std::string & msg, const u16 count, const u16 buttons = Dialog::OK)
{
    std::string str;
    String::AddInt(str, count);
    const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
    Surface image(gold.w(), gold.h() + 12);
    image.SetColorKey();
    image.Blit(gold);
    Text text(str, Font::SMALL);
    text.Blit((gold.w() - text.w()) / 2, gold.h(), image);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithArtifact(const std::string & hdr, const std::string & msg, const Artifact::artifact_t art, const u16 buttons = Dialog::OK)
{
    const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
    const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, Artifact::IndexSprite64(art));
    Surface image(border.w(), border.h());
    image.Blit(border);
    image.Blit(artifact, 5, 5);
    return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogMorale(const std::string & hdr, const std::string & msg, const bool good, u8 count)
{
    if(1 > count) count = 1;
    if(3 < count) count = 3;
    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, (good ? 2 : 3));
    u8 offset = sprite.w() * 4 / 3;
    Surface image(sprite.w() + offset * (count - 1), sprite.h());
    image.SetColorKey();
    for(u8 ii = 0; ii < count; ++ii) image.Blit(sprite, offset * ii, 0);
    return Dialog::SpriteInfo(hdr, msg, image);
}

u16 DialogLuck(const std::string & hdr, const std::string & msg, const bool good, u8 count)
{
    if(1 > count) count = 1;
    if(3 < count) count = 3;
    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, (good ? 0 : 1));
    u8 offset = sprite.w() * 4 / 3;
    Surface image(sprite.w() + offset * (count - 1), sprite.h());
    image.SetColorKey();
    for(u8 ii = 0; ii < count; ++ii) image.Blit(sprite, offset * ii, 0);
    return Dialog::SpriteInfo(hdr, msg, image);
}

void BattleLose(Heroes &hero, const u8 reason)
{
    AGG::PlaySound(M82::KILLFADE);
    hero.FadeOut();
    world.GetKingdom(hero.GetColor()).RemoveHeroes(&hero);
    hero.SetFreeman(reason);
    Game::Focus::Get().Reset(Game::Focus::HEROES);
    Game::Focus::Get().Redraw();
}

void PlayPickupSound(void)
{
    M82::m82_t wav = M82::UNKNOWN;

    switch(Rand::Get(1, 7))
    {
        case 1: wav = M82::PICKUP01; break;
        case 2: wav = M82::PICKUP02; break;
        case 3: wav = M82::PICKUP03; break;
        case 4: wav = M82::PICKUP04; break;
        case 5: wav = M82::PICKUP05; break;
        case 6: wav = M82::PICKUP06; break;
        case 7: wav = M82::PICKUP07; break;

        default: return;
    }

    AGG::PlaySound(wav);
}

void AnimationRemoveObject(const Maps::Tiles & tile)
{
    Maps::TilesAddon *addon = NULL;

    switch(tile.GetObject())
    {
	case MP2::OBJ_FLOTSAM:
	case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_BOTTLE:	addon = const_cast<Maps::Tiles &>(tile).FindWaterResource(); break;

        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_ANCIENTLAMP:
	case MP2::OBJ_RESOURCE:	addon = const_cast<Maps::Tiles &>(tile).FindResource(); break;

	case MP2::OBJ_ARTIFACT:	addon = const_cast<Maps::Tiles &>(tile).FindArtifact(); break;
	case MP2::OBJ_CAMPFIRE:	addon = const_cast<Maps::Tiles &>(tile).FindCampFire(); break;
	case MP2::OBJ_MONSTER:  addon = const_cast<Maps::Tiles &>(tile).FindMonster(); break;

	default: break;
    }

    if(NULL == addon) return;

    const Rect & area = GameArea::Get().GetRect();
    const Point pos(tile.GetIndex() % world.w() - area.x, tile.GetIndex() / world.w() - area.y);

    const s16 dstx = BORDERWIDTH + TILEWIDTH * pos.x;
    const s16 dsty = BORDERWIDTH + TILEWIDTH * pos.y;

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    u16 index = MAXU16;
    const Heroes *hero = NULL;
    if(Maps::ScanDistanceObject(tile.GetIndex(), MP2::OBJ_HEROES, 1, &index)) hero = world.GetHeroes(index);

    Surface sf(tile.GetSurface());

    const Sprite & sprite = AGG::GetICN(MP2::GetICNObject(addon->object), addon->index);
    sf.Blit(sprite, sprite.x(), sprite.y());

    // if animation sprite
    if(0 != (index = ICN::AnimationFrame(MP2::GetICNObject(addon->object), addon->index)))
    {
	const Sprite & sprite = AGG::GetICN(MP2::GetICNObject(addon->object), index);
	sf.Blit(sprite, sprite.x(), sprite.y());
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();
    u32 ticket = 0;
    u8 alpha = 250;

    while(le.HandleEvents() && alpha > 10)
    {
	if(Game::ShouldAnimateInfrequent(ticket, 1))
        {
	    cursor.Hide();
	    tile.RedrawTile();
	    tile.RedrawBottom(addon);
            sf.SetAlpha(alpha);
	    display.Blit(sf, dstx, dsty);
	    tile.RedrawTop();
	    if(hero) hero->Redraw(false);
            cursor.Show();
            display.Flip();
            alpha -= 20;
        }

        ++ticket;
    }
}

// action to next cell
void Heroes::Action(const u16 dst_index)
{
    if(Game::AI == world.GetKingdom(GetColor()).Control()) return AIAction(dst_index);

    const MP2::object_t object = (dst_index == GetIndex() ? GetUnderObject() : world.GetTiles(dst_index).GetObject());

    switch(object)
    {
	case MP2::OBJ_MONSTER:	ActionToMonster(*this, object, dst_index); break;

        case MP2::OBJ_CASTLE:	ActionToCastle(*this, object, dst_index); break;
        case MP2::OBJ_HEROES:	ActionToHeroes(*this, object, dst_index); break;

        case MP2::OBJ_BOAT:	ActionToBoat(*this, object, dst_index); break;
	case MP2::OBJ_COAST:	ActionToCoast(*this, object, dst_index); break;

        // resource object
        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATERWHEEL:	ActionToResource(*this, object, dst_index); break;

        case MP2::OBJ_WAGON:		ActionToWagon(*this, object, dst_index); break;
        case MP2::OBJ_SKELETON:		ActionToSkeleton(*this, object, dst_index); break;

        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:		ActionToPickupResource(*this, object, dst_index); break;

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:	ActionToTreasureChest(*this, object, dst_index); break;
        case MP2::OBJ_ANCIENTLAMP:	ActionToAncientLamp(*this, object, dst_index); break;
        case MP2::OBJ_FLOTSAM:		ActionToFlotSam(*this, object, dst_index); break;

        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_ARTIFACT: 	ActionToArtifact(*this, object, dst_index); break;

        // shrine circle
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:		ActionToShrine(*this, object, dst_index); break;

        // witchs hut
        case MP2::OBJ_WITCHSHUT: 	ActionToWitchsHut(*this, object, dst_index); break;

        // info message
        case MP2::OBJ_SIGN:		ActionToSign(*this, object, dst_index); break;

        // luck modification
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_IDOL:		ActionToGoodLuckObject(*this, object, dst_index); break;

	case MP2::OBJ_PYRAMID:		ActionToPoorLuckObject(*this, object, dst_index); break;

        case MP2::OBJ_MAGICWELL: 	ActionToMagicWell(*this, object, dst_index); break;
        case MP2::OBJ_TRADINGPOST:	ActionToTradingPost(*this); break;

        // primary skill modification
        case MP2::OBJ_FORT:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_DOCTORHUT:
        case MP2::OBJ_STANDINGSTONES:	ActionToPrimarySkillObject(*this, object, dst_index); break;

        // morale modification
        case MP2::OBJ_OASIS:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_WATERINGHOLE:
        case MP2::OBJ_BUOY:		ActionToGoodMoraleObject(*this, object, dst_index); break;

	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:	ActionToPoorMoraleObject(*this, object, dst_index); break;

        // experience modification
        case MP2::OBJ_GAZEBO:		ActionToExperienceObject(*this, object, dst_index); break;
        case MP2::OBJ_DAEMONCAVE:	ActionToDaemonCave(*this, object, dst_index); break;

        // teleports
	case MP2::OBJ_STONELIGHTS:	ActionToTeleports(*this, dst_index); break;
	case MP2::OBJ_WHIRLPOOL:	ActionToWhirlpools(*this, dst_index); break;

	// obsv tower
	case MP2::OBJ_OBSERVATIONTOWER:	ActionToObservationTower(*this, object, dst_index); break;
        case MP2::OBJ_MAGELLANMAPS:	ActionToMagellanMaps(*this, object, dst_index); break;

	// capture color object
	case MP2::OBJ_ALCHEMYLAB:
        case MP2::OBJ_MINES:
	case MP2::OBJ_SAWMILL:
        case MP2::OBJ_LIGHTHOUSE:	ActionToCaptureObject(*this, object, dst_index); break;

        case MP2::OBJ_ABANDONEDMINE:	ActionToAbandoneMine(*this, object, dst_index); break;

	// accept army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
	case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:	ActionToDwellingJoinMonster(*this, object, dst_index); break;

	// recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_DESERTTENT:	ActionToDwellingRecruitMonster(*this, object, dst_index); break;

        // battle and recruit army
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:	ActionToDwellingBattleMonster(*this, object, dst_index); break;

        case MP2::OBJ_ARTESIANSPRING:	ActionToArtesianSpring(*this, object, dst_index); break;

        case MP2::OBJ_XANADU:		ActionToXanadu(*this, object, dst_index); break;

        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_FREEMANFOUNDRY:	ActionToUpgradeArmyObject(*this, object, dst_index); break;

        case MP2::OBJ_EVENT:		ActionToEvent(*this, object, dst_index); break;
        
        case MP2::OBJ_OBELISK:          ActionToObelisk(*this, object, dst_index); break;

        case MP2::OBJ_TREEKNOWLEDGE:	ActionToTreeKnowledge(*this, object, dst_index); break;

	case MP2::OBJ_ORACLE:		ActionToOracle(*this, object, dst_index); break;
        case MP2::OBJ_SPHINX:		ActionToShinx(*this, object, dst_index); break;

	// loyalty version
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:	ActionToDwellingRecruitMonster(*this, object, dst_index); break;
        case MP2::OBJ_ALCHEMYTOWER:	ActionToAlchemistsTower(*this, object, dst_index); break;
        case MP2::OBJ_STABLES:		ActionToStables(*this, object, dst_index); break;
        case MP2::OBJ_ARENA:		ActionToArena(*this, object, dst_index); break;
        case MP2::OBJ_MERMAID:		ActionToGoodLuckObject(*this, object, dst_index); break;
        case MP2::OBJ_SIRENS:		ActionToSirens(*this, object, dst_index); break;
        case MP2::OBJ_JAIL:		ActionToJail(*this, object, dst_index); break;
        case MP2::OBJ_HUTMAGI:		ActionToHutMagi(*this, object, dst_index); break;
        case MP2::OBJ_EYEMAGI:		ActionToEyeMagi(*this, object, dst_index); break;

        // object
	default:
	    if(Settings::Get().Debug()) Error::Verbose("Heroes::Action: FIXME: " + std::string(MP2::StringObject(object)));
	    break;
    }
}

void ActionToMonster(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Monster monster(tile);
    Army::army_t army;
    army.JoinTroop(monster, tile.GetCountMonster());
    army.ArrangeForBattle();

    if(Settings::Get().Debug()) Error::Verbose("ActionToMonster: " + hero.GetName() + " attack monster " + monster.GetName());

    const u32 exp = army.CalculateExperience();
    const Army::battle_t b = Army::Battle(hero, army, tile);

    switch(b)
    {
	case Army::WIN:
	{
	    hero.IncreaseExperience(exp);
	    Maps::TilesAddon *addon = tile.FindMonster();
	    if(addon)
	    {
		AGG::PlaySound(M82::KILLFADE);
		const u32 uniq = addon->uniq;
		AnimationRemoveObject(tile);
		tile.Remove(uniq);
		tile.SetObject(MP2::OBJ_ZERO);

		// remove shadow from left cell
		if(Maps::isValidDirection(dst_index, Direction::LEFT))
		    world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT)).Remove(uniq);
	    }
	    hero.ActionAfterBattle();
	    break;
	}

	case Army::RETREAT:
	case Army::SURRENDER:
	case Army::LOSE:
	    BattleLose(hero, b);
	    if(!Settings::Get().Original())
	    {
	        tile.SetCountMonster(army.GetCountMonsters(monster));
	    }
	    break;
        
        default: break;
    }
}

void ActionToHeroes(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Heroes *other_hero = world.GetHeroes(dst_index);

    if(! other_hero) return;

    if(hero.GetColor() == other_hero->GetColor())
    {
        if(Settings::Get().Debug()) Error::Verbose("ActionToHeroes: " + hero.GetName() + " meeting " + other_hero->GetName());

        hero.MeetingDialog(const_cast<Heroes &>(*other_hero));
    }
    else
    {
	if(Settings::Get().Debug()) Error::Verbose("ActionToHeroes: " + hero.GetName() + " attack enemy hero " + other_hero->GetName());
	const Army::battle_t b = Army::Battle(hero, const_cast<Heroes &>(*other_hero), world.GetTiles(dst_index));
	const u32 exp = other_hero->GetArmy().CalculateExperience();

	switch(b)
	{
	    case Army::WIN:
		hero.TakeArtifacts(const_cast<Heroes &>(*other_hero));
		hero.IncreaseExperience(exp);
		AGG::PlaySound(M82::KILLFADE);
		(*other_hero).FadeOut();
    		world.GetKingdom((*other_hero).GetColor()).RemoveHeroes(other_hero);
    		const_cast<Heroes &>(*other_hero).SetFreeman(b);
	        // set default army
		const_cast<Heroes &>(*other_hero).GetArmy().Reset(true);
		hero.ActionAfterBattle();
		break;

	    case Army::LOSE:
	    case Army::RETREAT:
	    case Army::SURRENDER:
		BattleLose(hero, b);
		break;

            default: break;
	}
    }
}

void ActionToCastle(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Castle *castle = world.GetCastle(dst_index);

    if(! castle) return;

    if(hero.GetColor() == castle->GetColor())
    {
	if(Settings::Get().Debug()) Error::Verbose("ActionToCastle: " + hero.GetName() + " goto castle " + castle->GetName());

	Mixer::Reduce();

	const_cast<Castle *>(castle)->OpenDialog();

	Mixer::Enhance();
    }
    else
    {
	if(Settings::Get().Debug()) Error::Verbose("ActionToCastle: " + hero.GetName() + " attack enemy castle " + castle->GetName());

	const u32 exp = castle->GetArmy().isValid() ? castle->GetArmy().CalculateExperience() : 0;
    Army::battle_t b;
    if(castle->GetArmy().isValid())
    {
        if(castle->isCastle())
            b = Army::Battle(hero, const_cast<Castle &>(*castle), world.GetTiles(dst_index));
        else b = Army::Battle(hero, const_cast<Army::army_t &>(castle->GetArmy()), world.GetTiles(dst_index));
    }
    else b = Army::WIN;

	switch(b)
	{
	    case Army::WIN:
		if(exp) hero.IncreaseExperience(exp);
		world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
		world.GetKingdom(hero.GetColor()).AddCastle(castle);
		world.CaptureObject(dst_index, hero.GetColor());
		Game::SelectBarCastle::Get().Redraw();
		if(exp) hero.ActionAfterBattle();
		break;

	    case Army::LOSE:
	    case Army::RETREAT:
	    case Army::SURRENDER:
		BattleLose(hero, b);
		break;

            default: break;
	}
    }
}

void ActionToBoat(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(hero.isShipMaster()) return;

    const u16 from_index = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    Maps::Tiles & tiles_from = world.GetTiles(from_index);
    Maps::Tiles & tiles_to = world.GetTiles(dst_index);

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();
    hero.ResetMovePoints();
    tiles_from.SetObject(MP2::OBJ_COAST);
    hero.SetCenter(dst_index);
    hero.SetShipMaster(true);
    tiles_to.SetObject(MP2::OBJ_HEROES);
    hero.SaveUnderObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("ActionToBoat: " + hero.GetName() + " to boat");
}

void ActionToCoast(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(! hero.isShipMaster()) return;

    const u16 from_index = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    Maps::Tiles & tiles_from = world.GetTiles(from_index);
    Maps::Tiles & tiles_to = world.GetTiles(dst_index);

    hero.ResetMovePoints();
    tiles_from.SetObject(MP2::OBJ_BOAT);
    hero.SetCenter(dst_index);
    hero.SetShipMaster(false);
    tiles_to.SetObject(MP2::OBJ_HEROES);
    hero.SaveUnderObject(MP2::OBJ_ZERO);
    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    if(Settings::Get().Debug()) Error::Verbose("ActionToCoast: " + hero.GetName() + " to coast");
}

void ActionToPickupResource(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    const u8 count = tile.GetQuantity2();

    switch(tile.GetQuantity1())
    {
	case Resource::WOOD: resource.wood += count; break;
    	case Resource::MERCURY: resource.mercury += count; break;
    	case Resource::ORE: resource.ore += count; break;
    	case Resource::SULFUR: resource.sulfur += count; break;
    	case Resource::CRYSTAL: resource.crystal += count; break;
    	case Resource::GEMS: resource.gems += count; break;
    	case Resource::GOLD: resource.gold += 100 * count; break;

	default: break;
    }

    PlayPickupSound();
    AnimationRemoveObject(tile);
    tile.RemoveObjectSprite();

    // dialog
    switch(obj)
    {
	case MP2::OBJ_CAMPFIRE:
	    // force reset sound
	    tile.SetObject(MP2::OBJ_ZERO);
	    Game::EnvironmentSoundMixer();

	    resource.gold += 100 * count;
	    Dialog::ResourceInfo(MP2::StringObject(obj), _("Ransacking an enemy camp, you discover a hidden cache of treasures."), resource);
	break;

	case MP2::OBJ_BOTTLE:
	    Dialog::Message(MP2::StringObject(obj), world.MessageSign(dst_index), Font::BIG, Dialog::OK);
	break;

	default: break;
    }
    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("ActionToPickupResource: " + hero.GetName() + " pickup small resource");
}

void ActionToResource(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u8 count = tile.GetQuantity2();
    Resource::funds_t resource;

    switch(tile.GetQuantity1())
    {
	case Resource::WOOD: resource.wood += count; break;
    	case Resource::MERCURY: resource.mercury += count; break;
    	case Resource::ORE: resource.ore += count; break;
    	case Resource::SULFUR: resource.sulfur += count; break;
    	case Resource::CRYSTAL: resource.crystal += count; break;
    	case Resource::GEMS: resource.gems += count; break;
    	case Resource::GOLD: resource.gold += 100 * count; break;

	default: break;
    }

    if(resource.GetValidItems())
    {
	PlaySoundSuccess;
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    }
    else
	PlaySoundVisited;

    // dialog
    switch(obj)
    {
	    case MP2::OBJ_WINDMILL:
	    	if(resource.GetValidItems())
		    Dialog::ResourceInfo(_("The keeper of the mill announces:"), _("\"Milord, I have been working very hard to provide you with these resources, come back next week for more.\""), resource);
		else
		    Dialog::Message(_("The keeper of the mill announces:"), _("\"Milord, I am sorry, there are no resources currently available. Please try again next week.\""), Font::BIG, Dialog::OK);
	    break;

	    case MP2::OBJ_WATERWHEEL:
	    	if(resource.GetValidItems())
		    Dialog::ResourceInfo(_("The keeper of the mill announces:"), _("\"Milord, I have been working very hard to provide you with this gold, come back next week for more.\""), resource);
		else
		    Dialog::Message(_("The keeper of the mill announces:"), _("\"Milord, I am sorry, there is no gold currently available. Please try again next week.\""), Font::BIG, Dialog::OK);
	    break;
	    
	    case MP2::OBJ_LEANTO:
	    	if(resource.GetValidItems())
		    Dialog::ResourceInfo(_("You've found an abandoned lean-to."), _("Poking about, you discover some resources hidden nearby."), resource);
		else
		    Dialog::Message(_("The lean-to is long abandoned."), _("There is nothing of value here."), Font::BIG, Dialog::OK);
	    break;

    	    case MP2::OBJ_MAGICGARDEN:
	    	if(resource.GetValidItems())
		    Dialog::ResourceInfo(_("You catch a leprechaun foolishly sleeping amidst a cluster of magic mushrooms."), _("In exchange for his freedom, he guides you to a small pot filled with precious things."), resource);
		else
		    Dialog::Message(_("You've found a magic garden, the kind of place that leprechauns and faeries like to cavort in, but there is no one here today."), _("Perhaps you should try again next week."), Font::BIG, Dialog::OK);
    	    break;

	    default: break;
    }

    tile.SetQuantity1(0);
    tile.SetQuantity2(0);

    if(Settings::Get().Debug()) Error::Verbose("ActionToResource: " + hero.GetName() + " pickup small resource");
}

void ActionToSkeleton(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

	if(hero.PickupArtifact(art))
	{
	    PlayPickupSound();
	    DialogWithArtifact("You come upon the remains of an unfortunate adventurer.", "Searching through the tattered clothing, you find " + Artifact::String(art), art);

	    tile.SetQuantity1(0);
	    tile.SetQuantity2(0);
	}
    }
    else
    {
	PlaySoundVisited;
	Dialog::Message("You come upon the remains of an unfortunate adventurer.", "Searching through the tattered clothing, you find nothing.", Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToSkeleton: " + hero.GetName());
}

void ActionToWagon(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	if(hero.PickupArtifact(art))
	{
	    PlayPickupSound();
	    DialogWithArtifact("You come across an old wagon left by a trader who didn't quite make it to safe terrain.", "Searching inside, you find the " + Artifact::String(art), art);

	    tile.SetQuantity1(0);
	}
    }
    else
    if(tile.GetQuantity1() && tile.GetQuantity2())
    {
	const u8 count = tile.GetQuantity2();
	Resource::funds_t resource;

	switch(tile.GetQuantity1())
	{
	    case Resource::WOOD:	resource.wood += count; break;
    	    case Resource::MERCURY:	resource.mercury += count; break;
    	    case Resource::ORE:		resource.ore += count; break;
    	    case Resource::SULFUR:	resource.sulfur += count; break;
    	    case Resource::CRYSTAL:	resource.crystal += count; break;
    	    case Resource::GEMS:	resource.gems += count; break;
    	    case Resource::GOLD:	resource.gold += 100 * count; break;

	    default: break;
	}

	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

	PlayPickupSound();
	Dialog::ResourceInfo("You come across an old wagon left by a trader who didn't quite make it to safe terrain.", "Inside, you find some of the wagon's cargo still intact.", resource);

	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    {
    	PlaySoundVisited;
	Dialog::Message("You come across an old wagon left by a trader who didn't quite make it to safe terrain.", "Unfortunately, others have found it first, and the wagon is empty.", Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToWagon: " + hero.GetName());
}

void ActionToFlotSam(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    std::string body;
    Resource::funds_t resource;

    resource.gold += 100 * tile.GetQuantity1();
    resource.wood += tile.GetQuantity2();

    if(resource.gold && resource.wood)
	body = _("You search through the flotsam, and find some wood and some gold.");
    else
    if(resource.wood)
	body = _("You search through the flotsam, and find some wood.");
    else
    if(resource.gold)
        body = _("You search through the flotsam, and find some gold.");
    else
	body = _("You search through the flotsam, but find nothing.");

    PlayPickupSound();
    AnimationRemoveObject(tile);


    if(resource.GetValidItems())
    {
	Dialog::ResourceInfo(MP2::StringObject(obj), body, resource);
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    }
    else
	Dialog::Message(MP2::StringObject(obj), body, Font::BIG, Dialog::OK);

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("ActionToFlotSam: " + hero.GetName() + " pickup small resource");
}

void ActionToShrine(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Spell::spell_t spell = world.SpellFromShrine(dst_index);
    const std::string & spell_name = Spell::String(spell);
    const u8 spell_level = Spell::Level(spell);

    std::string head;
    std::string body;

    switch(spell_level)
    {
	case 1:
	    head = _("Shrine of the 1st Circle");
	    body = _("You come across a small shrine attended by a group of novice acolytes. In exchange for your protection, they agree to teach you a simple spell - '%{spell}'.");
	    break;
	case 2:
	    head = _("Shrine of the 2st Circle");
	    body = _("You come across an ornate shrine attended by a group of rotund friars. In exchange for your protection, they agree to teach you a spell - '%{spell}'.");
	    break;
	case 3:
	    head = _("Shrine of the 3st Circle");
	    body = _("You come across a lavish shrine attended by a group of high priests. In exchange for your protection, they agree to teach you a sophisticated spell - '%{spell}'.");
	    break;
	default: return;
    }
    
    String::Replace(body, "%{spell}", spell_name);

    // check spell book
    if(!hero.HasArtifact(Artifact::MAGIC_BOOK))
    {
	PlaySoundFailure;
	body += _(" Unfortunately, you have no Magic Book to record the spell with.");
	Dialog::Message(head, body, Font::BIG, Dialog::OK);
	return;
    }

    // check valid level spell and wisdom skill
    if(3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM))
    {
	PlaySoundFailure;
	body += _(" Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.");
	Dialog::Message(head, body, Font::BIG, Dialog::OK);
	return;
    }

    PlaySoundSuccess;
    hero.AppendSpellToBook(spell);
    hero.SetVisited(dst_index, Settings::Get().Original() ? Visit::LOCAL : Visit::GLOBAL); // see dialog_quickinfo
    Dialog::SpellInfo(spell_name, body, spell);

    if(Settings::Get().Debug()) Error::Verbose("ActionToShrine: " + hero.GetName());
}

void ActionToWitchsHut(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Skill::Secondary::skill_t skill = world.SkillFromWitchsHut(dst_index);
    const std::string & skill_name = Skill::Secondary::String(skill);
    const std::string head = _("Witch's Hut");

    std::string body = _("You approach the hut and observe a witch inside studying an ancient tome on %{skill}.");
    String::Replace(body, "%{skill}", skill_name);

    // check full
    if(HEROESMAXSKILL == hero.CountSecondarySkill())
    {
	PlaySoundFailure;
	Dialog::Message(body, _("As you approach, she turns and focuses her one glass eye on you. \"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\""), Font::BIG, Dialog::OK);
	return;
    }

    // check present skill
    if(hero.HasSecondarySkill(skill))
    {
	PlaySoundVisited;
	Dialog::Message(body, _("As you approach, she turns and speaks. \"You already know that which I would teach you. I can help you no further.\""), Font::BIG, Dialog::OK);
	return;
    }

    PlaySoundSuccess;
    hero.LearnBasicSkill(skill);
    hero.SetVisited(dst_index, Settings::Get().Original() ? Visit::LOCAL : Visit::GLOBAL); // see dialog_quickinfo

    body = _("An ancient and immortal witch living in a hut with bird's legs for stilts teaches you %{skill} for her own inscrutable purposes.");
    String::Replace(body, "%{skill}", skill_name);
    Dialog::SkillInfo(skill_name, body, skill, Skill::Level::BASIC);

    if(Settings::Get().Debug()) Error::Verbose("ActionToWitchsHut: " + hero.GetName());
}

void ActionToGoodLuckObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    std::string body_true;
    std::string body_false;

    switch(obj)
    {
        case MP2::OBJ_FOUNTAIN:
    	    body_false = _("You drink from the enchanted fountain, but nothing happens.");
    	    body_true = _("As you drink the sweet water, you gain luck for your next battle.");
    	    break;

        case MP2::OBJ_FAERIERING:
    	    body_false = _("You enter the faerie ring, but nothing happens.");
    	    body_true = _("Upon entering the mystical faerie ring, your army gains luck for its next battle.");
    	    break;

        case MP2::OBJ_IDOL:
	    body_false = _("You've found an ancient and weathered stone idol. It is supposed to grant luck to visitors, but since the stars are already smiling upon you, it does nothing.");
	    body_true = _("You've found an ancient and weathered stone idol. Kissing it is supposed to be lucky, so you do. The stone is very cold to the touch.");
    	    break;

        case MP2::OBJ_MERMAID:
	    body_false = _("The mermaids silently entice you to return later and be blessed again.");
	    body_true = _("The magical, soothing beauty of the Mermaids reaches you and your crew. Just for a moment, you forget your worries and bask in the beauty of the moment. The mermaids charms bless you with increased luck for your next combat.");
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	// modify luck
        hero.SetVisited(dst_index);
	AGG::PlaySound(M82::GOODLUCK);
	DialogLuck(MP2::StringObject(obj), body_true, true, 1);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToGoodLuckObject: " + hero.GetName());
}

void ActionToPoorLuckObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const bool battle = tile.GetQuantity1();
    bool complete = false;
    std::string body;

    switch(obj)
    {
        case MP2::OBJ_PYRAMID:
    	    if(battle)
    	    {
		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message(_("You come upon the pyramid of a great and ancient king."), _("You are tempted to search it for treasure, but all the old stories warn of fearful curses and undead guardians. Will you search?"), Font::BIG, Dialog::OK))
		{
		    // battle
		    Army::army_t army;
		    army.FromGuardian(tile);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    PlaySoundSuccess;
			    hero.IncreaseExperience(exp);
			    complete = true;
			    const Spell::spell_t spell(static_cast<Spell::spell_t>(tile.GetQuantity1()));
			    // check magick book
			    if(!hero.HasArtifact(Artifact::MAGIC_BOOK))
				Dialog::Message(MP2::StringObject(obj), _("Unfortunately, you have no Magic Book to record the spell with."), Font::BIG, Dialog::OK);
			    else
			    // check skill level for wisdom
			    if(Skill::Level::EXPERT > hero.GetLevelSkill(Skill::Secondary::WISDOM))
			    	Dialog::Message(MP2::StringObject(obj), _("Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it."), Font::BIG, Dialog::OK);
			    else
			    {
				Dialog::SpellInfo(Spell::String(spell), _("Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell."), spell, true);
				hero.AppendSpellToBook(spell);
			    }
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
    	    }
    	    else
    		body = _("You come upon the pyramid of a great and ancient king. Routine exploration reveals that the pyramid is completely empty.");
    	    break;

    	default: return;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify luck
        hero.SetVisited(dst_index);
	AGG::PlaySound(M82::BADLUCK);
	DialogLuck(MP2::StringObject(obj), body, false, 2);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToPoorLuckObject: " + hero.GetName());
}

void ActionToSign(Heroes &hero, const u8 obj, const u16 dst_index)
{
    PlaySoundWarning;
    Dialog::Message("Sign", world.MessageSign(dst_index), Font::BIG, Dialog::OK);
    if(Settings::Get().Debug()) Error::Verbose("ActionToSign: " + hero.GetName());
}

void ActionToMagicWell(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(hero.GetSpellPoints() == max)
    {
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A drink at the well is supposed to restore your spell points, but you are already at maximum."), Font::BIG, Dialog::OK);
    }
    else
    // check already visited
    if(hero.isVisited(MP2::OBJ_MAGICWELL))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A second drink at the well in one day will not help you."), Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max);
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A drink from the well has restored your spell points to maximum."), Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToMagicWell: " + hero.GetName());
}

void ActionToTradingPost(Heroes &hero)
{
    PlaySoundSuccess;
    Dialog::Marketplace(true);
    if(Settings::Get().Debug()) Error::Verbose("ActionToTradingPost: " + hero.GetName());
}

void ActionToPrimarySkillObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    std::string body_false;
    std::string body_true;
    Skill::Primary::skill_t skill = Skill::Primary::ATTACK;

    switch(obj)
    {
        case MP2::OBJ_FORT:
    	    skill = Skill::Primary::DEFENCE;
    	    body_false = _("\"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"");
    	    body_true = _("The soldiers living in the fort teach you a few new defensive tricks.");
    	    break;

        case MP2::OBJ_MERCENARYCAMP:
    	    skill = Skill::Primary::ATTACK;
    	    body_false = _("You've come upon a mercenary camp practicing their tactics. \"You're too advanced for us,\" the mercenary captain says. \"We can teach nothing more.\"");
    	    body_true = _("You've come upon a mercenary camp practicing their tactics. The mercenaries welcome you and your troops and invite you to train with them.");
    	    break;

        case MP2::OBJ_DOCTORHUT:
    	    skill = Skill::Primary::KNOWLEDGE;
    	    body_false = _("\"Go 'way!\", the witch doctor barks, \"you know all I know.\"");
    	    body_true = _("An Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails.");
    	    break;

        case MP2::OBJ_STANDINGSTONES:
    	    skill = Skill::Primary::POWER;
    	    body_false = _("You've found a group of Druids worshipping at one of their strange stone edifices. Silently, the Druids turn you away, indicating they have nothing new to teach you.");
    	    body_true = _("You've found a group of Druids worshipping at one of their strange stone edifices. Silently, they teach you new ways to cast spells.");
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	// increase skill
	hero.IncreasePrimarySkill(skill);
	hero.SetVisited(dst_index);
	Dialog::SkillInfo(MP2::StringObject(obj), body_true, skill);

	// fix double action tile
	if(obj == MP2::OBJ_STANDINGSTONES)
	{
	    if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
	    if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToPrimarySkillObject: " + hero.GetName());
}

void ActionToPoorMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    const bool battle = (tile.GetQuantity1() || tile.GetQuantity2());
    bool complete = false;
    std::string body;
    
    switch(obj)
    {
        case MP2::OBJ_GRAVEYARD:
    	    if(battle)
    	    {
    		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("", _("You tentatively approach the burial ground of ancient warriors. Do you want to search the graves?"), Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    army.FromGuardian(tile);
		    army.Dump();
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
			    Resource::funds_t resource;
			    resource.gold = tile.GetQuantity2() * 100;
			    PlaySoundSuccess;
			    DialogWithArtifactAndGold(MP2::StringObject(obj), _("Upon defeating the zomies you search the graves and find something!"), art, resource.gold);
			    hero.PickupArtifact(art);
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = _("Upon defeating the Zombies you spend several hours searching the graves and find nothing. Such a despicable act reduces your army's morale.");
    	    break;

        case MP2::OBJ_SHIPWRECK:
    	    if(battle)
    	    {
    		PlaySoundWarning;
    		if(Dialog::YES == Dialog::Message("", _("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the shipwreck?"), Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    army.FromGuardian(tile);
		    Resource::funds_t resource;
		    Artifact::artifact_t art = Artifact::UNKNOWN;
                    switch(tile.GetQuantity2())
                    {
                	case 10: resource.gold = 1000; break;
                	case 15: resource.gold = 2000; break;
                	case 25: resource.gold = 5000; break;
                	case 50: resource.gold = 2000; art = Artifact::Artifact(tile.GetQuantity1()); break;
                	default: Error::Warning("ActionToPoorMoraleObject: unknown variant for ShipWreck, index: ", dst_index); break;
                    }

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    PlaySoundSuccess;
			    if(art == Artifact::UNKNOWN)
				DialogWithGold(MP2::StringObject(obj), _("Upon defeating the Ghosts you sift through the debris and find something!"), resource.gold);
			    else
			    {
				DialogWithArtifactAndGold(MP2::StringObject(obj), _("Upon defeating the Ghosts you sift through the debris and find something!"), art, resource.gold);
				hero.PickupArtifact(art);
			    }
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = _("Upon defeating the Ghosts you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.");
    	    break;

        case MP2::OBJ_DERELICTSHIP:
    	    if(battle)
    	    {
    		PlaySoundWarning;
    		if(Dialog::YES == Dialog::Message("", _("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the ship?"), Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    army.FromGuardian(tile);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    Resource::funds_t resource;
			    resource.gold = tile.GetQuantity2() * 100;
			    PlaySoundSuccess;
			    DialogWithGold(MP2::StringObject(obj), _("Upon defeating the Skeletons you sift through the debris and find something!"), resource.gold);
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = _("Upon defeating the Skeletons you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.");
    	    break;

    	default: return;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify morale
	hero.SetVisited(dst_index);
	AGG::PlaySound(M82::BADMRLE);
	DialogMorale(MP2::StringObject(obj), body, false, 1);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToPoorMoraleObject: " + hero.GetName());
}

void ActionToGoodMoraleObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    std::string body_true;
    std::string body_false;
    u16 move = 0;

    switch(obj)
    {
        case MP2::OBJ_BUOY:
    	    body_false = _("Your men spot a navigational buoy, confirming that you are on course.");
    	    body_true = _("Your men spot a navigational buoy, confirming that you are on course and increasing their morale.");
    	    break;

        case MP2::OBJ_OASIS:
    	    body_false = _("The drink at the oasis is refreshing, but offers no further benefit. The oasis might help again if you fought a battle first.");
    	    body_true = _("A drink at the oasis fills your troops with strength and lifts their spirits.  You can travel a bit further today.");
    	    move = 800;	// + 8TP, from FAQ
    	    break;

        case MP2::OBJ_WATERINGHOLE:
    	    body_false = _("The drink at the watering hole is refreshing, but offers no further benefit. The watering hole might help again if you fought a battle first.");
	    body_true = _("A drink at the watering hole fills your troops with strength and lifts their spirits. You can travel a bit further today.");
    	    move = 400;	// + 4TP, from FAQ
    	    break;

        case MP2::OBJ_TEMPLE:
    	    body_false = _("It doesn't help to pray twice before a battle. Come back after you've fought.");
    	    body_true = _("A visit and a prayer at the temple raises the morale of your troops.");
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
    	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	// modify morale
	hero.SetVisited(dst_index);
	AGG::PlaySound(M82::GOODMRLE);
	DialogMorale(MP2::StringObject(obj), body_true, true, (obj == MP2::OBJ_TEMPLE ? 2 : 1));
        hero.IncreaseMovePoints(move);

	// fix double action tile
	if(obj == MP2::OBJ_OASIS)
	{
	    if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
	    if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToGoodMoraleObject: " + hero.GetName());
}


void ActionToExperienceObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    std::string body_true;
    std::string body_false;
    
    u32 exp = 0;

    switch(obj)
    {
        case MP2::OBJ_GAZEBO:
    	    body_false = _("An old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have taught you all I can.\"");
    	    body_true = _("An old Knight appears on the steps of the gazebo. \"My liege, I will teach you all that I know to aid you in your travels.\"");
    	    exp = 1000;
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundWarning;
	DialogWithExp(MP2::StringObject(obj), body_true, exp);

	// visit
	hero.SetVisited(dst_index);
	hero.IncreaseExperience(exp);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToExperienceObject: " + hero.GetName());
}

void ActionToArtifact(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

    bool conditions = false;
    const u8 c = 0x0f & tile.GetQuantity2();

    switch(obj)
    {
        case MP2::OBJ_SHIPWRECKSURVIROR:
        {
	    conditions = true;
	    PlaySoundSuccess;
	    std::string str = _("You've pulled a shipwreck survivor from certain death in an unforgiving ocean. Grateful, he rewards you for your act of kindness by giving you the %{art}.");
	    String::Replace(str, "%{art}", Artifact::String(art));
	    DialogWithArtifact(MP2::StringObject(obj), str, art);
	}
	    break;

	case MP2::OBJ_ARTIFACT:
	    switch(c)
	    {
		// 1,2,3 - 2000g, 2500g+3res,3000g+5res
		case 1:
		case 2:
		case 3:
		{
		    u8 type = tile.GetQuantity2() >> 4;
		    Resource::resource_t r = Resource::UNKNOWN;

		    switch(type)
		    {
			case 1: r = Resource::WOOD; break;
        		case 2: r = Resource::MERCURY; break;
        		case 3: r = Resource::ORE; break;
        		case 4: r = Resource::SULFUR; break;
        		case 5: r = Resource::CRYSTAL; break;
        		case 6: r = Resource::GEMS; break;
			default:
			    r = Resource::Rand();
			    // store variants
			    switch(r)
			    {
				case Resource::WOOD: tile.SetQuantity2(0x10 + c); break;
        			case Resource::MERCURY: tile.SetQuantity2(0x20 + c); break;
        			case Resource::ORE: tile.SetQuantity2(0x30 + c); break;
        			case Resource::SULFUR: tile.SetQuantity2(0x40 + c); break;
        			case Resource::CRYSTAL: tile.SetQuantity2(0x50 + c); break;
        			case Resource::GEMS: tile.SetQuantity2(0x60 + c); break;
				default: break;
			    }
			    break;
		    }

		    std::string header;

		    Resource::funds_t payment;
		    if(1 == c)
		    {
			header = _("A leprechaun offers you the %{art} for the small price of %{gold} Gold.");
			String::Replace(header, "%{gold}", 2000);
			payment += Resource::funds_t(Resource::GOLD, 2000);
		    }
		    else
		    if(2 == c)
		    {
			header = _("A leprechaun offers you the %{art} for the small price of %{gold} Gold and %{count} %{res}.");
			String::Replace(header, "%{gold}", 2500);
			String::Replace(header, "%{count}", 3);
			String::Replace(header, "%{res}", Resource::String(r));
			payment += Resource::funds_t(Resource::GOLD, 2500);
			payment += Resource::funds_t(r, 3);
		    }
		    else
		    {
			header = _("A leprechaun offers you the %{art} for the small price of %{gold} Gold and %{count} %{res}.");
			String::Replace(header, "%{gold}", 3000);
			String::Replace(header, "%{count}", 5);
			String::Replace(header, "%{res}", Resource::String(r));
			payment += Resource::funds_t(Resource::GOLD, 3000);
			payment += Resource::funds_t(r, 5);
		    }
		    String::Replace(header, "%{art}", Artifact::String(art));

		    PlaySoundWarning;
		    if(Dialog::YES == DialogWithArtifact(header, _("Do you wish to buy this artifact?"), art, Dialog::YES | Dialog::NO))
		    {
			if(world.GetKingdom(hero.GetColor()).AllowPayment(payment))
			{
		    	    conditions = true;
		    	    world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
			}
			else
			{
			    PlaySoundFailure;
			    Dialog::Message(_("You try to pay the leprechaun, but realize that you can't afford it."), _("The leprechaun stamps his foot and ignores you."), Font::BIG, Dialog::OK);
			}
		    }
		    else
			Dialog::Message("", _("Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you."), Font::BIG, Dialog::OK);
		    break;
		}

		// 4,5 - need have skill wisard or leadership,
		case 4:
		case 5:
		{
		    if(4 == c)
		    {
			if(hero.HasSecondarySkill(Skill::Secondary::WISDOM))
			{
			    PlaySoundSuccess;
			    DialogWithArtifact(MP2::StringObject(obj), _("You've found the artifact: ") + Artifact::String(art), art, Dialog::OK);
			    conditions = true;
			}
			else
			{
			    PlaySoundFailure;
			    std::string str = _("The hermit tells you that he is willing to give the %{art} to the first wise person he meets.");
			    String::Replace(str, "%{art}", Artifact::String(art));
			    Dialog::Message(_("You've found the humble dwelling of a withered hermit."), str, Font::BIG, Dialog::OK);
			}
		    }
		    else
		    {
			if(hero.HasSecondarySkill(Skill::Secondary::LEADERSHIP))
			{
			    PlaySoundSuccess;
			    DialogWithArtifact(MP2::StringObject(obj), _("You've found the artifact: ") + Artifact::String(art), art, Dialog::OK);
			    conditions = true;
			}
			else
			{
			    PlaySoundFailure;
			    std::string str = _("The soldier tells you that he is willing to pass on the %{art} to the first true leader he meets.");
			    String::Replace(str, "%{art}", Artifact::String(art));
			    Dialog::Message(_("You've come across the spartan quarters of a retired soldier."), str, Font::BIG, Dialog::OK);
			}
		    }
		    break;
		}

		// 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		{
		    bool battle = true;
		    Army::army_t army;
		    army.FromGuardian(tile);
		    const Army::Troop & troop = army.FirstValid();
		    const std::string & monster = troop.GetName();

		    PlaySoundWarning;

		    if(6 == c)
			Dialog::Message(_("You come upon an ancient artifact."), _("As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot."), Font::BIG, Dialog::OK);
		    else
		    {
			std::string str = _("Unfortunately, it's guarded by a nearby %{monster}. Do you want to fight the %{monster} for the artifact?");
			String::Replace(str, "%{monster}", monster);
			battle = (Dialog::YES == Dialog::Message(_("Through a clearing you observe an ancient artifact."), str,	Font::BIG, Dialog::YES | Dialog::NO));
		    }
		    
		    if(battle)
		    {
			const u32 exp = army.CalculateExperience();
			const Army::battle_t b = Army::Battle(hero, army, tile);
			switch(b)
			{
			    case Army::WIN:
			    {
				hero.IncreaseExperience(exp);
				conditions = true;
				PlaySoundSuccess;
				std::string str = _("Victorious, you take your prize, the %{art}.");
				String::Replace(str, "%{art}", Artifact::String(art));
				DialogWithArtifact(MP2::StringObject(obj), str, art);
				hero.ActionAfterBattle();
			    }
			    break;

			    case Army::RETREAT:
			    case Army::SURRENDER:
			    case Army::LOSE:
				BattleLose(hero, b);
				break;
        
    			default: break;
			}
		    }
		    else
		    {
			PlaySoundFailure;
			Dialog::Message("", _("Discretion is the better part of valor, and you decide to avoid this fight for today."), Font::BIG, Dialog::OK);
		    }
		    break;
		}

		default:
		    PlaySoundSuccess;
		    DialogWithArtifact(MP2::StringObject(obj), _("You've found the artifact: ") + Artifact::String(art), art);
		    conditions = true;
		    break;
	    }
	break;

	default: break;
    }

    if(conditions && hero.PickupArtifact(art))
    {
	PlayPickupSound();
	AnimationRemoveObject(tile);

	tile.RemoveObjectSprite();
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
	tile.SetObject(MP2::OBJ_ZERO);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToArtifact: " + hero.GetName() + " pickup artifact");
}

void ActionToTreasureChest(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    resource.gold = tile.GetQuantity2() * 100;

    PlayPickupSound();
    AnimationRemoveObject(tile);

    // dialog
    if(Maps::Ground::WATER == tile.GetGround())
    {
	std::string message;

	if(0 == resource.gold)
	{
	    Dialog::Message("Chest", _("After spending hours trying to fish the chest out of the sea, you open it, only to find it empty."), Font::BIG, Dialog::OK);
	}
	else
	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

	    if(hero.PickupArtifact(art))
	    {
		message = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold and the %{art}.");
		String::Replace(message, "%{gold}", resource.gold);
		String::Replace(message, "%{art}", Artifact::String(art));
		DialogWithArtifactAndGold("Chest", message, art, resource.gold);
	    }
	    else
	    {
		resource.gold = 1500;	// it is from FAQ
		message = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces.");
		String::Replace(message, "%{gold}", resource.gold);
		DialogWithGold("Chest", message, resource.gold);
	    }
	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	}
	else
	{
	    message = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces.");
	    String::Replace(message, "%{gold}", resource.gold);
	    DialogWithGold("Chest", message, resource.gold);

	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	}
    }
    else
    {
	std::string message;

	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	
	    if(hero.PickupArtifact(art))
	    {
		message = _("After scouring the area, you fall upon a hidden chest, containing the ancient artifact %{art}.");
		String::Replace(message, "%{art}", Artifact::String(art));
		DialogWithArtifact("Chest", message, art);
	    }
	    else
	    {
		resource.gold = 1000;	// it is from FAQ
		message = _("After scouring the area, you fall upon a hidden chest, containing the %{gold} gold pieces.");
		String::Replace(message, "%{gold}", resource.gold);
		DialogWithGold("Chest", message, resource.gold);
	    }
	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	}
	else
	if(resource.gold >= 500)
	{
	    const u16 expr = resource.gold - 500;
	    message = _("After scouring the area, you fall upon a hidden treasure cache. You may take the gold or distribute the gold to the peasants for experience. Do you wish to keep the gold?");

	    if(Dialog::SelectGoldOrExp("Chest", message, resource.gold, expr))
		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    else
		hero.IncreaseExperience(expr);
	}
    }

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("ActionToTreasureChest: " + hero.GetName() + " pickup chest");
}

void ActionToAncientLamp(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u32 count = tile.GetCountMonster();

    PlaySoundSuccess;
    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("You stumble upon a dented and tarnished lamp lodged deep in the earth. Do you wish to rub the lamp?"), Font::BIG, Dialog::YES|Dialog::NO))
    {
	const u16 recruit = Dialog::RecruitMonster(Monster::GENIE, count);
	if(recruit)
	{
	    if(hero.GetArmy().JoinTroop(Monster::GENIE, recruit))
	    {
		if(recruit == count)
		{
		    PlayPickupSound();
		    AnimationRemoveObject(tile);
		    tile.SetCountMonster(0);
		    tile.RemoveObjectSprite();
		    tile.SetObject(MP2::OBJ_ZERO);
		}
		else
		    tile.SetCountMonster(count - recruit);
	    }
	    else
		Dialog::Message(Monster::String(Monster::GENIE), _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToTreasureChest: " + hero.GetName() + " pickup chest");
}

void ActionToTeleports(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextTeleport(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	AGG::PlaySound(M82::RSBRYFZL);
	Error::Warning("ActionToTeleports: action unsuccessfully...");
	return;
    }

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();

    hero.SetCenter(index_to);
    hero.Scoute();

    world.GetTiles(index_from).SetObject(MP2::OBJ_STONELIGHTS);
    world.GetTiles(index_to).SetObject(MP2::OBJ_HEROES);

    Game::Focus::Get().Redraw();
    Display::Get().Flip();

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    if(Settings::Get().Debug()) Error::Verbose("ActionToStoneLights: " + hero.GetName());
}

void ActionToWhirlpools(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextWhirlpool(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	AGG::PlaySound(M82::RSBRYFZL);
	Error::Warning("ActionToWhirlpools: action unsuccessfully...");
	return;
    }

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();

    hero.SetCenter(index_to);
    hero.Scoute();

    world.GetTiles(index_from).SetObject(MP2::OBJ_WHIRLPOOL);
    world.GetTiles(index_to).SetObject(MP2::OBJ_HEROES);

    Game::Focus::Get().Redraw();
    Display::Get().Flip();

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    if(Rand::Get(1))
    {
	PlaySoundWarning;
	Dialog::Message(_("A whirlpool engulfs your ship."), _("Some of your army has fallen overboard."), Font::BIG, Dialog::OK);
	Army::Troop & troops = hero.GetArmy().GetWeakestTroop();
	const u16 c = troops.Count() / 2;
	troops.SetCount(c ? c : 1);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToWhirlpools: " + hero.GetName());
}

void ActionToAbandoneMine(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    PlaySoundWarning;
    if(Dialog::YES == Dialog::Message(_("You come upon an abandoned gold mine."), _("The mine appears to be haunted. Do you wish to enter?"), Font::BIG, Dialog::YES | Dialog::NO))
    {
	Army::army_t army;
	army.FromGuardian(tile);

	const u32 exp = army.CalculateExperience();
	const Army::battle_t b = Army::Battle(hero, army, tile);

	switch(b)
	{
	    case Army::WIN:
		hero.IncreaseExperience(exp);
		PlaySoundSuccess;
		DialogWithGold(MP2::StringObject(obj), _("You beat the Ghosts and are able to restore the mine to production."), 1000);
		tile.SetQuantity1(0);
		tile.UpdateAbandoneMineSprite();
		world.CaptureObject(dst_index, hero.GetColor());
		hero.ActionAfterBattle();
		break;

	    case Army::RETREAT:
	    case Army::SURRENDER:
	    case Army::LOSE:
		BattleLose(hero, b);
		break;
        
    	    default: break;
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToAbandoneMine: " + hero.GetName() + " captured: " + std::string(MP2::StringObject(obj)));
}

/* capture color object */
void ActionToCaptureObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    std::string header;
    std::string body;

    Resource::resource_t res = Resource::UNKNOWN;
    Surface *sf = NULL;

    switch(obj)
    {
	case MP2::OBJ_ALCHEMYLAB:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 1);

	    sf = new Surface(sprite.w(), sprite.h() + 14);
	    sf->SetColorKey();
	    sf->Blit(sprite);
	    body = "1 / day";
    	    Text text(body, Font::SMALL);
    	    text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

	    res = Resource::MERCURY;
	    header = MP2::StringObject(obj);
	    body = _("You have taken control of the local Alchemist shop. It will provide you with one unit of Mercury per day.");
	}
	    break;
        case MP2::OBJ_MINES:
    	{
    	    const Maps::TilesAddon * taddon = world.GetTiles(dst_index).FindMines();

            // ore
            if(0 == taddon->index)
            {
		const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 2);

		sf = new Surface(sprite.w(), sprite.h() + 14);
		sf->SetColorKey();
		sf->Blit(sprite);
		body = "2 / day";
    		Text text(body, Font::SMALL);
    		text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf); // Here is pink fringing, becose letter sprite use shadow. Wiil fix later...

        	res = Resource::ORE;
        	header = _("Ore Mine");
        	body = _("You gain control of an ore mine. It will provide you with two units of ore per day.");
            }
            else
            // sulfur
            if(1 == taddon->index)
            {
		const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 3);

		sf = new Surface(sprite.w(), sprite.h() + 14);
		sf->SetColorKey();
		sf->Blit(sprite);
		body = "1 / day";
    		Text text(body, Font::SMALL);
    		text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

        	res = Resource::SULFUR;
        	header = _("Sulfur Mine");
		body = _("You gain control of a sulfur mine. It will provide you with one unit of sulfur per day.");
            }
            else
            // crystal
            if(2 == taddon->index)
            {
		const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 4);

		sf = new Surface(sprite.w(), sprite.h() + 14);
		sf->SetColorKey();
		sf->Blit(sprite);
		body = "1 / day";
    		Text text(body, Font::SMALL);
    		text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

        	res = Resource::CRYSTAL;
        	header = _("Crystal Mine");
		body = _("You gain control of a crystal mine. It will provide you with one unit of crystal per day.");
            }
            else
            // gems
            if(3 == taddon->index)
            {
		const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 5);

		sf = new Surface(sprite.w(), sprite.h() + 14);
		sf->SetColorKey();
		sf->Blit(sprite);
		body = "1 / day";
    		Text text(body, Font::SMALL);
    		text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

        	res = Resource::GEMS;
        	header = _("Gems Mine");
		body = _("You gain control of a gem mine. It will provide you with one unit of gems per day.");
            }
            else
            // gold
            if(4 == taddon->index)
            {
		const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 6);

		sf = new Surface(sprite.w(), sprite.h() + 14);
		sf->SetColorKey();
		sf->Blit(sprite);
		body = "1000 / day";
    		Text text(body, Font::SMALL);
    		text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

        	res = Resource::GOLD;
        	header = _("Gold Mine");
		body = _("You gain control of a gold mine. It will provide you with 1000 gold per day.");
            }
    	}
    	    break;
	case MP2::OBJ_SAWMILL:
	{
	    const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 0);

	    sf = new Surface(sprite.w(), sprite.h() + 12);
	    sf->SetColorKey();
	    sf->Blit(sprite);
	    body = "2 / day";
    	    Text text(body, Font::SMALL);
    	    text.Blit((sf->w() - text.w()) / 2, sf->h() - 12, *sf);

    	    res = Resource::WOOD;
	    header = MP2::StringObject(obj);
	    body = _("You gain control of a sawmill. It will provide you with two units of wood per day.");
	}
	    break;

        case MP2::OBJ_LIGHTHOUSE:
	    header = MP2::StringObject(obj);
    	    body = _("The lighthouse is now under your control, and all of your ships will now move further each turn.");
	    break;

        default:
    	    Error::Warning("ActionToCaptureObject: unknown captured: " + std::string(MP2::StringObject(obj)));
    	    return;
    }

    // capture object
    if(hero.GetColor() != world.ColorCapturedObject(dst_index))
    {
	PlaySoundSuccess;
	world.CaptureObject(dst_index, hero.GetColor());

	if(sf) Dialog::SpriteInfo(header, body, *sf);
	else Dialog::Message(header, body, Font::BIG, Dialog::OK);
    }

    if(sf) delete sf;
    if(Settings::Get().Debug()) Error::Verbose("ActionToCaptureObject: " + hero.GetName() + " captured: " + std::string(MP2::StringObject(obj)));
}

void ActionToDwellingJoinMonster(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    switch(obj)
    {
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
	case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT: break;
	default: return;
    }

    const u32 count = tile.GetCountMonster();

    if(count)
    {
	const Monster monster(Monster::FromObject(obj));
        std::string message = _("A group of %{monster} with a desire for greater glory wish to join you.");
        String::Replace(message, "%{monster}", monster.GetName());

        AGG::PlaySound(M82::EXPERNCE);
	if(Dialog::YES == Dialog::Message(message, _("Do you accept?"), Font::BIG, Dialog::YES|Dialog::NO))
	{
	    if(!hero.GetArmy().JoinTroop(monster, count))
		Dialog::Message(monster.GetName(), _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
	    else
		tile.SetCountMonster(0);
	}
    }
    else
    {
	PlaySoundVisited;
	Dialog::Message("", _("As you approach the dwelling, you notice that there is no one here."), Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToDwellingJoinMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToDwellingRecruitMonster(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    
    std::string msg_full, msg_void;

    switch(obj)
    {
        case MP2::OBJ_RUINS:
    	    msg_void = _("You search the ruins, but the Medusas that used to live here are gone. Perhaps there will be more next week.");
    	    msg_full = _("You've found some Medusas living in the ruins. They are willing to join your army for a price. Do you want to recruit Medusas?");
    	    break;

        case MP2::OBJ_TREECITY:
    	    msg_void = _("You've found a Sprite Tree City. Unfortunately, none of the Sprites living there wish to join an army. Maybe next week.");
    	    msg_full = _("Some of the Sprites living in the tree city are willing to join your army for a price. Do you want to recruit Sprites?");
    	    break;
  
        case MP2::OBJ_WAGONCAMP:
    	    msg_void = _("A colorful Rogues' wagon stands empty here. Perhaps more Rogues will be here later.");
    	    msg_full = _("Distant sounds of music and laughter draw you to a colorful wagon housing Rogues. Do you wish to have any Rogues join your army?");
    	    break;

	case MP2::OBJ_DESERTTENT:
    	    msg_void = _("A group of tattered tents, billowing in the sandy wind, beckons you. The tents are unoccupied. Perhaps more Nomads will be here later.");
    	    msg_full = _("A group of tattered tents, billowing in the sandy wind, beckons you. Do you wish to have any Nomads join you during your travels?");
    	    break;

        case MP2::OBJ_EARTHALTAR:
	    msg_void = _("The pit of mud bubbles for a minute and then lies still.");
	    msg_full = _("As you approach the bubbling pit of mud, creatures begin to climb out and position themselves around it. In unison they say: \"Mother Earth would like to offer you a few of her troops. Do you want to recruit Earth Elementals?\"");
	    break;

        case MP2::OBJ_AIRALTAR:
	    msg_void = _("You enter the structure of white stone pillars, and find nothing.");
	    msg_full = _("White stone pillars support a roof that rises up to the sky. As you enter the structure, the dead air of the outside gives way to a whirling gust that almost pushes you back out. The air current materializes into a barely visible form. The creature asks, in what can only be described as a loud whisper: \"Why have you come? Are you here to call upon the forces of the air?\"");
	    break;

        case MP2::OBJ_FIREALTAR:
	    msg_void = _("No Fire Elementals approach you from the lava pool.");
	    msg_full = _("Beneath a structure that serves to hold in heat, Fire Elementals move about in a fiery pool of molten lava. A group of them approach you and offer their services. Would you like to recruit Fire Elementals?");
	    break;

        case MP2::OBJ_WATERALTAR:
	    msg_void = _("A face forms in the water for a moment, and then is gone.");
	    msg_full = _("Crystalline structures cast shadows over a small reflective pool of water. You peer into the pool, and a face that is not your own peers back. It asks: \"Would you like to call upon the powers of water?\"");
	    break;

	case MP2::OBJ_BARROWMOUNDS:
	    msg_void = _("This burial site is deathly still.");
	    msg_full = _("Restless spirits of long dead warriors seeking their final resting place offer to join you in hopes of finding peace. Do you wish to recruit ghosts?");
	    break;

	default: return;
    }

    const u32 count = tile.GetCountMonster();

    if(count)
    {
	const Monster monster(Monster::FromObject(obj));
	PlaySoundSuccess;
	if(Dialog::YES == Dialog::Message("", msg_full, Font::BIG, Dialog::YES | Dialog::NO))
	{
	    const u16 recruit = Dialog::RecruitMonster(monster, count);
	    if(recruit)
	    {
		if(!hero.GetArmy().JoinTroop(monster, recruit))
		    Dialog::Message(monster.GetName(), _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
		else
		    tile.SetCountMonster(count - recruit);
	    }
	}
    }
    else
    {
	PlaySoundVisited;
	Dialog::Message("", msg_void, Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToDwellingRecruitMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToDwellingBattleMonster(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // yet no one captured.
    const bool battle = (Color::GRAY == world.ColorCapturedObject(dst_index));
    const u32 count = tile.GetCountMonster();
    bool complete = false;

    switch(obj)
    {
        case MP2::OBJ_CITYDEAD:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message(MP2::StringObject(obj), _("The City of the Dead is empty of life, and empty of unlife as well. Perhaps some undead will move in next week."), Font::BIG, Dialog::OK);
		    break;
		}
		else
	    	{
		    PlaySoundSuccess;
		    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("Some Liches living here are willing to join your army for a price. Do you want to recruit Liches?"), Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.FromGuardian(tile);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("You've found the ruins of an ancient city, now inhabited solely by the undead. Will you search?"), Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("Some of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price. Do you want to recruit Liches?"), Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
	    break;

        case MP2::OBJ_TROLLBRIDGE:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message(MP2::StringObject(obj), _("You've found one of those bridges that Trolls are so fond of living under, but there are none here. Perhaps there will be some next week."), Font::BIG, Dialog::OK);
		    break;
		}
		else
		{
		    PlaySoundSuccess;
	    	    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("Some Trolls living under a bridge are willing to join your army, but for a price. Do you want to recruit Trolls?"), Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.FromGuardian(tile);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("Trolls living under the bridge challenge you. Will you fight them?"), Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("A few Trolls remain, cowering under the bridge. They approach you and offer to join your forces as mercenaries. Do you want to buy any Trolls?"), Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
    	    break;

	case MP2::OBJ_DRAGONCITY:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message(MP2::StringObject(obj), _("The Dragon city has no Dragons willing to join you this week. Perhaps a Dragon will become available next week."), Font::BIG, Dialog::OK);
		    break;
		}
		else
		{
		    PlaySoundSuccess;
	    	    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("The Dragon city is willing to offer some Dragons for your army for a price. Do you wish to recruit Dragons?"), Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.FromGuardian(tile);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("You stand before the Dragon City, a place off-limits to mere humans. Do you wish to violate this rule and challenge the Dragons to a fight?"), Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("Having defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price. Do you wish to recruit Dragons?"), Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
	    break;

	default: return;
    }

    // recruit monster
    if(count && complete)
    {
	const Monster monster(Monster::FromObject(obj));
        const u16 recruit = Dialog::RecruitMonster(monster, count);
        if(recruit)
        {
    	    if(!hero.GetArmy().JoinTroop(monster, recruit))
		Dialog::Message(monster.GetName(), _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
    	    else
    		tile.SetCountMonster(count - recruit);
    	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToDwellingBattleMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToObservationTower(Heroes &hero, const u8 obj, const u16 dst_index)
{
    PlaySoundSuccess;
    Dialog::Message(MP2::StringObject(obj), _("From the observation tower, you are able to see distant lands."), Font::BIG, Dialog::OK);
    Maps::ClearFog(dst_index, OBSERVATIONTOWERSCOUTE, hero.GetColor());
}

void ActionToArtesianSpring(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();
    const std::string & name = MP2::StringObject(MP2::OBJ_ARTESIANSPRING);

    if(hero.isVisited(MP2::OBJ_ARTESIANSPRING))
    {
	PlaySoundVisited;
	Dialog::Message(name, _("The spring only refills once a week, and someone's already been here this week."), Font::BIG, Dialog::OK);
    }
    else
    if(hero.GetSpellPoints() == max * 2)
    {	
	PlaySoundFailure;
	Dialog::Message(name, _("A drink at the spring is supposed to give you twice your normal spell points, but you are already at that level."), Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max * 2);
	Dialog::Message(name, _("A drink from the spring fills your blood with magic! You have twice your normal spell points in reserve."), Font::BIG, Dialog::OK);

	// fix double action tile
	{
	    const Maps::Tiles & tile = world.GetTiles(dst_index);

	    if(Maps::isValidDirection(tile.GetIndex(), Direction::LEFT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::LEFT));
	    if(Maps::isValidDirection(tile.GetIndex(), Direction::RIGHT) &&
		tile.GetUniq1() == world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT)).GetUniq1()) hero.SetVisited(Maps::GetDirectionIndex(tile.GetIndex(), Direction::RIGHT));
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToArtesianSpring: " + hero.GetName());
}

void ActionToXanadu(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), _("Recognizing you, the butler refuses to admit you. \"The master,\" he says, \"will not see the same student twice.\""), Font::BIG, Dialog::OK);
    }
    else
    {
	bool access = false;
	switch(hero.GetLevelSkill(Skill::Secondary::DIPLOMACY))
	{
	    case Skill::Level::BASIC:
		if(7 < hero.GetLevel()) access = true; break;
	    case Skill::Level::ADVANCED:
		if(5 < hero.GetLevel()) access = true; break;
	    case Skill::Level::EXPERT:
		if(3 < hero.GetLevel()) access = true; break;
	    default:
		if(9 < hero.GetLevel()) access = true; break;
	}

	if(access)
	{
	    PlaySoundSuccess;
	    Dialog::Message(MP2::StringObject(obj), _("The butler admits you to see the master of the house. He trains you in the four skills a hero should know."), Font::BIG, Dialog::OK);
	    hero.IncreasePrimarySkill(Skill::Primary::ATTACK);
	    hero.IncreasePrimarySkill(Skill::Primary::DEFENCE);
	    hero.IncreasePrimarySkill(Skill::Primary::KNOWLEDGE);
	    hero.IncreasePrimarySkill(Skill::Primary::POWER);
	    hero.SetVisited(dst_index);
	}
	else
	{
	    PlaySoundFailure;
	    Dialog::Message(MP2::StringObject(obj), _("The butler opens the door and looks you up and down. \"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs. \"Come back when you think yourself worthy.\""), Font::BIG, Dialog::OK);
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToXanadu: " + hero.GetName());
}

void ActionToUpgradeArmyObject(Heroes &hero, const u8 obj, const u16 dst_index)
{
    std::string monsters;
    std::string monsters_upgrade;
    std::string msg1;
    std::string msg2;
    std::string msg3;
    
    std::vector<Monster::monster_t> mons;
    mons.reserve(3);

    switch(obj)
    {
	case MP2::OBJ_HILLFORT:
	    if(hero.GetArmy().HasMonster(Monster::DWARF))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::DWARF);
		mons.push_back(Monster::DWARF);
		monsters = Monster(Monster::DWARF).GetMultiName();
		monsters_upgrade = Monster(Monster::Upgrade(Monster::DWARF)).GetMultiName();
	    }
	    if(hero.GetArmy().HasMonster(Monster::ORC))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::ORC);
		mons.push_back(Monster::ORC);
		if(monsters.size()) monsters += ", ";
		monsters += Monster(Monster::ORC).GetMultiName();
		if(monsters_upgrade.size()) monsters_upgrade += ", ";
		monsters_upgrade += Monster(Monster::Upgrade(Monster::ORC)).GetMultiName();
	    }
	    if(hero.GetArmy().HasMonster(Monster::OGRE))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::OGRE);
		mons.push_back(Monster::OGRE);
		if(monsters.size()) monsters += ", ";
		monsters += Monster(Monster::OGRE).GetMultiName();
		if(monsters_upgrade.size()) monsters_upgrade += ", ";
		monsters_upgrade += Monster(Monster::Upgrade(Monster::OGRE)).GetMultiName();
	    }
	    msg1 = _("All of the %{monsters} you have in your army have been trained by the battle masters of the fort. Your army now contains %{monsters2}.");
	    String::Replace(msg1, "%{monsters}", monsters);
	    String::Replace(msg1, "%{monsters2}", monsters_upgrade);
	    msg2 = _("An unusual alliance of Orcs, Ogres, and Dwarves offer to train (upgrade) any such troops brought to them. Unfortunately, you have none with you.");
	    break;

	case MP2::OBJ_FREEMANFOUNDRY:
	    if(hero.GetArmy().HasMonster(Monster::PIKEMAN))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::PIKEMAN);
		mons.push_back(Monster::PIKEMAN);
		monsters = Monster(Monster::PIKEMAN).GetMultiName();
		monsters_upgrade = Monster(Monster::Upgrade(Monster::PIKEMAN)).GetMultiName();
	    }
	    if(hero.GetArmy().HasMonster(Monster::SWORDSMAN))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::SWORDSMAN);
		mons.push_back(Monster::SWORDSMAN);
		if(monsters.size()) monsters += ", ";
		monsters += Monster(Monster::SWORDSMAN).GetMultiName();
		if(monsters_upgrade.size()) monsters_upgrade += ", ";
		monsters_upgrade += Monster(Monster::Upgrade(Monster::SWORDSMAN)).GetMultiName();
	    }
	    if(hero.GetArmy().HasMonster(Monster::IRON_GOLEM))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::IRON_GOLEM);
		mons.push_back(Monster::IRON_GOLEM);
		if(monsters.size()) monsters += ", ";
		monsters += Monster(Monster::IRON_GOLEM).GetMultiName();
		if(monsters_upgrade.size()) monsters_upgrade += ", ";
		monsters_upgrade += Monster(Monster::Upgrade(Monster::IRON_GOLEM)).GetMultiName();
	    }
	    msg1 = _("All of your %{monsters} have been upgraded into %{monsters2}.");
	    String::Replace(msg1, "%{monsters}", monsters);
	    String::Replace(msg1, "%{monsters2}", monsters_upgrade);
	    msg2 = _("A blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems.  Unfortunately, you have none of these troops in your army, so he can't help you.");
	    break;

	default: break;
    }

    if(mons.size())
    {
	// composite sprite
	u8 ox = 0;
	const Sprite & br = AGG::GetICN(ICN::STRIP, 12);
	Surface sf(br.w() * mons.size() + (mons.size() - 1) * 4, br.h());
	sf.SetColorKey();
	std::vector<Monster::monster_t>::const_iterator it1 = mons.begin();
	std::vector<Monster::monster_t>::const_iterator it2 = mons.end();
	for(; it1 != it2; ++it1)
	{
	    sf.Blit(br, ox, 0);
	    switch(Monster(*it1).GetRace())
	    {
		case Race::KNGT:	sf.Blit(AGG::GetICN(ICN::STRIP, 4), ox + 6, 6); break;
		case Race::BARB:	sf.Blit(AGG::GetICN(ICN::STRIP, 5), ox + 6, 6); break;
		case Race::SORC:	sf.Blit(AGG::GetICN(ICN::STRIP, 6), ox + 6, 6); break;
		case Race::WRLK:	sf.Blit(AGG::GetICN(ICN::STRIP, 7), ox + 6, 6); break;
		case Race::WZRD:	sf.Blit(AGG::GetICN(ICN::STRIP, 8), ox + 6, 6); break;
		case Race::NECR:	sf.Blit(AGG::GetICN(ICN::STRIP, 9), ox + 6, 6); break;
		default:		sf.Blit(AGG::GetICN(ICN::STRIP, 10), ox + 6, 6); break;
	    }
	    const Sprite & mon = AGG::GetICN(Monster(Monster::Upgrade(*it1)).ICNMonh(), 0);
	    sf.Blit(mon, ox + 6 + mon.x(), 6 + mon.y());
	    ox += br.w() + 4;
	}
	Dialog::SpriteInfo(MP2::StringObject(obj), msg1, sf);
    }
    else
    {
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(obj), msg2, Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToUpgradeArmyObject: " + hero.GetName());
}

void ActionToMagellanMaps(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(1000 > world.GetKingdom(hero.GetColor()).GetFundsGold())
    {
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(obj), _("The captain sighs. \"You don't have enough money, eh?  You can't expect me to give my maps away for free!\""), Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundWarning;
	if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("A retired captain living on this refurbished fishing platform offers to sell you maps of the sea he made in his younger days for 1,000 gold. Do you wish to buy the maps?"), Font::BIG, Dialog::YES | Dialog::NO))
	    world.ActionForMagellanMaps(hero.GetColor());

	Game::Focus::Get().Redraw();
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToMagellanMaps: " + hero.GetName());
}

void ActionToEvent(Heroes &hero, const u8 obj, const u16 dst_index)
{
    // check event maps
    const GameEvent::Coord* event_maps = world.GetEventMaps(hero.GetColor(), dst_index);
    if(event_maps)
    {
        if(event_maps->GetResource().GetValidItems())
        {
    	    world.GetKingdom(hero.GetColor()).AddFundsResource(event_maps->GetResource());
	    PlaySoundSuccess;
    	    Dialog::ResourceInfo("", event_maps->GetMessage(), event_maps->GetResource());
	}
	else
        if(event_maps->GetMessage().size())
            Dialog::Message("", event_maps->GetMessage(), Font::BIG, Dialog::OK);

	if(Artifact::UNKNOWN != event_maps->GetArtifact())
	{
	    if(hero.PickupArtifact(event_maps->GetArtifact()))
	    {
		PlayPickupSound();
		DialogWithArtifact("You find " + Artifact::String(event_maps->GetArtifact()), "", event_maps->GetArtifact());
	    }
	}
    }

    world.GetTiles(dst_index).SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("ActionToEvent: " + hero.GetName());
}

void ActionToObelisk(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(!hero.isVisited(obj, Visit::GLOBAL))
    {
        hero.SetVisited(dst_index, Visit::GLOBAL);
        AGG::PlaySound(M82::EXPERNCE);
        Dialog::Message(MP2::StringObject(obj), _("You come upon an obelisk made from a type of stone you have never seen before. Staring at it intensely, the smooth surface suddenly changes to an inscription. The inscription is a piece of a lost ancient map. Quickly you copy down the piece and the inscription vanishes as abruptly as it appeared."), Font::BIG, Dialog::OK);
    }
    
    Dialog::PuzzleMaps();
    if(Settings::Get().Debug()) Error::Verbose("ActionToObelisk: " + hero.GetName());
}

void ActionToTreeKnowledge(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), _("Upon your approach, the tree opens its eyes in delight. \"It is good to see you, my student. I hope my teachings have helped you.\""), Font::BIG, Dialog::OK);
    }
    else
    {
	bool conditions = false;
	Resource::funds_t payment;
	switch(tile.GetQuantity2())
	{
	    case 10:	payment.gems = 10; break;
	    case 20:	payment.gold = 2000; break;
	    default:	conditions = true; break;
	}

        if(conditions)
        {
	    const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
	    const std::string & body = _("Upon your approach, the tree opens its eyes in delight. \"Ahh, an adventurer! Allow me to teach you a little of what I have learned over the ages.\"");
	    Dialog::SpriteInfo(MP2::StringObject(obj), body, sprite);
	}
	else
	{
	    if(world.GetKingdom(hero.GetColor()).AllowPayment(payment))
            {
		const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
		std::string body = _("Upon your approach, the tree opens its eyes in delight.");
		body += " ";
		body += payment.gold ? _("\"Ahh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere 2000 gold.\"") :
		        _("\"Ahh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere 10 gems.\"");
		body += " ";
		body += _("(Just bury it around my roots.)");
		conditions = Dialog::YES == Dialog::SpriteInfo(MP2::StringObject(obj), body, sprite, Dialog::YES|Dialog::NO);
    	    }
	    else
	    {
		std::string body = _("Tears brim in the eyes of the tree.");
		body += " ";
		body += payment.gold ? _("\"I need 2000 gold.\"") : _("\"I need 10 gems.\"");
		body += " ";
		body += _("it whispers. (sniff) \"Well, come back when you can pay me.\"");
		Dialog::Message(MP2::StringObject(obj), body, Font::BIG, Dialog::OK);
	    }
	}

	if(conditions)
	{
    	    if(payment.GetValidItems()) world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
	    hero.SetVisited(dst_index);
	    Error::Verbose("level: ", hero.GetLevel());
	    hero.IncreaseExperience(hero.GetExperienceFromLevel(hero.GetLevel()) - hero.GetExperience());
	}
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToTreeKnowledge: " + hero.GetName());
}

void ActionToOracle(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Dialog::ThievesGuild(0xFF);
    if(Settings::Get().Debug()) Error::Verbose("ActionToOracle: " + hero.GetName());
}

void ActionToDaemonCave(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("The entrance to the cave is dark, and a foul, sulfurous smell issues from the cave mouth. Will you enter?"), Font::BIG, Dialog::YES|Dialog::NO))
    {
	if(tile.GetQuantity2())
	{
	    Resource::funds_t resource;

	    if(Dialog::YES == Dialog::Message("", _("You find a powerful and grotesque Demon in the cave. \"Today,\" it rasps, \"you will fight and surely die. But I will give you a choice of deaths. You may fight me, or you may fight my servants. Do you prefer to fight my servants?\""), Font::BIG, Dialog::YES|Dialog::NO))
	    {
		// battle with earth elements
		Army::army_t army;
		army.FromGuardian(tile);

		// battle
		const u32 exp = army.CalculateExperience();
		const Army::battle_t b = Army::Battle(hero, army, tile);
		switch(b)
		{
		    case Army::WIN:
			hero.IncreaseExperience(exp);
			hero.ActionAfterBattle();
			resource.gold = 2500;
			DialogWithGold("", _("Upon defeating the daemon's servants, you find a hidden cache with 2500 gold."), 2500);
			world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			tile.SetQuantity2(0);
		    break;

		    case Army::RETREAT:
		    case Army::SURRENDER:
		    case Army::LOSE:
			BattleLose(hero, b);
			break;
        
    		    default: break;
		}
	    }
	    // check variants
	    else
	    {
		u16 exp = 0;
		switch(tile.GetQuantity2())
		{
		    case 1:
			exp = 1000;
			DialogWithExp("", _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive 1,000 experience points."), exp);
    			hero.IncreaseExperience(exp);
			tile.SetQuantity2(0);
			break;
		    case 2:
		    {
		    	exp = 1000;
			const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
			std::string str = _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and find the %{art} in the back of the cave.");
			String::Replace(str, "%{art}", Artifact::String(art));
			if(Artifact::UNKNOWN != art) DialogArtifactWithExp("", str, art, exp);
    			hero.PickupArtifact(art);
    			hero.IncreaseExperience(exp);
			tile.SetQuantity1(Artifact::UNKNOWN);
			tile.SetQuantity2(0);
			break;
		    }
		    case 3:
		    	exp = 1000;
			resource.gold = 2500;
			DialogGoldWithExp("", _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive 1,000 experience points and 2,500 gold."), resource.gold, exp);
    			hero.IncreaseExperience(exp);
			world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			tile.SetQuantity2(0);
			break;
		    default:
		    {
			bool remove = true;
			if(2500 <= world.GetKingdom(hero.GetColor()).GetFundsGold())
			{
			    if(Dialog::YES == Dialog::Message("", _("The Demon leaps upon you and has its claws at your throat before you can even draw your sword. \"Your life is mine,\" it says. \"I will sell it back to you for 2,500 gold.\""), Font::BIG, Dialog::YES|Dialog::NO))
			    {
				remove = false;
				resource.gold = 2500;
				world.GetKingdom(hero.GetColor()).OddFundsResource(resource);
			    }
			}
			else
			    Dialog::Message("", _("Seeing that you do not have 2500 gold, the demon slashes you with its claws, and the last thing you see is a red haze."), Font::BIG, Dialog::OK);

			if(remove) BattleLose(hero, Army::LOSE);
			tile.SetQuantity2(0);
			break;
		    }
		}
	    }
	}
	else
	    Dialog::Message("", _("Except for evidence of a terrible battle, the cave is empty."), Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToDaemonCave: " + hero.GetName());
}

void ActionToAlchemistsTower(Heroes &hero, const u8 obj, const u16 dst_index)
{
/*
As you enter the Alchemist's Tower, a hobbled, graying man in a brown cloak makes his way towards you. e checks your pack, and sees
that you have 1 cursed item. For 750 gold, the alchemist will remove it for you. Do you pay?
As you enter the Alchemist's Tower, a hobbled, graying man in a brown cloak makes his way towards you. He checks your pack, and sees
that you have %d cursed items. For 750 gold, the alchemist will remove them for you. Do you pay?
You hear a voice from behind the locked door, "You don't have enough gold to pay for my services."
You hear a voice from high above in the tower, "Go away! I can't help you!"
*/
    Dialog::Message("", _("You hear a voice from high above in the tower, \"Go away! I can't help you!\""), Font::BIG, Dialog::OK);

    if(Settings::Get().Debug()) Error::Verbose("ActionToAlchemistsTower: " + hero.GetName());
}

void ActionToStables(Heroes &hero, const u8 obj, const u16 dst_index)
{
    const bool cavalry = hero.GetArmy().HasMonster(Monster::CAVALRY);
    const bool visited = hero.isVisited(obj);
    std::string body;

    if(!cavalry && visited)
	body = _("The head groom approaches you and speaks, \"You already have a fine horse, and have no inexperienced cavalry which might make use of our trained war horses.\"");
    else
    if(!cavalry && !visited)
	body = _("As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, his endurance will wane with a lot of heavy riding, and you must return for a fresh mount in a week. We also have many fine war horses which could benefit mounted soldiers, but you have none we can help.\"");
    else
    if(cavalry && visited)
	body = _("The head groom speaks to you, \"That is a fine looking horse you have. I am afraid we can give you no better, but the horses your cavalry are riding look to be of poor breeding stock. We have many trained war horses which would aid your riders greatly. I insist you take them.\"");
    else
    if(cavalry && !visited)
	body = _("As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, he will grow tired in a week. You must also let me give better horses to your mounted soldiers, their horses look shoddy and weak.\"");


    // check already visited
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
    }
    else
    {
	hero.SetVisited(dst_index);
	PlaySoundSuccess;
        hero.IncreaseMovePoints(400);
    }

    if(cavalry) hero.GetArmy().UpgradeMonsters(Monster::CAVALRY);

    Dialog::Message("", body, Font::BIG, Dialog::OK);

    if(Settings::Get().Debug()) Error::Verbose("ActionToStables: " + hero.GetName());
}

void ActionToArena(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), _("The Arena guards turn you away."), Font::BIG, Dialog::OK);
    }
    else
    {
	hero.SetVisited(dst_index);
	PlaySoundSuccess;
	hero.IncreasePrimarySkill(Dialog::SelectSkillFromArena());
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToArena: " + hero.GetName());
}

void ActionToSirens(Heroes &hero, const u8 obj, const u16 dst_index)
{
    if(hero.isVisited(obj, Visit::GLOBAL))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), _("As the sirens sing their eerie song, your small, determined army manages to overcome the urge to dive headlong into the sea."), Font::BIG, Dialog::OK);
    }
    else
    {
	u32 exp = hero.GetArmy().ActionToSirens();
	std::string str = _("You have your crew stop up their ears with wax before the sirens' eerie song has any chance of luring them to a watery grave. An eerie wailing song emanates from the sirens perched upon the rocks. Many of your crew fall under its spell, and dive into the water where they drown. You are now wiser for the visit, and gain %{exp} experience.");
	String::Replace(str, "%{exp}", exp);

	hero.SetVisited(dst_index, Visit::GLOBAL);
	PlaySoundSuccess;
	Dialog::Message(MP2::StringObject(obj), str, Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToSirens: " + hero.GetName());
}

void ActionToJail(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Kingdom & kingdom = world.GetKingdom(hero.GetColor());

    if(kingdom.GetHeroes().size() < KINGDOMMAXHEROES)
    {
	Maps::Tiles & tile = world.GetTiles(dst_index);
	PlaySoundSuccess;
	Dialog::Message(MP2::StringObject(obj), _("In a dazzling display of daring, you break into the local jail and free the hero imprisoned there, who, in return, pledges loyalty to your cause."), Font::BIG, Dialog::OK);

	AnimationRemoveObject(tile);
	tile.RemoveObjectSprite();
	tile.SetObject(MP2::OBJ_ZERO);

	Heroes *prisoner = world.FromJail(tile.GetIndex());

	if(prisoner)
	{
	    const Point center(tile.GetIndex() % world.w(), tile.GetIndex() / world.w());
	    prisoner->Recruit(hero.GetColor(), center);
	    prisoner->ResetModes(Heroes::JAIL);
	    kingdom.AddHeroes(prisoner);
	}
    }
    else
    {
	std::string str = _("You already have %{count} heroes, and regretfully must leave the prisoner in this jail to languish in agony for untold days.");
	String::Replace(str, "%{count}", KINGDOMMAXHEROES);
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(obj), str, Font::BIG, Dialog::OK);
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToJail: " + hero.GetName());
}

void ActionToHutMagi(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Dialog::Message(MP2::StringObject(obj), _("You enter a rickety hut and talk to the magician who lives there. He tells you of places near and far which may aid you in your journeys."), Font::BIG, Dialog::OK);

    if(!hero.isVisited(obj, Visit::GLOBAL))
    {
	hero.SetVisited(dst_index, Visit::GLOBAL);
	world.ActionToEyeMagi(hero.GetColor());
    }

    if(Settings::Get().Debug()) Error::Verbose("ActionToHutMagi: " + hero.GetName());
}

void ActionToEyeMagi(Heroes &hero, const u8 obj, const u16 dst_index)
{
    Dialog::Message(MP2::StringObject(obj), _("This eye seems to be intently studying its surroundings."), Font::BIG, Dialog::OK);

    if(Settings::Get().Debug()) Error::Verbose("ActionToEyeMagi: " + hero.GetName());
}

void ActionToShinx(Heroes &hero, const u8 obj, const u16 dst_index)
{
    GameEvent::Riddle* riddle = world.GetSphinx(dst_index);

    if(riddle && riddle->isValid())
    {
	if(Dialog::YES == Dialog::Message("", _("\"I have a riddle for you,\" the Sphinx says. \"Answer correctly, and you shall be rewarded. Answer incorrectly, and you shall be eaten. Do you accept the challenge?\""), Font::BIG, Dialog::YES|Dialog::NO))
	{
	    std::string header(_("The Sphinx asks you the following riddle: %{riddle}. Your answer?"));
	    String::Replace(header, "%{riddle}", riddle->GetMessage());
	    std::string answer;
	    Dialog::InputString(header, answer);
	    if(riddle->AnswerCorrect(answer))
	    {
		const Resource::funds_t & res = riddle->GetResource();
		const Artifact::artifact_t art = riddle->GetArtifact();
		const std::string say = _("Looking somewhat disappointed, the Sphinx sighs. You've answered my riddle so here's your reward. Now begone.");
		const u8 count = res.GetValidItems();

		if(count)
		{
		    if(1 == count && res.gold && Artifact::UNKNOWN != art)
			DialogWithArtifactAndGold("", say, art, res.gold);
		    else
		    {
			Dialog::ResourceInfo("", say, res);
			if(Artifact::UNKNOWN != art) DialogWithArtifact("", say, art);
		    }
		}
		else
		if(Artifact::UNKNOWN != art) DialogWithArtifact("", say, art);

		riddle->SetQuiet();
		hero.SetVisited(dst_index, Visit::GLOBAL);
	    }
	    else
	    {
		Dialog::Message("", _("\"You guessed incorrectly,\" the Sphinx says, smiling. The Sphinx swipes at you with a paw, knocking you to the ground. Another blow makes the world go black, and you know no more."), Font::BIG, Dialog::OK);
		BattleLose(hero, Army::LOSE);
	    }
	}
    }
    else
	Dialog::Message(MP2::StringObject(obj), _("You come across a giant Sphinx. The Sphinx remains strangely quiet."), Font::BIG, Dialog::OK);

    if(Settings::Get().Debug()) Error::Verbose("ActionToShinx: " + hero.GetName());
}
