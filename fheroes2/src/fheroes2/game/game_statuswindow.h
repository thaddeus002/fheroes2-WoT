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

#ifndef H2STATUSWINDOW_H
#define H2STATUSWINDOW_H

#include "thread.h"
#include "gamedefs.h"
#include "game.h"
#include "resource.h"

class Surface;
class Castle;
class Heroes;

namespace Game
{

class StatusWindow
{
public:
    typedef enum { UNKNOWN, DAY, FUNDS, ARMY, RESOURCE } info_t;

    static StatusWindow & Get(void);

    void SetPos(const Point &pt);
    const Rect & GetRect(void) const;
    
    void Redraw(void);
    void SetState(const info_t info);
    void NextState(void);

    void SetResource(const Resource::resource_t, u16);

    void RedrawAITurns(u8 color, u8 progress) const;

private:
    StatusWindow();
    void DrawKingdomInfo(const u8 oh = 0) const;
    void DrawDayInfo(const u8 oh = 0) const;
    void DrawArmyInfo(const u8 oh = 0) const;
    void DrawResourceInfo(const u8 oh = 0) const;
    void DrawBackground(void) const;
    static u32 ResetResourceStatus(u32, void *);

    Rect pos;
    u8 count;

    info_t state;

    Resource::resource_t lastResource;
    u16 countLastResource;
    Timer timerShowLastResource;
    info_t old_state;
};

};

#endif
