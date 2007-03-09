/*************************************************************************** 
 *   Copyright (C) 2006 by Andrey Afletdinov                               * 
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

#ifndef H2HEROES_H
#define H2HEROES_H

#include <utility>
#include "race.h"
#include "color.h"
#include "morale.h"
#include "luck.h"
#include "army.h"
#include "artifact.h"
#include "gamedefs.h"

#define HEROESMAXSKILL		8
#define HEROESMAXARTIFACT	14
#define HEROESMAXARMY		5

/*
#define HEROESMAXCOUNT		60
#define SCOUTINGBASE		4

#define DEFAULT_KNGT_ATTACK	2
#define DEFAULT_KNGT_DEFENCE	2
#define DEFAULT_KNGT_POWER	1
#define DEFAULT_KNGT_KNOWLEDGE	1
#define DEFAULT_BRBN_ATTACK	3
#define DEFAULT_BRBN_DEFENCE	1
#define DEFAULT_BRBN_POWER	1
#define DEFAULT_BRBN_KNOWLEDGE	1
#define DEFAULT_NCRM_ATTACK	1
#define DEFAULT_NCRM_DEFENCE	0
#define DEFAULT_NCRM_POWER	2
#define DEFAULT_NCRM_KNOWLEDGE	2
#define DEFAULT_SCRS_ATTACK	0
#define DEFAULT_SCRS_DEFENCE	0
#define DEFAULT_SCRS_POWER	2
#define DEFAULT_SCRS_KNOWLEDGE	3
#define DEFAULT_WRLK_ATTACK	0
#define DEFAULT_WRLK_DEFENCE	0
#define DEFAULT_WRLK_POWER	3
#define DEFAULT_WRLK_KNOWLEDGE	2
#define DEFAULT_WZRD_ATTACK	0
#define DEFAULT_WZRD_DEFENCE	1
#define DEFAULT_WZRD_POWER	2
#define DEFAULT_WZRD_KNOWLEDGE	2
*/

namespace Skill
{
    typedef enum { BASIC, ADVANCED, EXPERT } level_t;

    typedef enum {
	PATHFINDING,
	ARCHERY,
	LOGISTICS,
	SCOUTING,
	DIPLOMACY,
	NAVIGATION,
	LEADERSHIP,
	WISDOM,
	MYSTICISM,
	LUCK,
	BALLISTICS,
	EAGLEEYE,
	NECROMANCY,
	ESTATES,
	NONE
    } types_t;
    
    typedef std::pair<types_t, level_t> skill_t;
};

namespace Heroes
{
    typedef enum {
	// knight
	LORDKILBURN, SIRGALLANTH, ECTOR, GVENNETH, TYRO, AMBROSE, RUBY, MAXIMUS, DIMITRY,
	// barbarian
	THUNDAX, FINEOUS, JOJOSH, CRAGHACK, JEZEBEL, JACLYN, ERGON, TSABU, ATLAS,
	// sorceress
	ASTRA, NATASHA, TROYAN, VATAWNA, REBECCA, GEM, ARIEL, CARLAWN, LUNA,
	// warlock
	ARIE, ALAMAR, VESPER, CRODO, BAROK, KASTORE, AGAR, FALAGAR, WRATHMONT,
	// wizard
	MYRA, FLINT, DAWN, HALON, MYRINI, WILFREY, SARAKIN, KALINDRA, MANDIGAL,
	// necromancer
	ZOM, DARLANA, ZAM, RANLOO, CHARITY, RIALDO, ROXANA, SANDRO, CELIA,
	// from campain
	ROLAND, UNKNOWN1, UNKNOWN2, ARCHIBALD, SANDYSANDY, BRAX,
    } heroes_t;

    typedef struct { 
	Race::race_t		race;
	const std::string	name;
	Color::color_t		color;
	u8			attack;
	u8			defence;
	u8			power;
	u8			knowledge;
	u32			experience;
	u16			magic_point;
	Morale::morale_t	morale;
	Luck::luck_t		luck;
	Skill::skill_t		skill[HEROESMAXSKILL];
	Artifact::artifact_t	artifact[HEROESMAXARTIFACT];
	Army::army_t		army[HEROESMAXARMY];
	bool			format_spread;
	float			move_point;
	u16			index_maps;
	//MagicBook		book
    } stats_t;
};

#endif
