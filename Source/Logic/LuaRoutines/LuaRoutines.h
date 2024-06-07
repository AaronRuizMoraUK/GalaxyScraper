#ifndef LUA_ROUTINES_H
#define LUA_ROUTINES_H

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

/**
* Generic routines for lua.
*/

namespace LuaRoutines {

	// LUA ROUTINES
	static int print_debug(lua_State *ls);
	static int enable_collisions(lua_State *ls);
	static int enable_ia(lua_State *ls);
	static int enable_animation(lua_State *ls);
	static int enable_logic(lua_State *ls);
	
	// LUA REGISTER FUNCTION
	void registerLuaRoutines(lua_State *ls);

}

#endif //LUA_ROUTINES_H
