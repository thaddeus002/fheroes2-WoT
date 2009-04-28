#include <cmath>
#include <algorithm>
#include "engine.h"
#include "settings.h"
#include "army_troop.h"
#include "battle.h"
#include "battle_types.h"
#include "world.h"
#include "maps_tiles.h"
#include "mp2.h"
#include "display.h"
#include "agg.h"
#include "castle.h"
#include "speed.h"
#include "kingdom.h"
#include "cursor.h"
#include "button.h"

#define le LocalEvent::GetLocalEvent()
#define cursor Cursor::Get()
#define display Display::Get()

namespace Battle
{
    Point g_baseOffset;
}

/** Convert an #army_t into a #BattleArmy_t for use in battle.
 *  \param[in]  army       army to convert
 *  \param[out] battleArmy converted result
 */
void Army::ArmyToBattleArmy(const Army::army_t &army, Army::BattleArmy_t &battleArmy)
{
    for(u16 i = 0; i < army.Size(); i++)
        if(army.At(i).isValid())
            battleArmy.push_back(Army::BattleTroop(army.At(i)));
        else battleArmy.push_back(Army::BattleTroop(Monster::UNKNOWN, 0));
}

/** Convert a #BattleArmy_t into an #army_t after use in battle.
 *  \param[in]  battleArmy  army to convert
 *  \param[out] army        converted result
 */
void Army::BattleArmyToArmy(const Army::BattleArmy_t &battleArmy, Army::army_t &army)
{
    army.Import(battleArmy);
}

/** Directly translate screen coordinate to battle field cell without half cell round up of Scr2Bf.
 *  \param pt  Coordinate to convert
 *  \return  Possibly invalid battlefield coordinate
 */
Point Battle::Scr2BfDirect(const Point & pt)
{
    return Point((pt.x - g_baseOffset.x - BFX - (((pt.y - g_baseOffset.y - BFY)/CELLH) % 2 ? CELLW/2 : 0))/CELLW ,
                 (pt.y - g_baseOffset.y - BFY)/CELLH);
}

/** Translate screen coordinate to battle field cell, rounding up by a half cell.
 *  \param pt  Coordinate to convert
 *  \return  Possibly invalid battlefield coordinate
 */
Point Battle::Scr2Bf(const Point & pt)
{
    return Point((pt.x - g_baseOffset.x - BFX + (((pt.y - g_baseOffset.y - BFY + CELLH)/CELLH) % 2 ? CELLW/2 : 0))/CELLW ,
                 (pt.y - g_baseOffset.y - BFY + CELLH)/CELLH);
}

/** Translate battlefield cell to absolute screen position at bottom left of cell.
 *  \param pt  Coordinate to convert
 */
Point Battle::Bf2Scr(const Point & pt)
{
    return Point(BFX + pt.x*CELLW + (pt.y % 2 ? 0 : CELLW/2), BFY + pt.y*CELLH) + g_baseOffset;
}

/** Translate battlefield cell to relative screen position at bottom left of cell.
 *  \param pt  Coordinate to convert
 */
Point Battle::Bf2ScrNoOffset(const Point& pt)
{
    return Bf2Scr(pt) - g_baseOffset;
}

/** Determine if given #Point is a valid cell index on the battlefield.
 */
bool Battle::BfValid(const Point & pt)
{
    return pt.x >= 0 && pt.x < BFW && pt.y >= 0 && pt.y < BFH;
}

/** Initiate a battle between two heroes.
 *  \param h1    Left hero
 *  \param h2    Right hero
 *  \param tile  Map tile from which to derive visual elements
 *  \return Battle status relative to left hero
 */
Army::battle_t Army::Battle(Heroes &h1, Heroes &h2, const Maps::Tiles &tile, u32 &exp)
{
    Battle::BattleControl battle(h1, h2, tile, exp);
    return battle.GetStatus();
}

/** Initiate a battle between a hero and an army.
 *  \param h1    Left hero
 *  \param a     Right army
 *  \param tile  Map tile from which to derive visual elements
 *  \return Battle status relative to left hero
 */
Army::battle_t Army::Battle(Heroes &h, Army::army_t &a, const Maps::Tiles &tile, u32 &exp)
{
    Battle::BattleControl battle(h, a, tile, exp);
    return battle.GetStatus();
}

/** Initiate a battle between a hero and a castle.
 *  \param h1    Left hero
 *  \param a     Right castle
 *  \param tile  Map tile from which to derive visual elements
 *  \return Battle status relative to left hero
 */
Army::battle_t Army::Battle(Heroes &h, Castle &c, const Maps::Tiles &tile, u32 &exp)
{
    Battle::BattleControl battle(h, c, tile, exp);
    return battle.GetStatus();
}

//////////////////////////////////////////////////////////////////////////////////////////

namespace Battle
{
    
    BattleCastle::BattleCastle(Castle &castle)
    : m_castle(castle)
    {
        m_drawbridge = CLOSED;
        for(u16 i = 0; i < sizeof(m_wall) / sizeof(m_wall[0]); i++)
            m_wall[i] = UNBROKEN;
        for(u16 i = 0; i < sizeof(m_tower) / sizeof(m_tower[0]); i++)
            m_tower[i] = UNBROKEN;
        
        switch(getRace())
        {
            case Race::KNGT:
                m_groundICN = ICN::CASTBKGK;
                m_wallICN = ICN::CASTLEK;
                break;
            case Race::BARB:
                m_groundICN = ICN::CASTBKGB;
                m_wallICN = ICN::CASTLEB;
                break;
            case Race::SORC:
                m_groundICN = ICN::CASTBKGS;
                m_wallICN = ICN::CASTLES;
                break;
            case Race::WRLK:
                m_groundICN = ICN::CASTBKGW;
                m_wallICN = ICN::CASTLEW;
                break;
            case Race::WZRD:
                m_groundICN = ICN::CASTBKGZ;
                m_wallICN = ICN::CASTLEZ;
                break;
            case Race::NECR:
                m_groundICN = ICN::CASTBKGN;
                m_wallICN = ICN::CASTLEN;
                break;
            default:
                break;
        }

        RecreateCastle();
    }

    BattleCastle::~BattleCastle()
    {
        DestroyCastle();
    }

    void BattleCastle::OpenDrawbridge()
    {
        if(m_drawbridge == BROKEN)
            return;
        
        m_drawbridge = OPEN;
        RecreateCastle();
    }

    void BattleCastle::CloseDrawbridge()
    {
        if(m_drawbridge != OPEN)
            return;

        m_drawbridge = CLOSED;
        RecreateCastle();
    }

    void BattleCastle::RecreateMoat()
    {
        m_moatCells.clear();
        if(!hasMoat())
            return;
        
        m_moatCells.reserve(9);
        m_moatCells.push_back(Point(7, 0));
        m_moatCells.push_back(Point(7, 1));
        m_moatCells.push_back(Point(6, 2));
        m_moatCells.push_back(Point(6, 3));
        if(getDrawbridge() == CLOSED)
        {
            m_moatCells.push_back(Point(5, 4));
            m_moatCells.push_back(Point(6, 4));
        }
        m_moatCells.push_back(Point(6, 5));
        m_moatCells.push_back(Point(6, 6));
        m_moatCells.push_back(Point(7, 7));
        m_moatCells.push_back(Point(7, 8));
    }

    void BattleCastle::DestroyCastle()
    {
        for(u16 i = 0; i < m_castleParts.size(); i++)
            delete m_castleParts[i];
        m_castleParts.clear();
    }

    void BattleCastle::RecreateCastle()
    {
        DestroyCastle();
        
        u8 offset = hasFortifications() ? 28 : 5;
        u8 idx = 0;
        const Sprite *sprite;

        sprite = &AGG::GetICN(m_wallICN, offset + (u8)getWall(idx));
        m_castleParts.push_back(new CastlePart(sprite, Point(8, 0))); offset++; 
    
        sprite = &AGG::GetICN(m_wallICN, 17 + (getTower(idx) != UNBROKEN ? 2 : hasRight() ? 1 : 0));
        m_castleParts.push_back(new CastlePart(sprite, Point(8, 1))); idx++;
    
        sprite = &AGG::GetICN(m_wallICN, offset + (u8)getWall(idx));
        m_castleParts.push_back(new CastlePart(sprite, Point(7, 2))); offset++;
    
        sprite = &AGG::GetICN(m_wallICN, 17 + (getTower(idx) != UNBROKEN ? 2 : 0));
        m_castleParts.push_back(new CastlePart(sprite, Point(7, 3))); idx++;
    
        sprite = &AGG::GetICN(m_wallICN, 4);
        
        if(getWall(idx) != UNBROKEN)
        {
            m_drawbridge = BROKEN;
            sprite = NULL;
        }
        m_castleParts.push_back(new CastlePart(sprite, Point(6, 4)));
    
        sprite = &AGG::GetICN(m_wallICN, 17 + (getTower(idx) != UNBROKEN ? 2 : 0));
        m_castleParts.push_back(new CastlePart(sprite, Point(7, 5))); idx++;
    
        sprite = &AGG::GetICN(m_wallICN, offset + (u8)getWall(idx));
        m_castleParts.push_back(new CastlePart(sprite, Point(7, 6))); offset++;

        sprite = &AGG::GetICN(m_wallICN, 17 + (getTower(idx) != UNBROKEN ? 2 : hasLeft() ? 1 : 0));
        m_castleParts.push_back(new CastlePart(sprite, Point(8, 7))); idx++;
    
        sprite = &AGG::GetICN(m_wallICN, offset + (u8)getWall(idx));
        m_castleParts.push_back(new CastlePart(sprite, Point(8, 8))); offset++;
        
        sprite = &AGG::GetICN(m_wallICN, 20 + (getTower(idx) != UNBROKEN ? 6 : 0));
        m_castleParts.push_back(new CastlePart(sprite, Point(11, 1))); idx++;

        if(getDrawbridge() != CLOSED)
            sprite = &AGG::GetICN(m_wallICN, 21 + (getDrawbridge() == OPEN ? 0 : 3));
        else sprite = NULL;

        m_castleParts.push_back(new CastlePart(sprite, Point(4, 4)));

        RecreateMoat();
    }

    u32 BattleCastle::countBuildings() const
    {
        return m_castle.CountBuildings();
    }
    
    Race::race_t BattleCastle::getRace() const
    {
        return m_castle.GetRace();
    }
    
    bool BattleCastle::hasLeft() const
    {
        return m_castle.isBuild(Castle::BUILD_LEFTTURRET);
    }
    
    bool BattleCastle::hasRight() const
    {
        return m_castle.isBuild(Castle::BUILD_RIGHTTURRET);
    }
    
    bool BattleCastle::hasMoat() const
    {
        return m_castle.isBuild(Castle::BUILD_MOAT);
    }
    
    bool BattleCastle::hasFortifications() const
    {
        return getRace() == Race::KNGT && m_castle.isBuild(Castle::BUILD_SPEC);
    }
    
    BattleCastle::WallStatus BattleCastle::getWall(u8 wall)
    {
        return m_wall[wall];
    }

    BattleCastle::WallStatus BattleCastle::getTower(u8 tower)
    {
        return m_tower[tower];
    }

    BattleCastle::DrawbridgeStatus BattleCastle::getDrawbridge()
    {
        return m_drawbridge;
    }

    Point BattleCastle::getCastlePoint(u8 part)
    {
        if(part % 2)
            return m_castleParts[part]->m_logical;
        else return *castlePartStatus(part) != DESTROYED ? m_castleParts[part]->m_logical : Point(-1, -1);
    }

    Point BattleCastle::getCastlePartDisplay(u8 part)
    {
        if(!m_castleParts[part]->m_sprite)
            return Bf2ScrNoOffset(m_castleParts[part]->m_logical);
        
        if(m_castleParts[part]->m_sprite->x() > 0)
            return Point(m_castleParts[part]->m_sprite->x(), m_castleParts[part]->m_sprite->y());
        else
            return Point(m_castleParts[part]->m_sprite->x(), m_castleParts[part]->m_sprite->y())
                    + Bf2ScrNoOffset(m_castleParts[part]->m_logical);
    }

    const Sprite &BattleCastle::getCastlePartSprite(u8 part)
    {
        if(!m_castleParts[part]->m_sprite)
            return AGG::GetICN(ICN::CASTLEZ, 0);
        
        return *m_castleParts[part]->m_sprite;
    }

    BattleCastle::WallStatus *BattleCastle::castlePartStatus(u8 part)
    {
        if(part % 2)
            return &m_tower[part / 2];
        else return &m_wall[part / 2];
    }

    u8 BattleCastle::CalculateDamage(u8 part, HeroBase *hero)
    {
        /* Arbitrary calculations that I invented:
           - none:     1/3 chance of basic hit
           - basic:    1/2 chance of basic hit
           - advanced: 1/1 chance of hit, 1/3 chance of demolish
           - expert:   1/1 chance of demolish

           With fortifications, the list becomes:
           - none:     1/4
           - basic:    1/3
           - advanced: 1/2, 1/3
           - expert:   1/2
        */
        
        s8 mod = hero->GetLevelSkill(Skill::Secondary::BALLISTICS);

        if(hasFortifications())
            mod--;

        s8 res = Rand::Get(std::min(mod, (s8)Skill::Level::EXPERT), Skill::Level::EXPERT);
        if(res >= Skill::Level::ADVANCED)
        {
            if(mod == Skill::Level::EXPERT)
                return 2;
            else return (2 == Rand::Get(0, 2)) + 1;
        }
        else return 0;
    }
    
    void BattleCastle::DamagePart(u8 part, u8 damage)
    {
        while(damage--)
            *castlePartStatus(part) = NextStatus(*castlePartStatus(part));
        RecreateCastle();
    }

    BattleCastle::WallStatus BattleCastle::NextStatus(WallStatus status)
    {
        if(status == UNBROKEN)
            return DAMAGED;
        else return DESTROYED;        
    }

    /** Determine if a given battlefield point is part of a castle moat.
     *  \param p  #Point to consider.
     */
    bool BattleCastle::IsCellInMoat(const Point &p)
    {
        return std::find(m_moatCells.begin(), m_moatCells.end(), p) != m_moatCells.end();
    }

