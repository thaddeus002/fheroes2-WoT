/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include "army.h"
#include "color.h"
#include "cursor.h"
#include "artifact.h"
#include "settings.h"
#include "heroes_base.h"
#include "battle_arena.h"
#include "skill.h"
#include "agg.h"
#include "world.h"
#include "kingdom.h"
#include "server.h"
#include "localclient.h"
#include "battle2.h"

void PlayPickupSound(void);

namespace Battle2
{
    void PickupArtifactsAction(HeroBase &, HeroBase &, bool);
    void EagleEyeSkillAction(HeroBase &, const SpellStorage &, bool);
    void NecromancySkillAction(Army::army_t &, u32, bool);
}

Battle2::Result Battle2::Loader(Army::army_t & army1, Army::army_t & army2, s32 mapsindex)
{
    Settings & conf = Settings::Get();
    Color::color_t mycolor = conf.MyColor();

    army1.BattleInit();
    army2.BattleInit();

    if(IS_DEBUG(DBG_BATTLE , DBG_INFO))
    {
	DEBUG(DBG_BATTLE, DBG_INFO, "start");
	army1.Dump();
	army2.Dump();
    }

    if(conf.ExtLowMemory())
        AGG::ICNRegistryEnable(true);

    AGG::ResetMixer();
    bool local = Game::CONTROL_LOCAL == army1.GetControl() || Game::CONTROL_LOCAL == army2.GetControl();
#ifdef WITH_NET
    if(Network::isLocalClient()) local = true;
#endif

    Arena arena(army1, army2, mapsindex, local);

    u16 turn = 1;

    Result result;

#ifdef WITH_NET
    if(Network::isLocalClient())
	arena.NetworkTurn(result);
    else
#endif
    while(1)
    {
	arena.Turns(turn, result);

	if(result.army1 || result.army2) break;

	++turn;
    }

#ifdef WITH_NET
    if(Network::isRemoteClient())
    {
        if(Game::CONTROL_REMOTE == army1.GetControl()) FH2Server::Get().BattleSendResult(army1.GetColor(), result);
        if(Game::CONTROL_REMOTE == army2.GetControl()) FH2Server::Get().BattleSendResult(army2.GetColor(), result);
    }
#endif

    AGG::ResetMixer();
    conf.SetMyColor(mycolor);

    Army::army_t *army_wins = (result.army1 & RESULT_WINS ? &army1 : (result.army2 & RESULT_WINS ? &army2 : NULL));
    Army::army_t *army_loss = (result.army1 & RESULT_LOSS ? &army1 : (result.army2 & RESULT_LOSS ? &army2 : NULL));
    const u8 loss_result =  result.army1 & RESULT_LOSS ? result.army1 : result.army2;

    // fix experience
    if(army_wins && army_loss)
    {
	Army::army_t killed1, killed2;
	army1.BattleExportKilled(killed1);
	army2.BattleExportKilled(killed2);
	result.exp1 = killed2.CalculateExperience();
	result.exp2 = killed1.CalculateExperience();
    }

    // fade arena
    if(local) arena.FadeArena();

    // dialog summary
    if(local) arena.DialogBattleSummary(result);

    const u32 killed2 = army_loss ? army_loss->BattleKilled() : 0;

    army1.BattleQuit();
    army2.BattleQuit();

    // pickup artifact
    if(army_wins && army_wins->GetCommander() &&
	army_loss && army_loss->GetCommander() &&
	!((RESULT_RETREAT | RESULT_SURRENDER) & loss_result) &&
	Skill::Primary::HEROES == army_wins->GetCommander()->GetType() &&
	Skill::Primary::HEROES == army_loss->GetCommander()->GetType())
	PickupArtifactsAction(*army_wins->GetCommander(), *army_loss->GetCommander(), local && (Game::CONTROL_LOCAL == army_wins->GetControl()));

    // eagle eye capability
    if(army_wins && army_wins->GetCommander() &&
	army_loss && army_loss->GetCommander() &&
	army_wins->GetCommander()->GetLevelSkill(Skill::Secondary::EAGLEEYE) &&
	Skill::Primary::HEROES == army_loss->GetCommander()->GetType())
	    EagleEyeSkillAction(*army_wins->GetCommander(), arena.GetUsageSpells(), local && (Game::CONTROL_LOCAL == army_wins->GetControl()));

    // necromancy capability
    if(army_wins && army_wins->GetCommander() &&
	army_wins->GetCommander()->GetLevelSkill(Skill::Secondary::NECROMANCY))
	    NecromancySkillAction(*army_wins, killed2, local && (Game::CONTROL_LOCAL == army_wins->GetControl()));

    if(IS_DEBUG(DBG_BATTLE , DBG_INFO))
    {
	DEBUG(DBG_BATTLE, DBG_INFO, "end");
	army1.Dump();
	army2.Dump();
    }

    // update army
    if(army1.GetCommander() && Skill::Primary::HEROES == army1.GetCommander()->GetType())
    {
	// hard reset army
	if(!army1.isValid() || (result.army1 & RESULT_RETREAT)) army1.Reset(false);
    }

    // update army
    if(army2.GetCommander() && Skill::Primary::HEROES == army2.GetCommander()->GetType())
    {
	// hard reset army
        if(!army2.isValid() || (result.army2 & RESULT_RETREAT)) army2.Reset(false);
    }

    if(conf.ExtLowMemory())
    {
        AGG::ICNRegistryEnable(false);
        AGG::ICNRegistryFreeObjects();
    }

    DEBUG(DBG_BATTLE, DBG_INFO, "army1: " << (result.army1 & RESULT_WINS ? "wins" : "loss") << ", army2: " << (result.army2 & RESULT_WINS ? "wins" : "loss"));

    return result;
}

