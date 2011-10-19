/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "artifact.h"
#include "skill.h"
#include "spell.h"
#include "pairs.h"
#include "settings.h"
#include "world.h"
#include "maps_tiles.h"

bool Maps::Tiles::QuantityIsValid(void) const
{
    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_ABANDONEDMINE:
	    return true;

        case MP2::OBJ_PYRAMID:
	    return QuantitySpell().isValid();

        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
        case MP2::OBJ_DERELICTSHIP:
        case MP2::OBJ_WATERWHEEL:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_MAGICGARDEN:
	    return quantity2;

        case MP2::OBJ_SKELETON:
	    return QuantityArtifact() != Artifact::UNKNOWN;

        case MP2::OBJ_WAGON:
	    return QuantityArtifact() != Artifact::UNKNOWN || quantity2;

        case MP2::OBJ_DAEMONCAVE:
	    return QuantityVariant();

        default: break;
    }

    return false;
}

u8 Maps::Tiles::QuantityVariant(void) const
{
    return quantity2 >> 4;
}

u8 Maps::Tiles::QuantityExt(void) const
{
    return 0x0f & quantity2;
}

void Maps::Tiles::QuantitySetVariant(u8 variant)
{
    quantity2 &= 0x0f;
    quantity2 |= variant << 4;
}

void Maps::Tiles::QuantitySetExt(u8 ext)
{
    quantity2 &= 0xf0;
    quantity2 |= (0x0f & ext);
}

Skill::Secondary Maps::Tiles::QuantitySkill(void) const
{
    Skill::Secondary skill;

    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
            switch(QuantityVariant())
            {
                case 4: skill.SetSkill(Skill::Secondary::LEADERSHIP); break;
                case 5: skill.SetSkill(Skill::Secondary::WISDOM); break;
                default: break;
            }
            break;

        case MP2::OBJ_WITCHSHUT: skill.SetSkill(quantity1); break;

        default: break;
    }

    return skill;
}

void Maps::Tiles::QuantitySetSkill(u8 skill)
{
    switch(GetObject(false))
    {
        case MP2::OBJ_WITCHSHUT:
            quantity1 = skill;
            break;

        default: break;
    }
}

Spell Maps::Tiles::QuantitySpell(void) const
{
    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
            return Spell(QuantityVariant() == 15 ? quantity1 : Spell::NONE);

        case MP2::OBJ_SHRINE1:
        case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:
        case MP2::OBJ_PYRAMID:
	    return Spell(quantity1);

        default: break;
    }

    return Spell(Spell::NONE);
}

void Maps::Tiles::QuantitySetSpell(u8 spell)
{
    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
        case MP2::OBJ_SHRINE1:
        case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:
        case MP2::OBJ_PYRAMID:
	    quantity1 = spell;
	    break;

        default: break;
    }
}

Artifact Maps::Tiles::QuantityArtifact(void) const
{
    switch(GetObject(false))
    {
        case MP2::OBJ_WAGON:    return Artifact(quantity2 ? Artifact::UNKNOWN : quantity1);

        case MP2::OBJ_SKELETON:
        case MP2::OBJ_DAEMONCAVE:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	    return Artifact(quantity1);

        case MP2::OBJ_ARTIFACT:
	    if(QuantityVariant() == 15)
	    {
        	Artifact art(Artifact::SPELL_SCROLL);
        	art.SetSpell(QuantitySpell().GetID());
		return art;
	    }
	    else
		return Artifact(quantity1);

        default: break;
    }

    return Artifact(Artifact::UNKNOWN);

}

void Maps::Tiles::QuantitySetArtifact(u8 art)
{
    quantity1 = art;
}

void Maps::Tiles::QuantitySetResource(u8 res, u16 count)
{
    quantity1 = res;
    quantity2 = res == Resource::GOLD ? count / 100 : count;
}

u16 Maps::Tiles::QuantityGold(void) const
{
    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
	{
            switch(QuantityVariant())
            {
                case 1: return 2000;
                case 2: return 2500;
                case 3: return 3000;
                default: break;
            }
            break;
	}
	break;

        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_WATERWHEEL:
	case MP2::OBJ_TREEKNOWLEDGE:
            return quantity1 == Resource::GOLD ? 100 * quantity2 : 0;

        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_CAMPFIRE:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_DERELICTSHIP:
        case MP2::OBJ_GRAVEYARD:
            return 100 * quantity2;

        case MP2::OBJ_DAEMONCAVE:
            switch(QuantityVariant())
            {
        	case 2:
		case 4: return 2500;
                default: break;
            }
            break;

        case MP2::OBJ_SHIPWRECK:
            switch(QuantityVariant())
            {
                case 1:  return 1000;
                case 2:  return 2000;
                case 3:  return 5000;
                default: return 2000;
            }
            break;

        default: break;
    }

    return 0;
}