    ///////////////////////////////////////////////////////////////

    Catapult::Catapult()
    {
        sprite = &AGG::GetICN(ICN::CATAPULT, 0);
        back = NULL;
    }

    Catapult::~Catapult()
    {
        delete back;
    }

    void Catapult::Fire(Battlefield &battlefield, BattleCastle &castle)
    {
        int target;
        std::vector<int> targets;
        do
        {
            //First attack existing walls
            for(int i = 0; i < MAX_CASTLE_WALLS; i++)
            {
                if(castle.getWall(i) != BattleCastle::DESTROYED && i != 2)
                    targets.push_back(i * 2);
            }
            if(targets.size()) break;
        
            //Next attack existing decorative towers
            if(castle.getTower(1) == BattleCastle::UNBROKEN) targets.push_back(BattleCastle::LEFT_DRAWBRIDGE_TOWER);
            if(castle.getTower(2) == BattleCastle::UNBROKEN) targets.push_back(BattleCastle::RIGHT_DRAWBRIDGE_TOWER);
            if(targets.size()) break;
        
            //Then attack left or right towers
            if(castle.getTower(0) == BattleCastle::UNBROKEN) targets.push_back(BattleCastle::LEFT_TOWER);
            if(castle.getTower(3) == BattleCastle::UNBROKEN) targets.push_back(BattleCastle::RIGHT_TOWER);
            if(targets.size()) break;
        
            //Then attack main tower
            if(castle.getTower(4) == BattleCastle::UNBROKEN)
            {
                targets.push_back(BattleCastle::TOWER_KEEP);
                break;
            }
            //Finally attack drawbridge
            else if(castle.getWall(2) == BattleCastle::UNBROKEN)
            {
                targets.push_back(BattleCastle::DRAWBRIDGE);
                break;
            }
        } while(0);
    
        if(!targets.size())
            return;
    
        target = targets[Rand::Get(0, targets.size() - 1)];
        u8 damage = castle.CalculateDamage(target, battlefield.GetHero(0));
    
        if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
            Animate(battlefield, castle, target, damage);
        else castle.DamagePart(target, damage);
    }
}
    
//////////////////////////////////////////////////////////////////////////////////////////

Battle::BattleControl::BattleControl(Heroes &hero1, Heroes &hero2, const Maps::Tiles &tile, u32 &exp)
: m_battlefield(NULL, &hero1, &hero2, hero1.GetArmy(), hero2.GetArmy(), g_baseOffset, tile)
{
    Army::BattleArmy_t hero1Army, hero2Army;
    ArmyToBattleArmy(hero1.GetArmy(), hero1Army);
    ArmyToBattleArmy(hero2.GetArmy(), hero2Army);
    m_commanders[0] = hero1.GetArmy().GetCommander();
    m_commanders[1] = hero2.GetArmy().GetCommander();
    m_battleStatus = RunBattle(&hero1, &hero2);
    BattleArmyToArmy(m_battlefield.GetArmy(0), hero1.GetArmy());
    BattleArmyToArmy(m_battlefield.GetArmy(1), hero2.GetArmy());
    exp = BattleSummaryVsHero(hero1, hero1Army, hero2, hero2Army);
}

Battle::BattleControl::BattleControl(Heroes &hero, Army::army_t& army, const Maps::Tiles &tile, u32 &exp)
: m_battlefield(NULL, &hero, NULL, hero.GetArmy(), army, g_baseOffset, tile)
{
    Army::BattleArmy_t heroArmyOrig, oppArmy, oppArmyOrig;
    ArmyToBattleArmy(hero.GetArmy(), heroArmyOrig);
    ArmyToBattleArmy(army, oppArmyOrig);
    m_commanders[0] = hero.GetArmy().GetCommander();
    m_commanders[1] = army.GetCommander();
    m_battleStatus = RunBattle(&hero, NULL);
    BattleArmyToArmy(m_battlefield.GetArmy(0), hero.GetArmy());
    BattleArmyToArmy(m_battlefield.GetArmy(1), army);
    ArmyToBattleArmy(army, oppArmy);
    exp = BattleSummaryVsArmy(hero, heroArmyOrig, oppArmy, oppArmyOrig);
}

Battle::BattleControl::BattleControl(Heroes &hero, Castle &castle, const Maps::Tiles &tile, u32 &exp)
: m_battlefield(&castle, &hero, NULL, hero.GetArmy(), castle.GetArmy(), g_baseOffset, tile)
{
    Army::BattleArmy_t heroArmyOrig, oppArmy, oppArmyOrig;
    ArmyToBattleArmy(hero.GetArmy(), heroArmyOrig);
    HeroBase *hero2 = castle.GetHeroes() != &hero ? castle.GetHeroes() : NULL;
    if(hero2)
    {
        u8 idx = 0;
        while(idx < ARMYMAXTROOPS)
        {
            Army::Troop &troop = castle.GetArmy().At(idx);
            if(!hero2->GetArmy().JoinTroop(troop))
                break;
            troop.SetCount(0);
            idx++;
        }
        
        ArmyToBattleArmy(hero2->GetArmy(), oppArmyOrig);
    }
    else
    {
        if(castle.GetCaptain().isValid())
            hero2 = &castle.GetCaptain();
        ArmyToBattleArmy(castle.GetArmy(), oppArmyOrig);
    }
    m_battlefield.SetHero(1, hero2);

    m_commanders[0] = hero.GetArmy().GetCommander();
    m_commanders[1] = castle.GetArmy().GetCommander();
    
    m_battleStatus = RunBattle(&hero, hero2);
    BattleArmyToArmy(m_battlefield.GetArmy(0), hero.GetArmy());
    if(hero2)
    {
        BattleArmyToArmy(m_battlefield.GetArmy(1), hero2->GetArmy());
        exp = BattleSummaryVsHero(hero, heroArmyOrig, *hero2, oppArmyOrig);
    }
    else
    {
        BattleArmyToArmy(m_battlefield.GetArmy(1), castle.GetArmy());
        exp = BattleSummaryVsArmy(hero, heroArmyOrig, oppArmy, oppArmyOrig);
    }

    if(m_battleStatus == Army::WIN) //TODO: Vanquish present hero?
        castle.GetArmy().Clear();
}

Battle::BattleControl::~BattleControl()
{
    if(m_gui) delete m_gui;
}

bool Battle::BattleControl::ShowLeftHeroResults(HeroBase *right, Army::battle_t result)
{
    if(right && world.GetKingdom(right->GetColor()).Control() == Game::LOCAL &&
       world.GetMyKingdom().GetColor() == right->GetColor())
        return false;
    else return true;
}

u32 Battle::BattleControl::CalculateRaisedDead(HeroBase &hero, u32 perished)
{
    float mod = 0.0f;
    switch(hero.GetLevelSkill(Skill::Secondary::NECROMANCY))
    {
        case Skill::Level::BASIC:
            mod = 0.1f;
            break;
        case Skill::Level::ADVANCED:
            mod = 0.2f;
            break;
        case Skill::Level::EXPERT:
            mod = 0.3f;
            break;
        default:
            break;
    }
    return static_cast<u32>(perished * mod);
}

Spell::spell_t Battle::BattleControl::GetLearnedSpell(HeroBase &hero, std::vector<Spell::spell_t> &castSpells)
{
    u8 maxLevel = 0;
    u8 chance = 0;
    switch(hero.GetLevelSkill(Skill::Secondary::EAGLEEYE))
    {
        case Skill::Level::BASIC:
            maxLevel = 2;
            chance = 1;
            break;
        case Skill::Level::ADVANCED:
            maxLevel = 3;
            chance = 2;
            break;
        case Skill::Level::EXPERT:
            maxLevel = 4;
            chance = 3;
            break;
        default:
            break;
    }
    std::vector<Spell::spell_t> availableSpells;
    std::vector<Spell::spell_t>::const_iterator it1 = castSpells.begin();
    std::vector<Spell::spell_t>::const_iterator it2 = castSpells.end();
    for(; it1 != it2; it1++)
        if(Spell::Level(*it1) <= maxLevel)
            availableSpells.push_back(*it1);
    if(!availableSpells.size())
        return Spell::NONE;
    Spell::spell_t learned = *Rand::Get<Spell::spell_t>(availableSpells);
    if(Rand::Get(0, 10) <= chance)
        return learned;
    else return Spell::NONE;
}

u32 Battle::BattleControl::BattleSummaryVsArmy(HeroBase &hero, const Army::BattleArmy_t &heroOrig, const Army::BattleArmy_t &army, const Army::BattleArmy_t &armyOrig)
{
    Army::ArmyPairs armies;
    Army::BattleArmy_t ownArmy;
    ArmyToBattleArmy(hero.GetArmy(), ownArmy);
    armies.push_back(std::make_pair(&ownArmy, &heroOrig));
    armies.push_back(std::make_pair(&army, &armyOrig));
    u32 deadRaised = Army::WIN == m_battleStatus ? CalculateRaisedDead(hero, m_perished) : 0;
    BattleSummary(hero, m_experience[0], armies, NULL, Spell::NONE, deadRaised, m_battleStatus);
    return Army::WIN == m_battleStatus ? m_experience[0] : 0;
}

u32 Battle::BattleControl::BattleSummaryVsHero(HeroBase &hero, const Army::BattleArmy_t &heroOrig, HeroBase &hero2, const Army::BattleArmy_t &hero2Orig)
{
    Army::ArmyPairs armies;
    Army::BattleArmy_t ownArmy, otherArmy;
    ArmyToBattleArmy(hero.GetArmy(), ownArmy);
    ArmyToBattleArmy(hero2.GetArmy(), otherArmy);
    armies.push_back(std::make_pair(&ownArmy, &heroOrig));
    armies.push_back(std::make_pair(&otherArmy, &hero2Orig));
    Spell::spell_t eagleEye(Spell::NONE);
    u32 deadRaised = 0;
    const BagArtifacts *artifacts = NULL;
    HeroBase *target = NULL, *other = NULL;
    u32 exp = 0;
    bool leftResults = ShowLeftHeroResults(&hero2, m_battleStatus);
    bool success = false;
    if(leftResults)
    {
        if(m_battleStatus == Army::WIN)
        {
            success = true;
            target = &hero;
            other = &hero2;
            exp = m_experience[0];
        }
        else
        {
            target = &hero2;
            other = &hero;
        }
    }
    else
    {
        if(m_battleStatus == Army::LOSE)
        {
            success = true;
            target = &hero2;
            other = &hero;
            exp = m_experience[1];
        }
        else
        {
            target = &hero;
            other = &hero2;
        }
    }
    if(success)
    {
        artifacts = &other->GetBagArtifacts();
        deadRaised = CalculateRaisedDead(*target, m_perished);
        eagleEye = GetLearnedSpell(*target, m_spellsCast);
    }
    BattleSummary(*target, exp, armies, artifacts, eagleEye, deadRaised, m_battleStatus);
    return exp;
}

void Battle::BattleControl::InitializeLogicSettings(HeroBase *hero1, HeroBase *hero2)
{
    Army::army_t army[2] = {
        Army::army_t(m_commanders[0]),
        Army::army_t(m_commanders[1])
    };
    Army::BattleArmyToArmy(m_battlefield.GetArmy(0), army[0]);
    Army::BattleArmyToArmy(m_battlefield.GetArmy(1), army[1]);

    const Army::BattleTroop &troop = m_battlefield.GetArmy(1)[1];
    printf("%d %d\n", troop.GetID(), troop.Count());

    if((hero1 && world.GetKingdom(hero1->GetColor()).Control() == Game::LOCAL) ||
       world.GetKingdom(army[0].GetColor()).Control() == Game::LOCAL)
        BattleSettings::Get().ResetModes(BattleSettings::OPT_LOGICONLY);
    else if((hero2 && world.GetKingdom(hero2->GetColor()).Control() == Game::LOCAL) ||
            world.GetKingdom(army[1].GetColor()).Control() == Game::LOCAL)
        BattleSettings::Get().ResetModes(BattleSettings::OPT_LOGICONLY);
    else BattleSettings::Get().SetModes(BattleSettings::OPT_LOGICONLY);
}

