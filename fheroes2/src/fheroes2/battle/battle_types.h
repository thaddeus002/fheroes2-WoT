#ifndef H2BATTLE_TYPES_H
#define H2BATTLE_TYPES_H

typedef std::vector<Point> PointList;
typedef s8 TroopIndex;
typedef std::vector<TroopIndex> IndexList;

#define CELLW 44
#define CELLH 42
#define BFX 86
#define BFY 108
#define BFW 11
#define BFH 9
#define MORALEBOUND 10

#define do_button(b, lf, rt) \
    if(le.MousePressLeft(b)) (b).PressDraw();	\
    else (b).ReleaseDraw();                     \
    if(le.MouseClickLeft(b) && !(b).isDisable()) lf;    \
    if(le.MousePressRight(b)&& !(b).isDisable()) rt;

#define do_rbutton(b, lf, rt) \
    if(le.MouseClickLeft(b) && !(b).isDisable()) { (b).isPressed() ? (b).ReleaseDraw() : (b).PressDraw(); lf; } \
    if(le.MousePressRight(b) && !(b).isDisable()) rt;

class Button;
class Captain;

namespace Battle
{
    extern Point g_baseOffset;

    enum {
        PREATTACK = 0,
        MISSILE_FLIGHT,
        MISSILE_IMPACT,
        POSTATTACK
    };
    
    Point Scr2BfDirect(const Point & pt); // translate coordinate to battle field
    Point Scr2Bf(const Point & pt); // translate coordinate to battle field
    Point Bf2Scr(const Point & pt); // translate to screen (absolute position)
    Point Bf2ScrNoOffset(const Point& pt); // translate to screen (offset to frame border)
    bool BfValid(const Point & pt); // check battle field point

    void BattleSummary(HeroBase &hero, u32 exp, const Army::ArmyPairs &armies,
                       const BagArtifacts *artifacts, Spell::spell_t spell,
                       u32 deadRaised, Army::battle_t status);
    Army::battle_t HeroStatus(HeroBase &, StatusBar &, Spell::spell_t &, bool, bool, bool);
    void SettingsDialog();

    struct BattleSettings : public BitModes
    {
        BattleSettings();
        static BattleSettings &Get();
        
        enum {
            OPT_GRID        = 0x01,
            OPT_MOUSESHADOW = 0x02,
            OPT_MOVESHADOW  = 0x04,
            OPT_LOGICONLY   = 0x08
        };
    };

    const u16 MAX_CASTLE_PARTS = 9;
    const u16 MAX_CASTLE_WALLS = 5;
    const u16 MAX_CASTLE_TOWERS = 5;
    
    class BattleCastle
    {
      public:
        BattleCastle(Castle &);
        ~BattleCastle();
        void OpenDrawbridge();
        void CloseDrawbridge();
        void RecreateMoat();
        void RecreateCastle();
        void DestroyCastle();
        bool hasMoat() const;
        bool hasLeft() const;
        bool hasRight() const;
        bool hasFortifications() const;
        Race::race_t getRace() const;
        u32 countBuildings() const;

        enum WallStatus {
            UNBROKEN = 0,
            DAMAGED = 4,
            DESTROYED = 8
        };
        
        enum DrawbridgeStatus {
            CLOSED,
            OPEN,
            BROKEN
        };
        
        enum CastlePieces {
            FIRST_WALL,
            LEFT_TOWER,
            SECOND_WALL,
            LEFT_DRAWBRIDGE_TOWER,
            DRAWBRIDGE,
            RIGHT_DRAWBRIDGE_TOWER,
            THIRD_WALL,
            RIGHT_TOWER,
            FOURTH_WALL,
            TOWER_KEEP
        };

        WallStatus getWall(u8);
        WallStatus getTower(u8);
        DrawbridgeStatus getDrawbridge();
        Point getCastlePoint(u8);
        Point getCastlePartDisplay(u8);
        const Sprite &getCastlePartSprite(u8);
        WallStatus *castlePartStatus(u8 part);

