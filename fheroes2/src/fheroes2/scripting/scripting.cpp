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

#include <string>
#include "scripting.h"
#include "dialog.h"

#ifdef WITH_LUA

#define MAXARG 4

typedef struct {
  Lua::VARTYPE output, input[MAXARG];
  void * func;
} Entry;

int Callback(lua_State* st);

std::map<long, Entry> entries;
lua_State * state = 0;
long func_counter = 0;

// service class
class _lua {
  public:
    _lua();
    ~_lua();
};

_lua::_lua()
{
  state = luaL_newstate();
  luaL_openlibs(state);
  Lua::RegisterCommonFunc();
}

_lua::~_lua()
{
  lua_close(state);
}

_lua tmp;
// end service class

// debug functions
void alert()
{
  Dialog::Message("Alert", "This function is called from Lua script.", Font::BIG, Dialog::OK);
}
void alert1(std::string& str)
{
  Dialog::Message("Alert", str, Font::BIG, Dialog::OK);
}
void message(std::string& str1, std::string& str2)
{
  Dialog::Message(str1, str2, Font::BIG, Dialog::OK);
}

void Lua::RegisterFunc(const char * name, void* func, Lua::VARTYPE retval, int argc, ...)
{
  va_list ap;
  va_start(ap, argc);
  Entry entry;
  memset(&entry, 0, sizeof(Entry));
  entry.output = retval;
  entry.func = (void*)func;
  int count = argc < MAXARG ? argc : MAXARG;
  for(int i=0; i<count; i++) entry.input[i] = (Lua::VARTYPE)va_arg(ap, int);
  entries[func_counter] = entry;
  va_end(ap);
  lua_pushinteger(state, func_counter++);
  lua_pushcclosure(state, &Callback, 1);
  lua_setglobal(state, name);
}

void Lua::DoString(const char * str)
{
  luaL_dostring(state, str);
}

void Lua::DoFile(const char * file)
{
  luaL_dofile(state, file);
}

int Lua::GetGlobalInt(const char* name, const char* field)
{
  int r=0;
  lua_getglobal(state, name);
  if(field && lua_istable(state, -1)) {  // get field
    lua_pushstring(state, field);
    lua_gettable(state, -2);
    r = lua_tointeger(state, -1);
    lua_pop(state, 1);
  } else {                           // get value
    r = lua_tointeger(state, -1);
    lua_pop(state, 1);
  }
  return r;
}

std::string Lua::GetGlobalStr(const char* name, const char* field)
{
  size_t size;
  std::string r;
  lua_getglobal(state, name);
  if(field && lua_istable(state, -1)) {  // get field
    lua_pushstring(state, field);
    lua_gettable(state, -2);
    if(lua_isstring(state, -1)) r = lua_tolstring(state, -1, &size);
    lua_pop(state, 1);
  } else {                           // get value
    if(lua_isstring(state, -1)) r = lua_tolstring(state, -1, &size);
    lua_pop(state, 1);
  }
  return r;
}

double Lua::GetGlobalNumber(const char* name, const char* field)
{
  double r=0;
  lua_getglobal(state, name);
  if(field && lua_istable(state, -1)) {  // get field
    lua_pushstring(state, field);
    lua_gettable(state, -2);
    if(lua_isnumber(state, -1)) r = lua_tonumber(state, -1);
    lua_pop(state, 1);
  } else {                           // get value
    if(lua_isnumber(state, -1)) r = lua_tonumber(state, -1);
    lua_pop(state, 1);
  }
  return r;
}

bool Lua::GetGlobalBool(const char* name, const char* field)
{
  bool r=0;
  lua_getglobal(state, name);
  if(field && lua_istable(state, -1)) {  // get field
    lua_pushstring(state, field);
    lua_gettable(state, -2);
    r = lua_toboolean(state, -1);
    lua_pop(state, 1);
  } else {                           // get value
    r = lua_toboolean(state, -1);
    lua_pop(state, 1);
  }
  return r;
}