ResourceCount Maps::Tiles::QuantityResourceCount(void) const
{
    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
            switch(QuantityVariant())
            {
                case 1: return ResourceCount(Resource::GOLD, QuantityGold());
                case 2: return ResourceCount(Resource::FromIndexSprite2(QuantityExt() - 1), 3);
                case 3: return ResourceCount(Resource::FromIndexSprite2(QuantityExt() - 1), 5);
                default: break;
            }
	    break;

        case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_TREASURECHEST:
    	    return ResourceCount(Resource::GOLD, QuantityGold());

        case MP2::OBJ_FLOTSAM:
            return ResourceCount(Resource::WOOD, quantity1);

        default: break;
    }

    return ResourceCount(quantity1, Resource::GOLD == quantity1 ? QuantityGold() : quantity2);
}

Funds Maps::Tiles::QuantityFunds(void) const
{
    const ResourceCount & rc = QuantityResourceCount();

    switch(GetObject(false))
    {
        case MP2::OBJ_ARTIFACT:
            switch(QuantityVariant())
            {
                case 1: return Funds(rc);
                case 2:
                case 3: return Funds(Resource::GOLD, QuantityGold()) + Funds(rc);
                default: break;
            }
    	    break;

        case MP2::OBJ_CAMPFIRE:
            return Funds(Resource::GOLD, QuantityGold()) + Funds(rc);

        case MP2::OBJ_FLOTSAM:
            return Funds(Resource::GOLD, QuantityGold()) + Funds(Resource::WOOD, quantity1);

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_DERELICTSHIP:
        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
        case MP2::OBJ_DAEMONCAVE:
            return Funds(Resource::GOLD, QuantityGold());

        default: break;
    }

    return Funds(rc);
}

void Maps::Tiles::QuantitySetColor(u8 col)
{
    quantity1 = col;
}

u8 Maps::Tiles::QuantityColor(void) const
{
    return quantity1;
}

u8 Maps::Tiles::QuantityTeleportType(void) const
{
    return quantity1;
}

void Maps::Tiles::QuantitySetTeleportType(u8 type)
{
    quantity1 = type;
}

Monster Maps::Tiles::QuantityMonster(void) const
{
    if(mp2_object == MP2::OBJ_MONSTER)
    {
	const Maps::TilesAddon *addon = FindObjectConst(MP2::OBJ_MONSTER);
	if(addon) return Monster(addon->index + 1);
    }
    else
    if(MP2::isCaptureObject(mp2_object))
	return world.GetCapturedObject(maps_index).GetTroop();

    return Monster(Monster::UNKNOWN);
}

Army::Troop Maps::Tiles::QuantityTroop(void) const
{
    if(mp2_object == MP2::OBJ_MONSTER)
	return Army::Troop(QuantityMonster(), MonsterCount());
    else
    if(MP2::isCaptureObject(GetObject(false)))
	return world.GetCapturedObject(maps_index).GetTroop();

    return Army::Troop();
}

void Maps::Tiles::QuantityReset(void)
{
    quantity1 = 0;
    quantity2 = 0;

    switch(GetObject(false))
    {
        case MP2::OBJ_SKELETON:
        case MP2::OBJ_WAGON:
        case MP2::OBJ_ARTIFACT:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
        case MP2::OBJ_DAEMONCAVE:
	    QuantitySetArtifact(Artifact::UNKNOWN);
	    break;

	default: break;
    }

    if(MP2::isPickupObject(mp2_object))
        mp2_object = MP2::OBJ_ZERO;
}