void Battle2::PickupArtifactsAction(HeroBase & hero1, HeroBase & hero2, bool local)
{
    BagArtifacts & bag1 = hero1.GetBagArtifacts();
    BagArtifacts & bag2 = hero2.GetBagArtifacts();

    for(u8 ii = 0; ii < bag2.size(); ++ii)
    {
	Artifact & art = bag2[ii];

	if(art.isUltimate())
	{
	    art = Artifact::UNKNOWN;
	}
	else
	if(art() != Artifact::UNKNOWN && art() != Artifact::MAGIC_BOOK)
        {
            BagArtifacts::iterator it = std::find(bag1.begin(), bag1.end(), Artifact((Artifact::UNKNOWN)));
            if(bag1.end() != it)
            {
        	*it = art;
        	if(local)
		{
		    PlayPickupSound();
		    Dialog::ArtifactInfo(_("You have captured an enemy artifact!"), art.GetName(), art);
		}
    	    }
    	    art = Artifact::UNKNOWN;
	}
    }
}

void Battle2::EagleEyeSkillAction(HeroBase & hero, const SpellStorage & spells, bool local)
{
    if(spells.empty() ||
	!hero.HaveSpellBook()) return;

    SpellStorage new_spells;
    new_spells.reserve(10);

    const Skill::Secondary eagleeye(Skill::Secondary::EAGLEEYE, hero.GetLevelSkill(Skill::Secondary::EAGLEEYE));

    // filter spells
    for(SpellStorage::const_iterator
	it = spells.begin(); it != spells.end(); ++it)
    {
	const Spell & sp = *it;
    	if(!hero.HaveSpell(sp))
	{
	    switch(eagleeye.Level())
	    {
		case Skill::Level::BASIC:
		    // 20%
		    if(3 > sp.Level() && eagleeye.GetValues() >= Rand::Get(1, 100)) new_spells.push_back(sp);
		    break;
		case Skill::Level::ADVANCED:
		    // 30%
		    if(4 > sp.Level() && eagleeye.GetValues() >= Rand::Get(1, 100)) new_spells.push_back(sp);
		    break;
		case Skill::Level::EXPERT:
		    // 40%
		    if(5 > sp.Level() && eagleeye.GetValues() >= Rand::Get(1, 100)) new_spells.push_back(sp);
		    break;
		default: break;
    	    }
	}
    }

    // add new spell
    for(SpellStorage::const_iterator
	it = new_spells.begin(); it != new_spells.end(); ++it)
    {
	const Spell & sp = *it;
	if(local)
	{
	    std::string msg = _("Through eagle-eyed observation, %{name} is able to learn the magic spell %{spell}.");
	    String::Replace(msg, "%{name}", hero.GetName());
	    String::Replace(msg, "%{spell}", sp.GetName());
	    PlayPickupSound();
	    Dialog::SpellInfo("", msg, sp);
	}
    }

    hero.AppendSpellsToBook(new_spells, true);
}

