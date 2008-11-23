/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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


#include <cmath>
#include <algorithm>
#include <utility>
#include "battle.h"
#include "agg.h"
#include "ground.h"
#include "world.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "portrait.h"
#include "spell.h"
#include "castle.h"
#include "heroes.h"
#include "kingdom.h"
#include "maps_tiles.h"

#define CELLW 44
#define CELLH 42
#define BFX 86
#define BFY 108
#define BFW 11
#define BFH 9
#define MORALEBOUND 10

#define display Display::Get()
#define le LocalEvent::GetLocalEvent()
#define cursor Cursor::Get()
#define world World::GetWorld()

#define do_button(b, lf, rt) \
if(le.MousePressLeft(b)) b.PressDraw();	\
else b.ReleaseDraw(); \
if(le.MouseClickLeft(b) && !b.isDisable()) lf; \
if(le.MousePressRight(b)&& !b.isDisable()) rt;

#define do_rbutton(b, lf, rt) \
if(le.MouseClickLeft(b) && !b.isDisable()) { b.isPressed() ? b.ReleaseDraw() : b.PressDraw(); lf; } \
if(le.MousePressRight(b) && !b.isDisable()) rt;

static Dialog::FrameBorder *frameborder = NULL;

namespace Army {
    bool O_GRID = false, O_SHADM = false, O_SHADC = false, O_AUTO = false;
    long EXP1, EXP2;
    Point dst_pt;
    std::vector<Point> movePoints;
    Dialog::StatusBar *statusBar1;
    Dialog::StatusBar *statusBar2;
    typedef struct {
	ICN::icn_t icn;
	Point scrpoint, bfpoint;
    } CObj;
    std::vector<Point> blockedCells;
    std::vector<CObj> cobjects;
    Army::BattleArmy_t bodies1, bodies2;
    
    enum {
        PREATTACK = 0,
        MISSILE_FLIGHT,
        MISSILE_IMPACT
    };
        
    typedef std::vector<std::pair<const Army::BattleArmy_t *, const Army::BattleArmy_t *> > ArmyPairs;

    BattleArmy_t ArmyToBattleArmy(const Army::army_t &);
    army_t BattleArmyToArmy(const Army::BattleArmy_t &);

    bool ArmyExists(BattleArmy_t &army);
    battle_t BattleInt(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile);
    battle_t HumanTurn(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack);
    battle_t CompTurn(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack);
    bool CleanupBodies(Army::BattleArmy_t &army, u16 idx, Army::BattleArmy_t &bodies);
    
    void DrawArmySummary(const Army::BattleArmy_t &orig, const Army::BattleArmy_t &current, const Rect &draw);
    void BattleSummary(const std::string &name, const ArmyPairs &armies, const std::vector<Artifact::artifact_t> *artifacts, Spell::spell_t, int deadRaised, Army::battle_t status);
    void BattleSummaryVsArmy(Heroes &hero, const Army::BattleArmy_t &heroOrig, const Army::BattleArmy_t &army, const Army::BattleArmy_t &armyOrig, Army::battle_t status);
    void BattleSummaryVsHero(Heroes &hero, const Army::BattleArmy_t &heroOrig, Heroes &hero2, const Army::BattleArmy_t &hero2Orig, Army::battle_t status);
    
    bool DangerousUnitPredicate(Army::BattleTroop first, Army::BattleTroop second);
    bool AttackNonRangedTroop(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack);
    bool AttackRangedTroop(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack);
    bool AttackTroopInList(const Army::BattleArmy_t &troop, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack);
    void MoveToClosestTroopInList(const Army::BattleArmy_t &troop, const Army::BattleTroop &myTroop, Point &move);
    void MoveOrAttack(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack, bool rangedFirst);
    
    void GetTargets(std::vector<Army::BattleTroop *> &targets, const Army::BattleTroop &attacker, const Point &attack, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2);
    
    bool IsDamageFatal(int damage, Army::BattleTroop &troop);
    bool AnimateCycle(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &move, const Point &attack);
    bool AnimateAttack(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &attack);
    bool AnimateMove(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &move);
    bool MagicSelectTarget(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, bool reflect, Spell::spell_t spell);
    void MagicPreCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::BattleTroop*> &aff1, std::vector<Army::BattleTroop*> &aff2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile);
    bool MagicCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::BattleTroop*> &aff1, std::vector<Army::BattleTroop*> &aff2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile);
    
    void InitBackground(const Maps::Tiles & tile);
    void DrawBackground(const Maps::Tiles & tile);
    void DrawObject(const Point &pt);
    void DrawCaptain(const Race::race_t race, u16 animframe, bool reflect=false);
    Rect DrawHero(const Heroes & hero, u16 animframe, bool reflect=false, int fframe=0);
    void DrawArmy(Army::BattleArmy_t & army1, Army::BattleArmy_t & army2, int animframe=-1);
    void DrawTroop(Army::BattleTroop & troop, int animframe=-1);
    
    void InitArmyPosition(Army::BattleArmy_t & army, bool compact, bool reflect=false);
    void StartTurn(Army::BattleArmy_t &army);
    
    inline Point Scr2Bf(const Point & pt); // translate coordinate to battle field
    inline Point Bf2Scr(const Point & pt); // translate to screen (offset to frame border)
    inline bool BfValid(const Point & pt); // check battle field point
    
    int FindTroop(const Army::BattleArmy_t &army, const Point &p);
    int FindTroop(const std::vector<Army::BattleTroop*> &army, const Point &p);
    int FindTroopExact(const Army::BattleArmy_t &army, const Point &p);
    bool FindTroopAt(const Army::BattleTroop &troop, const Point &p);
    bool CellFree(const Point &p, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip=99);
    bool CellFreeFor(const Point &p, const Army::BattleTroop &troop, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip=99);
    Point GetReachableAttackCell(const Army::BattleTroop &target, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int troopN);
    std::vector<Point> *FindPath(const Point& start, const Point &end, int moves, const Army::BattleTroop &troop, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip);
    int CanAttack(const Army::BattleTroop &myTroop, const std::vector<Point> &moves, const Army::BattleTroop &enemyTroop, const Point &d);
    
    void SettingsDialog();
    battle_t HeroStatus(Heroes &hero, Dialog::StatusBar &statusBar, Spell::spell_t &spell, bool quickshow, bool cansurrender=false, bool locked=false);
    
    void DrawShadow(const Point &pt);
    void DrawCell(const Point &pt);
    void DrawMark(const Point &point, int animframe=0);
    void DrawPath(const std::vector<Point> &path);
    
    void PrepMovePoints(int troopN, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2);
    void PrepMovePointsInt(const Army::BattleTroop &troop, const Point &p, int move, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip);
    
    void AttackStatus(const std::string &str);
    
    bool GoodMorale(Heroes *hero, const Army::BattleTroop &troop);
    bool BadMorale(Heroes *hero, const Army::BattleTroop &troop);
    int CheckLuck(Heroes *hero, const Army::BattleTroop &troop);
    bool CanRetaliate(const Army::BattleTroop &attacker, const Army::BattleTroop &defender);
    bool CanAttackTwice(Monster::monster_t attacker);
    int AdjustDamage(Monster::monster_t attacker, Monster::monster_t defender, int damage, bool ranged);

    void Temp(int, int, Point);
}

Army::BattleArmy_t Army::ArmyToBattleArmy(const Army::army_t &army)
{
    Army::BattleArmy_t battleArmy;
    for(u16 i = 0; i < army.size(); i++)
        battleArmy.push_back(Army::BattleTroop(army[i]));
    return battleArmy;
}

Army::army_t Army::BattleArmyToArmy(const Army::BattleArmy_t &battleArmy)
{
    Army::army_t army;
    for(u16 i = 0; i < battleArmy.size(); i++)
        army.push_back(battleArmy[i]);
    return army;
}

Army::battle_t Army::Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile)
{
    Army::army_t heroArmy = hero1.GetArmy();
    Army::army_t oppArmy = hero2.GetArmy();
    Army::battle_t status = BattleInt(&hero1, &hero2, hero1.GetArmy(), hero2.GetArmy(), tile);
    BattleSummaryVsHero(hero1, ArmyToBattleArmy(heroArmy), hero2, ArmyToBattleArmy(oppArmy), status);
    return status;
}

Army::battle_t Army::Battle(Heroes& hero, Army::army_t& army, const Maps::Tiles & tile)
{
    Army::army_t heroArmy = hero.GetArmy();
    Army::army_t oppArmy = army;
    Army::battle_t status = BattleInt(&hero, 0, hero.GetArmy(), army, tile);
    BattleSummaryVsArmy(hero, ArmyToBattleArmy(heroArmy), ArmyToBattleArmy(army), ArmyToBattleArmy(oppArmy), status);
    return status;
}

Army::battle_t Army::Battle(Heroes& hero, Castle& castle, const Maps::Tiles &tile)
{
    Army::army_t heroArmy = hero.GetArmy();
    Army::army_t oppArmy = castle.GetArmy();
    // TODO
    //Army::battle_t status = BattleInt(&hero, 0, const_cast<std::vector<Army::BattleTroop>&>(hero.GetArmy()), army, tile);
    Army::battle_t status = WIN;
    BattleSummaryVsArmy(hero, ArmyToBattleArmy(heroArmy), ArmyToBattleArmy(castle.GetArmy()), ArmyToBattleArmy(oppArmy), status);
    return status;
}

void Army::BattleSummaryVsArmy(Heroes &hero, const Army::BattleArmy_t &heroOrig, const Army::BattleArmy_t &army, const Army::BattleArmy_t &armyOrig, Army::battle_t status)
{
    ArmyPairs armies;
    Army::BattleArmy_t ownArmy = ArmyToBattleArmy(hero.GetArmy());
    armies.push_back(std::make_pair(&ownArmy, &heroOrig));
    armies.push_back(std::make_pair(&army, &armyOrig));
    //TODO: Necromancy
    BattleSummary(hero.GetName(), armies, NULL, Spell::NONE, 0, status);
}

void Army::BattleSummaryVsHero(Heroes &hero, const Army::BattleArmy_t &heroOrig, Heroes &hero2, const Army::BattleArmy_t &hero2Orig, Army::battle_t status)
{
    ArmyPairs armies;
    Army::BattleArmy_t ownArmy = ArmyToBattleArmy(hero.GetArmy());
    Army::BattleArmy_t otherArmy = ArmyToBattleArmy(hero2.GetArmy());
    armies.push_back(std::make_pair(&ownArmy, &heroOrig));
    armies.push_back(std::make_pair(&otherArmy, &hero2Orig));
    //TODO: Necromancy
    //TODO: Eagle eye
    const std::vector<Artifact::artifact_t> *artifacts = status == WIN ? &hero2.GetArtifacts() : NULL;
    BattleSummary(hero.GetName(), armies, artifacts, Spell::NONE, 0, status);
    Heroes *from, *to;
    if(status == WIN)
    {
        from = &hero2;
        to = &hero;
    }
    else
    {
        from = &hero;
        to = &hero2;
    }
    if(from->GetArtifacts().size() > 1)
    {
        //Transfer all artifacts except the spell book
        to->GetArtifacts().insert(to->GetArtifacts().end(), from->GetArtifacts().begin() + 1, from->GetArtifacts().end());
        from->GetArtifacts().clear();
    }
}

void Army::DrawArmySummary(const Army::BattleArmy_t &orig, const Army::BattleArmy_t &current, const Rect &draw)
{
    std::vector<std::pair<const Sprite *, int> > killed;
    u16 width = 0, height = 0;
    for(u16 i = 0; i < orig.size(); i++)
    {
        int delta = orig[i].Count() - current[i].Count();
        if(!orig[i].isValid() || delta <= 0)
            continue;
        const Sprite &monster = AGG::GetICN(ICN::MONS32, orig[i].Monster());
        killed.push_back(std::make_pair(&monster, delta));
        width += monster.w() + 10;
        height = std::max(height, monster.h());
    }
    if(killed.size())
    {
        width -= 10;
        Surface surf(width, height + 15);
        surf.SetDisplayFormat();
        surf.SetColorKey();
        int x = 0;
        for(u16 i = 0; i < killed.size(); i++)
        {
            const Sprite &sprite = *killed[i].first;
            surf.Blit(sprite, x, surf.h() - sprite.h() - 15);
            std::string amount;
            String::AddInt(amount, killed[i].second);
            Text number(amount, Font::SMALL);
            number.Blit(Point(x + (sprite.w() - number.width()) / 2, surf.h() - 15), surf);
            x += sprite.w() + 10;
        }
        display.Blit(surf, draw.x + (draw.w - surf.w()) / 2, draw.y);
    }
    else
    {
        Text none(tr("battle.none"), Font::SMALL);
        none.Blit(draw.x + (draw.w - none.width()) / 2, draw.y);
    }
}

