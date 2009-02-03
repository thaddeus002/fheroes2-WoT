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

namespace Battle
{
    extern Point g_baseOffset;
    
    Point Scr2BfDirect(const Point & pt); // translate coordinate to battle field
    Point Scr2Bf(const Point & pt); // translate coordinate to battle field
    Point Bf2Scr(const Point & pt); // translate to screen (absolute position)
    Point Bf2ScrNoOffset(const Point& pt); // translate to screen (offset to frame border)
    bool BfValid(const Point & pt); // check battle field point

    void BattleSummary(const std::string &name, const Army::ArmyPairs &armies,
                       const BagArtifacts *artifacts, Spell::spell_t spell,
                       int deadRaised, Army::battle_t status);
    Army::battle_t HeroStatus(Heroes &, StatusBar &, Spell::spell_t &, bool, bool, bool);
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
    
    class Battlefield
    {
      public:
        Battlefield(Heroes *, Heroes *, Army::army_t &, Army::army_t &, Point, const Maps::Tiles &);

        Army::BattleArmy_t &GetArmy(u8);
        Heroes *GetHero(u8);
        TroopIndex GetIndexFromTroop(const Army::BattleTroop &);
        Army::BattleTroop &GetTroopFromIndex(TroopIndex);
        void GetTroopOrdering(IndexList &);
        static TroopIndex ApplyIndexModifier(TroopIndex);
        u8 GetSideFromIndex(TroopIndex);

        static int FindTroop(const Army::BattleArmy_t &, const Point &);
        static int FindTroop(const std::vector<Army::BattleTroop*> &, const Point &);
        static int FindTroopExact(const Army::BattleArmy_t &, const Point &);
        static bool IsTroopAt(const Army::BattleTroop &, const Point &);
        bool CellFree(const Point &, int skip=99);
        bool CellFreeFor(const Point &, const Army::BattleTroop &, int skip=99);
        PointList *FindPath(const Point& , const Point &, int , const Army::BattleTroop &, TroopIndex);

        int CanAttack(const Army::BattleTroop &, const PointList &, const Army::BattleTroop &, const Point &);
        bool IsTroopCloseTo(const Army::BattleTroop &, const Army::BattleTroop &);

        void GetTargets(std::vector<Army::BattleTroop *> &, const Army::BattleTroop &, const Point &);

        bool CleanupBodies(TroopIndex);

        void Redraw(const PointList * = NULL, const Point * = NULL, const Army::BattleTroop * = NULL);

        void AnimateMorale(bool, const Army::BattleTroop &);
        void AnimateMagic(std::vector<Army::BattleTroop*> &, Heroes *, Heroes *, bool, Spell::spell_t);

        Rect &GetHeroRect(u8 num) { return m_heroRect[num]; }
        
      private:
        void InitArmyPosition(Army::BattleArmy_t &, bool, bool);
        
        class Background
        {
          public:
            Background(const Point &, const Maps::Tiles &);
            void Draw();
            void DrawOverBackground(const PointList &, const Point &, const Army::BattleTroop &);
            void DrawCell(const Point&pt);
            void DrawMark(const Point&pt, int animframe=0);
            void DrawShadow(const Point &pt);
            void DrawPath(const PointList &path);
            void DrawObject(const Point &pt) const;
            void DrawCaptain(const Race::race_t race, u16 animframe, bool reflect) const;
            Rect DrawHero(const Heroes & hero, u16 animframe, bool reflect, int fframe=-1);
            void DrawArmies(Army::BattleArmy_t & army1, Army::BattleArmy_t & army2, int animframe, bool idleAnimations);
            void DrawTroop(Army::BattleTroop & troop, int animframe=-1);
            void AddBody(const Army::BattleTroop &);
            void GetBlockedCells(PointList &) const;
            void DrawMorale(bool, const Army::BattleTroop &);
            void MagicAnimation(std::vector<Army::BattleTroop*> &, Heroes *, Heroes *, bool, Spell::spell_t);
            
          private:
            Point dst_pt;
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
        };
        
        Background m_background;
        PointList m_blockedCells;
        Army::BattleArmy_t *m_army[2];
        Heroes *m_hero[2];
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
        
