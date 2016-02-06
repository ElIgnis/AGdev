#include "Application.h"
//#include <vld.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(void)
{
	lua_State *Lua_Init = lua_open();

	//Load the libs
	luaL_openlibs(Lua_Init);

	//run a lua script here
	luaL_dofile(Lua_Init, "Lua/AppConfig.Lua");

	if (luaL_loadfile(Lua_Init, "Lua/AppConfig.Lua") || lua_pcall(Lua_Init, 0, 0, 0))
	{
		printf("error: %s", lua_tostring(Lua_Init, -1));
		return -1;
	}

	lua_getglobal(Lua_Init, "INIT_ENGINE");
	if (!lua_isstring(Lua_Init, -1))
	{
		printf("Invalid file name specified\n");
		return -1;
	}

	string EngineInit = (string)lua_tostring(Lua_Init, 1);

	lua_close(Lua_Init);

	// get instance of the game engine and run
	Application &engine = Application::getInstance();
	engine.Init(EngineInit);
	engine.Run();
	engine.Exit();

	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();
}