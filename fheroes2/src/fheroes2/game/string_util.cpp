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

#include <cstring>
#include <climits>
#include "types.h"
#include "string_util.h"

char *dirname_internal(const char *path)
{
    static char buff[PATH_MAX];
    strncpy(buff, path, PATH_MAX);
    char *c = strrchr(buff, SEPARATOR);
    if(!c) strcpy(buff, ".");
    else *c = 0;
    return buff;
}

char *basename_internal(const char *path)
{
    static char buff[FILENAME_MAX];
    const char *c = strrchr(path, SEPARATOR);
    if(!c) strncpy(buff, path, FILENAME_MAX);
    else strcpy(buff, c + 1);
    return buff;
}