Army::battle_t Battle::BattleControl::RunBattle(HeroBase *hero1, HeroBase *hero2)
{
    InitializeLogicSettings(hero1, hero2);

    m_experience[0] = 0;
    m_experience[1] = 0;
    m_perished = 0;
    
    bool haveDisplay = !(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY));
    
    if(haveDisplay)
        m_gui = new GUI(hero1, hero2);
    else m_gui = NULL;
    
    Army::battle_t battle_status = Army::NONE;
    GUI::interaction_t status = GUI::NONE;

    if(haveDisplay)
    {
        m_battlefield.Redraw();
        m_gui->Redraw();
        display.Flip();
    }

    IndexList troopOrder;
    m_battlefield.GetTroopOrdering(troopOrder);
    s8 currentTroop = 0;

    BattleTurn *turn[2];
    turn[0] = CreateTurn(m_commanders[0], m_battlefield.GetArmy(0), m_battlefield.GetArmy(1));
    turn[1] = CreateTurn(m_commanders[1], m_battlefield.GetArmy(1), m_battlefield.GetArmy(0));

    bool validMove = false; //FIXME: Hack so that battles terminate if no unit can move
    
    NewTurn();
    
    while(1)
    {
        status = GUI::NONE;
        
        // Sanity check in case opposing army is killed via spells
        if(!Army::isArmyValid(m_battlefield.GetArmy(0)) || !Army::isArmyValid(m_battlefield.GetArmy(1)))
          break;
        
        Point move(-1, -1), attack(-1, -1);

        if(currentTroop == (s8)troopOrder.size())
        {
            if(!validMove) //HACK for termination if battle is halted after full turn
            {
                Error::Warning("Battle ended due to faulty AI.");
                break;
            }
            else validMove = false;
        }

        for(u16 idx = 0; idx < troopOrder.size(); idx++)
        {
            Army::BattleTroop &troop = m_battlefield.GetTroopFromIndex(troopOrder[idx]);
            troop.SetMoving(Army::NOT_MOVING);
        }
        
        Army::BattleTroop &troop = NextValidTroop(currentTroop, troopOrder);
        TroopIndex troopIdx = troopOrder[currentTroop - 1];
        u8 currentTurn = m_battlefield.GetSideFromIndex(troopIdx);
        
        troop.ResetModes(Army::ATTACKED);
        troop.SetMoving(Army::SELECTED);

        AdjustMorale(turn[currentTurn]->GetHero(), troop);
        
        if(troop.Modes(Army::MORALE_BAD))
        {
            if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
            {
                std::string str = _("Low morale causes the %{name} to freeze in panic.");
                std::string monst = troop.GetName();
                String::Lower(monst);
                String::Replace(str, "%{name}", monst);
                m_gui->Status(str);
                m_gui->Redraw();
                m_battlefield.AnimateMorale(false, troop);
            }
            continue;
        }
        
        status = turn[currentTurn]->Run(troop, troopIdx, move, attack);
        if(status == GUI::SPELL)
        {
            PerformMagic(turn[currentTurn]->GetSpellTargets(), m_battlefield.GetHero(currentTurn), turn[currentTurn]->GetSpell());
            currentTroop--;
            continue;
        }
        else if(status == GUI::AUTO)
        {
            HeroBase *hero = turn[currentTurn]->GetHero();
            Army::BattleArmy_t &own = turn[currentTurn]->GetOwnArmy();
            Army::BattleArmy_t &opp = turn[currentTurn]->GetOppArmy();
            delete turn[currentTurn];
            turn[currentTurn] = CreateTurn(hero, own, opp, true);
            currentTroop--;
            continue;
        }
        else if(status == GUI::SKIP)
        {
            validMove = true;
            continue;
        }
        else if(status != GUI::NONE)
            break;

        if(!PerformMove(troopIdx, move, !BfValid(attack)) && !BfValid(attack))
        {
            Error::Warning("Battle: invalid move attempted");
            continue;
        }
        else validMove = true;
        
        if(!BfValid(attack))
            continue;

        bool attackerAlive = true;
        bool defenderAlive = true;

        troop.SetMoving(Army::NOT_MOVING);
        
        while(1)
        {
            TroopIndex defIdx;
            Army::BattleTroop *defender = NULL;
            
            defIdx = m_battlefield.FindTroop(turn[currentTurn]->GetOppArmy(), attack);
            defender = &turn[currentTurn]->GetOppArmy()[defIdx];
            
            if(PerformAttack(troopIdx, attack))
            {
                defender->SetModes(Army::RETALIATED);
                TroopIndex defIdx = m_battlefield.GetIndexFromTroop(*defender);
                PerformAttack(defIdx, troop.Position()); //FIXME: bug #2399649
                
                attackerAlive = troop.isValid();
            }
            m_battlefield.CleanupBodies();
            defenderAlive = defender->isValid();

            if(!attackerAlive || !defenderAlive || !troop.isTwiceAttack() || troop.Modes(Army::ATTACKED) ||
               (troop.Modes(Army::HANDFIGHTING) && troop.isArchers()))
                break;
            
            troop.SetModes(Army::ATTACKED);
        }
        
        if(!Army::isArmyValid(m_battlefield.GetArmy(0)) || !Army::isArmyValid(m_battlefield.GetArmy(1)))
          break;

        if(troop.Modes(Army::MORALE_GOOD))
        {
            currentTroop--;
            if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
            {
                std::string str = _("High morale enables the %{name} to attack again.");
                std::string monst = troop.GetName();
                String::Lower(monst);
                String::Replace(str, "%{name}", monst);
                m_gui->Status(str);
                m_gui->Redraw();
                m_battlefield.AnimateMorale(true, troop);
            }
        }
    }

    if(status == GUI::NONE)
    {
        if(!Army::isArmyValid(m_battlefield.GetArmy(0)))
            battle_status = Army::LOSE;
        else if(!Army::isArmyValid(m_battlefield.GetArmy(1)))
            battle_status = Army::WIN;
    }
    else if(status == GUI::RETREAT)
        battle_status = Army::RETREAT;
    else if(status == GUI::SURRENDER)
        battle_status = Army::SURRENDER;

    for(u8 i = 0; i < 2; i++)
        if(m_battlefield.GetHero(i))
            m_battlefield.GetHero(i)->ResetModes(Heroes::SPELLCASTED);

    delete turn[0];
    delete turn[1];

    return battle_status;
}

Battle::BattleTurn *Battle::BattleControl::CreateTurn(const Skill::Primary *hero, Army::BattleArmy_t &ownArmy, Army::BattleArmy_t &oppArmy, bool forceComputer /* = false */)
{
    const HeroBase *actualHero = dynamic_cast<const HeroBase *>(hero);
    if(!forceComputer && hero && world.GetKingdom(hero->GetColor()).Control() == Game::LOCAL)
        return new HumanTurn(&m_battlefield, m_gui, const_cast<HeroBase *>(actualHero), ownArmy, oppArmy);
    else return new ComputerTurn(&m_battlefield, m_gui, const_cast<HeroBase *>(actualHero), ownArmy, oppArmy);
}

void Battle::BattleControl::NewTurn()
{
    //Error::Warning("Battle::NewTurn");
    
    Army::NewTurn(m_battlefield.GetArmy(0));
    Army::NewTurn(m_battlefield.GetArmy(1));
    for(u8 i = 0; i < 2; i++)
        if(m_battlefield.GetHero(i))
            m_battlefield.GetHero(i)->ResetModes(Heroes::SPELLCASTED);

    int times = 1 + m_battlefield.GetHero(0)->GetLevelSkill(Skill::Secondary::BALLISTICS) / 2;
    if(times == 1 && m_battlefield.GetHero(0)->HasArtifact(Artifact::BALLISTA))
        times++;
    while(times--)
    {
        if(m_battlefield.GetCatapult())
            m_battlefield.GetCatapult()->Fire(m_battlefield, *m_battlefield.GetCastle());
    }

    BattleCastle *castle = m_battlefield.GetCastle();
    if(castle)
    {
        Battle::BattleTurn *tempTurn = CreateTurn(m_battlefield.GetHero(1),
                                                  m_battlefield.GetArmy(1),
                                                  m_battlefield.GetArmy(0),
                                                  true);
        Battle::ComputerTurn *compTurn = (Battle::ComputerTurn *)tempTurn;
        Point attack;

        BattleCastle::CastlePieces piece = BattleCastle::TOWER_KEEP;
        attack = compTurn->GetMostDangerousOpposingUnit();
        PerformTowerAttack(attack, castle->getCastlePoint(piece), piece);

        if(castle->hasLeft())
        {
            piece = BattleCastle::LEFT_TOWER;
            attack = compTurn->GetMostDangerousOpposingUnit();
            PerformTowerAttack(attack, castle->getCastlePoint(piece), piece);
        }
        
        if(castle->hasRight())
        {
            piece = BattleCastle::RIGHT_TOWER;
            attack = compTurn->GetMostDangerousOpposingUnit();
            PerformTowerAttack(attack, castle->getCastlePoint(piece), piece);
        }

        delete tempTurn;
    }
}

void Battle::BattleControl::PerformTowerAttack(const Point &attack, const Point &from, BattleCastle::CastlePieces piece)
{
    Army::Troop archer(Monster::ARCHER, m_battlefield.GetCastle()->countBuildings());
    if(piece != BattleCastle::TOWER_KEEP)
        archer.SetCount(archer.Count() / 2);
    long damage = Rand::Get(archer.GetDamageMin(), archer.GetDamageMax());
    
    TroopIndex idx = m_battlefield.FindTroop(m_battlefield.GetArmy(0), attack);
    Army::BattleTroop &troop = m_battlefield.GetTroopFromIndex(idx);
    
    if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        Point start = Bf2Scr(from);
        u8 state = MISSILE_FLIGHT;
        Point delta = Bf2Scr(attack) - start;
        int icnframe=-1, frame=0;
        u32 animat = 0;
        int maxind = AGG::GetICNCount(ICN::KEEP);
        double angle = M_PI_2 - atan2(-delta.y, delta.x);
        icnframe = (int)(angle/M_PI * maxind);
        
        Monster::animstate_t targetstate;
        M82::m82_t targetsound;
        if(troop.IsDamageFatal(damage)) {
            targetstate = Monster::AS_DIE;
            targetsound = troop.M82Kill();
        } else {
            targetstate = Monster::AS_PAIN;
            targetsound = troop.M82Wnce();
        }

        const int deltaX = abs(from.x - attack.x);
        const int deltaY = abs(from.y - attack.y);
        const int MISSFRAMES = std::max(std::max(deltaX, deltaY) * 2 / 3, 1); //avoid divide by zero
        delta.x /= MISSFRAMES;
        delta.y /= MISSFRAMES;

        while(LocalEvent::GetLocalEvent().HandleEvents())
        {
            int scale = state == MISSILE_FLIGHT ? 2 : 4;
            if(Game::ShouldAnimateInfrequent(animat++, scale))
            {
                m_battlefield.Redraw();

                if(state == MISSILE_FLIGHT)
                {    
                    start += delta;
                    frame ++;
                    if(frame >= MISSFRAMES)
                    {
                        state++;
                        frame = 0;
                    }
                    display.Blit(AGG::GetICN(ICN::KEEP, abs(icnframe), icnframe < 0), start);
                }
                else if(state == MISSILE_IMPACT)
                {
                    if(!frame)
                    {
                        int perished = troop.ApplyDamage(damage);
                        troop.Animate(targetstate);
                        AGG::PlaySound(targetsound);
                        std::string message = _("%{tower} does %{dmg} damage.");
                        String::Replace(message, "%{dmg}", damage);
                        if(perished)
                        {
                            std::string creature = troop.GetName(perished);
                            String::Lower(creature);
                            message += _("  %{perished} %{creature} %{perish}.");
                            String::Replace(message, "%{creature}", creature);
                            String::Replace(message, "%{perished}", perished);
                            if(perished != 1)
                                String::Replace(message, "%{perish}", _("perish"));
                            else String::Replace(message, "%{perish}", _("perishes"));
                        }
                        if(piece == BattleCastle::TOWER_KEEP)
                            String::Replace(message, "%{tower}", "Garrison");
                        else String::Replace(message, "%{tower}", "Tower");
                        m_gui->Status(message);
                        frame++;
                    }
                    else if(troop.astate != Monster::AS_NONE) troop.Animate();
                    else break;
                }

                m_gui->Redraw();
                display.Flip();
            }
        }

        
    }
            
    m_battlefield.CleanupBodies();
}

Army::BattleTroop &Battle::BattleControl::NextValidTroop(s8 &currentTroop, IndexList &troopOrder)
{
    Army::BattleTroop *troop;
    do
    {
        if((u8)currentTroop == troopOrder.size())
        {
            currentTroop = 0;
            NewTurn();
        }
        TroopIndex troopIdx = troopOrder[currentTroop++];
        troop = &m_battlefield.GetTroopFromIndex(troopIdx);
    } while(!troop->isValid());
    return *troop;
}

void Battle::BattleControl::AdjustMorale(HeroBase *hero, Army::BattleTroop &troop)
{
    if(!troop.isAffectedByMorale())
        return;
    
    int m = Rand::Get(1, MORALEBOUND);
    if(hero && hero->GetMorale() >= m)
        troop.SetModes(Army::MORALE_GOOD);
    else if(hero && hero->GetMorale() <= -m)
        troop.SetModes(Army::MORALE_BAD);
}

namespace Battle
{
    
    class MoveAction
    {
      public:
        MoveAction(Army::BattleTroop &);
        virtual ~MoveAction();
        virtual bool Step() = 0;

      protected:
        Point m_end;
        Point m_step;
        Army::BattleTroop *m_troop;
        u8 m_st, m_len, m_prep, m_post;
        u8 m_curstep, m_numsteps, m_part;
        u8 m_frame;
    };

    class WalkAction : public MoveAction
    {
      public:
        WalkAction(Battlefield &, const Point &, Army::BattleTroop &, PointList *);
        ~WalkAction();
        bool Step();

      private:
        PointList *m_path;
        Battlefield &m_battlefield;
    };

    class FlyAction : public MoveAction
    {
      public:
        FlyAction(const Point &, Army::BattleTroop &);
        bool Step();
    };

}

Battle::MoveAction::MoveAction(Army::BattleTroop &troop)
: m_troop(&troop)
{
    m_frame = m_curstep = m_numsteps = m_st = m_prep = m_part = m_post = m_len = 0;
    m_troop->SetMoving(Army::IN_MOTION);
}

Battle::MoveAction::~MoveAction()
{
}
    
Battle::WalkAction::WalkAction(Battlefield &battlefield, const Point &start, Army::BattleTroop &myTroop, PointList *path)
: Battle::MoveAction(myTroop)
, m_path(path)
, m_battlefield(battlefield)
{
    if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        m_step = m_path->back() - myTroop.Position();
        myTroop.SetReflect(m_step.x < 0);
        if(myTroop.isWide() && myTroop.WasReflected() != myTroop.IsReflected())
            myTroop.SetPosition(myTroop.Position() + Point( myTroop.WasReflected() ? -1 : 1, 0 ));
    }
    else
    {
        myTroop.SetScreenPosition(Bf2Scr(myTroop.Position()));
        myTroop.GetAnimFrames(Monster::AS_WALK, m_st, m_len);
        m_step.x = (Bf2Scr(m_path->back()).x - myTroop.ScreenPosition().x) / m_len;
        m_step.y = (Bf2Scr(m_path->back()).y - myTroop.ScreenPosition().y) / m_len;
        myTroop.SetReflect(m_step.x < 0);
        if(myTroop.isWide() && myTroop.WasReflected() != myTroop.IsReflected())
            myTroop.SetPosition(myTroop.Position() + Point( myTroop.WasReflected() ? -1 : 1, 0 ));
        if(m_step.y) m_step.x = -m_step.x;
        else m_step.x = 0;
        myTroop.SetScreenPosition(Bf2Scr(myTroop.Position()));

        myTroop.Animate(Monster::AS_WALK);

        /*DrawTroop(myTroop);
          Display::Get().Flip();
          while(le.HandleEvents())
          {
          if(le.KeyPress(KEY_RETURN))
          break;
          }*/
    }
    
    if(m_battlefield.GetCastle()
    && (std::find(m_path->begin(), m_path->end(), Point(6,4)) != m_path->end()
    || std::find(m_path->begin(), m_path->end(), Point(5,4)) != m_path->end()))
    {
        m_battlefield.GetCastle()->OpenDrawbridge();
    }
}
    