void Maps::Tiles::QuantityUpdate(void)
{
    switch(GetObject(false))
    {
        case MP2::OBJ_WITCHSHUT:
            QuantitySetSkill(Skill::Secondary::RandForWitchsHut());
            break;

        case MP2::OBJ_SHRINE1:
            QuantitySetSpell(Rand::Get(1) ? Spell::RandCombat(1)() : Spell::RandAdventure(1)());
            break;

        case MP2::OBJ_SHRINE2:
            QuantitySetSpell(Rand::Get(1) ? Spell::RandCombat(2)() : Spell::RandAdventure(2)());
            break;

        case MP2::OBJ_SHRINE3:
            QuantitySetSpell(Rand::Get(1) ? Spell::RandCombat(3)() : Spell::RandAdventure(3)());
            break;

        case MP2::OBJ_SKELETON:
        {
            Rand::Queue percents(2);
            // 80%: empty
            percents.Push(0, 80);
            // 20%: artifact 1 or 2 or 3
            percents.Push(1, 20);

            if(percents.Get())
            {
                switch(Rand::Get(1, 3))
                {
                    case 1: QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL1)); break;
                    case 2: QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL2)); break;
                    case 3: QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL3)); break;
                    default: break;
                }
            }
        }
        break;

	case MP2::OBJ_WAGON:
        {
            quantity2 = 0;

            Rand::Queue percents(3);
            // 20%: empty
            percents.Push(0, 20);
            // 10%: artifact 1 or 2
            percents.Push(1, 10);
            // 50%: resource
            percents.Push(2, 50);

            switch(percents.Get())
            {
                case 1: QuantitySetArtifact(Artifact::Rand(Rand::Get(1) ? Artifact::ART_LEVEL1 : Artifact::ART_LEVEL2)); break;
                case 2: QuantitySetResource(Resource::Rand(), Rand::Get(2, 5)); break;
                default: break;
            }
        }
        break;

        case MP2::OBJ_ARTIFACT:
	{
	    TilesAddon* addon = FindObject(MP2::OBJ_ARTIFACT);
            if(addon)
            {
                u8 art = Artifact::FromMP2IndexSprite(addon->index).GetID();

                if(Artifact::UNKNOWN != art)
                {
                    if(art == Artifact::SPELL_SCROLL)
                    {
            		QuantitySetVariant(15);
                        // spell from origin mp2
    			QuantitySetSpell(1 + (quantity2 * 256 + quantity1) / 8);
                    }
                    else
                    {
                        // 0: 70% none
                        // 1,2,3 - 2000g, 2500g+3res, 3000g+5res,
                        // 4,5 - need have skill wisard or leadership,
                        // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4,
                        // 15 - spell
                        u8 cond = Rand::Get(1, 10) < 4 ? Rand::Get(1, 13) : 0;

                        // always available
                        if(Settings::Get().ExtNoRequirementsForArtifacts())
                    	    cond = 0;

            		QuantitySetVariant(cond);
    			QuantitySetArtifact(art);

                        if(cond == 2 || cond == 3)
			    QuantitySetExt(Resource::GetIndexSprite2(Resource::Rand()) + 1);
                    }
                }
	    }
	}
        break;

        case MP2::OBJ_RESOURCE:
	{
	    TilesAddon* addon = FindObject(MP2::OBJ_RESOURCE);
            if(addon)
            {
                const u8 res = Resource::FromIndexSprite(addon->index);
                u16 count = 0;

                switch(res)
                {
                    case Resource::GOLD: count = 100 * Rand::Get(5, 10); break;
                    case Resource::WOOD:
                    case Resource::ORE:  count = Rand::Get(5, 10); break;
                    default:             count = Rand::Get(3, 6); break;
                }

                QuantitySetResource(res, count);
            }
	}
        break;

        case MP2::OBJ_CAMPFIRE:
        {
            // 4-6 rnd resource and + 400-600 gold
            QuantitySetResource(Resource::Rand(), Rand::Get(4, 6));
        }
        break;

        case MP2::OBJ_MAGICGARDEN:
            // 5 gems or 500 gold
            if(Rand::Get(1))
                QuantitySetResource(Resource::GEMS, 5);
            else
                QuantitySetResource(Resource::GOLD, 500);
        break;

        case MP2::OBJ_WATERWHEEL:
            // first week 500 gold, next week 1000 gold
            QuantitySetResource(Resource::GOLD, (0 == world.CountDay() ? 500 : 1000));
        break;

        case MP2::OBJ_WINDMILL:
        {
            u8 res = Resource::WOOD;
            // except: wood, bugs: #3117478
            while(res == Resource::WOOD) res = Resource::Rand();

            // 2 rnd resource
            QuantitySetResource(res, 2);
        }
        break;

        case MP2::OBJ_LEANTO:
            // 1-4 rnd resource
            QuantitySetResource(Resource::Rand(), Rand::Get(1, 4));
        break;

        case MP2::OBJ_FLOTSAM:
        {
            switch(Rand::Get(1, 4))
            {
                // 25%: empty
                default: break;
                // 25%: 500 gold + 10 wood
                case 1: QuantitySetResource(Resource::GOLD, 500); quantity1 = 10; break;
                // 25%: 200 gold + 5 wood
                case 2: QuantitySetResource(Resource::GOLD, 200); quantity1 = 5; break;
                // 25%: 5 wood
                case 3: quantity1 = 5; break;
            }
        }
        break;

        case MP2::OBJ_SHIPWRECKSURVIROR:
        {
            Rand::Queue percents(3);
            // 55%: artifact 1
            percents.Push(1, 55);
            // 30%: artifact 2
            percents.Push(1, 30);
            // 15%: artifact 3
            percents.Push(1, 15);

            // variant
            switch(percents.Get())
            {
                case 1:  QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL1)); break;
                case 2:  QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL2)); break;
                default: QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL3)); break;
            }
        }
        break;

	case MP2::OBJ_WATERCHEST:
        {
            Rand::Queue percents(3);
            // 20% - empty
            percents.Push(0, 20);
            // 70% - 1500 gold
            percents.Push(1, 70);
            // 10% - 1000 gold + art
            percents.Push(2, 10);

            u8 art = Artifact::UNKNOWN;
            u16 gold = 0;

            // variant
            switch(percents.Get())
            {
                default: break; // empty
                case 1:  gold = 1500; break;
                case 2:  gold = 1000; art = Artifact::Rand(Artifact::ART_LEVEL1); break;
            }

            QuantitySetResource(Resource::GOLD, gold);
            QuantitySetArtifact(art);
        }
        break;

	case MP2::OBJ_TREASURECHEST:
	if(isWater())
	{
    	    SetObject(MP2::OBJ_WATERCHEST);
	    QuantityUpdate();
	}
	else
        {
    	    UpdateTreasureChestSprite(*this);

            Rand::Queue percents(4);
            // 31% - 2000 gold or 1500 exp
            percents.Push(1, 31);
            // 32% - 1500 gold or 1000 exp
            percents.Push(2, 32);
            // 32% - 1000 gold or 500 exp
            percents.Push(3, 32);
            // 5% - art
            percents.Push(4, 5);

            u8 art = Artifact::UNKNOWN;
            u16 gold = 0;

            // variant
            switch(percents.Get())
            {
                case 1: gold = 2000; break;
                case 2: gold = 1500; break;
                case 3: gold = 1000; break;
                default: art = Artifact::Rand(Artifact::ART_LEVEL1); break;
            }

            QuantitySetResource(Resource::GOLD, gold);
            QuantitySetArtifact(art);
        }
        break;

        case MP2::OBJ_DERELICTSHIP:
            QuantitySetResource(Resource::GOLD, 5000);
        break;

        case MP2::OBJ_SHIPWRECK:
        {
            Rand::Queue percents(4);
            // 40% - 10ghost(1000g)
            percents.Push(1, 40);
            // 30% - 15 ghost(2000g)
            percents.Push(2, 30);
            // 20% - 25ghost(5000g)
            percents.Push(3, 20);
            // 10% - 50ghost(2000g+art)
            percents.Push(4, 10);

            u8 cond = percents.Get();

	    QuantitySetVariant(cond);
	    QuantitySetArtifact(cond == 4 ? Artifact::Rand(Artifact::ART_LEVEL123) : Artifact::UNKNOWN);
        }
        break;

        case MP2::OBJ_GRAVEYARD:
            // 1000 gold + art
            QuantitySetResource(Resource::GOLD, 1000);
            QuantitySetArtifact(Artifact::Rand(Artifact::ART_LEVEL123));
        break;

        case MP2::OBJ_PYRAMID:
        {
            // random spell level 5
            const Spell & spell = Rand::Get(1) ? Spell::RandCombat(5) : Spell::RandAdventure(5);
            QuantitySetSpell(spell());
        }
        break;

        case MP2::OBJ_DAEMONCAVE:
	{
            // 1000 exp or 1000 exp + 2500 gold or 1000 exp + art or (-2500 or remove hero)
            u8 cond = Rand::Get(1, 4);
            QuantitySetVariant(cond);
            QuantitySetArtifact(cond == 3 ? Artifact::Rand(Artifact::ART_LEVEL123) : Artifact::UNKNOWN);
	}
        break;

	case MP2::OBJ_TREEKNOWLEDGE:
            // variant: 10 gems, 2000 gold or free
            switch(Rand::Get(1, 3))
            {
                case 1: QuantitySetResource(Resource::GEMS, 10); break;
                case 2: QuantitySetResource(Resource::GOLD, 2000); break;
                default: break;
            }
	break;

        case MP2::OBJ_BARRIER:
        case MP2::OBJ_TRAVELLERTENT:
            QuantitySetColor(BarrierColor::FromMP2(quantity1));
        break;

        case MP2::OBJ_ALCHEMYLAB:
	    QuantitySetResource(Resource::MERCURY, 1);
	break;

        case MP2::OBJ_SAWMILL:
	    QuantitySetResource(Resource::WOOD, 2);
	break;

        case MP2::OBJ_MINES:
	{
	    TilesAddon* addon = FindObject(MP2::OBJ_MINES);
	    if(addon) switch(addon->index)
	    {
    		case 0: QuantitySetResource(Resource::ORE, 2); break;
    		case 1: QuantitySetResource(Resource::SULFUR, 1); break;
    		case 2: QuantitySetResource(Resource::CRYSTAL, 1); break;
    		case 3: QuantitySetResource(Resource::GEMS, 1); break;
    		case 4: QuantitySetResource(Resource::GOLD, 1000); break;
    		default: break;
	    }
	}
	break;

        case MP2::OBJ_ABANDONEDMINE:
	{
            Army::Troop & troop = world.GetCapturedObject(maps_index).GetTroop();

            // I checked in Heroes II: min 3 x 13, and max 3 x 15
	    troop.Set(Monster::GHOST, 3 * Rand::Get(13, 15));

            if(! Settings::Get().ExtAbandonedMineRandom())
		QuantitySetResource(Resource::GOLD, 1000);
            else
            switch(Rand::Get(1, 5))
            {
                case 1:  QuantitySetResource(Resource::ORE, 2); break;
                case 2:  QuantitySetResource(Resource::SULFUR, 1); break;
                case 3:  QuantitySetResource(Resource::CRYSTAL, 1); break;
                case 4:  QuantitySetResource(Resource::GEMS, 1); break;
                default: QuantitySetResource(Resource::GOLD, 1000); break;
            }
        }
	break;

        case MP2::OBJ_STONELIGHTS:
    	    UpdateStoneLightsSprite(*this);
        break;

        case MP2::OBJ_FOUNTAIN:
            UpdateFountainSprite(*this);
        break;

        case MP2::OBJ_EVENT:
        {
	    TilesAddon* addon = FindObject(MP2::OBJ_EVENT);
 	    // remove event sprite
            if(addon) Remove(addon->uniq);
	}
        break;

	case MP2::OBJ_BOAT:
        {
	    TilesAddon* addon = FindObject(MP2::OBJ_BOAT);
	    // remove small sprite boat
            if(addon) Remove(addon->uniq);
        }
        break;

        case MP2::OBJ_RNDARTIFACT:
        case MP2::OBJ_RNDARTIFACT1:
        case MP2::OBJ_RNDARTIFACT2:
        case MP2::OBJ_RNDARTIFACT3:
            // modify rnd artifact sprite
            UpdateRNDArtifactSprite(*this);
            QuantityUpdate();
        break;

        case MP2::OBJ_RNDRESOURCE:
            // modify rnd resource sprite
            UpdateRNDResourceSprite(*this);
            QuantityUpdate();
        break;

        case MP2::OBJ_MONSTER:
        case MP2::OBJ_RNDMONSTER:
        case MP2::OBJ_RNDMONSTER1:
        case MP2::OBJ_RNDMONSTER2:
        case MP2::OBJ_RNDMONSTER3:
        case MP2::OBJ_RNDMONSTER4:
            // modify rnd monster sprite
            UpdateMonsterInfo(*this);
        break;

	// join dwelling
        case MP2::OBJ_ANCIENTLAMP:
            MonsterSetCount(Monster(Monster::FromObject(MP2::OBJ_ANCIENTLAMP)).GetRNDSize(true));
        break;

        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
        // recruit dwelling
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_DESERTTENT:
        case MP2::OBJ_TROLLBRIDGE:
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:
            MonsterSetCount(0);
        break;

        default: break;
    }

    // reset hero
    if(mp2_object == MP2::OBJ_HEROES && ! world.GetHeroes(maps_index))
    {
	SetObject(MP2::OBJ_ZERO);
        DEBUG(DBG_GAME, DBG_WARN, "incorrect heroes info, reset tile: " << maps_index);
    }
}