        u8 CalculateDamage(u8, HeroBase *);
        void DamagePart(u8, u8);
        static WallStatus NextStatus(WallStatus status);

        bool IsCellInMoat(const Point &);

        void DrawBackground();
        void DrawForeground();
        void DrawCell(const Point &);
        
      private:
        Castle &m_castle;
        PointList m_moatCells;
        DrawbridgeStatus m_drawbridge;
        WallStatus m_wall[5];
        WallStatus m_tower[5];
        ICN::icn_t m_wallICN, m_groundICN;

        struct CastlePart
        {
            CastlePart(const Sprite *s, Point p2)
            : m_sprite(s), m_logical(p2) {}
            
            const Sprite *m_sprite;
            Point m_logical;
        };
        std::vector<CastlePart *> m_castleParts;
    };

    class Battlefield;

    class Catapult
    {
      public:
        Catapult();
        ~Catapult();
        void Fire(Battlefield &, BattleCastle &);
        void Draw();
        
      private:
        void Animate(Battlefield &, BattleCastle &, u8, u8);
        
        Background *back;
        const Sprite *sprite;
        Point point;
    };
    
    class Battlefield
    {
      public:
        Battlefield(Castle *, HeroBase *, HeroBase *, Army::army_t &, Army::army_t &, const Point &, const Maps::Tiles &);
        ~Battlefield();

        void InitializeArmies();
        Army::BattleArmy_t &GetArmy(u8);
        void SetArmy(u8, Army::army_t &);
        HeroBase *GetHero(u8);
        void SetHero(u8, HeroBase *);
        TroopIndex GetIndexFromTroop(const Army::BattleTroop &);
        Army::BattleTroop &GetTroopFromIndex(TroopIndex);
        void GetTroopOrdering(IndexList &);
        static TroopIndex ApplyIndexModifier(TroopIndex);
        u8 GetSideFromIndex(TroopIndex);

        static TroopIndex FindTroop(const Army::BattleArmy_t &, const Point &);
        static TroopIndex FindTroop(const std::vector<Army::BattleTroop*> &, const Point &);
        static TroopIndex FindTroopExact(const Army::BattleArmy_t &, const Point &);
        static bool IsTroopAt(const Army::BattleTroop &, const Point &);
        bool CellFree(const Point &, TroopIndex skip=99);
        bool CellFreeFor(const Point &, const Army::BattleTroop &, TroopIndex skip=99);
        PointList *FindPath(const Point& , const Point &, int , const Army::BattleTroop &, TroopIndex);

        bool IsRangedAttackBlocked(const Army::BattleTroop &, const Army::BattleTroop &);
        int CanAttack(const Army::BattleTroop &, const PointList &, const Army::BattleTroop &, const Point &);
        bool IsTroopCloseTo(const Army::BattleTroop &, const Army::BattleTroop &);

        void GetTargets(std::vector<Army::BattleTroop *> &, const Army::BattleTroop &, const Point &);

        void CleanupBodies();

        void Redraw(const PointList * = NULL, const Point * = NULL, const Army::BattleTroop * = NULL);
        void RedrawBackground(const PointList * = NULL, const Army::BattleTroop * = NULL);

        void AnimateMorale(bool, const Army::BattleTroop &);
        void AnimateMagic(std::vector<Army::BattleTroop*> &, HeroBase *, HeroBase *, bool, Spell::spell_t);

        Rect &GetHeroRect(u8 num) { return m_heroRect[num]; }
        BattleCastle *GetCastle() { return m_castle; }
        Catapult *GetCatapult() { return m_catapult; }
        
      private:
        void InitArmyPosition(Army::BattleArmy_t &, bool, bool);
        