Battle::WalkAction::~WalkAction()
{
    TroopIndex idx = m_battlefield.GetIndexFromTroop(*m_troop);
    idx += idx < 0 ? -5 : 5; //FIXME: This is a gross hack to ensure that we don't actually check for a real troop
    
    if(m_battlefield.GetCastle()
    && m_battlefield.CellFree(Point(5,4), idx)
    && m_battlefield.CellFree(Point(6,4), idx))
        m_battlefield.GetCastle()->CloseDrawbridge();
    
    delete m_path;
}
    
bool Battle::WalkAction::Step()
{
    Army::BattleTroop &myTroop = *m_troop;

    if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        myTroop.SetPosition(m_path->back());
        m_path->pop_back();
        m_step.x = m_path->back().x - myTroop.Position().x;
        m_step.y = m_path->back().y - myTroop.Position().y;
        myTroop.SetReflect(m_step.x < 0);
        if(myTroop.isWide() && myTroop.IsReflected() != myTroop.OriginalReflection())
            myTroop.SetPosition(myTroop.Position() + Point( myTroop.IsReflected() ? 1 : -1, 0 ));
    }
    else
    {
        myTroop.SetScreenPosition(myTroop.ScreenPosition() + m_step);
        if(myTroop.astate != Monster::AS_WALK && m_path->size())
        {
            if(m_path->size() > 1)
                AGG::PlaySound(myTroop.M82Move());
            myTroop.SetPosition(m_path->back());
            myTroop.SetScreenPosition(Bf2Scr(myTroop.Position()));
            m_path->pop_back();
            if(!m_path->size())
                return false;
            m_step.x = (Bf2Scr(m_path->back()).x - myTroop.ScreenPosition().x) / m_len;
            m_step.y = (Bf2Scr(m_path->back()).y - myTroop.ScreenPosition().y) / m_len;
            myTroop.SetReflect(m_step.x < 0);
            if(myTroop.isWide() && myTroop.IsReflected() != myTroop.OriginalReflection())
                myTroop.SetPosition(myTroop.Position() + Point( myTroop.IsReflected() ? 1 : -1, 0 ));
            myTroop.SetScreenPosition(Bf2Scr(myTroop.Position()));
            if(m_step.y) m_step.x = -m_step.x;
            else m_step.x = 0;
            myTroop.Animate(Monster::AS_WALK);

            /*DrawTroop(myTroop);
              Display::Get().Flip();
              while(le.HandleEvents())
              {
              if(le.KeyPress(KEY_RETURN))
              break;
              }*/
        }
        m_frame++;
    }

    return !m_path->size();
}

Battle::FlyAction::FlyAction(const Point &move, Army::BattleTroop &myTroop)
: Battle::MoveAction(myTroop)
{
    const Point &start = Bf2Scr(myTroop.Position());
    if(myTroop.isWide())
        m_end = Bf2Scr(move + Point( move.x < myTroop.Position().x ? 1 : 0 ) );
    else m_end = Bf2Scr(move);

    myTroop.SetMoving(Army::IN_MOTION);
        
    myTroop.SetReflect(m_end.x < start.x);
    if(myTroop.isWide() && myTroop.WasReflected() != myTroop.IsReflected())
        myTroop.SetPosition(myTroop.Position() + Point( myTroop.WasReflected() ? -1 : 1, 0 ));

    if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        m_step.x = myTroop.Position().x - move.x;
        m_step.y = myTroop.Position().y - move.y;
    }
    else
    {
        myTroop.SetScreenPosition(Bf2Scr(myTroop.Position()));

        myTroop.GetAnimFrames(Monster::AS_WALK, m_st, m_len);
        if(m_len >= 12) m_prep = m_post = 4;
        else if(m_len >= 8) m_prep = m_post = 2;
        else m_prep = m_post = 1;
        m_step = m_end - start;
        const int deltaX = abs(myTroop.Position().x - move.x);
        const int deltaY = abs(myTroop.Position().y - move.y);
        m_numsteps = std::max(std::max(deltaX, deltaY), 1); // avoid divide by zero
        m_step.x /= m_numsteps;
        m_step.y /= m_numsteps;

        myTroop.Animate(Monster::AS_WALK);
    }
}
    
bool Battle::FlyAction::Step()
{
    Army::BattleTroop &myTroop = *m_troop;
    if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        myTroop.SetPosition(myTroop.Position() + m_step);
        //return myTroop.Position() == Scr2Bf(m_end);
        return true; //HACK????
    }
    else
    {
        if(m_frame >= m_prep && !m_part) {
            AGG::PlaySound(myTroop.M82Move());
            m_part = 1;
        }
        if(m_part == 1) {
            myTroop.SetScreenPosition(myTroop.ScreenPosition() + m_step);
            m_curstep ++;
            if(myTroop.aframe >= m_st + m_len - m_post - 1) {
                AGG::PlaySound(myTroop.M82Move());
                myTroop.aframe = m_st + m_prep;
            }
            if(m_curstep == m_numsteps) {
                m_part = 3;
                myTroop.SetScreenPosition(m_end);
                myTroop.aframe = m_st + m_len - m_post;
            }
        }
        m_frame++;
        return (m_part == 3 && myTroop.astate != Monster::AS_WALK);
    }
}
    
bool Battle::BattleControl::PerformMove(TroopIndex troopN, const Point &move, bool resetReflection)
{
    Army::BattleTroop &myTroop = m_battlefield.GetTroopFromIndex(troopN);
    u16 animat = 0;
    
    if(BfValid(move) && move != myTroop.Position())
    {
        PointList *path = NULL;
        MoveAction *action;
        
        if(myTroop.isFly())
            action = new FlyAction(move, myTroop);
        else
        {
            path = m_battlefield.FindPath(myTroop.Position(), move, myTroop.GetSpeed(), myTroop, troopN);
            if(path)
                action = new WalkAction(m_battlefield, move, myTroop, path);
            else
            {
                //Dialog::Message(_("Error"), _("Path not found!"), Font::BIG, Dialog::OK);
                Error::Warning("Path not found!");
                return false;
            }
        }
        while(le.HandleEvents())
        {
            bool shouldAnimate;

            if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
                shouldAnimate = true;
            else
            {
                if(myTroop.isFly())
                    shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 4);
                else
                    shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 2);
            }
            
            if(shouldAnimate) {
                if(action->Step())
                    break;
                if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
                {
                    m_battlefield.Redraw();
                    m_gui->Redraw();
                    display.Flip();
                }
            }
        }
        delete action;
            
        myTroop.SetPosition(move);
        if(myTroop.isWide() && myTroop.IsReflected() != myTroop.OriginalReflection())
            myTroop.SetPosition(myTroop.Position() + Point( myTroop.IsReflected() ? 1 : -1, 0 ));
    }

    if(resetReflection)
    {
        myTroop.ResetReflection();
        if(myTroop.isWide() && myTroop.IsReflected() != myTroop.WasReflected())
            myTroop.SetPosition(myTroop.Position() + Point( myTroop.IsReflected() ? 1 : -1, 0 ));
    }
    
    return true;
}

long Battle::BattleControl::CalculateDamage(const Army::BattleTroop &attacker, const Army::BattleTroop &defender)
{
    if(attacker.GetID() == Monster::GENIE && Rand::Get(0, 100) < 9)
    {
        u32 half = static_cast<u32>(defender.Count() * 1.0f / 2 + 0.5f);
        long damage = (half - 1) * defender.Monster::GetHitPoints() + defender.GetHitPoints();
        return damage;
    }
    
    long damage = attacker.GetDamageVersus(defender);

    u8 defense = defender.GetDefense();

    if(m_battlefield.GetCastle())
    {
        if(m_battlefield.GetCastle()->IsCellInMoat(defender.Position()))
        {
            if(defense - 3 > 0)
                defense -= 3;
            else defense = 1;
        }

        if(attacker.Modes(Army::HANDFIGHTING))
            if(m_battlefield.IsRangedAttackBlocked(attacker, defender))
                damage /= 2;
    }

    float d = attacker.GetAttack() - defense;
    d *= d > 0 ? 0.1f : 0.05f;
    if(d < -0.3f)
        d = 0.3f;
    else if(d > 3.0f)
        d = 3.0f;
    damage = (long)(((float)damage) * (1 + d));
    if(damage < 1)
        damage = 1;

    return damage;
}

void Battle::BattleControl::PerformAttackPreLogic(Army::BattleTroop &attacker, const std::vector<Army::BattleTroop *> &targets, bool ranged)
{
    TroopIndex tidx = m_battlefield.GetIndexFromTroop(attacker);
    u8 side = m_battlefield.GetSideFromIndex(tidx);
    for(u16 tid = 0; tid < targets.size(); tid++)
    {
        Army::BattleTroop &target = *targets[tid];
            
        long damage = CalculateDamage(attacker, target);
        target.damageToApply = damage;
            
        //TODO: Don't gain experience from damage to own troop
        m_experience[side] += std::min(damage, (long)target.TotalHP());
    }
}

bool Battle::BattleControl::PerformAttackLogic(Army::BattleTroop &attacker, const std::vector<Army::BattleTroop *> &targets, bool ranged, long &damage, u16 &perished)
{
    bool retaliate = true;
    
    for(u16 tid = 0; tid < targets.size(); tid++)
    {
        Army::BattleTroop &target = *targets[tid];
        int tempPerished = target.ApplyDamage(target.damageToApply);
        if(!tid) //Only the original target matters
        {
            perished = tempPerished;
            damage = target.damageToApply;
            if(!target.Count())
                retaliate = false;
        }
        m_perished += tempPerished;
    }

    if(attacker.GetID() == Monster::GHOST)
        attacker.SetCount(attacker.Count() + perished);

    if(ranged) attacker.shots --;

    return retaliate;
}

void Battle::BattleControl::PerformAttackAnimation(Army::BattleTroop &attacker, const std::vector<Army::BattleTroop *> &targets, bool ranged)
{
    u16 animat = 0;
    u8 targetLen = 0;
    std::vector<std::pair<Monster::animstate_t, M82::m82_t> > targetActions;

    for(u16 tid = 0; tid < targets.size(); tid++)
    {
        Army::BattleTroop &target = *targets[tid];
        long damage = target.damageToApply;
        Monster::animstate_t targetstate;
        M82::m82_t targetsound;
        if(target.IsDamageFatal(damage)) {
            targetstate = Monster::AS_DIE;
            targetsound = target.M82Kill();
        } else {
            targetstate = Monster::AS_PAIN;
            targetsound = target.M82Wnce();
        }
        targetActions.push_back(std::make_pair(targetstate, targetsound));
            
        u8 start, currentTargetLen;
        target.GetAnimFrames(targetstate, start, currentTargetLen);
        targetLen = std::max(currentTargetLen, targetLen);
    }

    int state = PREATTACK, missframe = 0, missindex = 0;
    Point miss_start(attacker.Position().x + (attacker.isWide() ? (attacker.IsReflected() ? -1 : 1): 0), attacker.Position().y);
    miss_start = Bf2Scr(miss_start);
    Point miss_step(Bf2Scr(targets[0]->Position()));
    miss_step -= miss_start;
    const int deltaX = abs(attacker.Position().x - targets[0]->Position().x);
    const int deltaY = abs(attacker.Position().y - targets[0]->Position().y);
    const int MISSFRAMES = std::max(std::max(deltaX, deltaY) * 2 / 3, 1); //avoid divide by zero
    miss_step.x /= MISSFRAMES;
    miss_step.y /= MISSFRAMES;
    miss_start.y -= CELLH;
        
    u8 mytrooptate;
    if(targets[0]->Position().y > attacker.Position().y)
        mytrooptate = Monster::AS_ATT3P;
    else if(targets[0]->Position().y < attacker.Position().y)
        mytrooptate = Monster::AS_ATT1P;
    else
        mytrooptate = Monster::AS_ATT2P;

    bool reflect;
    if(attacker.Position().y % 2 == 0)
        reflect = attacker.Position().x >= targets[0]->Position().x;
    else reflect = attacker.Position().x > targets[0]->Position().x;
    attacker.SetReflect(reflect);
    if(attacker.isWide() && attacker.WasReflected() != attacker.IsReflected())
        attacker.SetPosition(attacker.Position() + Point( attacker.WasReflected() ? -1 : 1, 0 ));
        
    int delayFrames;
         
    u8 start, prepLen, attackLen;
    attacker.GetAnimFrames(mytrooptate & ~Monster::AS_ATTPREP, start, attackLen);
    attacker.GetAnimFrames(Monster::AS_ATTPREP, start, prepLen);
    attacker.attackRanged = ranged;
         
    if(!ranged) {
        AGG::PlaySound(attacker.M82Attk());
        delayFrames = prepLen + attackLen / 3;
    } else {
        AGG::PlaySound(attacker.M82Shot());
        delayFrames = prepLen + attackLen + 3;
        int maxind = AGG::GetICNCount(attacker.ICNMiss());
        if(maxind > 1) {
            double angle = M_PI_2 - atan2(-miss_step.y, miss_step.x);
            missindex = (int)(angle/M_PI * maxind);
        } else missindex = 0;
    }
    attacker.Animate(mytrooptate);
    while(le.HandleEvents())
    {
        bool shouldAnimate = false;
        if(ranged && state == MISSILE_FLIGHT)
            shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 2);
        else
            shouldAnimate = Game::ShouldAnimateInfrequent(++animat, 4);

        if(!shouldAnimate)
            continue;

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
            if(state == PREATTACK && !delayFrames--) 
                state ++;
            else if(state == MISSILE_FLIGHT && missframe >= MISSFRAMES)
            {
                for(u16 tid = 0; tid < targets.size(); tid++)
                {
                    targets[tid]->Animate(targetActions[tid].first);
                    AGG::PlaySound(targetActions[tid].second);
                }
                delayFrames = targetLen;
                state++;
            }
            else if(state == MISSILE_IMPACT && !delayFrames--) break;
        }
        m_battlefield.Redraw();
        m_gui->Redraw();
        if(state == MISSILE_FLIGHT) {
            display.Blit(AGG::GetICN(attacker.ICNMiss(), abs(missindex), missindex < 0), miss_start);
            miss_start += miss_step;
            missframe ++;
        }
        display.Flip();
    }

    attacker.ResetReflection();
    if(attacker.isWide() && attacker.IsReflected() != attacker.WasReflected())
        attacker.SetPosition(attacker.Position() + Point( attacker.IsReflected() ? 1 : -1, 0 ));
}

