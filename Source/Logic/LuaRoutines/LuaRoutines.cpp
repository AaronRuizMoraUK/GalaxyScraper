#include "LuaRoutines.h"
#include <Windows.h>
#include <string>
#include "Global/GlobalVariables.h"

namespace LuaRoutines {

	// LUA ROUTINES

	static int print_debug(lua_State *ls) {
		int num=lua_gettop(ls);

		if(num==1) {
			const char * msg = lua_tostring(ls, num);
			OutputDebugString(msg);
			MessageBox( NULL, msg, "LUA", MB_ICONERROR | MB_OK );
		}
		else {
			assert( !"Number of parameters incorrect in print_debug in LuaRoutines" );
		}

		return 0;
	}
	
	static int enable_collisions(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			Global::collisionEnabled = lua_toboolean(ls, 1) ? true : false;
		}
		else {
			assert( !"Number of parameters incorrect in enable_collisions in LuaRoutines" );
		}

		return 0;
	}

	static int enable_ia(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			Global::IAEnabled = lua_toboolean(ls, 1) ? true : false;
		}
		else {
			assert( !"Number of parameters incorrect in enable_ia in LuaRoutines" );
		}

		return 0;
	}

	static int enable_animation(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			Global::animationEnabled = lua_toboolean(ls, 1) ? true : false;
		}
		else {
			assert( !"Number of parameters incorrect in enable_animation in LuaRoutines" );
		}

		return 0;
	}
	
	static int enable_logic(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			Global::logicEnabled = lua_toboolean(ls, 1) ? true : false;
		}
		else {
			assert( !"Number of parameters incorrect in enable_logic in LuaRoutines" );
		}

		return 0;
	}


	// LUA REGISTER FUNCTION
	void registerLuaRoutines(lua_State *ls) 
	{
		lua_register(ls, "print_debug", &print_debug);
		lua_register(ls, "enable_collisions", &enable_collisions);
		lua_register(ls, "enable_ia", &enable_ia);
		lua_register(ls, "enable_animation", &enable_animation);
		lua_register(ls, "enable_logic", &enable_logic);
	}
}
