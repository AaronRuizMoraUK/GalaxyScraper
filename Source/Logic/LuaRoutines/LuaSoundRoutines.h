#ifndef LUA_SOUND_ROUTINES_H
#define LUA_SOUND_ROUTINES_H

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

/**
* Sound routines for lua.
*/

namespace LuaSoundRoutines {

	// LUA ROUTINES
	static int play_ambient(lua_State *ls);
	static int play_sample(lua_State *ls);
	static int stop_ambient(lua_State *ls);
	static int enable_samples(lua_State *ls);

	// LUA REGISTER FUNCTION
	void registerLuaSoundRoutines(lua_State *ls);

}

#endif //LUA_SOUND_ROUTINES_H
