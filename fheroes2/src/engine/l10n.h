/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>           *
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

#ifndef H2L10N_H
#define H2L10N_H

#include <string>
#include <map>
#include <vector>
#include "error.h"

#define tr(s) l10n::Get().FromKey(s)
#define NOL10N(s) s

namespace l10n
{
    class LocalizedString
    {
        public:
            LocalizedString();
            LocalizedString(const std::string &);
            LocalizedString(const LocalizedString &);
            LocalizedString &sub(const std::string &replace, int idx=-1);
            LocalizedString &sub(int, int idx=-1);
            operator std::string();
            size_t size();
            
        private:
            LocalizedString &operator=(const LocalizedString &);
            LocalizedString &operator=(const std::string &);
            
            std::string string;
            std::vector<std::string> subs;
    };

    class Strings
    {
        public:
            bool InitWithStringsFile(const std::string &);
            LocalizedString FromKey(const std::string &) throw(Error::Exception);
    
        private:
            std::map<std::string, std::string> localizedStrings;
    };

    Strings &Get();
}

#endif
