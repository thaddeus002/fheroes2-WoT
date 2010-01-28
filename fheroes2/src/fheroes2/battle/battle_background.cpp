/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#ifdef WITH_BATTLE1

#include <cmath>
#include "battle.h"
#include "battle_types.h"
#include "engine.h"
#include "icn.h"
#include "mp2.h"
#include "world.h"
#include "agg.h"
#include "settings.h"
#include "battle_troop.h"
#include "captain.h"

#define display Display::Get()
#define le LocalEvent::Get()

namespace Battle
{
    Battlefield::Background::Background(const Point &pt, const Maps::Tiles & tile)
    : dst_pt(pt)
    {
        flagframe = 0;
        
        m_fringe = m_background = ICN::UNKNOWN;
         u16 index = tile.GetIndex();
         bool trees = false;
         u16 x = index % world.w(), y = index / world.w();
         if((x < world.w() - 1 && world.GetTiles(index + 1).GetObject() == MP2::OBJ_TREES) ||
            (x > 0 && world.GetTiles(index - 1).GetObject() == MP2::OBJ_TREES) ||
            (y < world.h() - 1 && world.GetTiles(index + world.w()).GetObject() == MP2::OBJ_TREES) ||
            (y > 0 && world.GetTiles(index - world.w()).GetObject() == MP2::OBJ_TREES) )
             trees = true;
         if(tile.GetObject() == MP2::OBJN_GRAVEYARD) {
             m_background = ICN::CBKGGRAV;
             m_fringe = ICN::FRNG0001;
         }
         else
         switch(tile.GetGround()) {
             case Maps::Ground::DESERT: 
                 m_background = ICN::CBKGDSRT;
                 m_fringe = ICN::FRNG0003; 
                 break;
             case Maps::Ground::SNOW: 
                 m_background = trees ? ICN::CBKGSNTR : ICN::CBKGSNMT; 
                 m_fringe = trees ? ICN::FRNG0006 : ICN::FRNG0007;
                 break;
             case Maps::Ground::SWAMP: 
                 m_background = ICN::CBKGSWMP; 
                 m_fringe = ICN::FRNG0008; 
                 break;
             case Maps::Ground::WASTELAND: 
                 m_background = ICN::CBKGCRCK; 
                 m_fringe = ICN::FRNG0002; 
                 break;
             case Maps::Ground::BEACH: 
                 m_background = ICN::CBKGBEAC; 
                 m_fringe = ICN::FRNG0004; 
                 break;
             case Maps::Ground::LAVA: 
                 m_background = ICN::CBKGLAVA; 
                 m_fringe = ICN::FRNG0005; 
                 break;
             case Maps::Ground::DIRT: 
                 m_background = trees ? ICN::CBKGDITR : ICN::CBKGDIMT; 
                 m_fringe = trees ? ICN::FRNG0010 : ICN::FRNG0009;
                 break;
             case Maps::Ground::GRASS:
                 m_background = trees ? ICN::CBKGGRTR : ICN::CBKGGRMT; 
                 m_fringe = trees ? ICN::FRNG0011 : ICN::FRNG0012;
                 break;
             case Maps::Ground::WATER: 
                 m_background = ICN::CBKGWATR; 
                 m_fringe = ICN::FRNG0013; 
                 break;
             default:
                 break;
         }

          int obj = Rand::Get(1,4);
          for(int i=0; i < obj; i++) {
              Point p;
              p.x = Rand::Get(2, BFW-4);
              p.y = Rand::Get(0, BFH-1);
              TerrainObject cobj;
              cobj.wide = false;
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
                          cobj.wide = true;
                          break;
                  }
              }
              else
              switch(tile.GetGround()) {
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
                              cobj.wide = true;
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
                              cobj.wide = true;
                              break;
                          case 1:
                              cobj.icn = ICN::COBJ0006;
                              break;
                          case 2:
                              cobj.icn = ICN::COBJ0015;
                              cobj.wide = true;
                              break;
                          case 3:
                              cobj.icn = ICN::COBJ0016;
                              cobj.wide = true;
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
                              cobj.wide = true;
                              break;
                          case 2:
                              cobj.icn = ICN::COBJ0020;
                              cobj.wide = true;
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
                              cobj.wide = true;
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
                              cobj.wide = true;
                              break;
                          case 2:
                              cobj.icn = ICN::COBJ0031;
                              cobj.wide = true;
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
                              cobj.wide = true;
                              break;
                          case 2:
                              cobj.icn = ICN::COBJ0007;
                              cobj.wide = true;
                              break;
                          case 3:
                              cobj.icn = ICN::COBJ0011;
                              cobj.wide = true;
                              break;
                          case 4:
                              cobj.icn = ICN::COBJ0012;
                              break;
                          case 5:
                              cobj.icn = ICN::COBJ0014;
                              cobj.wide = true;
                              break;
                          case 6:
                              cobj.icn = ICN::COBJ0019;
                              cobj.wide = true;
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
              cobj.bf = p;
              m_terrainObjects.push_back(cobj);
          }
    }

    void Battlefield::Background::GetBlockedCells(PointList &blocked) const
    {
        for(u8 i = 0; i < m_terrainObjects.size(); i++)
        {
            Point p = m_terrainObjects[i].bf;
            blocked.push_back(p);
            if(m_terrainObjects[i].wide)
                blocked.push_back(p + Point(1, 0));
        }
    }
    
    void Battlefield::Background::Draw()
    {
        display.Blit(AGG::GetICN(m_background, 0), dst_pt);
        Point pt = dst_pt;
        pt.y += 200;
        display.Blit(AGG::GetICN(m_fringe, 0), pt);
    }

    void Battlefield::Background::DrawGrid()
    {
        if(BattleSettings::Get().Modes(BattleSettings::OPT_GRID))
        {
            Point p(0, 0);
            for(p.x = 0; p.x < BFW; p.x++)
                for(p.y = 0; p.y < BFH; p.y++)
                    DrawCell(p);
        }
    }

    void Battlefield::Background::DrawOverBackground(const PointList &movePoints, const Army::BattleTroop &troop)
    {
        if(BattleSettings::Get().Modes(BattleSettings::OPT_MOVESHADOW))
            for(u16 i = 0; i < movePoints.size(); i++)
                DrawShadow(movePoints[i]);
        
        /*DrawMark(troop.Position(), 1);
        if(troop.isWide()) {
            Point p = troop.Position();
            p.x += troop.IsReflected() ? -1 : 1;
            DrawMark(p);
            }*/
    }

    void Battlefield::Background::DrawCursor(const Point &cur_pt)
    {
        if(BattleSettings::Get().Modes(BattleSettings::OPT_MOUSESHADOW))
            if(BfValid(cur_pt))
                DrawShadow(cur_pt);
    }

    void Battlefield::Background::DrawShadow(const Point &pt)
    {
        static Surface shadow;
        if(!shadow.valid())
	{
	    shadow.Set(CELLW, (u16) (CELLH + 2 * CELLWDELTA - 6), 16, SWSURFACE);
	    shadow.SetColorKey();

            Rect r(0, 0, shadow.w(), shadow.h());
            r.y += (s16) (CELLWDELTA - 1);
            r.h -= (u16) (2*CELLWDELTA - 2);
            shadow.FillRect(0x00, 0x00, 0x00, r);
            r.x += 3;
            r.w -= 6;
            for(int i=1; i< CELLW/4; i+=3) {
                r.y -- ;
		r.h += 2;
                shadow.FillRect(0x00, 0x00, 0x00, r);
                r.x += 3;
                r.w -= 6;
            }
	    shadow.SetAlpha(0x30);
        }

        Point p = Bf2Scr(pt);
        p.y -= (s16)(CELLH+CELLWDELTA);
        p.x -= CELLW/2;
        display.Blit(shadow, p);
    }

    void Battlefield::Background::DrawCell(const Point&pt)
    {
        static Surface cell;
        if(!cell.valid())
	{
	    cell.Set(CELLW, (u16)(CELLH + 2 * CELLWDELTA - 6));
	    cell.SetColorKey();
	    const u32 color = cell.GetColor(0xE1);

	    // hexagon
	    cell.DrawLine(cell.w() / 2, 0, 0, (u16)(CELLWDELTA), color);
	    cell.DrawLine(cell.w() / 2, 0, cell.w() - 1, (u16)(CELLWDELTA), color);

	    cell.DrawLine(0, (u16)(CELLWDELTA), 0, (u16)(cell.h() - CELLWDELTA), color);
	    cell.DrawLine(cell.w() - 1, (u16)(CELLWDELTA), cell.w() - 1, (u16)(cell.h() - CELLWDELTA), color);

	    cell.DrawLine(0, (u16)(cell.h() - CELLWDELTA), cell.w() / 2, cell.h() -1, color);
	    cell.DrawLine(cell.w() - 1, (u16)(cell.h() - CELLWDELTA), cell.w() / 2, cell.h() - 1, color);
        }
        Point p = Bf2Scr(pt);
        p.y -= (s16)(CELLH+CELLWDELTA);
        p.x -= CELLW/2;
        display.Blit(cell, p);
    }

    void Battlefield::Background::DrawPath(const PointList &path)
    {
        for(u16 i = 0; i < path.size(); i++)
            DrawShadow(path[i]);
    }

    void Battlefield::Background::DrawObject(const Point &pt) const
    {
        for(u16 i = 0; i < m_terrainObjects.size(); i++)
            if(m_terrainObjects[i].bf == pt)
            {
                const Sprite &spr = AGG::GetICN(m_terrainObjects[i].icn, 0);
                Point scr = Bf2Scr(m_terrainObjects[i].bf);
                display.Blit(spr, scr.x + spr.x(), scr.y + spr.y());
                break;
            }
    }

    Rect Battlefield::Background::DrawHeroObject(const HeroBase &hero, u16 animframe, bool reflect, int fframe)
    {
        flagframe += fframe;

        ICN::icn_t icn;
        Rect rect, flagRect;
        if(hero.GetType() == Skill::Primary::CAPTAIN)
            DrawCaptain(hero, reflect, icn, rect, flagRect);
        else DrawHero(hero,  reflect, icn, rect, flagRect);

        const Sprite &sp = AGG::GetICN(icn, animframe, reflect);
        display.Blit(sp, rect.x, rect.y);

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
        display.Blit(AGG::GetICN(flag, flagframe%5, reflect), flagRect.x, flagRect.y);

        return rect;
    }

    void Battlefield::Background::DrawCaptain(const HeroBase &captain, bool reflect, ICN::icn_t &icn, Rect &rect, Rect &flagRect)
    {
        switch(captain.GetRace()) {
            case Race::BARB: icn = ICN::CMBTCAPB; break;
            case Race::KNGT: icn = ICN::CMBTCAPK; break;
            case Race::NECR: icn = ICN::CMBTCAPN; break;
            case Race::SORC: icn = ICN::CMBTCAPS; break;
            case Race::WRLK: icn = ICN::CMBTCAPW; break;
            case Race::WZRD: icn = ICN::CMBTCAPZ; break;
            default: icn = ICN::UNKNOWN;
        }
        const Sprite &sp = AGG::GetICN(icn, 1, reflect);
        rect.x = dst_pt.x + (reflect ? 590: 0);
        rect.y = dst_pt.y + 45;
        rect.w = sp.w();
        rect.h = sp.h();

        flagRect.x = dst_pt.x + (reflect ? 640 - sp.w() - 17: 17);
        flagRect.y = dst_pt.y + 50;
    }
    
    void Battlefield::Background::DrawHero(const HeroBase &hero, bool reflect, ICN::icn_t &icn, Rect &rect, Rect &flagRect)
    {
        switch(hero.GetRace()) {
            case Race::BARB: icn = ICN::CMBTHROB; break;
            case Race::KNGT: icn = ICN::CMBTHROK; break;
            case Race::NECR: icn = ICN::CMBTHRON; break;
            case Race::SORC: icn = ICN::CMBTHROS; break;
            case Race::WRLK: icn = ICN::CMBTHROW; break;
            case Race::WZRD: icn = ICN::CMBTHROZ; break;
            default: icn = ICN::UNKNOWN; break;
        }
        const Sprite & sp = AGG::GetICN(icn, 1, reflect);
        rect.x = dst_pt.x + (reflect?640-sp.w():0);
        rect.y = dst_pt.y + 75;
        rect.w = sp.w();
        rect.h = sp.h();

        flagRect.x = dst_pt.x + (reflect ? 640 - sp.w() + 17 : 17);
        flagRect.y = dst_pt.y + 80;
    }
    
    void Battlefield::Background::DrawForeground(BattleCastle *castle, Army::BattleArmy_t & army1, Army::BattleArmy_t & army2, int animframe, bool idleAnimations)
    {
        for(u16 i = 0; i < m_bodies.size(); i++)
            DrawTroop(m_bodies[i], castle);
        
        Army::BattleTroop *inMotion = NULL;
        for(u16 i = 0; i < army1.size(); i++)
            if(army1[i].isValid() && army1[i].isMoving() == Army::IN_MOTION)
                inMotion = &army1[i];
        for(u16 i = 0; i < army2.size(); i++)
            if(army2[i].isValid() && army2[i].isMoving() == Army::IN_MOTION)
                inMotion = &army2[i];
        
        Point p;
        Point adjusted = inMotion ? Scr2BfDirect(inMotion->ScreenPosition()) : Point(-1, -1);
        int t = -1;
        for(p.y = 0; p.y < BFH; p.y++) 
            for(p.x = 0; p.x < BFW; p.x++) {
                Point temp(-1, -1);
                if(p.y > BFH / 2)
                {
                    temp = p;
                    p.x = BFW - p.x - 1;
                }
                
                if(!castle) // No debris if castle present
                    DrawObject(p);

                if(castle && p.y < BFH / 2)
                    castle->DrawCell(p);
                
                if(t = FindTroopExact(army1, p), t >= 0 && army1[t].isMoving() != Army::IN_MOTION)
                {
                    DrawTroop(army1[t], castle, animframe);
                    army1[t].Animate();
                    if(idleAnimations && army1[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
                        army1[t].Animate(Monster::AS_IDLE);
                }
                if(t = FindTroopExact(army2, p), t >= 0 && army2[t].isMoving() != Army::IN_MOTION)
                {
                    DrawTroop(army2[t], castle, animframe);
                    army2[t].Animate();
                    if(idleAnimations && army2[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
                        army2[t].Animate(Monster::AS_IDLE);
                }

                if(castle && p.y >= BFH / 2)
                    castle->DrawCell(p);
                
                if(BfValid(adjusted) && p == adjusted)
                {
                    inMotion->Blit(inMotion->ScreenPosition(), inMotion->IsReflected());
                    inMotion->Animate();
                }

                if(BfValid(temp))
                    p = temp;
            }
    }

    Battlefield::Background::Indicator_t Battlefield::Background::IndicatorFromStatus(const Army::BattleTroop &troop)
    {
        //TODO: Check with original HoMM II status indicators
        if(troop.Modes(Army::SP_BLESS | Army::SP_STONESKIN | Army::SP_STEELSKIN))
            return GREEN;
        else if(troop.Modes(Army::SP_BLIND | Army::SP_PARALYZE | Army::SP_CURSE))
            return RED;
        else if(troop.Modes(Army::SP_HASTE | Army::SP_SHIELD))
            return YELLOW;
        else return BLUE; 
    }

    void Battlefield::Background::DrawTroop(Army::BattleTroop & troop, BattleCastle *castle, int animframe)
    {
        static u32 frame = 0;
        if(troop() == Monster::UNKNOWN) return;
        bool reflect = troop.IsReflected();
        bool wide = troop.isWide();
        Point pos = troop.isMoving() == Army::IN_MOTION ? Scr2BfDirect(troop.ScreenPosition())
                                                        : troop.Position();
        Point tp = Bf2Scr(pos);
        troop.Blit(tp, reflect, animframe);
        if(troop.isValid() && troop.isMoving() == Army::SELECTED)
        {
            frame -= animframe;
            u8 start, count;
            troop.GetAnimFrames(troop.astate, start, count, false);
            Surface *outline;
            if(troop.astate != Monster::AS_NONE)
                outline = troop.GetContour(troop.aframe - start + 1);
            else outline = troop.GetContour(troop.aframe - start);
            if(!outline)
            {
                DEBUG(DBG_BATTLE , DBG_WARN, "Background::DrawTroop: invalid contour " << troop.aframe);
            }
            else
            {
                outline->SetAlpha(abs((frame%21)-10)*20+55);
                Point offset(troop.GetBlitOffset(troop.aframe, troop.IsReflected()));
                display.Blit(*outline, tp + offset);
            }
        }

        if(troop.isMoving() != Army::IN_MOTION && castle && castle->hasMoat() &&
           (castle->IsCellInMoat(pos) ||
            (wide && castle->IsCellInMoat(pos + Point(reflect ? -1 : 1, 0)))))
        {
            const Sprite &moat = AGG::GetICN(ICN::MOATPART, pos.y);
            display.Blit(moat, moat.x() + g_baseOffset.x, moat.y() + g_baseOffset.y);
        }

        // No count if troop is in motion
        if(troop.isMoving() == Army::IN_MOTION || troop.isMoving() == Army::UNDER_ATTACK)
            return;
        
        // draw count
        if(troop.Count() == 0) return;
        int offset = wide ? CELLW : 0;
        tp.x += reflect ? -20 - offset : offset;
        tp.y -= 10;
        u16 ind = IndicatorFromStatus(troop);
        display.Blit(AGG::GetICN(ICN::TEXTBAR, ind), tp);
        std::string str;
        int count = troop.Count();
        if(count < 1000)
            String::AddInt(str, count);
        else if(count < 1000000)
            String::AddInt(str, count / 1000), str += "K";
        else 
            String::AddInt(str, count / 1000000), str += "M";
        Text text(str, Font::SMALL);
        tp.x += 10 - text.w() / 2;
        text.Blit(tp);
    }

    void Battlefield::Background::AddBody(const Army::BattleTroop &body)
    {
        m_bodies.push_back(body);
    }

    void Battlefield::Background::DrawMorale(bool goodMorale, const Army::BattleTroop &troop)
    {
        ICN::icn_t icn = goodMorale ? ICN::MORALEG : ICN::MORALEB;
        M82::m82_t m82 = goodMorale ? M82::GOODMRLE : M82::BADMRLE;

        AGG::PlaySound(m82);
        for(u16 i=0; i < AGG::GetICNCount(icn); i++) {
            const Sprite &sp = AGG::GetICN(icn, i);
            Rect pos_rt = sp;
            pos_rt += Bf2Scr(troop.Position());
            ::Background back(pos_rt);
            back.Save();
            display.Blit(sp, pos_rt);
            display.Flip();
            int animat = 0;
            while(le.HandleEvents()) {
                if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
            }
            back.Restore();
        }
    }

    void BattleCastle::DrawBackground()
    {
        const Sprite &ground = AGG::GetICN(m_groundICN, 1);
        const Sprite &backwall = AGG::GetICN(m_groundICN, hasFortifications() ? 4 : 3);
        const Sprite &reference = AGG::GetICN(ICN::CBKGDSRT, 0);
        Point groundPt(g_baseOffset.x + reference.w() - ground.w(), Bf2Scr(Point(0, 0)).y - CELLH/2 + 4);
        Point backwallPt(groundPt.x + ground.w() - backwall.w(), groundPt.y - backwall.h() + 4);

        display.Blit(ground, groundPt);
        display.Blit(backwall, backwallPt);
    
        if(hasMoat())
        {   
            const Sprite &moat = AGG::GetICN(ICN::MOATWHOL, 0);
            display.Blit(moat, Point(moat.x() + g_baseOffset.x, moat.y() + g_baseOffset.y));
        }
    }

    void BattleCastle::DrawForeground()
    {
        display.Blit(*m_castleParts[TOWER_KEEP]->m_sprite,
                     getCastlePartDisplay(TOWER_KEEP) + g_baseOffset);
    }

    void BattleCastle::DrawCell(const Point &pt)
    {
        for(u16 i = 0; i < m_castleParts.size(); i++)
        {
            if(m_castleParts[i]->m_logical == pt && m_castleParts[i]->m_sprite)
                display.Blit(*m_castleParts[i]->m_sprite, getCastlePartDisplay(i) + g_baseOffset);
        }
    }

    void Catapult::Draw()
    {
        point = Point(g_baseOffset.x, g_baseOffset.y + Bf2ScrNoOffset(Point(0, 5)).y);
        display.Blit(*sprite, point);
    }
    
    void Catapult::Animate(Battlefield &battlefield, BattleCastle &castle, u8 target, u8 damage)
    {
        int ticket = 0;
        int anim = 0;
        AGG::PlaySound(M82::CATSND00);
        int phase = PREATTACK;
        bool done = false;
        ICN::icn_t cloudICN = damage ? ICN::LICHCLOD : ICN::SMALCLOD;

        battlefield.RedrawBackground();
        if(!back)
            back = new Background(point - Point(0, 20), sprite->w(), sprite->h() + 20);
        back->Save();
        battlefield.Redraw();
        
        while(!done && le.HandleEvents())
        {
            if(!Game::ShouldAnimateInfrequent(ticket++, 2))
                continue;

            switch(phase)
            {
                case PREATTACK:
                {
                    if(anim == AGG::GetICNCount(ICN::CATAPULT))
                    {
                        phase++;
                        anim = 0;
                        break;
                    }
                    back->Restore();
                    const Sprite &catapult = AGG::GetICN(ICN::CATAPULT, anim++);
                    display.Blit(catapult, point - Point(catapult.w() - sprite->w(), catapult.h() - sprite->h()));
                    break;
                }
                case MISSILE_FLIGHT:
                {
                    battlefield.Redraw();
                    //ICN::BOULDER
                    
                    phase++;
                    AGG::PlaySound(M82::CATSND02);
                    if(damage)
                        castle.DamagePart(target, damage);
                    break;
                }
                case MISSILE_IMPACT:
                {
                    if(anim == AGG::GetICNCount(cloudICN))
                    {
                        phase++;
                        break;
                    }
                    battlefield.Redraw();
                    const Sprite &cloud = AGG::GetICN(cloudICN, anim++);
                    const Sprite &targetSprite = castle.getCastlePartSprite(target);
                    Point disp = castle.getCastlePartDisplay(target) + g_baseOffset;
                    display.Blit(cloud, disp - Point((cloud.w() - targetSprite.w()) / 2, (cloud.h() - targetSprite.h()) / 2));
                    break;
                }    
                case POSTATTACK:
                    done = true;
                    break;
            }
            display.Flip();
        }
    }
}

#endif