        class Background
        {
          public:
            Background(const Point &, const Maps::Tiles &);
            void Draw();
            void DrawGrid();
            void DrawOverBackground(const PointList &, const Army::BattleTroop &);
            void DrawCell(const Point&);
            void DrawMark(const Point&, int animframe=0);
            void DrawCursor(const Point &);
            void DrawShadow(const Point &);
            void DrawPath(const PointList &);
            void DrawObject(const Point &) const;
            Rect DrawHeroObject(const HeroBase &, u16, bool, int fframe=0);
            void DrawForeground(BattleCastle *, Army::BattleArmy_t &, Army::BattleArmy_t &, int, bool);
            void DrawTroop(Army::BattleTroop &, BattleCastle *, int animframe=-1);
            void AddBody(const Army::BattleTroop &);
            void GetBlockedCells(PointList &) const;
            void DrawMorale(bool, const Army::BattleTroop &);
            void MagicAnimation(std::vector<Army::BattleTroop*> &, HeroBase *, HeroBase *, bool, Spell::spell_t);
            
          private:
            void DrawCaptain(const Captain &, bool, ICN::icn_t &, Rect &, Rect &);
            void DrawHero(const Heroes &, bool, ICN::icn_t &, Rect &, Rect &);
            
            enum Indicator_t {
                BLUE   = 10,
                BLUE_2 = 11,
                GREEN  = 12,
                YELLOW = 13,
                RED    = 14
            };
            Indicator_t IndicatorFromStatus(const Army::BattleTroop &);
            
            const Point &dst_pt;
            bool m_trees;
            ICN::icn_t m_background;
            ICN::icn_t m_fringe;
            
            struct TerrainObject
            {
                ICN::icn_t icn;
                Point bf, scr;
                bool wide;
            };
            
            std::vector<TerrainObject> m_terrainObjects;
            Army::BattleArmy_t m_bodies;

            int flagframe;
        };
        
        Background m_background;
        Catapult *m_catapult;
        PointList m_blockedCells;
        BattleCastle *m_castle;
        Army::BattleArmy_t *m_army[2];
        HeroBase *m_hero[2];
        Rect m_heroRect[2];
    };
    
    class GUI
    {
      public:
        enum interaction_t {
            NONE,
            AUTO,
            SKIP,
            SPELL,
            SURRENDER,
            RETREAT
        };
        
        GUI(HeroBase *, HeroBase *);
        ~GUI();
        void Redraw();
        void Status(const std::string &, bool secondOnly = false);
        interaction_t Interact(TroopIndex, Spell::spell_t &, Battlefield &, bool &);
        
      private:
        Dialog::FrameBorder *m_frameborder;
        StatusBar *m_statusBar[2];
        Button *m_skip;
        Button *m_auto;
        Button *m_settings;
        HeroBase *m_hero[2];
    };

    class BattleTurn;
    
    class BattleControl
    {
      public:
        BattleControl(Heroes &, Heroes &, const Maps::Tiles &, u32 &);
        BattleControl(Heroes &, Army::army_t &, const Maps::Tiles &, u32 &);
        BattleControl(Heroes &, Castle &, const Maps::Tiles &, u32 &);
        ~BattleControl();
        Army::battle_t GetStatus() const { return m_battleStatus; }