        GUI(Heroes *, Heroes *);
        ~GUI();
        void Redraw();
        void Status(const std::string &, bool secondOnly = false);
        interaction_t Interact(TroopIndex, Spell::spell_t &, Battlefield &, bool &);
        
      private:
        Dialog::FrameBorder *m_frameborder;
        StatusBar m_statusBar[2];
        Button *m_skip;
        Button *m_auto;
        Button *m_settings;
        Heroes *m_hero[2];
    };

    class BattleTurn;
    
    class BattleControl
    {
      public:
        BattleControl(Heroes &, Heroes &, const Maps::Tiles &);
        BattleControl(Heroes &, Army::army_t &, const Maps::Tiles &);
        BattleControl(Heroes &, Castle &, const Maps::Tiles &);
        ~BattleControl();
        Army::battle_t GetStatus() const { return m_battleStatus; }

      private:
        void InitializeLogicSettings(Heroes *, Heroes *);
        Army::battle_t RunBattle(Heroes *, Heroes *);
        void BattleSummaryVsArmy(Heroes &, const Army::BattleArmy_t &, const Army::BattleArmy_t &, const Army::BattleArmy_t &);
        void BattleSummaryVsHero(Heroes &, const Army::BattleArmy_t &, Heroes &, const Army::BattleArmy_t &);
        BattleTurn *CreateTurn(Heroes *, Army::BattleArmy_t &, Army::BattleArmy_t &, bool forceComputer = false);
        Army::BattleTroop &NextValidTroop(s8 &, IndexList &);
        bool PerformMove(TroopIndex, const Point &);
        bool PerformAttack(TroopIndex, const Point &);
        bool PerformAttackLogic(Army::BattleTroop &, const std::vector<Army::BattleTroop *> &, bool, long &, u16 &);
        void PerformAttackAnimation(Army::BattleTroop &, const std::vector<Army::BattleTroop *> &, bool);
        bool PerformMagic(std::vector<Army::BattleTroop*> &, Heroes *, Spell::spell_t);
        void PerformMagicAnimation(std::vector<Army::BattleTroop*> &, Spell::spell_t, Heroes *);

        long CalculateDamage(const Army::BattleTroop &, const Army::BattleTroop &);
        bool AdjustMorale(Heroes *, Army::BattleTroop &);

        Army::battle_t m_battleStatus;
        GUI *m_gui;
        Battlefield m_battlefield;
    };

    class BattleTurn
    {
      public:
        BattleTurn(Battlefield *, GUI *, Heroes *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        virtual ~BattleTurn() {}
        virtual GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        virtual bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);
        Army::BattleArmy_t &GetOwnArmy() const { return *m_ownArmy; }
        Army::BattleArmy_t &GetOppArmy() const { return *m_oppArmy; }
        Heroes *GetHero() { return m_hero[0]; }
        Heroes *GetOppHero() { return m_hero[1]; }
        Spell::spell_t GetSpell() { return m_spell; }
        std::vector<Army::BattleTroop *> &GetSpellTargets() { return m_targets; }

      private:
        void PrepMovePoints(TroopIndex, const Army::BattleArmy_t &, const Army::BattleArmy_t &);
        void PrepMovePointsInt(const Army::BattleTroop &, const Point &, int, int);

      protected:
        Battlefield *m_battlefield;
        GUI *m_gui;
        PointList m_movePoints;
        Army::BattleArmy_t *m_ownArmy, *m_oppArmy;
        Heroes *m_hero[2];
        Spell::spell_t m_spell;
        std::vector<Army::BattleTroop *> m_targets;
    };

    class HumanTurn : public BattleTurn
    {
      public:
        HumanTurn(Battlefield *, GUI *, Heroes *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);
    };

    class ComputerTurn : public BattleTurn
    {
      public:
        ComputerTurn(Battlefield *, GUI *, Heroes *, Army::BattleArmy_t &, Army::BattleArmy_t &);
        GUI::interaction_t Run(Army::BattleTroop &, TroopIndex, Point &, Point &);
        bool MagicSelectTarget(std::vector<Army::BattleTroop *> &, const Spell::spell_t &);

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