void Lua::SetGlobalInt(int v, const char* name, const char* field)
{
  if(field) {
    lua_getglobal(state, name);
    if(lua_istable(state, -1)) {
      lua_pushstring(state, field);
      lua_pushinteger(state, v);
      lua_settable(state, -3);
    }
  } else {
    lua_pushinteger(state, v);
    lua_setglobal(state, name);
  }
}

void Lua::SetGlobalStr(std::string& v, const char* name, const char* field)
{
  if(field) {
    lua_getglobal(state, name);
    if(lua_istable(state, -1)) {
      lua_pushstring(state, field);
      lua_pushstring(state, v.c_str());
      lua_settable(state, -3);
    }
  } else {
    lua_pushstring(state, v.c_str());
    lua_setglobal(state, name);
  }
}

void Lua::SetGlobalNumber(double v, const char* name, const char* field)
{
  if(field) {
    lua_getglobal(state, name);
    if(lua_istable(state, -1)) {
      lua_pushstring(state, field);
      lua_pushnumber(state, v);
      lua_settable(state, -3);
    }
  } else {
    lua_pushnumber(state, v);
    lua_setglobal(state, name);
  }
}

void Lua::SetGlobalBool(bool v, const char* name, const char* field)
{
  if(field) {
    lua_getglobal(state, name);
    if(lua_istable(state, -1)) {
      lua_pushstring(state, field);
      lua_pushboolean(state, v);
      lua_settable(state, -3);
    }
  } else {
    lua_pushboolean(state, v);
    lua_setglobal(state, name);
  }
}

void Lua::SetGlobalTable(const char* name)
{
  lua_newtable(state);
  lua_setglobal(state, name);
}

// register usefull functions
void Lua::RegisterCommonFunc()
{
  RegisterFunc("alert", (void*)alert, Lua::VOID, 0);
  RegisterFunc("alert1", (void*)alert1, Lua::VOID, 1, Lua::STRING);
  RegisterFunc("msg", (void*)message, Lua::VOID, 2, Lua::STRING, Lua::STRING);
  RegisterFunc("message", (void*)Dialog::Message, Lua::INTEGER, 4, Lua::STRING, Lua::STRING, Lua::INTEGER, Lua::INTEGER);
}

// callback function

int Callback(lua_State* st)
{
  //Dialog::Message("Callback", "Callback was occurred.", Font::BIG, Dialog::OK);
  long id = lua_tointeger(st, lua_upvalueindex(1));
  std::map<long, Entry>::const_iterator it = entries.find(id);
  if(it != entries.end()) {
    // V V
    if(it->second.output == Lua::VOID && it->second.input[0] == Lua::VOID) {
      ((void(*)(void))it->second.func)();
    } else 
    // V S
    if(it->second.output == Lua::VOID && it->second.input[0] == Lua::STRING && it->second.input[1] == Lua::VOID) {
      if(lua_isstring(state, -1)) {
        size_t size;
        ((void(*)(std::string))it->second.func)(lua_tolstring(state, -1, &size));
      }
    } else 
    // V SS
    if(it->second.output == Lua::VOID && it->second.input[0] == Lua::STRING && it->second.input[1] == Lua::STRING 
    && it->second.input[2] == Lua::VOID) {
      if(lua_isstring(state, -1) && lua_isstring(state, -2)) {
        size_t size;
        ((void(*)
        (std::string, std::string))
        it->second.func)
        (lua_tolstring(state, -2, &size), lua_tolstring(state, -1, &size));
      }
    } else 
    // I SSII
    if(it->second.output == Lua::INTEGER && it->second.input[0] == Lua::STRING && it->second.input[1] == Lua::STRING 
    && it->second.input[2] == Lua::INTEGER && it->second.input[3] == Lua::INTEGER) {
      if(lua_isstring(state, -3) && lua_isstring(state, -4)) {
        size_t size;
        lua_pushinteger(state, 
        ((int(*)(std::string, std::string, int, u16))it->second.func)(lua_tolstring(state, -4, &size), lua_tolstring(state, -3, &size), lua_tointeger(state, -2), lua_tointeger(state, -1))
        );
        return 1;
      }
    } else {
      // TODO other types
    }
  }
  return 0;
}

#endif

