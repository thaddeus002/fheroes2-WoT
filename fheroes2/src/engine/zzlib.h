/***************************************************************************
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

#ifndef H2ZLIB_H
#define H2ZLIB_H

#ifdef WITH_ZLIB

#include <string>
#include <vector>

namespace ZLib
{
    bool	UnCompress(std::vector<char> &, const char*, size_t, bool debug = false);
    bool	UnCompress(std::vector<char> &, const std::vector<char> &, bool debug = false);
    bool	UnCompress(std::vector<char> &, const std::string &, bool debug = false);

    bool	Compress(std::vector<char> &, const char*, size_t);
    bool	Compress(std::vector<char> &, const std::vector<char> &);
    bool	Compress(std::vector<char> &, const std::string &);
};

#endif
#endif