void Battle2::NecromancySkillAction(Army::army_t & army1, u32 killed, bool local)
{
    if(0 == killed ||
	(army1.GetCount() == army1.Size() && !army1.HasMonster(Monster::SKELETON))) return;

    const HeroBase* hero = army1.GetCommander();

    // check necromancy shrine build
    u16 percent = 10 * world.GetKingdom(army1.GetColor()).GetCountNecromancyShrineBuild();

    // check artifact
    if(hero->HasArtifact(Artifact::SPADE_NECROMANCY)) percent += 10;

    // fix over 60%
    if(percent > 60) percent = 60;

    percent += hero->GetSecondaryValues(Skill::Secondary::NECROMANCY);
    
    // hard fix overflow
    if(percent > 90) percent = 90;

    const Monster mons(Monster::SKELETON);
    const u32 count = Monster::GetCountFromHitPoints(Monster::SKELETON, mons.GetHitPoints() * killed * percent / 100);
    army1.JoinTroop(mons, count);

    if(local)
    {
	std::string msg = _("Practicing the dark arts of necromancy, you are able to raise %{count} of the enemy's dead to return under your service as %{monster}");
	String::Replace(msg, "%{count}", count);
	String::Replace(msg, "%{monster}", mons.GetMultiName());
	Surface sf1(40, 45);
	const Sprite & sf2 = AGG::GetICN(ICN::MONS32, mons.GetSpriteIndex());
	sf1.SetColorKey();
	sf1.Blit(sf2, (sf1.w() - sf2.w()) / 2, 0);
	std::string str;
	String::AddInt(str, count);
	Text text(str, Font::SMALL);
	text.Blit((sf1.w() - text.w()) / 2, sf2.h() + 3, sf1);
	PlayPickupSound();
	Dialog::SpriteInfo("", msg, sf1);
    }

    DEBUG(DBG_BATTLE, DBG_TRACE, "raise: " << count << mons.GetMultiName());
}

u8 Battle2::Result::AttackerResult(void) const
{
    if(RESULT_SURRENDER & army1) return RESULT_SURRENDER;
    else
    if(RESULT_RETREAT & army1) return RESULT_RETREAT;
    else
    if(RESULT_LOSS & army1) return RESULT_LOSS;
    else
    if(RESULT_WINS & army1) return RESULT_WINS;

    return 0;
}

u8 Battle2::Result::DefenderResult(void) const
{
    if(RESULT_SURRENDER & army2) return RESULT_SURRENDER;
    else
    if(RESULT_RETREAT & army2) return RESULT_RETREAT;
    else
    if(RESULT_LOSS & army2) return RESULT_LOSS;
    else
    if(RESULT_WINS & army2) return RESULT_WINS;

    return 0;
}

u32 Battle2::Result::GetExperienceAttacker(void) const
{
    return exp1;
}

u32 Battle2::Result::GetExperienceDefender(void) const
{
    return exp2;
}

bool Battle2::Result::AttackerWins(void) const
{
    return army1 & RESULT_WINS;
}

bool Battle2::Result::DefenderWins(void) const
{
    return army2 & RESULT_WINS;
}
