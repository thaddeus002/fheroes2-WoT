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

#include <fstream>
#include "l10n.h"
#include "tools.h"
#include "error.h"
#include "types.h"

namespace l10n
{
    Strings &Get()
    {
        static Strings s_strings;
        return s_strings;
    }
    
    LocalizedString::LocalizedString() {}
    LocalizedString::LocalizedString(const std::string &other) : string(other) {}
    LocalizedString::LocalizedString(const LocalizedString &other)
    : string(other.string), subs(other.subs) {}
    
    LocalizedString &LocalizedString::sub(const std::string &replace, int idx /* = -1 */)
    {
        if(idx > 0)
            subs[idx - 1] = replace;
        else subs.push_back(replace);
        return *this;
    }
    
    LocalizedString &LocalizedString::sub(int replace, int idx /* = -1 */)
    {
        std::string strReplace;
        String::AddInt(strReplace, replace);
        return sub(strReplace, idx);
    }
    
    LocalizedString::operator std::string()
    {
        std::string subbed(string);
        for(u16 i = 0; i < subs.size(); i++)
        {
            std::string find = "%";
            String::AddInt(find, i + 1);
            size_t index = subbed.find(find);
            if(index != std::string::npos)
                subbed.replace(index, find.length(), subs[i]);
        }
        return subbed;
    }
    
    size_t LocalizedString::size()
    {
        return std::string(*this).size();
    }
    
    bool Strings::InitWithStringsFile(const std::string &filename)
    {
        if(filename.empty()) return false;
        std::fstream file(filename.c_str(), std::ios::in);
        if(! file || file.fail()) return false;

        std::string str;
        std::string prefix;
        while(std::getline(file, str))
        {
            String::Trim(str);
            
            if(String::Comment(str) || 0 == str.size()) continue;
            else if(str[0] == '[' && str[str.size() - 1] == ']')
            {
                prefix = str.substr(1, str.size() - 2);
                String::Trim(prefix);
                continue;
            }

            const size_t pos = str.find('=');
            if(std::string::npos != pos)
            {
                std::string left(str.substr(0, pos - 1));
                std::string right(str.substr(pos + 1, str.length() - pos - 1));

                String::Trim(left);
                String::Trim(right);

                std::string key;
                if(prefix != "")
                {
                    key += prefix;
                    key += ".";
                }
                key += left;
                
                localizedStrings[key] = right;
            }
        }

        file.close();
        return true;
    }
    
    LocalizedString Strings::FromKey(const std::string &key) throw(Error::Exception)
    {
        if(localizedStrings.find(key) != localizedStrings.end())
            return localizedStrings[key];
        else
        {
            std::string error("Unlocalized string requested (");
            error += key;
            error += ")";
            Error::Warning(error);
            return std::string("");
        }
    }
}
