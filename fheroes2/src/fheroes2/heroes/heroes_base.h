/*************************************************************************** 
 *   Copyright (C) 2009 by Josh Matthews  <josh@joshmatthews.net>          *
 *   Copyright (C) 2009 by Andrey Afletdinov                               * 
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

#ifndef H2HEROESBASE_H
#define H2HEROESBASE_H

#include "bitmodes.h"
#include "skill.h"
#include "spell_book.h"
#include "artifact.h"
#include "color.h"
#include "gamedefs.h"

typedef std::vector<Artifact> BagArtifacts;

namespace Army { class army_t; };

class HeroBase : public Skill::Primary, public BitModes
{
  public:
    virtual const std::string & GetName(void) const = 0;
    virtual const Army::army_t & GetArmy(void) const = 0;
    virtual Army::army_t & GetArmy(void) = 0;
    virtual bool HasArtifact(const Artifact::artifact_t) const = 0;
    virtual BagArtifacts & GetBagArtifacts(void) = 0;
    virtual void TakeArtifacts(Heroes &) = 0;
    virtual u16 GetMaxSpellPoints(void) const = 0;
    virtual void SetSpellPoints(const u16 point) = 0;
    virtual u16 GetSpellPoints(void) const = 0;
    virtual const SpellBook & GetSpellBook(void) const = 0;
    virtual SpellBook & GetSpellBook(void) = 0;
    virtual u8 GetLevelSkill(const Skill::Secondary::skill_t) const = 0;
    virtual Color::color_t GetColor(void) const = 0;
};

#endif
