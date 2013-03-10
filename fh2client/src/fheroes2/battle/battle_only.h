/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2BATTLE_ONLY_H
#define H2BATTLE_ONLY_H

#include "army.h"
#include "players.h"
#include "heroes_recruits.h"
#include "network.h"

class ArmyBar;
class PrimarySkillsBar;
class SecondarySkillsBar;
class ArtifactsBar;
class MoraleIndicator;
class LuckIndicator;

namespace Battle
{
    struct ControlInfo
    {
	ControlInfo(const Point & pt, u8 & ctrl) : result(ctrl),
    	    rtLocal(pt.x, pt.y, 24, 24), rtAI(pt.x + 75, pt.y, 24, 24) {};

	void Redraw(void);

	const u8 & result;

	const Rect rtLocal;
	const Rect rtAI;
    };

    class Only
    {
    public:
	Heroes*		hero1;
	Heroes*		hero2;

	Player		player1;
	Player		player2;

	Army*		army1;
	Army*		army2;
	Army		monsters;

	MoraleIndicator* moraleIndicator1;
	MoraleIndicator* moraleIndicator2;

	LuckIndicator* luckIndicator1;
	LuckIndicator* luckIndicator2;

	PrimarySkillsBar* primskill_bar1;
	PrimarySkillsBar* primskill_bar2;

	SecondarySkillsBar* secskill_bar1;
	SecondarySkillsBar* secskill_bar2;

	ArmyBar* selectArmy1;
	ArmyBar* selectArmy2;

	ArtifactsBar* selectArtifacts1;
	ArtifactsBar* selectArtifacts2;

	ControlInfo*	cinfo2;

	Rect		rtPortrait1;
	Rect		rtPortrait2;

	Rect		rtAttack1;
	Rect		rtAttack2;
	Rect		rtDefense1;
	Rect		rtDefense2;
	Rect		rtPower1;
	Rect		rtPower2;
	Rect		rtKnowledge1;
	Rect		rtKnowledge2;

	const Rect	rt1;
	Surface		sfb1;
	Surface		sfc1;

	const Rect	rt2;
	Surface		sfb2;
	Surface		sfc2;

	bool		ChangeSettings(void);
	void		RedrawBaseInfo(const Point &);
	void		StartBattle(void);
	void		UpdateHero1(const Point &);
	void		UpdateHero2(const Point &);
	bool        	WaitForPlayerAllocationFromServer(void); 
	bool        	WaitForArmyInfoFromServer(void); 
	bool        	WaitForNetworkMessage(int); 
	bool        	ProcessNetworkStateChange(const NetworkEvent&);
	void        	ProcessSetArmyEvent(const NetworkEvent&, bool, bool&, const Point&);
	bool        	ProcessNetworkEvents(bool&, const Point&, bool&, bool&, Button&); 

	static Only &	Get(void);
	static Recruits GetHeroesFromStreamBuf(StreamBuf &);

	Only();
    private:
    static void PackSecondarySkills(NetworkMessage&, const Skill::SecSkills&);
    static void PackTroops(NetworkMessage&, const Army&);
    static void SendNewHero(const Heroes*);
	static void SendPrimarySkills(u8, u8, u8, u8);
	static void SendSecondarySkills(const Skill::SecSkills&);
	static void SendTroops(const Army&);
	static void SendArtifacts(BagArtifacts&);
    };
}

StreamBase & operator<< (StreamBase &, const Battle::Only &);
StreamBase & operator>> (StreamBase &, Battle::Only &);

#endif
