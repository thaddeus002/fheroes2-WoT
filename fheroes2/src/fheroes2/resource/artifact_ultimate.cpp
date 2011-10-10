/***************************************************************************
 *   Copyright (C) 201 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "maps.h"
#include "interface_gamearea.h"
#include "artifact_ultimate.h"

UltimateArtifact::UltimateArtifact() : index(-1), isfound(false)
{
}

void UltimateArtifact::Set(s32 pos, const Artifact & a)
{
    static_cast<Artifact>(*this) = a;
    index = pos;
    isfound = false;

    if(Maps::isValidAbsIndex(index))
	Interface::GameArea::GenerateUltimateArtifactAreaSurface(index, puzzlemap);
    else
	Surface::FreeSurface(puzzlemap);
}

const Surface & UltimateArtifact::GetPuzzleMapSurface(void) const
{
    return puzzlemap;
}

const Artifact & UltimateArtifact::GetArtifact(void) const
{
    return *this;
}

bool UltimateArtifact::isFound(void) const
{
    return isfound;
}

void UltimateArtifact::SetFound(bool f)
{
    isfound = f;
}

bool UltimateArtifact::isPosition(s32 pos) const
{
    return 0 <= index && pos == index;
}

void UltimateArtifact::Reset(void)
{
    Artifact::Reset();
    Surface::FreeSurface(puzzlemap);
    index = -1;
    isfound = false;
}