void Army::BattleSummary(const std::string &name, const Army::ArmyPairs &armies,
                         const std::vector<Artifact::artifact_t> *artifacts,
                         Spell::spell_t spell, int deadRaised, Army::battle_t status)
{
    ICN::icn_t interface = Settings::Get().EvilInterface() ? ICN::WINLOSEE : ICN::WINLOSE;
    ICN::icn_t buttonIcon = Settings::Get().EvilInterface() ? ICN::WINCMBBE : ICN::WINCMBTB;
    const Sprite &background = AGG::GetICN(interface, 0);
    const Sprite &shadow = AGG::GetICN(interface, 1);
    const Sprite &button = AGG::GetICN(buttonIcon, 0);
    const int backgroundX = (display.w() - background.w()) / 2;
    const int backgroundY = (display.h() - background.h()) / 2;
    Button buttonDone(backgroundX + (background.w() - button.w()) / 2 + 1, backgroundY + background.h() - button.h() * 2 + 3, buttonIcon, 0, 1);
    
    std::string message[2];
    std::vector<std::pair<ICN::icn_t, Point> > animation;
    switch(status)
    {
        case WIN:
            animation.push_back(std::make_pair(ICN::WINCMBT, Point(32, 0)));
            message[0] = tr("battle.victory1");
            message[1] = tr("battle.victory2").sub(name).sub(1000); //FIXME
            break;
        case LOSE:
            animation.push_back(std::make_pair(ICN::CMBTLOS1, Point(0, 0)));
            animation.push_back(std::make_pair(ICN::CMBTLOS2, Point(0, 0)));
            animation.push_back(std::make_pair(ICN::CMBTLOS3, Point(0, 29)));
            message[0] = tr("battle.lose").sub(name);
            break;
        case SURRENDER:
            animation.push_back(std::make_pair(ICN::CMBTSURR, Point(32, 3)));
            message[0] = tr("battle.surrender");
            break;
        case RETREAT:
            animation.push_back(std::make_pair(ICN::CMBTFLE1, Point(34, 42)));
            animation.push_back(std::make_pair(ICN::CMBTFLE2, Point(0, 12)));
            animation.push_back(std::make_pair(ICN::CMBTFLE3, Point(0, 0)));
            message[0] = tr("battle.retreat").sub(name);
            break;
        default:
            break;
    }
    
    const Sprite &animBase = AGG::GetICN(animation[0].first, 0, false);
    const int baseAnimX = backgroundX + (background.w() - animBase.w()) / 2 - 1;
    const int baseAnimY = backgroundY + 36;
   
    cursor.SetThemes(cursor.WAR_POINTER); 
    display.Fade(100, false);
    display.Flip();
    cursor.Hide();
    display.Blit(shadow, backgroundX - 17, backgroundY + 10);
    display.Blit(background, backgroundX, backgroundY);
    display.Blit(animBase, baseAnimX, baseAnimY);
    buttonDone.Draw();
    
    TextBox first(message[0], Font::BIG, Rect(baseAnimX - 20, backgroundY + 180, animBase.w() + 40, background.h() - 160));
    if(message[1].size())
        TextBox::TextBox(message[1], Font::BIG, Rect(baseAnimX - 20, first.extents.y + first.extents.h,
                         animBase.w() + 40, background.h() - 160 - first.extents.h));
    
    const int textY = backgroundY + background.h() / 2 + 30;
    TextBox title(tr("battle.casualties"), Font::SMALL, Rect(backgroundX, textY, background.w(), 40));
    TextBox attacker(tr("battle.attacker"), Font::SMALL, Rect(backgroundX, title.extents.y + int(title.extents.h * 1.5f), background.w(), 40));
    TextBox defender(tr("battle.defender"), Font::SMALL, Rect(backgroundX, attacker.extents.y + attacker.extents.h * 4, background.w(), 40));
    
    Army::DrawArmySummary(*armies[0].second, *armies[0].first, Rect(backgroundX, attacker.extents.y + attacker.extents.h + 5, background.w(), 0));
    Army::DrawArmySummary(*armies[1].second, *armies[1].first, Rect(backgroundX, defender.extents.y + defender.extents.h + 5, background.w(), 0));
    
    cursor.Show();
    display.Flip();
    
    u32 ticket = 0;
    u32 frame = 1, subanim = 0, count = AGG::GetICNCount(animation[subanim].first);
    u16 artIndex = 1; //skip the magic book
    int state = 0;
    while(le.HandleEvents()) {
        if(le.MousePressLeft(buttonDone)) buttonDone.PressDraw();
        else buttonDone.ReleaseDraw();
        
        if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonDone))
        {
            bool done = false;
            switch(state)
            {
                case 0:
                    state++; //fall-through
                case 1:
                    if(!artifacts)
                        state++; //fall-through
                    else if(artIndex < artifacts->size())
                    {
                        display.Blit(background, backgroundX, backgroundY);
                        AGG::PlaySound(M82::PICKUP01);
                        const Sprite & art = AGG::GetICN(ICN::ARTIFACT, artifacts->at(artIndex) + 1);
                        std::string artName = Artifact::String(artifacts->at(artIndex));
                        Text artText(artName, Font::SMALL);
                        artText.Blit(backgroundX + (background.w() - artText.width()) / 2, buttonDone.y - 20 - art.h() - artText.height() - 10);
                        display.Blit(art, backgroundX + (background.w() - art.w()) / 2, buttonDone.y - 20 - art.h());
                        artIndex++;
                        break;
                    }
                    else state++; //fall-through
                case 2:
                    state++; //fall-through
                    if(spell != Spell::NONE)
                    {
                        //TODO: Eagle eye
                        display.Blit(background, backgroundX, backgroundY);
                        AGG::PlaySound(M82::PICKUP01);
                        break;
                    }
                case 3:
                    state++; //fall-through
                    if(deadRaised != 0)
                    {
                        //TODO: Necromancy
                        display.Blit(background, backgroundX, backgroundY);
                        AGG::PlaySound(M82::PICKUP01);
                        break;
                    }
                case 4:
                    done = true;
                    break;
            }
            if(done)
              break;
        }
        
        if(frame < count && Game::ShouldAnimateInfrequent(ticket++, 3))
        {
            const Sprite &animFrame = AGG::GetICN(animation[subanim].first, frame++, false);
            display.Blit(AGG::GetICN(animation[subanim].first, 0), baseAnimX, baseAnimY);
            display.Blit(animFrame, animation[subanim].second.x + baseAnimX, animation[subanim].second.y + baseAnimY);
            display.Flip();
            if(frame == count && animation[subanim].first != ICN::CMBTFLE3)
            {
                if(subanim < animation.size() - 1)
                    count = AGG::GetICNCount(animation[++subanim].first);
                frame = 1;
            }
        }
    }
    
    if(frameborder) delete frameborder;
    frameborder = NULL;
}

Army::battle_t Army::BattleInt(Heroes *hero1, Heroes *hero2, Army::army_t &basic_army1, Army::army_t &basic_army2, const Maps::Tiles &tile)
{
    Mixer::Reset();
    //FIXME: The music currently plays underneath the sound effect
    int track = ((int)MUS::BATTLE1 + Rand::Get(0, 2));
    AGG::PlayMusic((MUS::mus_t)track);
    
    AGG::PlaySound(M82::PREBATTL);
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();
    
    Army::battle_t status = AUTO;
    
    Army::BattleArmy_t army1 = ArmyToBattleArmy(basic_army1);
    Army::BattleArmy_t army2 = ArmyToBattleArmy(basic_army2);
    
    const Settings &settings = Settings::Get();
    O_GRID = settings.BattleGrid();
    O_SHADM = settings.BattleMovementShaded();
    O_SHADC = settings.BattleMouseShaded();
    
    frameborder = new Dialog::FrameBorder;

    dst_pt = Point(frameborder->GetArea().x, frameborder->GetArea().y);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    InitBackground(tile);
    DrawBackground(tile);
    if(hero1) DrawHero(*hero1, 1);
    if(hero2) DrawHero(*hero2, 1, true);
    InitArmyPosition(army1, false);
    InitArmyPosition(army2, false, true);
    DrawArmy(army1, army2, 1);

    display.Blit(AGG::GetICN(ICN::TEXTBAR, 0), dst_pt.x + 640-48, dst_pt.y + 480-36);
    display.Blit(AGG::GetICN(ICN::TEXTBAR, 4), dst_pt.x, dst_pt.y + 480-36);
    display.Blit(AGG::GetICN(ICN::TEXTBAR, 6), dst_pt.x, dst_pt.y + 480-18);

    statusBar1 = new Dialog::StatusBar(Point(dst_pt.x + 50, dst_pt.y + 480-36), AGG::GetICN(ICN::TEXTBAR, 8), Font::BIG);
    statusBar1->Clear(NOL10N(" "));
    statusBar2 = new Dialog::StatusBar(Point(dst_pt.x + 50, dst_pt.y + 480-16), AGG::GetICN(ICN::TEXTBAR, 9), Font::BIG);
    statusBar2->Clear(NOL10N(" "));

    display.Flip();
    Point move, attack;
    EXP1 = EXP2 = 0;  // experience = damage
    bool goodmorale;

    while(status == AUTO) {
	if(hero1) hero1->spellCasted = false;
	if(hero2) hero2->spellCasted = false;
	for(u16 i=0; i<army1.size(); i++) army1[i].ProceedMagic();
	for(u16 i=0; i<army2.size(); i++) army2[i].ProceedMagic();
	Speed::speed_t cursp = Speed::INSTANT;
	while(1) {
            StartTurn(army1);
            StartTurn(army2);
        
	    for(unsigned int i=0; i < army1.size(); i++) {
                bool attackerAlive = true;
		goodmorale = false;
		if(Monster::GetStats(army1[i].Monster()).speed == cursp && army1[i].Count() > 0 && !BadMorale(hero1, army1[i])) do {
                  redoturn:
		    battle_t s;
		    if(hero1 && world.GetKingdom(hero1->GetColor()).Control() == Game::LOCAL && !O_AUTO) {
			s = HumanTurn(hero1, hero2, army1, army2, tile, i, move, attack);
			if( s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLELOSE, false);
                            status = s;
                            break;
                        }
                        else if(s == AUTO)
                            goto redoturn;
		    } else {
			s = CompTurn(hero1, hero2, army1, army2, tile, i, move, attack);
			if( s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLELOSE, false);
                            status = s;
                            break;
                        }
		    }
                    bool defenderAlive = true;
                    int repeat = CanAttackTwice(army1[i].Monster()) ? 1 : 0;
                    do
                    {
                        //Perform the attack
                        if(AnimateCycle(hero1, hero2, army1, army2, tile, i, move, attack)) {
                            //Perform the retaliation
                            if(!AnimateCycle(hero1, hero2, army1, army2, tile, -FindTroop(army2, attack)-1, attack, move)) {
                                attackerAlive = !CleanupBodies(army1, i, bodies1);
                            }
                        } else {
                            int t = FindTroop(army2, attack);
                            if(t >= 0)
                                defenderAlive = !CleanupBodies(army2, t, bodies2);
                        }
                    } while(repeat-- > 0 && attackerAlive && defenderAlive);
		} while (attackerAlive && !goodmorale && ArmyExists(army2) && (goodmorale = GoodMorale(hero1, army1[i]), goodmorale));
                
                if(!ArmyExists(army2))
                    break;
	    }
	    for(unsigned int i=0; i < army2.size(); i++) {
		goodmorale = false;
                bool attackerAlive = true;
		if(Monster::GetStats(army2[i].Monster()).speed == cursp && army2[i].Count() > 0 && !BadMorale(hero2, army2[i])) do {
		    battle_t s;
		    if(hero2 && world.GetKingdom(hero2->GetColor()).Control() == Game::LOCAL && !O_AUTO) {
			s = HumanTurn(hero1, hero2, army1, army2, tile, -i-1, move, attack);
			if(s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLEWIN, false);
                            status = WIN;
                            break;
                        }
		    } else {
			s = CompTurn(hero1, hero2, army1, army2, tile, -i-1, move, attack);
			if(s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLEWIN, false);
                            status = WIN;
                            break;
                        }
		    }
                    bool defenderAlive = true;
                    int repeat = CanAttackTwice(army2[i].Monster()) ? 1 : 0;
                    do
                    {
                        //Perform the attack
                        if(AnimateCycle(hero1, hero2, army1, army2, tile, -i-1, move, attack)) {
                            //Perform the retaliation
                            if(!AnimateCycle(hero1, hero2, army1, army2, tile, FindTroop(army1, attack), attack, move)) {
                                attackerAlive = !CleanupBodies(army2, i, bodies2);
                            }
                        } else {
                            int t = FindTroop(army1, attack);
                            if(t >= 0)
                                defenderAlive = !CleanupBodies(army1, t, bodies1);
                        }
                    } while(repeat-- > 0 && defenderAlive && attackerAlive);
		} while (attackerAlive && !goodmorale && ArmyExists(army1) && (goodmorale = GoodMorale(hero2, army2[i]), goodmorale));
                
                if(!ArmyExists(army1))
                    break;
	    }
	    int c1 = 0, c2 = 0;
	    for(unsigned int i=0; i < army1.size(); i++) c1 += army1[i].Count();
	    for(unsigned int i=0; i < army2.size(); i++) c2 += army2[i].Count();
	    if(c1 == 0)
            {
                AGG::PlayMusic(MUS::BATTLELOSE, false);
                status = LOSE;
                break;
            }
	    if(c2 == 0)
            {
                AGG::PlayMusic(MUS::BATTLEWIN, false);
                status = WIN;
                break;
            }
	    if(cursp == Speed::CRAWLING) break;
	    else --cursp;
	}
    }
    
    // TODO remove summoned creatures
    
    basic_army1 = BattleArmyToArmy(army1);
    basic_army2 = BattleArmyToArmy(army2);
    return status;
}

/** Remove any units from the battle that are dead, and return the status of a particular unit
 *  \param army   Army in which to search for dead units
 *  \param idx    Specific index into \i army about which to return infomation
 *  \param bodies List of dead units
 *  \return True if \i army[idx] was cleaned up, and therefore dead, false otherwise
 */
bool Army::CleanupBodies(Army::BattleArmy_t &army, u16 idx, Army::BattleArmy_t &bodies)
{
    bool troopCleaned = false;
    for(u16 i = 0; i < army.size(); i++)
    {
        if(army[i].Count() == 0)
        {
            Army::BattleTroop body = army[i];
            bodies.push_back(body);
            army[i].SetMonster(Monster::UNKNOWN);
            if(i == idx)
                troopCleaned = true;
        }
    }
    return troopCleaned;
}

bool Army::ArmyExists(Army::BattleArmy_t &army)
{
    for(u16 i = 0; i < army.size(); i++)
        if(army[i].Count() > 0)
            return true;
    return false;
}