u8 Maps::Tiles::MonsterJoinCondition(void) const
{
    const Maps::TilesAddon *addon = FindObjectConst(MP2::OBJ_MONSTER);
    return addon ? 0x03 & addon->tmp : 0;
}

void Maps::Tiles::MonsterSetJoinCondition(u8 cond)
{
    Maps::TilesAddon *addon = FindObject(MP2::OBJ_MONSTER);
    if(addon)
    {
	addon->tmp &= 0xFC;
	addon->tmp |= (cond & 0x03);
    }
}

void Maps::Tiles::MonsterSetFixedCount(void)
{
    Maps::TilesAddon *addon = FindObject(MP2::OBJ_MONSTER);
    if(addon) addon->tmp |= 0x80;
}

bool Maps::Tiles::MonsterFixedCount(void) const
{
    const Maps::TilesAddon *addon = FindObjectConst(MP2::OBJ_MONSTER);
    return addon ? addon->tmp & 0x80 : 0;
}

bool Maps::Tiles::MonsterJoinConditionSkip(void) const
{
    return Monster::JOIN_CONDITION_SKIP == MonsterJoinCondition();
}

bool Maps::Tiles::MonsterJoinConditionMoney(void) const
{
    return Monster::JOIN_CONDITION_MONEY == MonsterJoinCondition();
}

