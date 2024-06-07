#ifndef LUA_BOILER_ROUTINES_H
#define LUA_BOILER_ROUTINES_H

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

/**
* Boiler planet routines for lua.
*/

namespace LuaBoilerRoutines {

	// LUA ROUTINES
	static int start_platform_sequence(lua_State *ls);
	static int init_zone(lua_State *ls);
	static int zone_entered(lua_State *ls);
	static int mini_boiler_killed_at_zone(lua_State *ls);
	static int open_next_zone_to(lua_State *ls);
	static int check_zone_platform_firing(lua_State *ls);
	static int check_platform_firing(lua_State *ls);
	static int check_lever_activated(lua_State *ls);
	static int check_canon_activated(lua_State *ls);
	static int change_canon_animation_controller(lua_State *ls);
	static int shoot_ball(lua_State *ls);
	static int player_checkpoint_achieved(lua_State *ls);
	static int update_player_fired_retries_count(lua_State *ls);
	static int reset_fired_death_count(lua_State *ls);
	static int reset_checkpoint_condition(lua_State *ls);
	static int boiler_door_next_animation(lua_State *ls);
	static int enable_player_flicker(lua_State *ls);
	static int set_fallen_condition(lua_State *ls);
	static int reset_boiler_camera_position(lua_State *ls);
	static int set_camera_distance(lua_State *ls);
	static int position_camera_to_look_boiler_door(lua_State *ls);	
	static int position_camera_to_look_boiler_up(lua_State *ls);	
	static int add_burned_particles_to_player(lua_State *ls);
	static int reset_logic(lua_State *ls);
	static int reset_checkpoints(lua_State *ls);
	static int place_player_initial_checkpoint(lua_State *ls);
	static int init_boss_enemy_flame_zone(lua_State *ls);
	static int enable_atmospheres_in_boiler(lua_State *ls);
	static int disable_atmospheres_in_boiler(lua_State *ls);
	static int init_boss_enemy_sequence(lua_State *ls);
	static int kill_boss_zone(lua_State *ls);
	static int active_godray(lua_State *ls);
	static int show_all_boss_zone(lua_State *ls);

	// LUA REGISTER FUNCTION
	void registerLuaBoilerRoutines(lua_State *ls);

}

#endif //LUA_BOILER_ROUTINES_H