Army::battle_t Army::HumanTurn(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack)
{
    std::vector<Army::BattleTroop> &myArmy = troopN >= 0 ? army1 : army2;
    std::vector<Army::BattleTroop> &enemyArmy = troopN >= 0 ? army2 : army1;
    move.x = move.y = attack.x = attack.y = -1;
    
    //Sanity check
    if(!ArmyExists(myArmy) || !ArmyExists(enemyArmy))
        return WIN;
    
    Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    PrepMovePoints(troopN, army1, army2);
    bool close = false;
    for(u16 j=0; j<enemyArmy.size(); j++) {
	Point tmpoint(0,0);
	std::vector<Point> tmpoints;
	tmpoints.push_back(enemyArmy[j].Position());
	if(CanAttack(enemyArmy[j], tmpoints, myTroop, tmpoint) >= 0) {
	    close = true;
	    break;
	}
    }
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    AttackStatus(NOL10N(""));

    DrawBackground(tile);
    Rect rectHero1, rectHero2;
    if(hero1) rectHero1 = DrawHero(*hero1, 1);
    if(hero2) rectHero2 = DrawHero(*hero2, 1, true);
    DrawArmy(army1, army2);

    Button buttonSkip(dst_pt.x + 640-48, dst_pt.y + 480-36, ICN::TEXTBAR, 0, 1);
    Button buttonAuto(dst_pt.x, dst_pt.y + 480-36, ICN::TEXTBAR, 4, 5);
    Button buttonSettings(dst_pt.x, dst_pt.y + 480-18, ICN::TEXTBAR, 6, 7);

    /*Dialog::StatusBar statusBar1(Point(dst_pt.x + 50, dst_pt.y + 480-36), AGG::GetICN(ICN::TEXTBAR, 8), Font::BIG);
    statusBar1.Clear(NOL10N(" "));
    Dialog::StatusBar statusBar2(Point(dst_pt.x + 50, dst_pt.y + 480-16), AGG::GetICN(ICN::TEXTBAR, 9), Font::BIG);
    statusBar2.Clear(NOL10N(" "));*/

    buttonSkip.Draw();
    buttonAuto.Draw();
    buttonSettings.Draw();

    display.Flip();
    cursor.Show();

    int i = 0;
    u16 animat = 0;
    Point cur_pt(-1,-1);
    Spell::spell_t spell = Spell::NONE;
    // dialog menu loop
    while(le.HandleEvents()) {
	if(le.KeyPress(KEY_RETURN)) {
            for(u16 i = 0; i < army2.size(); i++)
                army2[i].SetCount(0);
            return WIN;
	}
        if(le.KeyPress(SDLK_BACKSPACE)) {
            for(u16 i = 0; i < army1.size(); i++)
                army1[i].SetCount(0);
            return LOSE;
        }
        if(le.KeyPress(SDLK_BACKSLASH))
        {
            for(u16 i = 0; i < army1.size(); i++)
                army1[i].SetReflect(!army1[i].IsReflected());
        }
	if(le.KeyPress(SDLK_SPACE)) {
	    DrawBackground(tile);
	    Temp(0, 1, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_PLUS)) {
	    DrawBackground(tile);
	    Temp(1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_MINUS)) {
	    DrawBackground(tile);
	    Temp(-1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(Game::ShouldAnimateInfrequent(++animat, 3) && !i) {
	    cursor.Hide();
	    DrawBackground(tile);
	    if(O_SHADM)
		for(u16 i=0; i<movePoints.size(); i++)
		    DrawShadow(movePoints[i]);
	    if(hero1) DrawHero(*hero1, 1, false, 1);
	    if(hero2) DrawHero(*hero2, 1, true);
	    if(O_SHADC)
            {
                /*for(int x = -1; x <= 1; x++)
                    for(int y = -1; y <= 1; y++)
                        if(BfValid(cur_pt + Point(x, y)))
                            DrawShadow(cur_pt + Point(x, y));*/
                if(BfValid(cur_pt))
                    DrawShadow(cur_pt);
            }
	    DrawMark(myTroop.Position(), 1);
	    if(myMonster.wide) {
		Point p = myTroop.Position();
		p.x += myTroop.IsReflected() ? -1 : 1;
		DrawMark(p);
	    }
	    //display.Blit(AGG::GetICN(ICN::TEXTBAR, 8), Point(dst_pt.x + 50, dst_pt.y + 480-36));
	    statusBar1->Redraw();
	    DrawArmy(army1, army2);
	    cursor.Show();
	    display.Flip();
	}
	do_button(buttonSkip, return WIN, Dialog::Message(tr("battle.skip"), tr("battle.skip_info"), Font::BIG));
	do_button(buttonAuto, { O_AUTO=true; return AUTO; }, Dialog::Message(tr("battle.auto"), tr("battle.auto_info"), Font::BIG));
	do_button(buttonSettings, SettingsDialog(), Dialog::Message(tr("battle.options"), tr("battle.options_info"), Font::BIG));
	if(le.MouseClickLeft(rectHero1)) {
	    battle_t s = HeroStatus(*hero1, *statusBar2, spell, false, hero2, troopN < 0);
	    if(s == RETREAT) {
		if(Dialog::Message(NOL10N(""), tr("battle.retreat?"), Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
		    return s;
	    }
	    if(s == SURRENDER) return s;
	    if(spell != Spell::NONE && MagicSelectTarget(hero1, hero2, army1, army2, tile, false, spell))
		hero1->spellCasted = 0;//true;
	}
	if(le.MouseClickLeft(rectHero2)) {
	    battle_t s = HeroStatus(*hero2, *statusBar2, spell, false, hero1, troopN >= 0);
	    if(s == RETREAT) {
		if(Dialog::Message(NOL10N(""), tr("battle.retreat?"), Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
		    return s;
	    }
	    if(s == SURRENDER) return s;
	    if(spell != Spell::NONE && MagicSelectTarget(hero1, hero2, army1, army2, tile, true, spell))
		hero2->spellCasted = 0;//true;
	}
	if(le.MousePressRight(rectHero1)) HeroStatus(*hero1, *statusBar2, spell, true, false, false);
	if(le.MousePressRight(rectHero2)) HeroStatus(*hero2, *statusBar2, spell, true, false, false);
        if(le.MouseCursor(buttonSkip)) {
	    statusBar2->ShowMessage(tr("battle.skip_unit"));
	    cursor.SetThemes(cursor.WAR_POINTER);
        }else if(le.MouseCursor(buttonAuto)) {
	    statusBar2->ShowMessage(tr("battle.auto"));
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(buttonSettings)) {
	    statusBar2->ShowMessage(tr("battle.system_options"));
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(troopN >= 0 ? rectHero1 : rectHero2)) {
	    statusBar2->ShowMessage(tr("battle.hero_options"));
	    cursor.SetThemes(cursor.WAR_HELMET);
	} else if(le.MouseCursor(troopN >= 0 ? rectHero2 : rectHero1)) {
	    statusBar2->ShowMessage(tr("battle.opp_hero"));
	    cursor.SetThemes(cursor.WAR_INFO);
	} else {
	    int t = -1;
	    bool click;
	    cur_pt = Scr2Bf(le.MouseCursor()-dst_pt);
	    
            if(!le.MouseLeft()
            && cur_pt == Scr2Bf(le.MousePressLeft()-dst_pt)
            && cur_pt == Scr2Bf(le.MouseReleaseLeft()-dst_pt))
            {
		click = true;
		le.MouseClickLeft(Rect(0, 0, display.w(), display.h()));
	    } else click = false;
	    
            if(BfValid(cur_pt)) {
		// cursor on the battle field
		if(t = FindTroop(myArmy, cur_pt), t >= 0 && myArmy[t].Count() > 0) {
		    // troop from my army
		    statusBar2->ShowMessage(tr("battle.monster_info").sub(Monster::String(myArmy[t].Monster())));
		    cursor.SetThemes(cursor.WAR_INFO);
		    if(click) {
			cursor.SetThemes(cursor.POINTER); 
			Dialog::ArmyInfo(myArmy[t], false, false, false, true);
		    }
		    if(le.MouseRight()) {
			Dialog::ArmyInfo(myArmy[t], false, true, false, true);
		    }
		} else if(t = FindTroop(enemyArmy, cur_pt), t >= 0 && enemyArmy[t].Count() > 0) {
		    // enemy troop
		    int mp;
		    if(myTroop.shots > 0 && !close) {
                        std::string str = tr("battle.shoot_monster").sub(Monster::String(enemyArmy[t].Monster())).sub(myTroop.shots);
			statusBar2->ShowMessage(str);
			cursor.SetThemes(cursor.WAR_ARROW);
			if(click) {
			    attack = cur_pt;
			    return WIN;
			} else if(le.MouseRight()) {
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
			}
		    } else if(mp = CanAttack(myTroop, movePoints, enemyArmy[t], le.MouseCursor()-(Bf2Scr(cur_pt)+dst_pt)), mp >= 0) {
			std::string str = tr("battle.attack_monster").sub(Monster::String(enemyArmy[t].Monster()));
			statusBar2->ShowMessage(str);
			Point p1 = Bf2Scr(movePoints[mp]), p2 = Bf2Scr(cur_pt);
			if(p1.x > p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPRIGHT);
			else if(p1.x < p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPLEFT);
			else if(p1.x > p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMRIGHT);
			else if(p1.x < p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMLEFT);
			else if(p1.x > p2.x) cursor.SetThemes(cursor.SWORD_RIGHT);
			else if(p1.x < p2.x) cursor.SetThemes(cursor.SWORD_LEFT);
			//cursor.SetThemes(cursor.SWORD_TOPLEFT); // TODO
			if(click) {
			    move = movePoints[mp];
			    attack = cur_pt;
			    return WIN;
			} else if(le.MouseRight())
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
		    } else {
			// attack
			statusBar2->ShowMessage(tr("battle.view_monster").sub(Monster::String(enemyArmy[t].Monster())));
			cursor.SetThemes(cursor.WAR_INFO);
			if(click) {
			    cursor.SetThemes(cursor.POINTER);
			    Dialog::ArmyInfo(enemyArmy[t], false, false, false, true);
			} else if(le.MouseRight()) {
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
			}
		    }
		} else {
		    // empty cell
		    bool canmove = false;
		    for(u16 i=0; i<movePoints.size(); i++) if(movePoints[i] == cur_pt) {
			canmove = true; 
			break;
		    }
		    if(canmove) {
			std::string str;
			if(myMonster.fly) cursor.SetThemes(cursor.WAR_FLIGHT), str = tr("battle.fly");
			else cursor.SetThemes(cursor.WAR_MOVE), str = tr("battle.move");
			statusBar2->ShowMessage(tr("battle.move/fly_here").sub(str).sub(Monster::String(myTroop.Monster())));
			if(click) {
			    move = cur_pt;
			    return WIN;
			}
		    } else {
			statusBar2->ShowMessage(NOL10N(" "));
			cursor.SetThemes(cursor.WAR_NONE);
		    }
		}
	    } else {
		statusBar2->ShowMessage(NOL10N(" "));
		cursor.SetThemes(cursor.WAR_NONE);
	    }
	}
    }
    return WIN;
}

bool Army::DangerousUnitPredicate(Army::BattleTroop first, Army::BattleTroop second)
{
    return  (first.Count() * Monster::GetStats(first.Monster()).damageMax) >
            (second.Count() * Monster::GetStats(second.Monster()).damageMax);
}

/** Attempt to attack a unit in the given list
 *  \param[in]  list  List of units to consider
 *  \param[in]  army1   Left army
 *  \param[in]  army2   Right army
 *  \param[in]  troopN  Index into either \i army1 or \i army2.
 *  \param[out] move    Movement destination
 *  \param[out] attack  Point at which to attack
 *  \return True if an attack succeeded, false otherwise
 */
bool Army::AttackTroopInList(const Army::BattleArmy_t &list, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack)
{
    u16 idx;
    for(idx = 0; idx < list.size(); idx++)
    {
        Point attackFrom = GetReachableAttackCell(list[idx], army1, army2, troopN);
        if(BfValid(attackFrom))
        {
            attack = list[idx].Position();
            move = attackFrom;
            return true;
        }
    }
    return false;
}

struct DistancePredicate : public std::binary_function<Army::BattleTroop, Army::BattleTroop, bool>
{
    const Point &orig;
    DistancePredicate(const Point &init) : orig(init) {}
    
    bool operator()(const Army::BattleTroop &first, const Army::BattleTroop &second) const
    {
        return orig.distance(first.Position()) < orig.distance(second.Position());
    }
};

/** Move to the closest point to the closest unit in the list
 *  \param[in] list   List of units to consider
 *  \param[in] myTroop  Unit to move
 *  \param[out] move    Destination point
 */
void Army::MoveToClosestTroopInList(const Army::BattleArmy_t &list, const Army::BattleTroop &myTroop, Point &move)
{
    Army::BattleArmy_t tempTroop = list;
    sort(tempTroop.begin(), tempTroop.end(), DistancePredicate(myTroop.Position()));
    
    Point target = tempTroop.front().Position();
    Point closest = movePoints[0];
    for(u16 idx = 0; idx < movePoints.size(); idx++)
    {
        if(target.distance(movePoints[idx])
         < target.distance(closest))
            closest = movePoints[idx];
    }
    move = closest;
}

/** Find a valid move and/or attack to make
 *  \param[in] myMonster   Attacker monster
 *  \param[in] army1       Left army
 *  \param[in] army2       Right army
 *  \param[in] troopN      Index into \i army1 or \i army2
 *  \param[out] move       #Point to which to move
 *  \param[out] attack     #Point which to attack
 *  \param[in] rangedFirst Try to attack ranged units first
 */
void Army::MoveOrAttack(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack, bool rangedFirst)
{
    bool result;
    if(rangedFirst)
        result = AttackRangedTroop(myMonster, army1, army2, troopN, move, attack);
    else
        result = AttackNonRangedTroop(myMonster, army1, army2, troopN, move, attack);
    if(!result)
    {
        if(rangedFirst)
            result = AttackNonRangedTroop(myMonster, army1, army2, troopN, move, attack);
        else
            result = AttackRangedTroop(myMonster, army1, army2, troopN, move, attack);
    }
    if(!result)
    {
        const Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN - 1];
        const Army::BattleArmy_t &enemyArmy = troopN >= 0 ? army2 : army1;
        BattleArmy_t realArmy;
        for(u16 i = 0; i < enemyArmy.size(); i++)
            if(enemyArmy[i].isValid())
                realArmy.push_back(enemyArmy[i]);
        MoveToClosestTroopInList(realArmy, myTroop, move);
    }
}

/** Find a ranged enemy to attack, and set up the destination action and movement points
 *  \param[in] myMonster  Attacker monster
 *  \param[in] army1      Left army
 *  \param[in] army2      Right army
 *  \param[in] troopN     Index into \i army1 or \i army2
 *  \param[out] move      #Point to which to move
 *  \param[out] attack    #Point which to attack
 *  \return Whether a possible move or attack was found
 */
bool Army::AttackRangedTroop(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack)
{
    const Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN - 1];
    const Army::BattleArmy_t &enemyArmy = troopN >= 0 ? army2 : army1;
    Army::BattleArmy_t ranged;
    for(u16 i = 0; i < enemyArmy.size(); i++)
    {
        Monster::stats_t enemy = Monster::GetStats(enemyArmy[i].Monster());
        if(enemyArmy[i].Count() && enemy.shots)
            ranged.push_back(enemyArmy[i]);
    }
    if(ranged.size())
    {
        sort(ranged.begin(), ranged.end(), DangerousUnitPredicate);
        if(myTroop.shots)
        {
            attack = ranged.front().Position();
            return true;
        }
        else return AttackTroopInList(ranged, army1, army2, troopN, move, attack);
    }
    return false;
}

/** Find a non-ranged enemy to attack, and set up the destination action and movement points
 *  \param[in] myMonster  Attacker monster
 *  \param[in] army1      Left army
 *  \param[in] army2      Right army
 *  \param[in] troopN     Index into \i army1 or \i army2
 *  \param[out] move      #Point to which to move
 *  \param[out] attack    #Point which to attack
 *  \return Whether a possible move or attack was found
 */
bool Army::AttackNonRangedTroop(const Monster::stats_t &myMonster, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, int troopN, Point &move, Point &attack)
{
    const Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN - 1];
    std::vector<Army::BattleTroop> &enemyArmy = troopN >= 0 ? army2 : army1;
    std::vector<Army::BattleTroop> nonranged;
    for(u16 i = 0; i < enemyArmy.size(); i++)
    {
        Monster::stats_t enemy = Monster::GetStats(enemyArmy[i].Monster());
        if(enemyArmy[i].Count() && !enemy.shots)
            nonranged.push_back(enemyArmy[i]);
    }
    if(nonranged.size())
    {
        sort(nonranged.begin(), nonranged.end(), DangerousUnitPredicate);
        if(myTroop.shots)
        {
            attack = nonranged.front().Position();
            return true;
        }
        else return AttackTroopInList(nonranged, army1, army2, troopN, move, attack);
    }
    return false;
}

Army::battle_t Army::CompTurn(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack)
{
    std::vector<Army::BattleTroop> &myArmy = troopN >= 0 ? army1 : army2;
    std::vector<Army::BattleTroop> &enemyArmy = troopN >= 0 ? army2 : army1;
    move.x = move.y = attack.x = attack.y = -1;
    
    //Sanity check
    if(!ArmyExists(myArmy) || !ArmyExists(enemyArmy))
        return WIN;
    
    Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    PrepMovePoints(troopN, army1, army2);
    bool close = false;
    for(u16 j=0; j<enemyArmy.size(); j++) {
	Point tmpoint(0,0);
	std::vector<Point> tmpoints;
	tmpoints.push_back(enemyArmy[j].Position());
	if(CanAttack(enemyArmy[j], tmpoints, myTroop, tmpoint) >= 0) {
	    close = true;
	    break;
	}
    }
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    AttackStatus(NOL10N(""));

    DrawBackground(tile);
    Rect rectHero1, rectHero2;
    if(hero1) rectHero1 = DrawHero(*hero1, 1);
    if(hero2) rectHero2 = DrawHero(*hero2, 1, true);
    DrawArmy(army1, army2);

    Button buttonSkip(dst_pt.x + 640-48, dst_pt.y + 480-36, ICN::TEXTBAR, 0, 1);
    Button buttonAuto(dst_pt.x, dst_pt.y + 480-36, ICN::TEXTBAR, 4, 5);
    Button buttonSettings(dst_pt.x, dst_pt.y + 480-18, ICN::TEXTBAR, 6, 7);

    buttonSkip.Draw();
    buttonAuto.Draw();
    buttonSettings.Draw();

    display.Flip();
    
    //int i = 0;
    //u16 animat = 0;
    Point cur_pt(-1,-1);
    //Spell::spell_t spell = Spell::NONE;
    
    //TODO: Magic
    //TODO: Retreat/surrender
    
    if(myMonster.fly || (myMonster.shots && !close))
        MoveOrAttack(myMonster, army1, army2, troopN, move, attack, true);
    else if(myMonster.shots && close)
    {
        std::vector<Army::BattleTroop> closeEnemies;
        for(u16 j=0; j<enemyArmy.size(); j++)
        {
            Point tmpoint(0,0);
            std::vector<Point> tmpoints;
            tmpoints.push_back(enemyArmy[j].Position());
            if(CanAttack(enemyArmy[j], tmpoints, myTroop, tmpoint) >= 0)
                closeEnemies.push_back(enemyArmy[j]);
        }
        sort(closeEnemies.begin(), closeEnemies.end(), DangerousUnitPredicate);
        attack = closeEnemies.front().Position();
        move = myTroop.Position();
    }
    else
        MoveOrAttack(myMonster, army1, army2, troopN, move, attack, false);
    
    return WIN;
}

bool Army::IsDamageFatal(int damage, Army::BattleTroop &troop)
{
    return damage >= troop.TotalHP();
}

bool Army::AnimateMove(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &move)
{
    Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    u16 animat = 0;
    
    if(BfValid(move) && move != myTroop.Position()) {
	Point start = Bf2Scr(myTroop.Position()) + dst_pt;
	Point end = Bf2Scr(move) + dst_pt;
	Point tp = start;
	Point step;
	u8 st, len, prep=0, post=0;
	Monster::GetAnimFrames(myTroop.Monster(), Monster::AS_WALK, st, len);
	int frame = 0, curstep = 0, part = 0, numsteps;
	std::vector<Point> *path=0;
        
        /*DrawShadow(move);
        Display::Get().Flip();
        while(le.HandleEvents())
            if(le.KeyPress(KEY_RETURN))
                break;*/
        
	if(myMonster.fly) {
            myTroop.SetReflect(end.x < start.x);
	    if(len >= 12) prep = post = 4;
	    else if(len >= 8) prep = post = 2;
	    else prep = post = 1;
	    step = end-start;
            const int deltaX = abs(myTroop.Position().x - move.x);
            const int deltaY = abs(myTroop.Position().y - move.y);
            numsteps = std::max(std::max(deltaX, deltaY), 1); //avoid divide by zero
	    step.x /= numsteps;
	    step.y /= numsteps;
	} else {
	    if(path = FindPath(myTroop.Position(), move, Speed::Move(myMonster.speed), myTroop, army1, army2, troopN),!path) {
		Dialog::Message(tr("battle.error"), tr("battle.no_path"), Font::BIG, Dialog::OK);
		return false;
	    }
	    step.x = ((Bf2Scr(path->back())+dst_pt).x - tp.x) / len;
	    step.y = ((Bf2Scr(path->back())+dst_pt).y - tp.y) / len;
            myTroop.SetReflect(step.x < 0);
	    if(step.y) step.x = -step.x;
	    else step.x = 0;
            
            /*DrawTroop(myTroop);
            Display::Get().Flip();
            
            while(le.HandleEvents())
            {
                if(le.KeyPress(KEY_RETURN))
                    break;
            }*/
	}
	myTroop.Animate(Monster::AS_WALK);
        if(!myMonster.fly)
            AGG::PlaySound(myMonster.m82_move);
	while(le.HandleEvents()) {
            bool shouldAnimate = false;
            if(myMonster.fly)
                shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 4);
            else
                shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 2);
	    if(shouldAnimate) {
		if(myMonster.fly) {
		    if(frame >= prep && !part) {
			AGG::PlaySound(myMonster.m82_move);
			part = 1;
		    }
		    if(part == 1) {
			tp += step;
			curstep ++;
			if(myTroop.aframe >= st+len-post-1) {
			    AGG::PlaySound(myMonster.m82_move);
			    myTroop.aframe = st+prep;
			}
			if(curstep == numsteps) {
			    part = 3;
			    tp = end;
			    myTroop.aframe = st+len-post;
			}
		    }
		    if(part == 3 && myTroop.astate != Monster::AS_WALK) break;
		} else {
		    tp += step;
		    if(myTroop.astate != Monster::AS_WALK && path->size()) {
                        if(path->size() > 1)
                            AGG::PlaySound(myMonster.m82_move);
			myTroop.SetPosition(path->back());
			tp = Bf2Scr(myTroop.Position()) + dst_pt;
			path->pop_back();
			if(!path->size()) break;
			step.x = ((Bf2Scr(path->back())+dst_pt).x - tp.x) / len;
			step.y = ((Bf2Scr(path->back())+dst_pt).y - tp.y) / len;
                        myTroop.SetReflect(step.x < 0);
			if(step.y) step.x = -step.x;
			else step.x = 0;
			myTroop.Animate(Monster::AS_WALK);
                        
                        /*DrawTroop(myTroop);
                        Display::Get().Flip();
                        
                        while(le.HandleEvents())
                        {
                            if(le.KeyPress(KEY_RETURN))
                                break;
                        }*/
		    }
		}
		DrawBackground(tile);
		if(hero1) DrawHero(*hero1, 1, false, 1);
		if(hero2) DrawHero(*hero2, 1, true);
                /*if(path)
                    DrawPath(*path);*/
		Point p;
		int t=-1;
		for(p.y = 0; p.y < BFH; p.y++)
		    for(p.x = 0; p.x < BFW; p.x++) {
			DrawObject(p);
			if(t = FindTroopExact(army1, p), t >= 0) {
			    if(&army1[t] == &myTroop) {
                                myTroop.Blit(tp, myTroop.IsReflected());
                                myTroop.Animate();
			    } else {
				DrawTroop(army1[t]);
				army1[t].Animate();
			    }
			}
			if(t = FindTroopExact(army2, p), t >= 0) {
			    if(&army2[t] == &myTroop) {
				myTroop.Blit(tp, myTroop.IsReflected());
				myTroop.Animate();
			    } else {
				DrawTroop(army2[t]);
				army2[t].Animate();
			    }
			}
		    }
		display.Flip();
		frame ++;
	    }
	}
	myTroop.SetPosition(move);
	if(path) delete path;
    }
    
    return true;
}

/** Popular a list with the targets for the given unit's attack at a given point
 *  \param[out] targets List of troop which will be affected
 *  \param[in] attacker Troop that is attacking
 *  \param[in] attack   Point which is being attacked
 *  \param[in] army1    Left army
 *  \param[in] army2    Right army
 */
void Army::GetTargets(std::vector<Army::BattleTroop *> &targets, const Army::BattleTroop &attacker, const Point &attack, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2)
{
    int t;
    Army::BattleTroop &mainTarget = (t = FindTroop(army1, attack), t >= 0 ? army1[t] : army2[FindTroop(army2, attack)]);
    bool reflected = mainTarget.Position().x <= attacker.Position().x;
    targets.push_back(&mainTarget);
    switch(attacker.Monster())
    {
        case Monster::GREEN_DRAGON:
        case Monster::RED_DRAGON:
        case Monster::BLACK_DRAGON:
        case Monster::PHOENIX:
        case Monster::CYCLOPS:
        {
            bool wide = Monster::GetStats(attacker.Monster()).wide;
            Point base = attacker.Position();
            int xIncr = reflected ? 0 : 1;
            if(wide)
                base.x += xIncr;
            Point incr = attack - base;
            Point nextcell;
            if(!incr.y) //Same y-plane
                nextcell = attack + incr;
            else
                nextcell = attack + Point( 0, incr.y);
            //printf("incr: %d, %d\n", incr.x, incr.y);
            int t;
            if((t = FindTroop(army1, nextcell)) >= 0
            && &army1[t] != &attacker
            && &army1[t] != &mainTarget)
                targets.push_back(&army1[t]);
            else if((t = FindTroop(army2, nextcell)) >= 0
            && &army2[t] != &attacker
            && &army2[t] != &mainTarget)
                targets.push_back(&army2[t]);
            break;
        }
        
        case Monster::HYDRA:
        case Monster::LICH:
        case Monster::POWER_LICH:
        {
            Point base;
            if(attacker.Monster() == Monster::HYDRA)
            {
                base = attacker.Position();
                base.x += attacker.IsReflected() ? -1 : 1;
            }
            else base = attack;
        
            for(int x = -1; x <= 1; x++)
                for(int y = -1; y <= 1; y++)
                {
                    if(x == -1 && y) continue;
                    if(!x && !y) continue;
                    int t;
                    if((t = FindTroop(army1, base + Point(x, y))) >= 0
                    && &army1[t] != &attacker
                    && &army1[t] != &mainTarget)
                    {
                        targets.push_back(&army1[t]);
                    }
                    else
                    if((t = FindTroop(army2, base + Point(x, y))) >= 0
                    && &army2[t] != &attacker
                    && &army2[t] != &mainTarget)
                    {
                        targets.push_back(&army2[t]);
                    }
                }
            break;
        }
        
        default:
            break;
    }
}

bool Army::AnimateAttack(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &attack)
{
    Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    u16 animat = 0;
    bool retaliate = false;
    
    if(BfValid(attack))
    {
        std::vector<Army::BattleTroop *> targets;
        GetTargets(targets, myTroop, attack, army1, army2);
        
        Monster::monster_t targetMonster = targets[0]->Monster();
        
        bool closeAttack = false;
        {
            Point tmp(0,0);
            std::vector<Point> tmpmoves;
            tmpmoves.push_back(targets[0]->Position());
            if(CanAttack(*targets[0], tmpmoves, myTroop, tmp) >= 0)
            {
                closeAttack = true;
                if(CanRetaliate(myTroop, *targets[0]))
                {
                    retaliate = true;
                    targets[0]->SetRetaliated(true);
                }
            }
        }
        
	int sk_a = myMonster.attack + (myTroop.MasterSkill() ? myTroop.MasterSkill()->GetAttack() : 0);
	long damage = 0;
	switch(CheckLuck(troopN >= 0 ? hero1 : hero2, myTroop)) {
	case -1:
	    damage = myMonster.damageMin*myTroop.Count();
	    break;
	case 0:
	    damage = Rand::Get(myMonster.damageMin*myTroop.Count(), myMonster.damageMax*myTroop.Count());
	    break;
	case 1:
	    damage = myMonster.damageMax*myTroop.Count();
	    break;
	}
        
        bool ranged = myMonster.miss_icn != ICN::UNKNOWN && myTroop.shots > 0 && !closeAttack;
        int origDamage = damage;
        int perished;
        
        u8 targetLen = 0;
        std::vector<std::pair<Monster::animstate_t, M82::m82_t> > targetActions;
        
        for(u16 tid = 0; tid < targets.size(); tid++)
        {
            Army::BattleTroop &target = *targets[tid];
            damage = origDamage;
            
            int sk_d = Monster::GetStats(target.Monster()).defence + (target.MasterSkill() ? target.MasterSkill()->GetDefense() : 0);
            
            //Racial/unit modifiers apply here
            damage = AdjustDamage(myTroop.Monster(), target.Monster(), damage, ranged);
            
            // TODO bless and curse
            //damage *= sk_a;
            //damage /= sk_d;
            float d = sk_a-sk_d;
            d *= d>0 ? 0.1f : 0.05f;
            damage = (long)(((float)damage) * (1+d));
            
            //TODO: Don't gain experience from damage to own troop
            if(troopN >= 0) EXP1 += std::min(damage, (long)target.TotalHP());
            else EXP2 += std::min(damage, (long)target.TotalHP());
            
            Monster::animstate_t targetstate;
            M82::m82_t targetsound;
            if(IsDamageFatal(damage, target)) {
                targetstate = Monster::AS_DIE;
                targetsound = Monster::GetStats(target.Monster()).m82_kill;
            } else {
                targetstate = Monster::AS_PAIN;
                targetsound = Monster::GetStats(target.Monster()).m82_wnce;
            }
            targetActions.push_back(std::make_pair(targetstate, targetsound));
            
            u8 start, currentTargetLen;
            Monster::GetAnimFrames(target.Monster(), targetstate, start, currentTargetLen);
            targetLen = std::max(currentTargetLen, targetLen);
            
            int tempPerished = target.ApplyDamage(damage);
            if(!tid) //Only the original target matters
            {
                perished = tempPerished;
                if(!target.Count())
                    retaliate = false;
            }
        }
        
	int state = Army::PREATTACK, missframe = 0, missindex = 0;
	Point miss_start(myTroop.Position().x + (myMonster.wide ? (myTroop.IsReflected() ? -1 : 1): 0), myTroop.Position().y);
	miss_start = Bf2Scr(miss_start) + dst_pt;
	Point miss_step(Bf2Scr(targets[0]->Position()) + dst_pt);
	miss_step -= miss_start;
        const int deltaX = abs(myTroop.Position().x - targets[0]->Position().x);
        const int deltaY = abs(myTroop.Position().y - targets[0]->Position().y);
        const int MISSFRAMES = std::max(std::max(deltaX, deltaY) * 2 / 3, 1); //avoid divide by zero
	miss_step.x /= MISSFRAMES;
	miss_step.y /= MISSFRAMES;
	miss_start.y -= CELLH;
        
	Monster::animstate_t mytrooptate;
	if(targets[0]->Position().y > myTroop.Position().y)
	    mytrooptate = Monster::AS_ATT3P;
	else if(targets[0]->Position().y < myTroop.Position().y)
	    mytrooptate = Monster::AS_ATT1P;
	else
	    mytrooptate = Monster::AS_ATT2P;
        
        myTroop.SetReflect(myTroop.Position().x > targets[0]->Position().x);
        
        int delayFrames;
         
        u8 start, prepLen, attackLen;
        Monster::GetAnimFrames(myTroop.Monster(), (Monster::animstate_t)(mytrooptate & ~Monster::AS_ATTPREP), start, attackLen);
        Monster::GetAnimFrames(myTroop.Monster(), Monster::AS_ATTPREP, start, prepLen);
        myTroop.attackRanged = ranged;
         
	if(!ranged) {
	    AGG::PlaySound(myMonster.m82_attk);
	    delayFrames = prepLen + attackLen / 3;
	} else {
            AGG::PlaySound(myMonster.m82_shot);
            delayFrames = prepLen + attackLen + 3;
	    int maxind = AGG::GetICNCount(myMonster.miss_icn);
	    if(maxind > 1) {
		double angle = M_PI_2 - atan2(-miss_step.y, miss_step.x);
		missindex = (int)(angle/M_PI * maxind);
	    } else missindex = 0;
	}
	myTroop.Animate(mytrooptate);
	while(le.HandleEvents())
        {
            bool shouldAnimate = false;
            if(ranged && state == 1)
                shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 2);
            else
                shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 4);
	    if(shouldAnimate)
            {
		if(!ranged)
                {
                    //Time to start animating the targets?
                    if(delayFrames == 0) 
                    {
                        for(u16 tid = 0; tid < targets.size(); tid++)
                        {
                            targets[tid]->Animate(targetActions[tid].first);
                            AGG::PlaySound(targetActions[tid].second);
                        }
                    }
                    else if(delayFrames < 0)
                    {
                        //Are we finished animating all of the targets?
                        if(delayFrames < -targetLen)
                            break;
                    }
                    delayFrames --;
		}
                else
                {
		    if(state == Army::PREATTACK && !delayFrames--) 
			state ++;
		    else if(state == Army::MISSILE_FLIGHT && missframe >= MISSFRAMES)
                    {
                        for(u16 tid = 0; tid < targets.size(); tid++)
                        {
                            targets[tid]->Animate(targetActions[tid].first);
                            AGG::PlaySound(targetActions[tid].second);
                        }
                        delayFrames = targetLen;
                        state++;
		    }
                    else if(state == Army::MISSILE_IMPACT && !delayFrames--) break;
		}
		DrawBackground(tile);
		if(hero1) DrawHero(*hero1, 1, false, 1);
		if(hero2) DrawHero(*hero2, 1, true);
		Point p;
		int t=-1;
		for(p.y = 0; p.y < BFH; p.y++)
		    for(p.x = 0; p.x < BFW; p.x++) {
			DrawObject(p);
			if(t = FindTroopExact(army1, p), t >= 0) {
                            DrawTroop(army1[t]);
			    army1[t].Animate();
			}
			if(t = FindTroopExact(army2, p), t >= 0) {
			    DrawTroop(army2[t]);
			    army2[t].Animate();
			}
		    }
		if(state == Army::MISSILE_FLIGHT) {
		    display.Blit(AGG::GetICN(myMonster.miss_icn, abs(missindex), missindex < 0), miss_start);
		    miss_start += miss_step;
		    missframe ++;
		}
		display.Flip();
	    }
	}
        for(u16 tid = 0; tid < targets.size(); tid++)
        {
            targets[tid]->ResetReflection();
            DrawTroop(*targets[tid]);
        }
        
	std::string status;
        if(myTroop.Count() > 1)
            status = tr("battle.multiple_attack").sub(Monster::MultipleNames(myTroop.Monster())).sub(damage);
        else
            status = tr("battle.single_attack").sub(Monster::String(myTroop.Monster())).sub(damage);
        
        if(perished)
        {
            status += NOL10N("  ");
            if(perished > 1)
                status += tr("battle.multiple_perish").sub(perished).sub(Monster::MultipleNames(targetMonster));
            else status += tr("battle.single_perish").sub(perished).sub(Monster::String(targetMonster));
        }
        AttackStatus(status);
        
	if(ranged) myTroop.shots --;
	if(!ranged) {
	    std::vector<Army::BattleTroop*> ta1, ta2;
	    (troopN >= 0 ? ta1 : ta2).push_back(targets[0]);
	    MagicCycle(hero1, hero2, ta1, ta2, army1, army2, troopN<0, Spell::TroopAttack(myTroop.Monster()), tile);
	}
    }
    
    return retaliate;
}

