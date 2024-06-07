#ifndef LUA_STATE_ROUTINES_H
#define LUA_STATE_ROUTINES_H

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

/**
* State routines for lua.
*/

namespace LuaStateRoutines {

	// LUA ROUTINES
	static int load_menu_camera(lua_State *ls);
	static int reset_continues(lua_State *ls);
	
	static int place_player_initial_position(lua_State *ls);
	static int place_camera_initial_position(lua_State *ls);

	static int enable_camera_vibration(lua_State *ls);
	static int disable_camera_vibration(lua_State *ls);

	static int reset_iuranus_logic(lua_State *ls);
	static int reset_iuranus_collision_manager(lua_State *ls);
	static int reset_iuranus_enemy_manager(lua_State *ls);

	static int reset_boiler_logic(lua_State *ls);
	static int reset_boiler_collision_manager(lua_State *ls);
	static int reset_boiler_enemy_manager(lua_State *ls);
	static int set_camera_params(lua_State *ls);

	static int save_camera(lua_State *ls);
	static int restore_camera(lua_State *ls);
	static int load_camera_dummy(lua_State *ls);

	// LUA REGISTER FUNCTION
	void registerLuaStateRoutines(lua_State *ls);

}

#endif //LUA_STATE_ROUTINES_H