      private:
        void InitializeLogicSettings(HeroBase *, HeroBase *);
        void NewTurn();
        void PerformTowerAttack(const Point &, const Point &, BattleCastle::CastlePieces);
        Army::battle_t RunBattle(HeroBase *, HeroBase *);
        bool ShowLeftHeroResults(HeroBase &left, HeroBase *right, Army::battle_t result);
        u32 CalculateRaisedDead(HeroBase &hero, u32 perished);
        Spell::spell_t GetLearnedSpell(HeroBase &hero, std::vector<Spell::spell_t> &castSpells);
        u32 BattleSummaryVsArmy(HeroBase &, const Army::BattleArmy_t &, const Army::BattleArmy_t &, const Army::BattleArmy_t &);
        u32 BattleSummaryVsHero(HeroBase &, const Army::BattleArmy_t &, HeroBase &, const Army::BattleArmy_t &);
        BattleTurn *CreateTurn(const HeroBase *, Army::BattleArmy_t &, Army::BattleArmy_t &, bool forceComputer = false);
        Army::BattleTroop &NextValidTroop(s8 &, IndexList &);
        bool PerformMove(TroopIndex, const Point &, bool);
        bool PerformAttack(TroopIndex, const Point &);
        void PerformAttackPreLogic(Army::BattleTroop &, const std::vector<Army::BattleTroop *> &, bool);
        bool PerformAttackLogic(Army::BattleTroop &, const std::vector<Army::BattleTroop *> &, bool, long &, u16 &);
        void PerformAttackAnimation(Army::BattleTroop &, const std::vector<Army::BattleTroop *> &, bool);
        bool PerformMagic(std::vector<Army::BattleTroop*> &, HeroBase *, Spell::spell_t);
        void PerformMagicAnimation(std::vector<Army::BattleTroop*> &, Spell::spell_t, HeroBase *);

        long CalculateDamage(const Army::BattleTroop &, const Army::BattleTroop &);
        void ActOnGoodMorale(const Army::BattleTroop &, s8 &, bool &);
        void AdjustMorale(HeroBase *, Army::BattleTroop &);

        Army::battle_t m_battleStatus;
        GUI *m_gui;
        Battlefield m_battlefield;
        u32 m_experience[2];
        u32 m_perished;
        std::vector<Spell::spell_t> m_spellsCast;
        const HeroBase* m_commanders[2];
    };

    class BattleTurn
    {
      public:
        BattleTurn(Battlefield *, GUI *, HeroBase *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        virtual ~BattleTurn() {}
        virtual GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        virtual bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);
        Army::BattleArmy_t &GetOwnArmy() const { return *m_ownArmy; }
        Army::BattleArmy_t &GetOppArmy() const { return *m_oppArmy; }
        HeroBase *GetHero() { return m_hero[0]; }
        HeroBase *GetOppHero() { return m_hero[1]; }
        Spell::spell_t GetSpell() { return m_spell; }
        std::vector<Army::BattleTroop *> &GetSpellTargets() { return m_targets; }

      private:
        void PrepMovePoints(TroopIndex);
        void PrepMovePointsInt(const Army::BattleTroop &, const Point &, int, int);

      protected:
        Battlefield *m_battlefield;
        GUI *m_gui;
        PointList m_movePoints;
        Army::BattleArmy_t *m_ownArmy, *m_oppArmy;
        HeroBase *m_hero[2];
        Spell::spell_t m_spell;
        std::vector<Army::BattleTroop *> m_targets;
    };

    class HumanTurn : public BattleTurn
    {
      public:
        HumanTurn(Battlefield *, GUI *, HeroBase *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);
    };

    class ComputerTurn : public BattleTurn
    {
      public:
        ComputerTurn(Battlefield *, GUI *, HeroBase *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);
        Point GetMostDangerousOpposingUnit();

      private:
        static bool DangerousUnitPredicate(const Army::BattleTroop &, const Army::BattleTroop &);
        bool AttackTroopInList(const Army::BattleArmy_t &, TroopIndex, Point &, Point &);
        void MoveToClosestTroopInList(const Army::BattleArmy_t &, const Army::BattleTroop &, Point &);
        void MoveOrAttack(TroopIndex, Point &, Point &, bool);
        bool AttackRangedTroop(TroopIndex, Point &, Point &);
        bool AttackNonRangedTroop(TroopIndex, Point &, Point &);
        bool CanAttackFrom(const Army::BattleTroop &, const Army::BattleTroop &, const Point &);
        Point GetReachableAttackCell(const Army::BattleTroop &, const Army::BattleArmy_t &, const Army::BattleArmy_t &, TroopIndex);
    };
}

#endif
