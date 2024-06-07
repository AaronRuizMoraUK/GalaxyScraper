#ifndef LUA_IURANUS_ROUTINES_H
#define LUA_IURANUS_ROUTINES_H

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
}

/**
* Iuranus planet routines for lua.
*/

namespace LuaIuranusRoutines {

	// LUA ROUTINES
	static int create_enemy(lua_State *ls);
	static int create_initial_enemies(lua_State *ls);
	static int increment_eaten_enemy(lua_State *ls);
	static int decrement_enemies_in_screen(lua_State *ls);
	static int check_fart_to_do(lua_State *ls);
	static int player_win(lua_State *ls);
	static int player_lose(lua_State *ls);
	static int player_lose_timeout(lua_State *ls);
	static int take_player_control_out(lua_State *ls);
	static int take_player_control_out_but_anim(lua_State *ls);
	static int check_iuranus_victory_achieved(lua_State *ls);
	static int player_inside_ass(lua_State *ls);
	static int reset_counter_to_fail_point(lua_State *ls);
	static int expel_enemies_around_mouth(lua_State *ls);
	static int expel_enemies_around_ass(lua_State *ls);
	static int expel_player_from_ass(lua_State *ls);
	static int deep_player_into_ass(lua_State *ls);
	static int anim_iuranus_enemies_victory(lua_State *ls);
	static int reset_iuranus_enemies_states(lua_State *ls);
	static int anim_mouth(lua_State *ls);
	static int anim_ass(lua_State *ls);
	static int move_object(lua_State *ls);
	static int kill_enemies_in_mouth(lua_State *ls);
	static int add_particle(lua_State *ls);
	static int add_planet_particle(lua_State *ls);
	static int reset_iuranus_logic_counters(lua_State *ls);
	static int reset_iuranus_enemies_in_screen(lua_State *ls);
	static int play_player_expel_sample_rand(lua_State *ls);
	
	// LUA REGISTER FUNCTION
	void registerLuaIuranusRoutines(lua_State *ls);

}

#endif //LUA_IURANUS_ROUTINES_H