bool Maps::Tiles::MonsterJoinConditionFree(void) const
{
    return Monster::JOIN_CONDITION_FREE == MonsterJoinCondition();
}

bool Maps::Tiles::MonsterJoinConditionForce(void) const
{
    return Monster::JOIN_CONDITION_FORCE == MonsterJoinCondition();
}

u16 Maps::Tiles::MonsterCount(void) const
{
    return (static_cast<u16>(quantity1) << 8) | quantity2;
}

void Maps::Tiles::MonsterSetCount(u16 count)
{
    quantity1 = count >> 8;
    quantity2 = 0x00FF & count;
}

void Maps::Tiles::PlaceMonsterOnTile(Tiles & tile, const Monster & mons, u16 count, u32 uniq)
{
    tile.SetObject(MP2::OBJ_MONSTER);

    if(count)
    {
        tile.MonsterSetFixedCount();
	tile.MonsterSetCount(count);
    }
    else
	tile.MonsterSetCount(5 * mons.GetRNDSize(true));

    // skip join
    if(mons() == Monster::GHOST || mons.isElemental())
        tile.MonsterSetJoinCondition(Monster::JOIN_CONDITION_SKIP);
    else
    // fixed count: for money
    if(tile.MonsterFixedCount() ||
    // month of monster
       (world.GetWeekType().GetType() == Week::MONSTERS &&
        world.GetWeekType().GetMonster() == mons()))
        tile.MonsterSetJoinCondition(Monster::JOIN_CONDITION_MONEY);
    else
    {
        // 20% chance for join
        if(3 > Rand::Get(1, 10))
            tile.MonsterSetJoinCondition(Monster::JOIN_CONDITION_FREE);
        else
            tile.MonsterSetJoinCondition(Monster::JOIN_CONDITION_MONEY);
    }

    // set sprite
    if(uniq)
        tile.AddonsPushLevel1(TilesAddon(TilesAddon::UPPER, uniq, 0x33, mons.GetSpriteIndex()));
}

