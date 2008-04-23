/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov                                  *
 *   drmoriarty@rambler.ru                                                 *
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
#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <lua.hpp>
#include <map>

namespace Lua
{
    enum VARTYPE { VOID=0, STRING, INTEGER, NUMBER };

    void DoString(const char * str);
    void DoFile(const char * file);

    int  GetGlobalInt(const char* name, const char* field=0);
    std::string GetGlobalStr(const char* name, const char* field=0);
    double GetGlobalNumber(const char* name, const char* field=0);
    bool GetGlobalBool(const char* name, const char* field=0);

    void SetGlobalInt(int v, const char* name, const char* field=0);
    void SetGlobalStr(std::string& v, const char* name, const char* field=0);
    void SetGlobalNumber(double v, const char* name, const char* field=0);
    void SetGlobalBool(bool v, const char* name, const char* field=0);
    void SetGlobalTable(const char* name);

    //                    function name, address, type of return value, count of arguments, arguments types (Lua::VARTYPE)
    void RegisterFunc(const char * name, void* func, Lua::VARTYPE retval, int argc, ...);

    void RegisterCommonFunc();

};

#endif