bool Battle::BattleControl::PerformAttack(TroopIndex troopN, const Point &attack)
{
    Army::BattleTroop &myTroop = m_battlefield.GetTroopFromIndex(troopN);
    bool retaliate = false;
    u16 perished = 0;
    long damage = 0;
    
    if(!BfValid(attack))
        return retaliate;
        
    std::vector<Army::BattleTroop *> targets;
    m_battlefield.GetTargets(targets, myTroop, attack);
        
    Army::BattleTroop & targetTroop = *targets[0];
    if(m_battlefield.IsTroopCloseTo(targetTroop, myTroop))
    {
        myTroop.SetModes(Army::HANDFIGHTING);
        if(targetTroop.CanRetaliateAgainst(myTroop))
            retaliate = true;
    }
    else myTroop.ResetModes(Army::HANDFIGHTING);

    bool ranged = myTroop.isArchers() && myTroop.shots > 0 && !myTroop.Modes(Army::HANDFIGHTING);

    PerformAttackPreLogic(myTroop, targets, ranged);
    
    if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
        PerformAttackAnimation(myTroop, targets, ranged);

    retaliate &= PerformAttackLogic(myTroop, targets, ranged, damage, perished) && !ranged;

    if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        std::string status;
        
        // Heuristic for genies halving monster stack
        if(myTroop.GetID() == Monster::GENIE && abs(targetTroop.Count() - perished) <= 1)
        {
            status = _("%{name} %{destroys} half the enemy troops!");
            if(myTroop.Count() == 1)
                String::Replace(status, "%{destroys}", _("destroys"));
            else String::Replace(status, "%{destroys}", _("destroy"));
            String::Replace(status, "%{name}", myTroop.GetName());
        }
        else
        {
            status = _("%{name} %{does} %{value} damage.");
            if(myTroop.Count() == 1)
                String::Replace(status, "%{does}", _("does"));
            else String::Replace(status, "%{does}", _("do"));
            String::Replace(status, "%{name}", myTroop.GetName());
            String::Replace(status, "%{value}", damage);
        }
        
        if(perished)
        {
            std::string addon = _(" %{value} %{name} %{perishes}.");
            if(perished == 1)
                String::Replace(addon, "%{perishes}", _("perishes"));
            else String::Replace(addon, "%{perishes}", _("perish"));
    	    String::Replace(addon, "%{value}", perished);
            std::string name = targetTroop.GetName(perished);
            String::Lower(name);
    	    String::Replace(addon, "%{name}", name);
            status += addon;
        }

        m_gui->Status(status);
        m_gui->Redraw();
        display.Flip();
    }

    if(!ranged)
    {
        std::vector<Army::BattleTroop*> affected;
        affected.push_back(targets[0]);
        PerformMagic(affected, NULL, Spell::TroopAttack(myTroop));
    }
    
    return retaliate;
}

void Battle::BattleControl::PerformMagicAnimation(std::vector<Army::BattleTroop*> &affected, Spell::spell_t spell, HeroBase *hero)
{
    AGG::PlaySound(Spell::M82(spell));

    u8 spellPower = hero ? hero->GetPower() : 1;
    bool reflect = hero == m_battlefield.GetHero(1);
    m_battlefield.AnimateMagic(affected, m_battlefield.GetHero(0), m_battlefield.GetHero(1), reflect, spell);

    if(!Spell::isBad((u8)spell))
        return;
    
    for(u16 i = 0; i < affected.size(); i++) {
        u16 damage = Spell::GetInflictDamageVersus(spell, spellPower, affected[i]->GetID());
        if(!affected[i]->IsDamageFatal(damage))
            affected[i]->Animate(Monster::AS_PAIN);
        else
            affected[i]->Animate(Monster::AS_DIE);
    }
    
    int animat = 0;
    u16 frame = 0, mframe = AGG::GetICNCount(Spell::Icn(spell));
    while(le.HandleEvents())
    {
        if(Game::ShouldAnimateInfrequent(animat++, 3))
        {
            m_battlefield.Redraw();
            m_gui->Redraw();

            if(Spell::Icn(spell) != ICN::UNKNOWN)
            {
                const Sprite& spr = AGG::GetICN(Spell::Icn(spell), frame);
                for(u16 i = 0; i < affected.size(); i++)
                    display.Blit(spr, Bf2Scr(affected[i]->Position()) + spr);
            }
            
            display.Flip();
            if(frame >= mframe)
            {
                bool br = true;
                for(u16 i = 0; i < affected.size(); i++)
                    if(affected[i]->astate != Monster::AS_NONE)
                    {
                        u8 strt=0, len=0;
                        affected[i]->GetAnimFrames(Monster::AS_DIE, strt, len, false);
                        if(affected[i]->astate != Monster::AS_DIE || affected[i]->aframe < strt+len-1)
                            br = false;
                    }
                if(br) break;
            }
            frame ++;
        }
    }
}

