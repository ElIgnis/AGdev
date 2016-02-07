#pragma once

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class LuaFileLoader
{
public:
	LuaFileLoader();
	~LuaFileLoader();

//	lua_State* GetLuaState(void);
//	void OpenFile(string fileName);
//private:
//	lua_State *Lua_;
};

