#ifndef LUA_LOGIC_ROUTINES_H
#define LUA_LOGIC_ROUTINES_H

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

/**
* Logic routines for lua.
*/

namespace LuaLogicRoutines {

	// LUA ROUTINES
	static int create_timer(lua_State *ls); 
	static int delete_timer(lua_State *ls); 

	// LUA REGISTER FUNCTION
	void registerLuaLogicRoutines(lua_State *ls);

}

#endif //LUA_LOGIC_ROUTINES_H