void Maps::Tiles::UpdateMonsterInfo(Tiles & tile)
{
    Maps::TilesAddon *addon = tile.FindObject(MP2::OBJ_RNDMONSTER);
    Monster mons;

    switch(tile.GetObject())
    {
        case MP2::OBJ_RNDMONSTER:	mons = Monster::Rand().GetID(); break;
        case MP2::OBJ_RNDMONSTER1:      mons = Monster::Rand(Monster::LEVEL1).GetID(); break;
        case MP2::OBJ_RNDMONSTER2:      mons = Monster::Rand(Monster::LEVEL2).GetID(); break;
        case MP2::OBJ_RNDMONSTER3:      mons = Monster::Rand(Monster::LEVEL3).GetID(); break;
        case MP2::OBJ_RNDMONSTER4:      mons = Monster::Rand(Monster::LEVEL4).GetID(); break;
        default: break;
    }

    // set sprite
    if(addon)
	addon->index = mons() - 1; // ICN::MONS32 start from PEASANT

    // reset random
    tile.SetObject(MP2::OBJ_MONSTER);

    u16 count = 0;

    // update count (mp2 format)
    if(tile.quantity1 || tile.quantity2)
    {
        count = tile.quantity2;
        count <<= 8;
        count |= tile.quantity1;
        count >>= 3;
    }

    PlaceMonsterOnTile(tile, mons(), count, 0);
}