bool Battle::BattleControl::PerformMagic(std::vector<Army::BattleTroop*> &affected, HeroBase *hero, Spell::spell_t spell)
{
    if(spell == Spell::NONE) return false;
    if(!affected.size()) {
        Dialog::Message("", _("That spell will affect no one!"), Font::BIG, Dialog::OK);
        return false;
    }

    u8 spellpower = hero ? hero->GetPower() : 1 ;
    if(hero)
    {
        hero->SetModes(Heroes::SPELLCASTED);
        hero->SetSpellPoints(hero->GetSpellPoints() - Spell::Mana(spell));
    }

    if(!BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
    {
        u16 damage = Spell::GetInflictDamageVersus(spell, spellpower, affected[0]->GetID());
        if(damage)
        {
            std::string str;
            if(affected.size() == 1)
    	    {
    	        str = _("The %{spell} does %{value} damage.");
    	        String::Replace(str, "%{spell}", Spell::String(spell));
                String::Replace(str, "%{value}", damage);
            }
            else
            {
    	        str = _("The %{spell} does %{value} damage to the %{name}.");
    	        String::Replace(str, "%{spell}", Spell::String(spell));
                String::Replace(str, "%{value}", damage);
                std::string name = affected[0]->GetName();
                String::Lower(name);
    	        String::Replace(str, "%{name}", name);
            }
            m_gui->Status(str);
            m_gui->Status("");
            m_gui->Redraw();
        }

        PerformMagicAnimation(affected, spell, hero);
    }
    
    //TODO: message if spell is resisted by single unit?
    for(u16 i = 0; i < affected.size(); i++)
        affected[i]->ApplySpell(spell, spellpower);
    m_battlefield.CleanupBodies();
    
    return true;
}

//////////////////////////

namespace Battle
{
    BattleSettings::BattleSettings()
    {
        if(Settings::Get().BattleGrid())
            SetModes(OPT_GRID);
        else ResetModes(OPT_GRID);

        if(Settings::Get().BattleMovementShaded())
            SetModes(OPT_MOVESHADOW);
        else ResetModes(OPT_MOVESHADOW);

        if(Settings::Get().BattleMouseShaded())
            SetModes(OPT_MOUSESHADOW);
        else ResetModes(OPT_MOUSESHADOW);
    }

    BattleSettings &BattleSettings::Get()
    {
        static BattleSettings g_battleSettings;
        return g_battleSettings;
    }

////////////////////////////////////////////////////////////////////////////

    BattleTurn::BattleTurn(Battlefield *battlefield, GUI *gui, HeroBase *hero, Army::BattleArmy_t &ownArmy, Army::BattleArmy_t &oppArmy)
    : m_battlefield(battlefield)
    , m_gui(gui)
    , m_ownArmy(&ownArmy)
    , m_oppArmy(&oppArmy)
    {
        m_hero[0] = hero;
        m_hero[1] = NULL;
        m_spell = Spell::NONE;
    }

    GUI::interaction_t BattleTurn::Run(Army::BattleTroop &troop, TroopIndex troopIdx, Point &move, Point &attack)
    {
        move = attack = Point(-1, -1);

        PrepMovePoints(troopIdx);
        
        for(u16 j = 0; j < m_oppArmy->size(); j++)
        {
            if(m_battlefield->IsTroopCloseTo((*m_oppArmy)[j], troop))
            {
                troop.SetModes(Army::HANDFIGHTING);
                break;
            }
        }

        return GUI::NONE;
    }

    /** Populate the given troop's available movement cells.
     *  \param troopN  Index of troop for which to populate the possible movement points
     */
    void BattleTurn::PrepMovePoints(TroopIndex troopN)
    {
        m_movePoints.clear();
        Point p;
        const Army::BattleTroop &troop = m_battlefield->GetTroopFromIndex(troopN);
        if(troop.isFly())
        {
            for(p.x = 0; p.x < BFW; p.x ++) 
                for(p.y = 0; p.y < BFH; p.y ++)
                    if(m_battlefield->CellFreeFor(p, troop, troopN))
                        m_movePoints.push_back(p);
        }
        else
        {
            Point p = troop.Position();
            m_movePoints.push_back(p);
            PrepMovePointsInt(troop, p, troop.GetSpeed() + 1, troopN);
        }
    }

    /** Populate the movement vector with any free cells in range of the given point.
     * \param troop   Troop to prepare
     * \param p       Starting point
     * \param move    Movement speed
     * \param skip    Troop id to ignore if found
     */
    void BattleTurn::PrepMovePointsInt(const Army::BattleTroop &troop, const Point &p, int move, int skip)
    {
        if(BfValid(p) && m_battlefield->CellFreeFor(p, troop, skip) && move > 0)
        {
            if(std::find(m_movePoints.begin(), m_movePoints.end(), p) == m_movePoints.end())
                m_movePoints.push_back(p);

            if(m_battlefield->GetCastle()
            && m_battlefield->GetCastle()->IsCellInMoat(p)
            && troop.Position() != p)
            {
                if(p != Point(6,4) && p != Point(5, 4))
                    return;
                else if(m_battlefield->GetSideFromIndex(m_battlefield->GetIndexFromTroop(troop)) != 1)
                    return;
            }
            
            Point d;
            for(d.x = -1; d.x <= 1; d.x++)
                for(d.y = -1; d.y <= 1; d.y++) 
                    if(d.x || d.y )
                    {
                        if(p.y%2 && d.y && d.x>0) continue;
                        if(!(p.y%2) && d.y && d.x<0) continue;
                        PrepMovePointsInt(troop, p+d, move - 1, skip);
                    }
        }
    }

    /** Populate a given vector with predetermined targets for certain classes of spells.
     *  \param[out] targets  Array of troops which will be targets of the spell.
     *  \param[in]  spell    Spell being cast.
     *  \return true if further selection is needed, false otherwise.
     */
    bool BattleTurn::MagicSelectTarget(std::vector<Army::BattleTroop*> &targets, const Spell::spell_t &spell)
    {
        targets.clear();
        switch(Spell::Target(spell))
        {
            case Spell::ONEFRIEND:
            case Spell::ONEENEMY:
            case Spell::FREECELL:
                return true;
            case Spell::ALLFRIEND:
                for(u16 i = 0; i < m_ownArmy->size(); i++)
                    if(Spell::AllowSpell(spell, (*m_ownArmy)[i]))
                        targets.push_back(&(*m_ownArmy)[i]);
                return false;
            case Spell::ALLENEMY:
                for(u16 i = 0; i < m_oppArmy->size(); i++)
                    if(Spell::AllowSpell(spell, (*m_oppArmy)[i]))
                        targets.push_back(&(*m_oppArmy)[i]);
                return false;
            case Spell::ALLLIVE:
            case Spell::ALLDEAD:
            case Spell::ALL:
                for(u16 i = 0; i < m_ownArmy->size(); i++)
                    if(Spell::AllowSpell(spell, (*m_ownArmy)[i]))
                        targets.push_back(&(*m_ownArmy)[i]);
                for(u16 i = 0; i< m_oppArmy->size(); i++)
                    if(Spell::AllowSpell(spell, (*m_oppArmy)[i]))
                        targets.push_back(&(*m_oppArmy)[i]);
                return false;
            case Spell::NOTARGET:
                return false;
        }
        return false;
    }

//////////////////////////////////////////////////////////////////////////////

    HumanTurn::HumanTurn(Battlefield *battlefield, GUI *gui, HeroBase *hero, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2)
    : BattleTurn(battlefield, gui, hero, army1, army2)
    {
    }
    
    GUI::interaction_t HumanTurn::Run(Army::BattleTroop &troop, TroopIndex troopIdx, Point &move, Point &attack)
    {
        BattleTurn::Run(troop, troopIdx, move, attack);

        Army::BattleTroop &myTroop = troop;

        cursor.SetThemes(cursor.WAR_POINTER);
        cursor.Hide();
        //m_battlefield->Redraw();
        //m_gui->Status("");
        m_gui->Redraw();
        cursor.Show();
        display.Flip();

        Army::BattleArmy_t &army1 = troopIdx >= 0 ? *m_ownArmy : *m_oppArmy;
        Army::BattleArmy_t &army2 = troopIdx >= 0 ? *m_oppArmy : *m_ownArmy;
        
        u16 animat = 0;
        Point cur_pt(-1,-1);

        bool mouseMotion = false;

        // dialog menu loop
        while(le.HandleEvents())
        {
            if(le.KeyPress(KEY_RETURN)) {
                for(u16 i = 0; i < army2.size(); i++)
                    army2[i].SetCount(0);
                return GUI::NONE;
            }
            if(le.KeyPress(KEY_BACKSPACE)) {
                for(u16 i = 0; i < army1.size(); i++)
                    army1[i].SetCount(0);
                return GUI::NONE;
            }
            if(le.KeyPress(KEY_f) && m_battlefield->GetCatapult())
            {
                m_battlefield->GetCatapult()->Fire(*m_battlefield, *m_battlefield->GetCastle());
            }
            if(Game::ShouldAnimateInfrequent(++animat, 3)) {
                m_battlefield->Redraw(&m_movePoints, &cur_pt, &myTroop);
                m_gui->Redraw();
                display.Flip();
            }

            if(!(mouseMotion || le.MouseMotion() || le.MouseLeft() || le.MouseRight()))
                continue;
            else mouseMotion = true;

            bool mouseActive = false;
            GUI::interaction_t interaction = m_gui->Interact(m_battlefield->GetIndexFromTroop(myTroop), m_spell, *m_battlefield, mouseActive);
            if(interaction == GUI::SPELL && MagicSelectTarget(m_targets, m_spell))
                return interaction;
            else if(interaction != GUI::NONE)
                return interaction;

            int t = -1;
            bool click;
            cur_pt = Scr2Bf(le.MouseCursor());
	    
            if(!le.MouseLeft()
            && cur_pt == Scr2Bf(le.MousePressLeft())
            && cur_pt == Scr2Bf(le.MouseReleaseLeft()))
            {
                click = true;
                le.MouseClickLeft(Rect(0, 0, display.w(), display.h()));
            } else click = false;
	    
            if(!mouseActive && BfValid(cur_pt)) {
                // cursor on the battle field
                if(t = m_battlefield->FindTroop(*m_ownArmy, cur_pt), t >= 0 && (*m_ownArmy)[t].Count() > 0) {
                    // troop from my army
                    std::string str = _("View %{name} info.");
                    std::string name = (*m_ownArmy)[t].GetName();
                    String::Lower(name);
                    String::Replace(str, "%{name}", name);
                    m_gui->Status(str, true);
                    m_gui->Redraw();
                    cursor.SetThemes(cursor.WAR_INFO);
                    if(click) {
                        cursor.SetThemes(cursor.POINTER); 
                        Dialog::ArmyInfo((*m_ownArmy)[t], Dialog::BATTLE|Dialog::READONLY|Dialog::BUTTONS);
                    }
                    if(le.MouseRight()) {
                        Dialog::ArmyInfo((*m_ownArmy)[t], Dialog::BATTLE);
                    }
                } else if(t = m_battlefield->FindTroop((*m_oppArmy), cur_pt), t >= 0 && (*m_oppArmy)[t].Count() > 0) {
                    // enemy troop
                    int mp;
                    if(myTroop.shots > 0 && !myTroop.Modes(Army::HANDFIGHTING)) {
                        std::string str = _("Shoot %{name} (%{value} shot(s) left)");
                        std::string name = (*m_oppArmy)[t].GetName();
                        String::Lower(name);
                        String::Replace(str, "%{name}", name);
                        String::Replace(str, "%{value}", myTroop.shots);
                        m_gui->Status(str, true);
                        m_gui->Redraw();
                        if(m_battlefield->IsRangedAttackBlocked(myTroop, (*m_oppArmy)[t]))
                            cursor.SetThemes(cursor.WAR_BROKENARROW);
                        else
                            cursor.SetThemes(cursor.WAR_ARROW);
                        if(click) {
                            attack = cur_pt;
                            return GUI::NONE;
                        } else if(le.MouseRight()) {
                            Dialog::ArmyInfo((*m_oppArmy)[t], Dialog::BATTLE);
                        }
                    } else if(mp = m_battlefield->CanAttack(myTroop, m_movePoints, (*m_oppArmy)[t], le.MouseCursor() - Bf2Scr(cur_pt)), mp >= 0) {
                        std::string str = _("Attack %{name}");
                        std::string name = (*m_oppArmy)[t].GetName();
                        String::Lower(name);
                        String::Replace(str, "%{name}", name);
                        m_gui->Status(str, true);
                        m_gui->Redraw();
                        Point p1 = Bf2ScrNoOffset(m_movePoints[mp]), p2 = Bf2ScrNoOffset(cur_pt);
                        if(p1.x > p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPRIGHT);
                        else if(p1.x < p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPLEFT);
                        else if(p1.x > p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMRIGHT);
                        else if(p1.x < p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMLEFT);
                        else if(p1.x > p2.x) cursor.SetThemes(cursor.SWORD_RIGHT);
                        else if(p1.x < p2.x) cursor.SetThemes(cursor.SWORD_LEFT);
                        //cursor.SetThemes(cursor.SWORD_TOPLEFT); // TODO
                        if(click) {
                            move = m_movePoints[mp];
                            attack = cur_pt;
                            return GUI::NONE;
                        } else if(le.MouseRight())
                            Dialog::ArmyInfo((*m_oppArmy)[t], Dialog::BATTLE);
                    } else {
                        // attack
                        std::string str = _("View %{name} info.");
                        std::string name = (*m_oppArmy)[t].GetName();
                        String::Lower(name);
                        String::Replace(str, "%{name}", name);
                        m_gui->Status(str, true);
                        m_gui->Redraw();
                        cursor.SetThemes(cursor.WAR_INFO);
                        if(click) {
                            cursor.SetThemes(cursor.POINTER);
                            Dialog::ArmyInfo((*m_oppArmy)[t], Dialog::BATTLE|Dialog::READONLY|Dialog::BUTTONS);
                        } else if(le.MouseRight()) {
                            Dialog::ArmyInfo((*m_oppArmy)[t], Dialog::BATTLE);
                        }
                    }
                } else {
                    // empty cell
                    bool canmove = std::find(m_movePoints.begin(), m_movePoints.end(), cur_pt) != m_movePoints.end();
                    if(canmove) {
                        std::string str2, str;
                        if(myTroop.isFly()) cursor.SetThemes(cursor.WAR_FLIGHT), str2 = _("Fly");
                        else cursor.SetThemes(cursor.WAR_MOVE), str2 = _("Move");
                        str = _("%{move} %{name} here.");
                        String::Replace(str, "%{move}", str2);
                        std::string name = myTroop.GetName();
                        String::Lower(name);
                        String::Replace(str, "%{name}", name);
                        m_gui->Status(str, true);
                        m_gui->Redraw();
                        if(click) {
                            move = cur_pt;
                            return GUI::NONE;
                        }
                    } else {
                        m_gui->Status("", true);
                        m_gui->Redraw();
                        cursor.SetThemes(cursor.WAR_NONE);
                    }
                }
            } else if(!mouseActive) {
                m_gui->Status("", true);
                m_gui->Redraw();
                cursor.SetThemes(cursor.WAR_NONE);
            }
        }

        
        return GUI::NONE;
    }

    bool HumanTurn::MagicSelectTarget(std::vector<Army::BattleTroop *> &targets, const Spell::spell_t &spell)
    {
        if(!BattleTurn::MagicSelectTarget(targets, spell)) // No selection necessary for given spell
            return true;
        
        Army::BattleArmy_t *targetArmy = NULL;
        if(Spell::Target(spell) == Spell::ONEFRIEND)
            targetArmy = m_ownArmy;
        else if(Spell::Target(spell) == Spell::ONEENEMY)
            targetArmy = m_oppArmy;
        
        // select target
        while(le.HandleEvents())
        {
            Point cur_pt = Scr2Bf(le.MouseCursor());
            int t;
            
            if(targetArmy)
            {
                if(t = m_battlefield->FindTroop(*targetArmy, cur_pt), t >= 0
                && /*Spell::AllowSpell(spell, (*targetArmy)[t])*/
                   Spell::isTroopAffectedBySpell(spell, (*targetArmy)[t]))
                {
                    cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
                    if(le.MouseClickLeft(Rect(0, 0, display.w(), display.h())))
                    {
                        targets.push_back(&(*targetArmy)[t]);
                        break;
                    }
                } else cursor.SetThemes(cursor.SPELLNONE);
            }
            else if(Spell::Target(spell) == Spell::FREECELL)
            {
                if(m_battlefield->CellFree(cur_pt)) {
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
        return true;
    }
    
/////////////////////////////////////////////////////////////////////////////

    ComputerTurn::ComputerTurn(Battlefield *battlefield, GUI *gui, HeroBase *hero, Army::BattleArmy_t &army1, Army::BattleArmy_t &army2)
    : BattleTurn(battlefield, gui, hero, army1, army2)
    {
    }
    
    GUI::interaction_t ComputerTurn::Run(Army::BattleTroop &myTroop, TroopIndex troopN, Point &move, Point &attack)
    {
        BattleTurn::Run(myTroop, troopN, move, attack);

        //TODO: Spells
        //TODO: Retreat/surrender

        if(myTroop.isFly() || (myTroop.isArchers() && !myTroop.Modes(Army::HANDFIGHTING)))
            MoveOrAttack(troopN, move, attack, true);
        else if(myTroop.isArchers() && myTroop.Modes(Army::HANDFIGHTING))
        {
            Army::BattleArmy_t closeEnemies;
            const Army::BattleArmy_t &enemyArmy = *m_oppArmy;
            for(u16 j = 0; j < enemyArmy.size(); j++)
            {
                if(m_battlefield->IsTroopCloseTo(enemyArmy[j], myTroop))
                    closeEnemies.push_back(enemyArmy[j]);
            }
            std::sort(closeEnemies.begin(), closeEnemies.end(), DangerousUnitPredicate);
            attack = closeEnemies.front().Position();
            move = myTroop.Position();
        }
        else
            MoveOrAttack(troopN, move, attack, false);
        
        return GUI::NONE;
    }

    bool ComputerTurn::DangerousUnitPredicate(const Army::BattleTroop &first, const Army::BattleTroop &second)
    {
        return  first.GetDamageMax() > second.GetDamageMax();
    }

    Point ComputerTurn::GetMostDangerousOpposingUnit()
    {
        Army::BattleArmy_t oppArmy = *m_oppArmy;
        std::sort(oppArmy.begin(), oppArmy.end(), DangerousUnitPredicate);
        return oppArmy.front().Position();
    }
    
    /** Attempt to attack a unit in the given list
     *  \param[in]  list  List of units to consider
     *  \param[in]  troopN  Index of current troop
     *  \param[out] move    Movement destination
     *  \param[out] attack  Point at which to attack
     *  \return True if an attack succeeded, false otherwise
     */
    bool ComputerTurn::AttackTroopInList(const Army::BattleArmy_t &list, TroopIndex troopN, Point &move, Point &attack)
    {
        u16 idx;
        for(idx = 0; idx < list.size(); idx++)
        {
            Point attackFrom = GetReachableAttackCell(list[idx], *m_ownArmy, *m_oppArmy, troopN);
            if(BfValid(attackFrom))
            {
                attack = list[idx].Position();
                move = attackFrom;
                return true;
            }
        }
        return false;
    }
    
    struct ClosestDistancePredicate
    {
        const Point &orig;
        ClosestDistancePredicate(const Point &init) : orig(init) {}
        
        bool operator()(const Point &first, const Point &second) const
        {
            return orig.distance(first) < orig.distance(second);
        }
    };
    
    /** Move to the closest point to the closest unit in the list
     *  \param[in] list   List of units to consider
     *  \param[in] myTroop  Unit to move
     *  \param[out] move    Destination point
     */
    void ComputerTurn::MoveToClosestTroopInList(const Army::BattleArmy_t &list, const Army::BattleTroop &myTroop, Point &move)
    {
        PointList positions;
        Army::BattleArmy_t::const_iterator it = list.begin();
        // Make a list of all available target points (including wide creature locations)
        for(; it != list.end(); it++)
        {
            positions.push_back(it->Position());
            if(it->isWide())
                positions.push_back(it->Position() + (it->IsReflected() ? Point(-1, 0) : Point(1, 0)));
        }
        sort(positions.begin(), positions.end(), ClosestDistancePredicate(myTroop.Position()));
        
        // Find the move point that is nearest to the closest target point
        Point target = positions.front();
        Point closest = m_movePoints[0];
        for(u16 idx = 0; idx < m_movePoints.size(); idx++)
        {
            if(target.distance(m_movePoints[idx])
               < target.distance(closest))
                closest = m_movePoints[idx];
        }
        move = closest;
    }
    
    /** Find a valid move and/or attack to make
     *  \param[in] troopN      Index of current troop
     *  \param[out] move       #Point to which to move
     *  \param[out] attack     #Point which to attack
     *  \param[in] rangedFirst Try to attack ranged units first
     */
    void ComputerTurn::MoveOrAttack(TroopIndex troopN, Point &move, Point &attack, bool rangedFirst)
    {
        bool result;
        if(rangedFirst)
            result = AttackRangedTroop(troopN, move, attack);
        else
            result = AttackNonRangedTroop(troopN, move, attack);
        if(!result)
        {
            if(rangedFirst)
                result = AttackNonRangedTroop(troopN, move, attack);
            else
                result = AttackRangedTroop(troopN, move, attack);
        }
        if(!result)
        {
            const Army::BattleTroop &myTroop = m_battlefield->GetTroopFromIndex(troopN);
            const Army::BattleArmy_t &enemyArmy = *m_oppArmy;
            Army::BattleArmy_t realArmy;
            for(u16 i = 0; i < enemyArmy.size(); i++)
                if(enemyArmy[i].isValid())
                    realArmy.push_back(enemyArmy[i]);
            MoveToClosestTroopInList(realArmy, myTroop, move);
        }
    }
    
    /** Find a ranged enemy to attack, and set up the destination action and movement points
     *  \param[in] troopN     Attacking troop index
     *  \param[out] move      #Point to which to move
     *  \param[out] attack    #Point which to attack
     *  \return Whether a possible move or attack was found
     */
    bool ComputerTurn::AttackRangedTroop(TroopIndex troopN, Point &move, Point &attack)
    {
        const Army::BattleTroop &myTroop = m_battlefield->GetTroopFromIndex(troopN);
        Army::BattleArmy_t ranged;
        const Army::BattleArmy_t &enemyArmy = *m_oppArmy;
        for(u16 i = 0; i < enemyArmy.size(); i++)
        {
            if(enemyArmy[i].Count() && enemyArmy[i].isArchers())
                ranged.push_back(enemyArmy[i]);
        }
        if(ranged.size())
        {
            std::sort(ranged.begin(), ranged.end(), DangerousUnitPredicate);
            if(myTroop.shots)
            {
                attack = ranged.front().Position();
                return true;
            }
            else return AttackTroopInList(ranged, troopN, move, attack);
        }
        return false;
    }
    
    /** Find a non-ranged enemy to attack, and set up the destination action and movement points
     *  \param[in] troopN     Attacking troop index
     *  \param[out] move      #Point to which to move
     *  \param[out] attack    #Point which to attack
     *  \return Whether a possible move or attack was found
     */
    bool ComputerTurn::AttackNonRangedTroop(TroopIndex troopN, Point &move, Point &attack)
    {
        const Army::BattleTroop &myTroop = m_battlefield->GetTroopFromIndex(troopN);
        Army::BattleArmy_t nonranged;
        const Army::BattleArmy_t &enemyArmy = *m_oppArmy;
        for(u16 i = 0; i < enemyArmy.size(); i++)
        {
            if(enemyArmy[i].Count() && !enemyArmy[i].isArchers())
                nonranged.push_back(enemyArmy[i]);
        }
        if(nonranged.size())
        {
            std::sort(nonranged.begin(), nonranged.end(), DangerousUnitPredicate);
            if(myTroop.shots)
            {
                attack = nonranged.front().Position();
                return true;
            }
            else return AttackTroopInList(nonranged, troopN, move, attack);
        }
        return false;
    }

    /** Determine if a troop can attack an enemeny from the given cell
     *  \param attacker   Troop which is attacking
     *  \param target     Troop which is defending
     *  \param attackFrom Battlefield point from which to attack
     */
    bool ComputerTurn::CanAttackFrom(const Army::BattleTroop &attacker, const Army::BattleTroop &target, const Point &attackFrom)
    {
        //FIXME: These calculations are based on current reflection state
        std::vector<Point> from, to;
        from.push_back(attackFrom);
        if(attacker.isWide())
            from.push_back(attackFrom + Point( attacker.IsReflected() ? -1 : 1, 0 ));
        to.push_back(target.Position());
        if(target.isWide())
            to.push_back(target.Position() + Point ( target.IsReflected() ? -1 : 1, 0 ));

        s8 minDX, maxDX;
        if(target.Position().y % 2 == 0)
        {
            minDX = 0;
            maxDX = 1;
        }
        else
        {
            minDX = -1;
            maxDX = 0;
        }
    
        for(u16 i = 0; i < from.size(); i++)
            for(u16 j = 0; j < to.size(); j++)
            {
                int dx = from[i].x - to[j].x;
                int dy = from[i].y - to[j].y;
                //printf("- %d,%d to %d,%d: %d,%d (%d-%d)\n", from[i].x, from[i].y, to[j].x, to[j].y, dx, dy, minDX, maxDX);
            
                if(abs(dy) == 1 && dx >= minDX && dx <= maxDX)
                    return true;
                else if(!dy && abs(dx) == 1)
                    return true;
            }
        return false;
    }

    /** Find an open cell from which to attack a creature
     *  \param target Target to be attacked
     *  \param army1  Left army
     *  \param army2  Right army
     *  \param troopN Index in either \i army1 or \i army2
     *  \return Free cell if one is available, invalid if none
     */
    Point ComputerTurn::GetReachableAttackCell(const Army::BattleTroop &target, const Army::BattleArmy_t &army1, const Army::BattleArmy_t &army2, TroopIndex troopN)
    {
        const Army::BattleTroop &attacker = troopN >= 0 ? army1[troopN] : army2[-troopN - 1];
        bool targetWide = target.isWide();
        bool attackerWide = attacker.isWide();
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
                    /*DrawShadow(p + delta);
                      Display::Get().Flip();
                      while(le.HandleEvents())
                      if(le.KeyPress(KEY_RETURN))
                      break;*/
                
                    // if(delta.x > 0 && delta.y) continue;
                    if(m_battlefield->CellFreeFor(p + delta, attacker, troopN)
                    && std::find(m_movePoints.begin(), m_movePoints.end(), p + delta) != m_movePoints.end()
                    && CanAttackFrom(attacker, target, p + delta))
                        return p + delta;
                }
        return Point(-1, -1);
    }

    bool ComputerTurn::MagicSelectTarget(std::vector<Army::BattleTroop *> &targets, const Spell::spell_t &spell)
    {
        //TODO
        return true;
    }
    
////////////////////////////////////////////////////////////////////////////
    
    Battlefield::Battlefield(Castle *castle, HeroBase *hero1, HeroBase *hero2, Army::army_t &army1, Army::army_t &army2, const Point &pt, const Maps::Tiles &tiles)
    : m_background(pt, tiles)
    {
        if(!castle)
        {
            m_castle = NULL;
            m_catapult = NULL;
            m_background.GetBlockedCells(m_blockedCells);
        }
        else
        {
            m_castle = new BattleCastle(*castle);
            m_catapult = new Catapult;
            m_blockedCells.push_back(Point(0, 7)); //Block catapult cell, but nothing on battlefield
        }

        m_army[0] = new Army::BattleArmy_t;
        Army::ArmyToBattleArmy(army1, *m_army[0]);
        m_army[1] = new Army::BattleArmy_t;
        Army::ArmyToBattleArmy(army2, *m_army[1]);

        m_hero[0] = hero1;
        m_hero[1] = hero2;
        
        //TODO: Derive compact parameter from hero screen settings
        InitArmyPosition(*m_army[0], false, false);
        InitArmyPosition(*m_army[1], false, true);
    }

    Battlefield::~Battlefield()
    {
        if(m_castle) delete m_castle;
	if(m_catapult) delete m_catapult;
	if(m_army[0]) delete m_army[0];
	if(m_army[1]) delete m_army[1];
    }

    /** Remove any units from the battle that are dead.
     */
    void Battlefield::CleanupBodies()
    {
        for(u8 j = 0; j < 2; j++) {
            for(u16 i = 0; i < m_army[j]->size(); i++)
            {
                if((*m_army[j])[i].Count() != 0)
                    continue;
                
                Army::BattleTroop body = (*m_army[j])[i];
                m_background.AddBody(body);
                (*m_army[j])[i].SetMonster(Monster::UNKNOWN);
            }
        }
    }
    
    void Battlefield::InitArmyPosition(Army::BattleArmy_t & army, bool compact, bool reflect)
    {
        int x = reflect ? 10 : 0;
        int y = compact ? 2 : 0;
        for(unsigned int i=0; i < army.size(); i++)
        {
            if(army[i].GetID() != Monster::UNKNOWN)
            {
                const Monster &stats = army[i];
                AGG::PreloadObject(stats.ICNFile());
                army[i].SetPosition(Point(x, y));
                army[i].aframe = 1;
                army[i].astate = Monster::AS_NONE;
                army[i].shots = stats.GetShots();
                army[i].hp = stats.GetHitPoints();
                army[i].oldcount = army[i].Count();
                army[i].SetOriginalReflection(reflect);
            }
            y += compact ? 1 : 2;
        }
    }

    Army::BattleArmy_t &Battlefield::GetArmy(u8 num)
    {
        return *m_army[num];
    }

    HeroBase *Battlefield::GetHero(u8 num)
    {
        return m_hero[num];
    }

    void Battlefield::SetHero(u8 num, HeroBase *hero)
    {
        m_hero[num] = hero;
    }

    s8 Battlefield::GetIndexFromTroop(const Army::BattleTroop &troop)
    {
        for(u16 i = 0; i < m_army[0]->size(); i++)
            if(&(*m_army[0])[i] == &troop)
                return i;
        for(u16 i = 0; i < m_army[1]->size(); i++)
            if(&(*m_army[1])[i] == &troop)
                return ApplyIndexModifier(i);

        return 0xFF;
    }

    u8 Battlefield::GetSideFromIndex(s8 idx)
    {
        return idx < 0;
    }

    Army::BattleTroop &Battlefield::GetTroopFromIndex(s8 idx)
    {
        if(idx >= 0)
            return (*m_army[0])[idx];
        else return (*m_army[1])[ApplyIndexModifier(idx)];
    }

    TroopIndex Battlefield::ApplyIndexModifier(TroopIndex idx)
    {
        return -idx - 1;
    }

    void Battlefield::GetTroopOrdering(IndexList &order)
    {
        for(u8 speed = Speed::INSTANT; speed != Speed::STANDING; speed--)
        {
            IndexList ordered[2];
            GetTroopsOfSpeed(*m_army[0], speed, ordered[0]);
            GetTroopsOfSpeed(*m_army[1], speed, ordered[1]);
            std::transform(ordered[1].begin(), ordered[1].end(), ordered[1].begin(), ApplyIndexModifier);
            order.insert(order.end(), ordered[0].begin(), ordered[0].end());
            order.insert(order.end(), ordered[1].begin(), ordered[1].end());
        }
    }

    /** Determine if a specific troop is at a given point
     *  \param troop  Troop to find
     *  \param p      Point at which to search
     */
    bool Battlefield::IsTroopAt(const Army::BattleTroop &troop, const Point &p)
    {
        Point p2(troop.Position());
        if(troop.isWide()) p2.x += troop.IsReflected() ? -1 : 1;
        if(troop() != Monster::UNKNOWN && (p == troop.Position() || p == p2)) 
            return true;
        else return false;
    }

    /** Determine which troop, if one exists, is exactly at the given point
     *  \note No wide or reflection calculations are performed
     *  \param army Army through which to search
     *  \param p    Point at which to look
     *  \return The troop's index into \i army, or -1 if none found
     */
    TroopIndex Battlefield::FindTroopExact(const Army::BattleArmy_t &army, const Point &p)
    {
        for(unsigned int i=0; i<army.size(); i++) {
            if(army[i].GetID() != Monster::UNKNOWN && army[i].Position() == p)
                return i;
        }
        return -1;
    }

    /** Determine which troop, if one exists, is at the given point
     *  \param army Army through which to search
     *  \param p    Point at which to look
     *  \return The troop's index into \i army, or -1 if none found
     */
    TroopIndex Battlefield::FindTroop(const Army::BattleArmy_t &army, const Point &p)
    {
        for(unsigned int i=0; i<army.size(); i++) {
            if(IsTroopAt(army[i], p))
                return i;
        }
        return -1;
    }

    /** Determine which troop, if one exists, is at the given point
     *  \param army Army through which to search
     *  \param p    Point at which to look
     *  \return The troop's index into \i army, or -1 if none found
     */
    TroopIndex Battlefield::FindTroop(const std::vector<Army::BattleTroop*> &army, const Point &p)
    {
        for(unsigned int i=0; i<army.size(); i++) {
            if(IsTroopAt(*army[i], p))
                return i;
        }
        return -1;
    }

    /** Determine if a cell is free of obstruction (monsters and background)
     *  \param p      Battlefield cell
     *  \param skip   Troop id to ignore
     */
    bool Battlefield::CellFree(const Point &p, TroopIndex skip)
    {
        int t;
        if(!BfValid(p)) return false;
        if(t = FindTroop(*m_army[0], p), t >= 0 && (*m_army[0])[t].Count() > 0 && (skip < 0 || skip != t)) 
            return false;
        if(t = FindTroop(*m_army[1], p), t >= 0 && (*m_army[1])[t].Count() > 0 && (skip >= 0 || ApplyIndexModifier(skip) != t)) 
            return false;
        if(std::find(m_blockedCells.begin(), m_blockedCells.end(), p) != m_blockedCells.end())
            return false;

        BattleCastle *castle = GetCastle();
        if(castle)
            for(u16 i = 0; i < MAX_CASTLE_PARTS; i++)
                if(castle->getCastlePoint(i) == p)
                {
                    if(i == BattleCastle::DRAWBRIDGE)
                    {
                        if(GetSideFromIndex(skip) == 1)
                            return true;
                        else return false;
                    }
                    else return false;
                }

        return true;
    }

    /** Determine if a cell is free of obstruction for a specific troop
     *  \param p      Battlefield cell
     *  \param troop  Troop to consider
     *  \param skip   Troop ID to ignore
     */
    bool Battlefield::CellFreeFor(const Point &p, const Army::BattleTroop &troop, TroopIndex skip)
    {
        bool isFree = CellFree(p, skip);
        if(isFree && troop.isWide())
            isFree = CellFree(Point(p.x + (troop.IsReflected() ? -1 : 1), p.y), skip);
        return isFree;
    }

    /** Find a path from one point to another for a given troop.
     * \param start   Starting point
     * \param end     End point
     * \param moves   Maximum length of path
     * \param troop   Troop for which to find path
     * \param skip    Troop id to ignore if found
     * \return A #Point vector containing the path
     */
    PointList *Battlefield::FindPath(const Point& start, const Point &end, int moves, const Army::BattleTroop &troop, TroopIndex skip)
    {
        if(moves < 0 || !BfValid(start) || !CellFreeFor(start, troop, skip))
            return 0;

        if(start == end) return new std::vector<Point>();
        Point p = start;
        Point d;
        std::vector<Point> *path = 0, *tmpath = 0;
        int length = -1;
        for(d.x = -1; d.x <= 1; d.x++)
            for(d.y = -1; d.y <= 1; d.y++) 
                if(d.x || d.y ) {
                    if(p.y % 2 && d.y && d.x > 0) continue;
                    if(!(p.y % 2) && d.y && d.x < 0) continue;
                    if(tmpath = FindPath(p + d, end, moves - 1, troop, skip), tmpath) {
                        if(length < 0 || length > static_cast<int>(tmpath->size())) {
                            length = tmpath->size();
                            tmpath->push_back(p + d);
                            path = tmpath;
                            //printf("path has something\n");
                        } else delete tmpath;
                    }
                }
        return path;
    }

    /** Determine if a troop can attack an enemy from the given screen point
     *  \param myTroop    Attacker troop
     *  \param moves      #Point vector of valid cells attacker can move to
     *  \param enemyTroop Enemy target
     *  \param d          Screenspace position from which the attack would occur
     *  \return >= 0 if the attacker can attack the enemy
     */
    int Battlefield::CanAttack(const Army::BattleTroop &myTroop, const PointList &moves, const Army::BattleTroop &enemyTroop, const Point &d)
    {
        if(myTroop.Count() == 0) return -1;
        Point delta = d;
        //delta.x -= CELLW;
        delta.y += CELLH/2;
        const Monster &myMonster = myTroop;
        const Monster &enemyMonster = enemyTroop;
        std::vector<Point> from, to;
        to.push_back(enemyTroop.Position());
        if(enemyMonster.isWide())
            to.push_back(enemyTroop.Position() + Point ( enemyTroop.IsReflected() ? -1 : 1, 0 ));
        int mp = -1, min = 9999;
        for(u32 i=0; i<moves.size(); i++) {
            Point d, p1, p2;
            from.clear();
            from.push_back(moves[i]);
            if(myMonster.isWide())
                from.push_back(moves[i] + Point( myTroop.IsReflected() ? -1 : 1, 0 ));
            for(u32 j = 0; j < from.size(); j++)
                for(u32 k = 0; k < to.size(); k++)
                {
                    p1 = from[j];
                    p2 = to[k];
                    for(d.x = -1; d.x <= 1; d.x++)
                        for(d.y = -1; d.y <= 1; d.y++) 
                            if(d.x || d.y ) {
                                if(p1.y%2 && d.y && d.x>0) continue;
                                if(!(p1.y%2) && d.y && d.x<0) continue;
                                if(p1 + d == p2) {
                                    Point d2 = Bf2ScrNoOffset(p2)+delta - Bf2ScrNoOffset(p1);
                                    if(abs(d2.x) + abs(d2.y) < min) {
                                        min = abs(d2.x) + abs(d2.y);
                                        mp = i;
                                    }
                                }
                            }
                }
        }
        return mp;
    }

    bool Battlefield::IsTroopCloseTo(const Army::BattleTroop &first, const Army::BattleTroop &second)
    {
        Point tmpoint(0,0);
        PointList tmpoints;
        tmpoints.push_back(first.Position());
        return CanAttack(first, tmpoints, second, tmpoint) >= 0;
    }

    /** Popular a list with the targets for the given unit's attack at a given point
     *  \param[out] targets List of troop which will be affected
     *  \param[in] attacker Troop that is attacking
     *  \param[in] attack   Point which is being attacked
     *  \param[in] army1    Left army
     *  \param[in] army2    Right army
     */
    void Battlefield::GetTargets(std::vector<Army::BattleTroop *> &targets, const Army::BattleTroop &attacker, const Point &attack)
    {
        int t;
        Army::BattleTroop &mainTarget = (t = FindTroop(*m_army[0], attack), t >= 0 ? (*m_army[0])[t] : (*m_army[1])[FindTroop(*m_army[1], attack)]);
        bool reflected = mainTarget.Position().x <= attacker.Position().x;
        targets.push_back(&mainTarget);
        switch(attacker())
        {
            case Monster::GREEN_DRAGON:
            case Monster::RED_DRAGON:
            case Monster::BLACK_DRAGON:
            case Monster::PHOENIX:
            case Monster::CYCLOPS:
            {
                bool wide = attacker.isWide();
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
                /*printf("pos: %d, %d\n", attacker.Position().x, attacker.Position().y);
                  printf("attack: %d, %d\n", attack.x, attack.y);
                  printf("incr: %d, %d\n", incr.x, incr.y);*/
                int t;
                if((t = FindTroop(*m_army[0], nextcell)) >= 0
                   && &(*m_army[0])[t] != &attacker
                   && &(*m_army[0])[t] != &mainTarget)
                    targets.push_back(&(*m_army[0])[t]);
                else if((t = FindTroop(*m_army[1], nextcell)) >= 0
                        && &(*m_army[1])[t] != &attacker
                        && &(*m_army[1])[t] != &mainTarget)
                    targets.push_back(&(*m_army[1])[t]);
                break;
            }
        
            case Monster::HYDRA:
            case Monster::LICH:
            case Monster::POWER_LICH:
            {
                Point base;
                if(attacker == Monster::HYDRA)
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
                        if((t = FindTroop(*m_army[0], base + Point(x, y))) >= 0
                           && &(*m_army[0])[t] != &attacker
                           && &(*m_army[0])[t] != &mainTarget)
                        {
                            targets.push_back(&(*m_army[0])[t]);
                        }
                        else
                            if((t = FindTroop(*m_army[1], base + Point(x, y))) >= 0
                               && &(*m_army[1])[t] != &attacker
                               && &(*m_army[1])[t] != &mainTarget)
                            {
                                targets.push_back(&(*m_army[1])[t]);
                            }
                        }
                    break;
            }
        
            default:
                break;
        }
    }

    /** Determine if there are any obstructions between an attacker and a target troop.
     *  \param attacker Attacking troop.
     *  \param defender Defending troop.
     */
    bool Battlefield::IsRangedAttackBlocked(const Army::BattleTroop &attacker, const Army::BattleTroop &defender)
    {
        if(!GetCastle())
            return false;
        
        Point delta = defender.Position() - attacker.Position();
        if(delta.x < 0) // the defender in a castle gets no penalty
            return false;
        
        const s16 dX = delta.x;
        delta.x /= std::max(dX, delta.y);
        delta.y /= std::max(dX, delta.y);
        Point cur = attacker.Position();
        bool found = false;
        while(!found && cur.x < defender.Position().x)
        {
            for(u16 i = 0; i < MAX_CASTLE_PARTS; i++)
            {
                Point p = GetCastle()->getCastlePoint(i);
                if(GetCastle()->getCastlePoint(i) == cur)
                    found = true;
            }
            cur += delta;
        }
        return found;
    }

    void Battlefield::RedrawBackground(const PointList *movePoints, const Army::BattleTroop *troop)
    {
        m_background.Draw();
        if(m_castle)
            m_castle->DrawBackground();
        m_background.DrawGrid();
        if(movePoints)
            m_background.DrawOverBackground(*movePoints, *troop);
    }

    void Battlefield::Redraw(const PointList *movePoints, const Point *cur_pt, const Army::BattleTroop *troop)
    {
        cursor.Hide();
        RedrawBackground(movePoints, troop);
        if(m_catapult)
            m_catapult->Draw();
        if(m_hero[0]) m_heroRect[0] = m_background.DrawHeroObject(*m_hero[0], 1, false, 1);
        if(m_hero[1]) m_heroRect[1] = m_background.DrawHeroObject(*m_hero[1], 1, true);
        if(m_castle)
            m_castle->DrawForeground();
        m_background.DrawForeground(m_castle, *m_army[0], *m_army[1], -1, true);
        if(cur_pt)
            m_background.DrawCursor(*cur_pt);
        cursor.Show();
    }

    void Battlefield::AnimateMorale(bool morale, const Army::BattleTroop &troop)
    {
        m_background.DrawMorale(morale, troop);
    }

    void Battlefield::AnimateMagic(std::vector<Army::BattleTroop*> &aff, HeroBase *h1, HeroBase *h2, bool reflect, Spell::spell_t spell)
    {
        m_background.MagicAnimation(aff, h1, h2, reflect, spell);
    }

///////////////////////////////////////////////////////////////////////////////////    

    GUI::GUI(HeroBase *hero1, HeroBase *hero2)
    {
        m_frameborder = new Dialog::FrameBorder();
        g_baseOffset = Point(m_frameborder->GetArea().x, m_frameborder->GetArea().y);
        display.FillRect(0, 0, 0, Rect(g_baseOffset, 640, 480));

        Mixer::Reset();
        //FIXME: The music currently plays underneath the sound effect
        int track = ((int)MUS::BATTLE1 + Rand::Get(0, 2));
        AGG::PlayMusic((MUS::mus_t)track);
        
        AGG::PlaySound(M82::PREBATTL);
        cursor.SetThemes(cursor.WAR_POINTER);
        cursor.Hide();

        display.Blit(AGG::GetICN(ICN::TEXTBAR, 0), g_baseOffset.x + 640-48, g_baseOffset.y + 480-36);
        display.Blit(AGG::GetICN(ICN::TEXTBAR, 4), g_baseOffset.x, g_baseOffset.y + 480-36);
        display.Blit(AGG::GetICN(ICN::TEXTBAR, 6), g_baseOffset.x, g_baseOffset.y + 480-18);

        const Sprite & bar1 = AGG::GetICN(ICN::TEXTBAR, 8);
        const Sprite & bar2 = AGG::GetICN(ICN::TEXTBAR, 9);

        //display.Blit(bar1, g_baseOffset.x + 50, g_baseOffset.y + 480-36);
        //display.Blit(bar2, g_baseOffset.x + 50, g_baseOffset.y + 480-16);

        m_statusBar[0] = new StatusBar;
        m_statusBar[0]->SetCenter(g_baseOffset.x + 50 + bar1.w()/2, g_baseOffset.y + 480-26);
        m_statusBar[0]->ShowMessage("");
        m_statusBar[1] = new StatusBar;
        m_statusBar[1]->SetCenter(g_baseOffset.x + 50 + bar2.w()/2, g_baseOffset.y + 480-6);
        m_statusBar[1]->ShowMessage("");

        m_hero[0] = hero1;
        m_hero[1] = hero2;

        m_skip = new Button(g_baseOffset.x + 640-48, g_baseOffset.y + 480-36, ICN::TEXTBAR, 0, 1);
        m_auto = new Button(g_baseOffset.x, g_baseOffset.y + 480-36, ICN::TEXTBAR, 4, 5);
        m_settings = new Button(g_baseOffset.x, g_baseOffset.y + 480-18, ICN::TEXTBAR, 6, 7);
    }

    GUI::~GUI()
    {
        delete m_statusBar[0];
        delete m_statusBar[1];
        delete m_frameborder;
        delete m_skip;
        delete m_auto;
        delete m_settings;
    }

    void GUI::Redraw()
    {
        const Sprite & bar1 = AGG::GetICN(ICN::TEXTBAR, 8);
        const Sprite & bar2 = AGG::GetICN(ICN::TEXTBAR, 9);
        display.Blit(bar1, g_baseOffset.x + 50, g_baseOffset.y + 480-36);
        display.Blit(bar2, g_baseOffset.x + 50, g_baseOffset.y + 480-16);
        m_statusBar[0]->Redraw();
        m_statusBar[1]->Redraw();
    }

    void GUI::Status(const std::string &str, bool secondOnly /* = false */)
    {
        static std::string str1, str2;
        if(!secondOnly)
        {
            if(str2 != str)
            {
                str1 = str2;
                str2 = str;
            }
        }
        else
        {
            str1 = "";
            str2 = str;
        }
        m_statusBar[0]->ShowMessage(str1);
        m_statusBar[1]->ShowMessage(str2);
    }

    GUI::interaction_t GUI::Interact(TroopIndex troopN, Spell::spell_t &spell, Battlefield &battlefield, bool &mouseActive)
    {
        bool canSurrender = m_hero[0] && m_hero[1];
        do_button(*m_skip, return SKIP, Dialog::Message(_("Skip"), _("Skip the current creature. The current creature loses its turn and does not get to go again until the next round."), Font::BIG));
        do_button(*m_auto, return AUTO, Dialog::Message(_("Auto Combat"), _("Allows the computer to fight out the battle for you."), Font::BIG));
        do_button(*m_settings, SettingsDialog(), Dialog::Message(_("System Options"), _("Allows you to customize the combat screen."), Font::BIG));
        for(u8 i = 0; i < 2; i++)
        {
            if(le.MouseClickLeft(battlefield.GetHeroRect(i))) {
                bool ownTurn = battlefield.GetSideFromIndex(troopN) == i;
                Army::battle_t s = HeroStatus(*m_hero[i], *m_statusBar[1], spell, false, canSurrender, !ownTurn);
                if(s == Army::RETREAT) {
                    if(Dialog::Message("", _("Are you sure you want to retreat?"), Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
                        return RETREAT;
                }
                else if(s == Army::SURRENDER) return SURRENDER;
                else if(spell != Spell::NONE)
                    return SPELL;
            }
            else if(le.MousePressRight(battlefield.GetHeroRect(i)))
                HeroStatus(*m_hero[i], *m_statusBar[1], spell, true, false, false);
        }
        
        if(le.MouseCursor(*m_skip)) {
            m_statusBar[1]->ShowMessage(_("Skip this unit"));
            cursor.SetThemes(cursor.WAR_POINTER);
            mouseActive = true;
        }else if(le.MouseCursor(*m_auto)) {
            m_statusBar[1]->ShowMessage(_("Auto Combat"));
            cursor.SetThemes(cursor.WAR_POINTER);
            mouseActive = true;
        } else if(le.MouseCursor(*m_settings)) {
            m_statusBar[1]->ShowMessage(_("Customize system options"));
            cursor.SetThemes(cursor.WAR_POINTER);
            mouseActive = true;
        } else if(le.MouseCursor(troopN >= 0 ? battlefield.GetHeroRect(0) : battlefield.GetHeroRect(1))) {
            m_statusBar[1]->ShowMessage(_("Hero's Options"));
            cursor.SetThemes(cursor.WAR_HELMET);
            mouseActive = true;
        } else if(le.MouseCursor(troopN >= 0 ? battlefield.GetHeroRect(1) : battlefield.GetHeroRect(0))) {
            m_statusBar[1]->ShowMessage(_("View Opposing Hero"));
            cursor.SetThemes(cursor.WAR_INFO);
            mouseActive = true;
        }

        return NONE;
    }
}