bool Army::AnimateCycle(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, int troopN, const Point &move, const Point &attack)
{
    cursor.Hide();
    Army::BattleTroop &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    
    bool retaliate;
    
    retaliate = AnimateMove(hero1, hero2, army1, army2, tile, troopN, move);
    if(retaliate)
        retaliate = AnimateAttack(hero1, hero2, army1, army2, tile, troopN, attack);
    
    myTroop.ResetReflection();
    DrawTroop(myTroop);
    return retaliate;
}

bool Army::MagicSelectTarget(Heroes *hero1, Heroes *hero2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, const Maps::Tiles &tile, bool reflect, Spell::spell_t spell)
{
    std::vector<Army::BattleTroop*> ta1, ta2;
    Army::BattleArmy_t &myarmy = reflect ? army2 : army1;
    Army::BattleArmy_t &enarmy = reflect ? army1 : army2;
    switch(Spell::Target(spell)) {
    case Spell::ONEFRIEND:
    case Spell::ONEENEMY:
    case Spell::FREECELL:
	break;
    case Spell::ALLFRIEND:
	for(u16 i=0; i<myarmy.size(); i++) {
	    if(Spell::AllowSpell(spell, myarmy[i]))
		(reflect?ta2:ta1).push_back(&myarmy[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::ALLENEMY:
	for(u16 i=0; i<enarmy.size(); i++) {
	    if(Spell::AllowSpell(spell, enarmy[i]))
		(reflect?ta1:ta2).push_back(&enarmy[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::ALLLIVE:
    case Spell::ALLDEAD:
    case Spell::ALL:
	for(u16 i=0; i<army1.size(); i++) {
	    if(Spell::AllowSpell(spell, army1[i]))
		ta1.push_back(&army1[i]);
	}
	for(u16 i=0; i<army2.size(); i++) {
	    if(Spell::AllowSpell(spell, army2[i]))
		ta1.push_back(&army2[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::NOTARGET:
	return false;
    }
    // select target
    while(le.HandleEvents()) {
	Point cur_pt = Scr2Bf(le.MouseCursor()-dst_pt);
	int t;
	if(Spell::Target(spell) == Spell::ONEFRIEND) {
	    if(t = FindTroop(myarmy, cur_pt), t >= 0 && Spell::AllowSpell(spell, myarmy[t])) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		if(le.MouseClickLeft(Rect(0, 0, display.w(), display.h()))) {
		    (reflect?ta2:ta1).push_back(&myarmy[t]);
		    break;
		}
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else if(Spell::Target(spell) == Spell::ONEENEMY) {
	    if(t = FindTroop(enarmy, cur_pt), t >= 0) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		if(le.MouseClickLeft(Rect(0, 0, display.w(), display.h()))) {
		    (reflect?ta1:ta2).push_back(&enarmy[t]);
		    break;
		}
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else if(Spell::Target(spell) == Spell::FREECELL) {
	    if(CellFree(cur_pt, army1, army2)) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		// TODO COLDRING & SUMMON
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else {
	    cursor.SetThemes(cursor.SPELLNONE);
	}
	// exit
	if(le.KeyPress(KEY_ESCAPE)) return false;
	if(le.MousePressRight(Rect(0, 0, display.w(), display.h()))) return false;
    }    
    return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
}

void Army::MagicPreCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::BattleTroop*> &aff1, std::vector<Army::BattleTroop*> &aff2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile)
{
    Background back(Rect(dst_pt.x, dst_pt.y, 640, 480));
    back.Save();
    ICN::icn_t icn = ICN::UNKNOWN;
    int maxframe = 0, icnframe=-1;
    switch(spell) {
    case Spell::COLDRAY:
    case Spell::DISRUPTINGRAY:
    case Spell::ARROW:
	if(aff1.size() + aff2.size() == 1) {
	    Point start, end;
	    if(aff1.size() && hero2) {
		Rect hrect = DrawHero(*hero2, 1, true);
		start = hrect + Point(hrect.w, hrect.h/2);
		end = Bf2Scr(aff1[0]->Position())+dst_pt;
	    } else if(aff2.size() && hero1) {
		Rect hrect = DrawHero(*hero1, 1);
		start = hrect + Point(0, hrect.h/2);
		end = Bf2Scr(aff2[0]->Position())+dst_pt;
	    } else break;
	    int animat = 0;
	    int frame = 0;
	    Point delta = end - start;
	    switch(spell) {
	    case Spell::COLDRAY:
		icn = ICN::COLDRAY;
		maxframe = AGG::GetICNCount(icn);
		break;
	    case Spell::DISRUPTINGRAY:
		icn = ICN::DISRRAY;
		maxframe = AGG::GetICNCount(icn);
		break;
	    case Spell::ARROW: {
		icn = ICN::KEEP;
		maxframe = 16;
		int maxind = AGG::GetICNCount(icn);
		if(maxind > 1) {
		    double angle = M_PI_2 - atan2(-delta.y, delta.x);
		    icnframe = (int)(angle/M_PI * maxind);
		} else icnframe = 0;
		break;
	    }
	    default:
		break;
	    }
	    delta.x /= maxframe;
	    delta.y /= maxframe;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(animat++, 3)) {
		    display.Blit(AGG::GetICN(icn, icnframe>=0? icnframe : frame, reflect), start);
		    display.Flip();
		    start += delta;
		    frame ++;
		    if(frame >= maxframe) break;
		}
	    }
	}
	break;
    case Spell::LIGHTNINGBOLT:
    case Spell::CHAINLIGHTNING:
    case Spell::HOLYWORD:
    case Spell::HOLYSHOUT:
    case Spell::ARMAGEDDON:
    case Spell::ELEMENTALSTORM:
    case Spell::METEORSHOWER:
    case Spell::DEATHRIPPLE:
    case Spell::DEATHWAVE:
    case Spell::EARTHQUAKE:
	// TODO
    default:
	break;
    }
}

bool Army::MagicCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::BattleTroop*> &aff1, std::vector<Army::BattleTroop*> &aff2, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile)
{
    if(spell == Spell::NONE) return false;
    if(!aff1.size() && !aff2.size()) {
	Dialog::Message(NOL10N(""), tr("battle.affect_noone"), Font::BIG, Dialog::OK);
	return false;
    }
    MagicPreCycle(hero1, hero2, aff1, aff2, army1, army2, reflect, spell, tile);
    AGG::PlaySound(Spell::M82(spell));
    int frame = 0, mframe = 0;
    if(Spell::Icn(spell) != ICN::UNKNOWN) mframe = AGG::GetICNCount(Spell::Icn(spell));
    int damage = 0;
    int spellpower = (reflect?hero2:hero1) ? (reflect?hero2:hero1)->GetPower() : 1 ;
    if(Spell::Target(spell) != Spell::ONEFRIEND && Spell::Target(spell) != Spell::ALLFRIEND) {
	if(Spell::Power(spell) > 1) {
	    damage = Spell::Power(spell) * spellpower;
	}
    }
    if(damage) {
	for(u16 i=0; i<aff1.size(); i++) {
	    if(damage < (aff1[i]->Count()-1) * Monster::GetStats(aff1[i]->Monster()).hp + aff1[i]->hp)
		aff1[i]->Animate(Monster::AS_PAIN);
	    else
		aff1[i]->Animate(Monster::AS_DIE);
	}
	for(u16 i=0; i<aff2.size(); i++) {
	    if(damage < (aff2[i]->Count()-1) * Monster::GetStats(aff2[i]->Monster()).hp + aff2[i]->hp)
		aff2[i]->Animate(Monster::AS_PAIN);
	    else
		aff2[i]->Animate(Monster::AS_DIE);
	}
    }
    int animat = 0;
    while(le.HandleEvents()) {
	if(Game::ShouldAnimateInfrequent(animat++, 3)) {
	    // paint
	    DrawBackground(tile);
	    if(hero1) DrawHero(*hero1, 1, false, 1);
	    if(hero2) DrawHero(*hero2, 1, true);
	    Point p;
	    int t=-1;
	    for(p.y = 0; p.y < BFH; p.y++)
		for(p.x = 0; p.x < BFW; p.x++) {
		    DrawObject(p);
		    if(t = FindTroopExact(army1, p), t >= 0) {
			DrawTroop(army1[t]);
			army1[t].Animate();
		    }
		    if(t = FindTroopExact(army2, p), t >= 0) {
			DrawTroop(army2[t]);
			army2[t].Animate();
		    }
		}
	    const Sprite& spr = AGG::GetICN(Spell::Icn(spell), frame);
	    for(u16 i=0; i<aff1.size(); i++) display.Blit(spr, Bf2Scr(aff1[i]->Position())+dst_pt+spr);
	    for(u16 i=0; i<aff2.size(); i++) display.Blit(spr, Bf2Scr(aff2[i]->Position())+dst_pt+spr);
	    display.Flip();
	    if(frame >= mframe) {
		if(damage) {
		    bool br = true;
		    for(u16 i=0; i<aff1.size(); i++) if(aff1[i]->astate != Monster::AS_NONE) {
			u8 strt=0, len=0;
			Monster::GetAnimFrames(aff1[i]->Monster(), Monster::AS_DIE, strt, len, false);
			if(aff1[i]->astate != Monster::AS_DIE || aff1[i]->aframe < strt+len-1)
			    br = false;
		    }
		    for(u16 i=0; i<aff2.size(); i++) if(aff2[i]->astate != Monster::AS_NONE) {
			u8 strt=0, len=0;
			Monster::GetAnimFrames(aff2[i]->Monster(), Monster::AS_DIE, strt, len, false);
			if(aff2[i]->astate != Monster::AS_DIE || aff2[i]->aframe < strt+len-1)
			    br = false;
		    }
		    if(br) break;
		} else break;
	    }
	    frame ++;
	}
    }
    for(u16 i=0; i<aff1.size(); i++) {
	if(!reflect || AllowSpell(spell, *aff1[i]))
	    Spell::ApplySpell(spellpower, spell, *aff1[i]);
	else {};// TODO resistance
	if(aff1[i]->Count() == 0) {
	    Army::BattleTroop body = *aff1[i];
	    for(u16 j=0; j<army1.size(); j++) if(&army1[j] == aff1[i])
		army1.erase(army1.begin()+j);
	    bodies1.push_back(body);
	}
    }
    for(u16 i=0; i<aff2.size(); i++) {
	if(reflect || AllowSpell(spell, *aff2[i]))
	    Spell::ApplySpell(spellpower, spell, *aff2[i]);
	else {};// TODO resistance
	if(aff2[i]->Count() == 0) {
	    Army::BattleTroop body = *aff2[i];
	    for(u16 j=0; j<army2.size(); j++) if(&army2[j] == aff2[i])
		army2.erase(army2.begin()+j);
	    bodies2.push_back(body);
	}
    }
    if(damage) {
	std::string str;
        if(aff1.size() + aff2.size() == 1)
            str = tr("battle.spell_damage_single").sub(Spell::String(spell)).sub(damage)
                                                 .sub(Monster::MultipleNames((aff1.size()?aff1[0]:aff2[0])->Monster()));
        else str = tr("battle.spell_damage_multiple").sub(Spell::String(spell)).sub(damage);
	AttackStatus(str);
	AttackStatus(NOL10N(""));
    }
    return true;
}

bool Army::CanRetaliate(const Army::BattleTroop &attacker, const Army::BattleTroop &defender)
{
    const Monster::monster_t defMonster = defender.Monster();
    const Monster::monster_t attMonster = attacker.Monster();
    
    if(defender.HasRetaliated())
        if(defMonster != Monster::GRIFFIN)
            return false;
    
    switch(attMonster)
    {
        case Monster::VAMPIRE:
        case Monster::LORD_VAMPIRE:
        case Monster::SPRITE:
        case Monster::ROGUE:
            return false;
        default:
            break;
    }
    
    return !defender.HasRetaliated();
}

bool Army::CanAttackTwice(Monster::monster_t attacker)
{
    switch(attacker)
    {
        case Monster::WOLF:
        case Monster::PALADIN:
        case Monster::CRUSADER:
        case Monster::ELF:
        case Monster::GRAND_ELF:
        case Monster::RANGER:
            return true;
        default:
            return false;
    }
}

int Army::AdjustDamage(Monster::monster_t attacker, Monster::monster_t defender, int damage, bool ranged)
{
    if(attacker == Monster::CRUSADER
    && Monster::GetRace(defender) == Race::NECR)
        return damage * 2;
    
    else if(!ranged)
    {
        if(Monster::GetStats(attacker).miss_icn != ICN::UNKNOWN)
            switch(attacker)
            {
                case Monster::MAGE:
                case Monster::ARCHMAGE:
                case Monster::TITAN:
                    return damage;
                
                default:
                    return damage / 2;
            }
    }
    
    return damage;
}

void Army::InitBackground(const Maps::Tiles & tile)
{
    blockedCells.clear();
    cobjects.clear();
    bodies1.clear();
    bodies2.clear();
    int obj = Rand::Get(1,4);
    for(int i=0; i < obj; i++) {
	Point p;
	p.x = Rand::Get(2, BFW-4);
	p.y = Rand::Get(0, BFH-1);
	blockedCells.push_back(p);
	CObj cobj;
	if(tile.GetObject() == MP2::OBJN_GRAVEYARD) {
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0000;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0001;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0025;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	}
	else switch(tile.GetGround()) {
	case Maps::Ground::DESERT: 
	    switch(Rand::Get(0,1)) {
	    case 0:
		cobj.icn = ICN::COBJ0009;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0024;
		break;
	    }
	    break;
	case Maps::Ground::SNOW: 
	    switch(Rand::Get(0,1)) {
	    case 0:
		cobj.icn = ICN::COBJ0022;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0026;
		break;
	    }
	    break;
	case Maps::Ground::SWAMP: 
	    switch(Rand::Get(0,3)) {
	    case 0:
		cobj.icn = ICN::COBJ0004;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0006;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0015;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0016;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::WASTELAND: 
	    switch(Rand::Get(0,3)) {
	    case 0:
		cobj.icn = ICN::COBJ0013;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0018;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0020;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0021;
		break;
	    }
	    break;
	case Maps::Ground::BEACH: 
	    switch(Rand::Get(0,0)) {
	    case 0:
		cobj.icn = ICN::COBJ0017;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::LAVA: 
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0029;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0030;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0031;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::GRASS:
	case Maps::Ground::DIRT: 
	    switch(Rand::Get(0,9)) {
	    case 0:
		cobj.icn = ICN::COBJ0002;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0005;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0007;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0011;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 4:
		cobj.icn = ICN::COBJ0012;
		break;
	    case 5:
		cobj.icn = ICN::COBJ0014;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 6:
		cobj.icn = ICN::COBJ0019;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 7:
		cobj.icn = ICN::COBJ0027;
		break;
	    case 8:
		cobj.icn = ICN::COBJ0028;
		break;
	    case 9:
		cobj.icn = ICN::COBJ0008;
		break;
	    }
	    break;
	case Maps::Ground::WATER: 
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0003;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0010;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0023;
		break;
	    }
	    break;
	default:
	    cobj.icn = ICN::COBJ0000;
	    break;
	}
	//const Sprite & sp = AGG::GetICN(cobj.icn, 0, false);
	cobj.bfpoint = p;
	cobj.scrpoint = Bf2Scr(p)+dst_pt;
	//cobj.point.y -= sp.h();
	//cobj.point.x -= CELLH * 0.5f;
	cobjects.push_back(cobj);
    }
}

void Army::DrawBackground(const Maps::Tiles & tile)
{
    ICN::icn_t icn = ICN::UNKNOWN;
    ICN::icn_t frng = ICN::UNKNOWN;
    bool trees = false;
    u16 index = tile.GetIndex();
    u16 x = index%world.w(), y = index/world.w();
    if((x < world.w()-1 && world.GetTiles(index+1).GetObject() == MP2::OBJ_TREES) ||
       (x > 0 && world.GetTiles(index-1).GetObject() == MP2::OBJ_TREES) ||
       (y < world.h()-1 && world.GetTiles(index+world.w()).GetObject() == MP2::OBJ_TREES) ||
       (y > 0 && world.GetTiles(index-world.w()).GetObject() == MP2::OBJ_TREES) )
	trees = true;
    if(tile.GetObject() == MP2::OBJN_GRAVEYARD) {
	icn = ICN::CBKGGRAV;
	frng = ICN::FRNG0001;
    }
    else switch(tile.GetGround()) {
    case Maps::Ground::DESERT: 
	icn = ICN::CBKGDSRT;
	frng = ICN::FRNG0003; 
	break;
    case Maps::Ground::SNOW: 
	icn = trees ? ICN::CBKGSNTR : ICN::CBKGSNMT; 
	frng = trees ? ICN::FRNG0006 : ICN::FRNG0007;
	break;
    case Maps::Ground::SWAMP: 
	icn = ICN::CBKGSWMP; 
	frng = ICN::FRNG0008; 
	break;
    case Maps::Ground::WASTELAND: 
	icn = ICN::CBKGCRCK; 
	frng = ICN::FRNG0002; 
	break;
    case Maps::Ground::BEACH: 
	icn = ICN::CBKGBEAC; 
	frng = ICN::FRNG0004; 
	break;
    case Maps::Ground::LAVA: 
	icn = ICN::CBKGLAVA; 
	frng = ICN::FRNG0005; 
	break;
    case Maps::Ground::DIRT: 
	icn = trees ? ICN::CBKGDITR : ICN::CBKGDIMT; 
	frng = trees ? ICN::FRNG0010 : ICN::FRNG0009;
	break;
    case Maps::Ground::GRASS:
	icn = trees ? ICN::CBKGGRTR : ICN::CBKGGRMT; 
	frng = trees ? ICN::FRNG0011 : ICN::FRNG0012;
	break;
    case Maps::Ground::WATER: 
	icn = ICN::CBKGWATR; 
	frng = ICN::FRNG0013; 
	break;
    default:
	return;
    }
    display.Blit(AGG::GetICN(icn, 0), dst_pt);
    Point pt = dst_pt;
    pt.y += 200;
    display.Blit(AGG::GetICN(frng, 0), pt);
    // draw grid
    if(O_GRID) {
	Point p(0,0);
	for(p.x=0; p.x<BFW; p.x++)
	    for(p.y=0; p.y<BFH; p.y++)
		DrawCell(p);
    }
    for(u16 i=0; i<bodies1.size(); i++)
	DrawTroop(bodies1[i]);
    for(u16 i=0; i<bodies2.size(); i++)
	DrawTroop(bodies2[i]);
//     for(u16 i=0; i<cobjects.size(); i++) {
// 	const Sprite& spr = AGG::GetICN(cobjects[i].icn, 0);
// 	display.Blit(spr, cobjects[i].point.x + spr.x(), cobjects[i].point.y +spr.y());
//     }
}

void Army::DrawPath(const std::vector<Point> &path)
{
    for(u16 i = 0; i < path.size(); i++)
        DrawShadow(path[i]);
}

void Army::DrawObject(const Point &pt)
{
    for(u16 i=0; i<cobjects.size(); i++) if(cobjects[i].bfpoint == pt) {
	const Sprite& spr = AGG::GetICN(cobjects[i].icn, 0);
	display.Blit(spr, cobjects[i].scrpoint.x + spr.x(), cobjects[i].scrpoint.y + spr.y());
	break;
    }
}

void Army::DrawCaptain(const Race::race_t race, u16 animframe, bool reflect)
{
    ICN::icn_t cap;
    switch(race) {
    case Race::BARB: cap = ICN::CMBTCAPB; break;
    case Race::KNGT: cap = ICN::CMBTCAPK; break;
    case Race::NECR: cap = ICN::CMBTCAPN; break;
    case Race::SORC: cap = ICN::CMBTCAPS; break;
    case Race::WRLK: cap = ICN::CMBTCAPW; break;
    case Race::WZRD: cap = ICN::CMBTCAPZ; break;
    default: return;
    }
    display.Blit(AGG::GetICN(cap, animframe, reflect), dst_pt.x + (reflect?550:0), dst_pt.y + 75);
}

Rect Army::DrawHero(const Heroes & hero, u16 animframe, bool reflect, int fframe)
{
    static int flagframe = 0;
    flagframe += fframe;
    Rect r;
    ICN::icn_t cap;
    switch(hero.GetRace()) {
    case Race::BARB: cap = ICN::CMBTHROB; break;
    case Race::KNGT: cap = ICN::CMBTHROK; break;
    case Race::NECR: cap = ICN::CMBTHRON; break;
    case Race::SORC: cap = ICN::CMBTHROS; break;
    case Race::WRLK: cap = ICN::CMBTHROW; break;
    case Race::WZRD: cap = ICN::CMBTHROZ; break;
    default: return r;
    }
    const Sprite & sp = AGG::GetICN(cap, animframe, reflect);
    r.x = dst_pt.x + (reflect?640-sp.w():0);
    r.y = dst_pt.y + 75;
    r.w = sp.w();
    r.h = sp.h();
    display.Blit(sp, r.x, r.y);
    ICN::icn_t flag = ICN::UNKNOWN;
    switch(hero.GetColor()) {
    case Color::BLUE: flag = ICN::HEROFL00; break;
    case Color::GREEN: flag = ICN::HEROFL01; break;
    case Color::RED: flag = ICN::HEROFL02; break;
    case Color::YELLOW: flag = ICN::HEROFL03; break;
    case Color::ORANGE: flag = ICN::HEROFL04; break;
    case Color::PURPLE: flag = ICN::HEROFL05; break;
    case Color::GRAY: flag = ICN::HEROFL06; break;
    }
    display.Blit(AGG::GetICN(flag, flagframe%5, reflect), dst_pt.x + (reflect?640-sp.w()-17:17), dst_pt.y + 80);
    return r;
}

void Army::DrawArmy(Army::BattleArmy_t & army1, Army::BattleArmy_t & army2, int animframe)
{
    Point p;
    int t=-1;
    for(p.y = 0; p.y < BFH; p.y++) 
	for(p.x = 0; p.x < BFW; p.x++) {
	    DrawObject(p);
	    if(t = FindTroopExact(army1, p), t >= 0) {
		DrawTroop(army1[t], animframe);
		army1[t].Animate();
		if(army1[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
		    army1[t].Animate(Monster::AS_IDLE);
	    }
	    if(t = FindTroopExact(army2, p), t >= 0) {
		DrawTroop(army2[t], animframe);
		army2[t].Animate();
		if(army2[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
		    army2[t].Animate(Monster::AS_IDLE);
	    }
	}
}

void Army::DrawTroop(Army::BattleTroop & troop, int animframe)
{
    if(troop.Monster() == Monster::UNKNOWN) return;
    bool reflect = troop.IsReflected();
    bool wide = Monster::GetStats(troop.Monster()).wide;
    Point pos = troop.Position();
    Point tp = Bf2Scr(pos) + dst_pt;
    troop.Blit(tp, reflect, animframe);
    // draw count
    if(troop.Count() == 0) return;
    int offset = wide ? CELLW : 0;
    tp.x += reflect ? -30 + offset : 10 + offset;
    tp.y -= 10;
    // TODO color
    u16 ind = 10;  // blue
    // ind = 11;  // blue 2
    // ind = 12;  // green
    // ind = 13;  // yellow
    // ind = 14;  // red
    display.Blit(AGG::GetICN(ICN::TEXTBAR, ind), tp);
    std::string str;
    int count = troop.Count();
    if(count < 1000)
	String::AddInt(str, count);
    else if(count < 1000000)
	String::AddInt(str, count / 1000), str += NOL10N("K");
    else 
	String::AddInt(str, count / 1000000), str += NOL10N("M");
    tp.x += 10 - Text::width(str, Font::SMALL) / 2;
    tp.y -= 1;
    Text(str, Font::SMALL, tp);
}

void Army::InitArmyPosition(Army::BattleArmy_t & army, bool compact, bool reflect)
{
    int x = reflect ? 10 : 0;
    int y = compact ? 2 : 0;
    for(unsigned int i=0; i < army.size(); i++) {
	if(army[i].Monster() != Monster::UNKNOWN) {
	    const Monster::stats_t &stats = Monster::GetStats(army[i].Monster());
	    AGG::PreloadObject(stats.file_icn);
            army[i].SetPosition(Point(x, y));
	    army[i].aframe = 1;
	    army[i].astate = Monster::AS_NONE;
	    army[i].shots = stats.shots;
	    army[i].hp = stats.hp;
	    army[i].oldcount = army[i].Count();
            army[i].SetReflect(reflect);
            army[i].SetOriginalReflection(reflect);
            army[i].SetRetaliated(false);
	}
	y += compact ? 1 : 2;
    }
}

void Army::StartTurn(Army::BattleArmy_t &army)
{
    for(u16 i = 0; i < army.size(); i++)
    {
        switch(army[i].Monster())
        {
            case Monster::TROLL:
            case Monster::WAR_TROLL:
                //Trolls replenish health every turn
                army[i].hp = Monster::GetStats(army[i].Monster()).hp;
                break;
            default:
                break;
        }
        army[i].SetRetaliated(false);
    }
}

// translate coordinate to batle field
inline Point Army::Scr2Bf(const Point & pt)
{
    return Point((pt.x - BFX + (((pt.y - BFY + CELLH)/CELLH) % 2 ? CELLW/2 : 0))/CELLW , (pt.y - BFY + CELLH)/CELLH);
}

// translate to screen (offset to frame border)
inline Point Army::Bf2Scr(const Point & pt)
{
    return Point(BFX + pt.x*CELLW + (pt.y % 2 ? 0 : CELLW/2), BFY + pt.y*CELLH);
}

inline bool Army::BfValid(const Point & pt)
{
    return pt.x >= 0 && pt.x < BFW && pt.y >= 0 && pt.y < BFH;
}

/** Determine if a specific troop is at a given point
 *  \param troop  Troop to find
 *  \param p      Point at which to search
 */
bool Army::FindTroopAt(const Army::BattleTroop &troop, const Point &p)
{
    Point p2(troop.Position());
    if(Monster::GetStats(troop.Monster()).wide) p2.x += troop.IsReflected() ? -1 : 1;
    if(troop.Monster() != Monster::UNKNOWN && (p == troop.Position() || p == p2)) 
        return true;
    else return false;
}

/** Determine which troop, if one exists, is exactly at the given point
 *  \note No wide or reflection calculations are performed
 *  \param army Army through which to search
 *  \param p    Point at which to look
 *  \return The troop's index into \i army, or -1 if none found
 */
int Army::FindTroopExact(const Army::BattleArmy_t &army, const Point &p)
{
    for(unsigned int i=0; i<army.size(); i++) {
	if(army[i].Monster() != Monster::UNKNOWN && army[i].Position() == p)
            return i;
    }
    return -1;
}

/** Determine which troop, if one exists, is at the given point
 *  \param army Army through which to search
 *  \param p    Point at which to look
 *  \return The troop's index into \i army, or -1 if none found
 */
int Army::FindTroop(const Army::BattleArmy_t &army, const Point &p)
{
    for(unsigned int i=0; i<army.size(); i++) {
	if(FindTroopAt(army[i], p))
            return i;
    }
    return -1;
}

/** Determine which troop, if one exists, is at the given point
 *  \param army Army through which to search
 *  \param p    Point at which to look
 *  \return The troop's index into \i army, or -1 if none found
 */
int Army::FindTroop(const std::vector<Army::BattleTroop*> &army, const Point &p)
{
    for(unsigned int i=0; i<army.size(); i++) {
	if(FindTroopAt(*army[i], p))
            return i;
    }
    return -1;
}

/** Determine if a cell is free of obstruction (monsters and background)
 *  \param p      Battlefield cell
 *  \param army1  Left army
 *  \param army2  Right army
 *  \param skip   Troop id to ignore
 */
bool Army::CellFree(const Point &p, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip)
{
    int t;
    if(!BfValid(p)) return false;
    if(t = FindTroop(army1, p), t >= 0 && army1[t].Count() > 0 && (skip < 0 || skip != t)) 
	return false;
    if(t = FindTroop(army2, p), t >= 0 && army2[t].Count() > 0 && (skip >= 0 || -skip-1 != t)) 
	return false;
    for(u16 i=0; i<blockedCells.size(); i++) if(p == blockedCells[i]) return false;
    return true;
}

/** Determine if a cell is free of obstruction for a specific troop
 *  \param p      Battlefield cell
 *  \param troop  Troop to consider
 *  \param army1  Left army
 *  \param army2  Right army
 *  \param skip   Troop ID to ignore
 */
bool Army::CellFreeFor(const Point &p, const Army::BattleTroop &troop, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip)
{
    bool isFree = CellFree(p, army1, army2, skip);
    if(isFree && Monster::GetStats(troop.Monster()).wide)
      //WIDE-REVIEW
      isFree = CellFree(Point(p.x + (troop.IsReflected() ? -1 : 1), p.y), army1, army2, skip);
    return isFree;
}

void Army::SettingsDialog()
{ 
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::CSPANBKE : ICN::CSPANBKG;
    const ICN::icn_t butt = H2Config::EvilInterface() ? ICN::CSPANBTE : ICN::CSPANBTN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    
    // 0-2 speed
    // 3-5 monster info
    // 6,7 auto spell
    // 8,9 grid
    // 10,11 shadow movement
    // 12,13 shadow cursor
    Button buttonOK(pos_rt.x + 113, pos_rt.y + 252, butt, 0, 1);
    Button optGrid(pos_rt.x + 36, pos_rt.y + 157, ICN::CSPANEL, 8, 9);
    Button optSM(pos_rt.x + 128, pos_rt.y + 157, ICN::CSPANEL, 10, 11);
    Button optSC(pos_rt.x + 220, pos_rt.y + 157, ICN::CSPANEL, 12, 13);
    buttonOK.Draw();
    optGrid.Draw();
    optSM.Draw();
    optSC.Draw();
    if(O_GRID) optGrid.PressDraw();
    if(O_SHADM) optSM.PressDraw();
    if(O_SHADC) optSC.PressDraw();

    display.Flip();
    cursor.Show();

    // dialog menu loop
    while(le.HandleEvents()) {
	// exit
	if(le.KeyPress(KEY_ESCAPE)) {
	    return;
	}	
	do_button(buttonOK, return, {});
	do_rbutton(optGrid, O_GRID = !O_GRID, {});
	do_rbutton(optSM, O_SHADM = !O_SHADM, {});
	do_rbutton(optSC, O_SHADC = !O_SHADC, {});
    }
}

Army::battle_t Army::HeroStatus(Heroes &hero, Dialog::StatusBar &statusBar, Spell::spell_t &spell, bool quickshow, bool cansurrender, bool locked)
{
    spell = Spell::NONE;
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::VGENBKGE : ICN::VGENBKG;
    const ICN::icn_t butt = ICN::VIEWGEN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    display.Blit(Portrait::Hero(hero, Portrait::BIG), pos_rt.x + 27, pos_rt.y + 42);
    display.Blit(AGG::GetICN(butt, Color::GetIndex(hero.GetColor())+1), pos_rt.x + 148, pos_rt.y + 36);
    Point tp(pos_rt);
    std::string str;
    str = tr("hero.long_name").sub(hero.GetName()).sub(Race::String(hero.GetRace()));
    tp.x += 8 + pos_rt.w/2 - Text::width(str, Font::SMALL)/2;
    tp.y += 10;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.attack")).sub(hero.GetAttack());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 40;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.defense")).sub(hero.GetDefense());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 51;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.spell_power")).sub(hero.GetPower());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 62;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.knowledge")).sub(hero.GetKnowledge());
    String::AddInt(str, hero.GetKnowledge());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 73;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.morale")).sub(hero.GetMorale());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 84;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display").sub(tr("skills.luck")).sub(hero.GetLuck());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 95;
    Text(str, Font::SMALL, tp);
    str = tr("hero.skill_display_ratio").sub(tr("skills.spell_points")).sub(hero.GetSpellPoints()).sub(hero.GetMaxSpellPoints());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 117;
    Text(str, Font::SMALL, tp);
    
    Button buttonMag(pos_rt.x + 30, pos_rt.y + 148, butt, 9, 10);
    Button buttonRet(pos_rt.x + 89, pos_rt.y + 148, butt, 11, 12);
    Button buttonSur(pos_rt.x + 148, pos_rt.y + 148, butt, 13, 14);
    Button buttonOK(pos_rt.x + 207, pos_rt.y + 148, butt, 15, 16);
    buttonMag.SetDisable(!hero.SpellBook().Active() || hero.spellCasted || locked || quickshow);
    buttonRet.SetDisable(locked || quickshow);
    buttonSur.SetDisable(!cansurrender || locked || quickshow);
    buttonMag.Draw();
    buttonRet.Draw();
    buttonSur.Draw();
    buttonOK.Draw();

    display.Flip();
    cursor.Show();

    // dialog menu loop
    while(le.HandleEvents()) {
        if(quickshow) {
	    if(!le.MouseRight()) break;
        } else {
	    // exit
	    if(le.KeyPress(KEY_ESCAPE)) {
		return WIN;
	    }
	    do_button(buttonMag, {if(spell=hero.SpellBook().Open(Spell::Book::CMBT, true),spell!=Spell::NONE) {back.Restore();return WIN;}}, Dialog::Message(tr("battle.cast_spell"), tr("battle.cast_spell_info"), Font::BIG));
	    do_button(buttonRet, return RETREAT, Dialog::Message(tr("battle.retreat"), tr("battle.retreat_info"), Font::BIG));
	    do_button(buttonSur, return SURRENDER, Dialog::Message(tr("battle.surrender"), tr("battle.surrender_info"), Font::BIG));
	    do_button(buttonOK, return WIN, Dialog::Message(tr("dialog.cancel"), tr("battle.cancel_info"), Font::BIG));
	    if(le.MouseCursor(buttonMag)) {
		statusBar.ShowMessage(tr("battle.cast_spell"));
	    } else if(le.MouseCursor(buttonRet)) {
		statusBar.ShowMessage(tr("battle.retreat"));
	    } else if(le.MouseCursor(buttonSur)) {
		statusBar.ShowMessage(tr("battle.surrender"));
	    } else if(le.MouseCursor(buttonOK)) {
		statusBar.ShowMessage(tr("dialog.cancel"));
	    } else {
		statusBar.ShowMessage(tr("battle.hero_options"));
	    }
	}
    }
    return WIN;
}

void Army::DrawShadow(const Point &pt)
{
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    if(prep) {
	prep = false;
	u32 gr = shadow.MapRGB(0,0,0, 0x30);
	u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	for(int i=1; i<CELLW/2; i+=2) {
	    r.y--, r.h +=2;
	    shadow.FillRect(gr, r);
	    r.x += 2;
	    r.w -= 4;
	}
	//shadow.SetAlpha(0x30);
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

void Army::DrawCell(const Point&pt)
{
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    if(prep) {
	prep = false;
	u32 gr = shadow.MapRGB(0,128,0, 128);
	u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	shadow.FillRect(tr, r);
	for(int i=1; i<CELLW/2; i+=2) {
	    r.y--, r.h +=2;
	    shadow.FillRect(gr, r);
	    r.y++, r.h -=2;
	    shadow.FillRect(tr, r);
	    r.y--, r.h +=2;
	    r.x += 2;
	    r.w -= 4;
	}
	//shadow.SetAlpha(0x80);
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

void Army::DrawMark(const Point&pt, int animframe)
{
    static int frame = 0;
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    frame += animframe;
    /*if(prep)*/ {
	prep = false;
	u32 gr = shadow.MapRGB(255,255,0, abs((frame%21)-10)*20+55);
	//u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	//shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	for(int i=r.y; i<r.y+r.h; i++) shadow.SetPixel(r.x, i, gr), shadow.SetPixel(r.x+r.w-1, i, gr);
	//shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	//shadow.FillRect(tr, r);
	for(; r.x<CELLW/2; r.x++, r.w-=2) {
	    if(r.x%2) r.y--, r.h +=2;
	    for(int x = r.x; x < r.x+r.w; x += r.w-1)
		for(int y = r.y; y < r.y+r.h; y += r.h-1)
		    shadow.SetPixel(x, y, gr);
	    //shadow.FillRect(gr, r);
	}
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

/** Populate the global vector #movePoints with the given troop's available movement cells.
 *  \param troopN  Index into \c army1 or \c army2
 *  \param army1   The left army
 *  \param army2   The right army
 */
void Army::PrepMovePoints(int troopN, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2)
{
    movePoints.clear();
    Point p;
    const Army::BattleTroop &troop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &stat = Monster::GetStats(troop.Monster());
    if(stat.fly) {
	for(p.x = 0; p.x < BFW; p.x ++) 
	    for(p.y = 0; p.y < BFH; p.y ++) {
		if(CellFreeFor(p, troop, army1, army2, troopN))
		    movePoints.push_back(p);
	    }
    } else {
	Point p = troop.Position();
	movePoints.push_back(p);
	Point d;
	for(d.x = -1; d.x <= 1; d.x++)
	    for(d.y = -1; d.y <= 1; d.y++) 
		if(d.x || d.y ) {
		    if(p.y%2 && d.y && d.x>0) continue;
		    if(!(p.y%2) && d.y && d.x<0) continue;
		    PrepMovePointsInt(troop, p+d, Speed::Move(stat.speed), army1, army2, troopN);
		}
    }
}

/** Populate the global vector #movePoints with any free cells in range of the given point.
 * \param troop   Troop to prepare
 * \param p       Starting point
 * \param move    Movement speed
 * \param army1   The left army
 * \param army2   The right army
 * \param skip    Troop id to ignore if found
 */
void Army::PrepMovePointsInt(const Army::BattleTroop &troop, const Point &p, int move, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip)
{
    if(BfValid(p) && CellFreeFor(p, troop, army1, army2, skip) && move > 0) {
	bool exist = false;
	for(u16 i=0; i<movePoints.size(); i++) if(movePoints[i] == p) exist = true;
	if(!exist) movePoints.push_back(p);
	Point d;
	for(d.x = -1; d.x <= 1; d.x++)
	    for(d.y = -1; d.y <= 1; d.y++) 
		if(d.x || d.y ) {
		    if(p.y%2 && d.y && d.x>0) continue;
		    if(!(p.y%2) && d.y && d.x<0) continue;
		    PrepMovePointsInt(troop, p+d, move-1, army1, army2, skip);
		}
    }
}

/** Find a path from one point to another for a given troop.
 * \param start   Starting point
 * \param end     End point
 * \param moves   Maximum length of path
 * \param troop   Troop for which to find path
 * \param army1   Left army
 * \param army2   Right army
 * \param skip    Troop id to ignore if found
 * \return A #Point vector containing the path
 */
std::vector<Point> *Army::FindPath(const Point& start, const Point &end, int moves, const Army::BattleTroop &troop, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int skip)
{
    if(moves < 0 || !BfValid(start) || !CellFreeFor(start, troop, army1, army2, skip)) return 0;
    if(start == end) return new std::vector<Point>();
    Point p = start;
    Point d;
    std::vector<Point> *path = 0, *tmpath = 0;
    int length = -1;
    for(d.x = -1; d.x <= 1; d.x++)
	for(d.y = -1; d.y <= 1; d.y++) 
	    if(d.x || d.y ) {
		if(p.y%2 && d.y && d.x>0) continue;
		if(!(p.y%2) && d.y && d.x<0) continue;
		if(tmpath = FindPath(p+d, end, moves-1, troop, army1, army2, skip),tmpath) {
		    if(length < 0 || length > static_cast<int>(tmpath->size())) {
			length = tmpath->size();
			tmpath->push_back(p+d);
			path = tmpath;
		    } else delete tmpath;
		}
	    }
    return path;
}

void Army::AttackStatus(const std::string &str)
{
    static std::string str1, str2;
    str1 = str2;
    str2 = str;
    statusBar1->ShowMessage(str1);
    statusBar2->ShowMessage(str2);
}

//FIXME: This documentation is wrong because I don't actually understand what the function does.
/** Determine if a troop can attack a given enemy
 *  \param myTroop    Attacker troop
 *  \param moves      #Point vector of valid cells attacker can move to
 *  \param enemyTroop Enemy target
 *  \param d          Starting point
 *  \return Non-zero if the attacker can attack the enemy
 */
int Army::CanAttack(const Army::BattleTroop &myTroop, const std::vector<Point> &moves, const Army::BattleTroop &enemyTroop, const Point &d)
{
    if(myTroop.Count() == 0) return -1;
    Point delta = d;
    //delta.x -= CELLW;
    delta.y += CELLH/2;
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    const Monster::stats_t &enemyMonster = Monster::GetStats(enemyTroop.Monster());
    int mp = -1, min = 9999;
    for(u32 i=0; i<moves.size(); i++) {
	Point d, p1, p2;
	p1 = moves[i];
	p2 = enemyTroop.Position();
	do {
	    for(d.x = -1; d.x <= 1; d.x++)
		for(d.y = -1; d.y <= 1; d.y++) 
		    if(d.x || d.y ) {
			if(p1.y%2 && d.y && d.x>0) continue;
			if(!(p1.y%2) && d.y && d.x<0) continue;
			if(p1 + d == p2) {
			    Point d2 = Bf2Scr(p2)+delta - Bf2Scr(p1);
			    if(abs(d2.x) + abs(d2.y) < min) {
				min = abs(d2.x) + abs(d2.y);
				mp = i;
			    }
			}
		    }
            bool mod = false;
	    if(myMonster.wide && p1.x == moves[i].x) {
                //WIDE-REVIEW
		p1.x += myTroop.IsReflected() ? -1 : 1;
                //p1.x ++;
		mod = true;
	    }
	    if(enemyMonster.wide && p2.x == enemyTroop.Position().x) {
                //WIDE-REVIEW
		if(!mod) p1 = myTroop.Position();
		p2.x -= myTroop.IsReflected() ? -1 : 1;
                //p2.x --;
		mod = true;
	    }
            if(mod) continue;
	    else break;
	} while(1);
    }
    return mp;
}

/** Find an open cell from which to attack a creature
 *  \param target Target to be attacked
 *  \param army1  Left army
 *  \param army2  Right army
 *  \param troopN Index in either \i army1 or \i army2
 *  \return Free cell if one is available, invalid if none
 */
Point Army::GetReachableAttackCell(const Army::BattleTroop &target, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, int troopN)
{
    const Army::BattleTroop &attacker = troopN >= 0 ? army1[troopN] : army2[-troopN - 1];
    bool targetWide = Monster::GetStats(target.Monster()).wide;
    bool attackerWide = Monster::GetStats(attacker.Monster()).wide;
    Point delta, p = target.Position();
    int xstart, xend, xincr;
    int ystart, yend, yincr;
    if(target.Position().x > attacker.Position().x) {
        xstart = -1; xend = 2; xincr = 1;
    } else {
        xstart = 1; xend = -2; xincr = -1;
    }
    if(target.Position().y > attacker.Position().y) {
        ystart = -1; yend = 2; yincr = 1;
    } else {
        ystart = 1; yend = -2; yincr = -1;
    }
    if(targetWide)
    {
        //WIDE-REVIEW
        xend += xincr;
        xstart -= xincr;
    }
    if(attackerWide)
    {
        //WIDE-REVIEW
        xstart -= xincr;
        xend += xincr;
    }
    for(delta.x = xstart; delta.x != xend; delta.x += xincr)
        for(delta.y = ystart; delta.y != yend; delta.y += yincr)
            if((delta.x || delta.y) && BfValid(p + delta))
            {
                if(delta.x < 0 && delta.y) continue;
                if(CellFreeFor(p + delta, attacker, army1, army2, troopN)
                && find(movePoints.begin(), movePoints.end(), p + delta) != movePoints.end())
                    return p + delta;
            }
    return Point(-1);
}

bool Army::GoodMorale(Heroes *hero, const Army::BattleTroop &troop)
{
    if(Monster::GetRace(troop.Monster()) == Race::NECR) return false;
    if(Monster::GetRace(troop.Monster()) == Race::BOMG && troop.Monster() >= Monster::EARTH_ELEMENT) return false;
    int m = Rand::Get(1, MORALEBOUND);
    if(hero && (hero->GetMorale() >= m)) {
	AGG::PlaySound(M82::GOODMRLE);
	for(u16 i=0; i<AGG::GetICNCount(ICN::MORALEG); i++) {
	    const Sprite &sp = AGG::GetICN(ICN::MORALEG, i);
	    Rect pos_rt = sp;
	    pos_rt += Bf2Scr(troop.Position()) + dst_pt;
	    Background back(pos_rt);
	    back.Save();
	    display.Blit(sp, pos_rt);
	    display.Flip();
	    int animat = 0;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
	    }
	    back.Restore();
	}
	AttackStatus(tr("battle.high_morale").sub(Monster::MultipleNames(troop.Monster())));
	return true;
    }
    return false;
}

bool Army::BadMorale(Heroes *hero, const Army::BattleTroop &troop)
{
    if(Monster::GetRace(troop.Monster()) == Race::NECR) return false;
    if(Monster::GetRace(troop.Monster()) == Race::BOMG && troop.Monster() >= Monster::EARTH_ELEMENT) return false;
    int m = -Rand::Get(1, MORALEBOUND);
    if(hero && (hero->GetMorale() <= m)) {
	AGG::PlaySound(M82::BADMRLE);
	for(u16 i=0; i<AGG::GetICNCount(ICN::MORALEB); i++) {
	    const Sprite &sp = AGG::GetICN(ICN::MORALEB, i);
	    Rect pos_rt = sp;
	    pos_rt += Bf2Scr(troop.Position()) + dst_pt;
	    Background back(pos_rt);
	    back.Save();
	    display.Blit(sp, pos_rt);
	    display.Flip();
	    int animat = 0;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
	    }
	    back.Restore();
	}
	AttackStatus(tr("battle.low_morale").sub(Monster::MultipleNames(troop.Monster())));
	return true;
    }
    return false;
}

// return 1 for good luck, -1 for bad luck, 0 for none luck
int Army::CheckLuck(Heroes *hero, const Army::BattleTroop &troop)
{
    int luck = Rand::Get(0, MORALEBOUND);
    if(hero) {
	if(hero->GetLuck() < 0 && hero->GetLuck() <= -luck) {
	    AGG::PlaySound(M82::BADLUCK);
	    for(u16 i=0; i<AGG::GetICNCount(ICN::CLOUDLUK); i++) {
		const Sprite &sp = AGG::GetICN(ICN::CLOUDLUK, i);
		Rect pos_rt = sp;
		pos_rt += Bf2Scr(troop.Position()) + dst_pt;
		Background back(pos_rt);
		back.Save();
		display.Blit(sp, pos_rt);
		display.Flip();
		int animat = 0;
		while(le.HandleEvents()) {
		    if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
		}
		back.Restore();
	    }
	    return -1;
	}
	if(hero->GetLuck() > 0 && hero->GetLuck() >= luck) {
	    AGG::PlaySound(M82::GOODLUCK);
	    for(u16 i=0; i<AGG::GetICNCount(ICN::EXPMRL); i++) {
		const Sprite &sp = AGG::GetICN(ICN::EXPMRL, 0);
		Rect pos_rt = sp;
		pos_rt += Bf2Scr(troop.Position()) + dst_pt;
		pos_rt.y -= CELLH*2;
		pos_rt.x -= CELLW/2;
		Background back(pos_rt);
		back.Save();
		display.Blit(sp, pos_rt);
		display.Flip();
		int animat = 0;
		while(le.HandleEvents()) {
		    if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
		}
		back.Restore();
	    }
	    return 1;
	}
    }
    return 0;
}

void Army::Temp(int dicn, int dindex, Point pt)
{
    static int icn=0, index=0;
    if(dicn) {
	icn += dicn;
	index = 0;
    }
    index += dindex;
    if(icn < 0) icn = ICN::UNKNOWN-1;
    if(icn >= ICN::UNKNOWN) icn = 0;
    if(index < 0) index = 0;
    std::string str = ICN::GetString((ICN::icn_t)icn);
    str += NOL10N(" ");
    String::AddInt(str, index);
    str += NOL10N("-");
    int dh = 0, x = 0, y = 0;
    while(1) {
	const Sprite &sp = AGG::GetICN((ICN::icn_t)icn, index);
	if(sp.w() == 0 || sp.h() == 0) {
	    index --;
	    break;
	}
	if(dh < sp.h()) dh = sp.h();
	if(x + sp.w() > 640) {
	    x = 0;
	    y += dh;
	    if(y + sp.h() > 480) {
		index --;
		break;
	    }
	    dh = sp.h();
	}
	display.Blit(sp, pt.x + x, pt.y + y);
	x += sp.w();
	index ++;
	if(index >= AGG::GetICNCount((ICN::icn_t)icn)) break;
    }
    String::AddInt(str, index);
    Text(str, Font::SMALL, pt);
}
