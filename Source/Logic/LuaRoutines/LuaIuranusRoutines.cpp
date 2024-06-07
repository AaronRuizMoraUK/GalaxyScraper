#include "LuaIuranusRoutines.h"
#include <Windows.h>
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/SoundManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Global/GlobalVariables.h"
#include "Global/Definitions.h"

namespace LuaIuranusRoutines {

	// LUA ROUTINES
	
	// create_enemy
	static int create_enemy(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->createEnemy();
		
		return 0;
	}

	static int create_initial_enemies(lua_State *ls)
	{
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->createInitialEnemies();

		return 0;
	}

	static int increment_eaten_enemy(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->incrementEatenEnemy();

		return 0;
	}

	static int decrement_enemies_in_screen(lua_State *ls)
	{
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->decrementEnemiesInScreen();

		return 0;
	}

	static int check_fart_to_do(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->checkFartToDo();

		return 0;
	}
	
	static int player_win(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
			Global::youWin = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in player_win in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int player_lose(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
			Global::youLose = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in player_lose in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int player_lose_timeout(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
			Global::youLoseTimeOut = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in player_lose_timeout in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int take_player_control_out(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 1)
			Global::takePlayerControlOut = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in take_player_control_out in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int take_player_control_out_but_anim(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 1)
			Global::takePlayerControlOutUpdatingAnimation = lua_toboolean(ls, 1) ? true : false;
		else
			assert( !"Number of parameters incorrect in take_player_control_out_but_anim in LuaIuranusRoutines" );

		return 0;
	}

	static int player_inside_ass(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 1)
			Global::playerInsideAssVolume = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in player_inside_ass in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int check_iuranus_victory_achieved(lua_State *ls)
	{
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->checkVictoryAchieved();

		return 0;
	}

	static int reset_counter_to_fail_point(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->resetCountersToFailPoint();

		return 0;
	}

	static int expel_enemies_around_mouth(lua_State *ls)
	{
		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->checkForEnemiesInMouthExpelRadius();

		return 0;
	}

	static int expel_player_from_ass(lua_State *ls)
	{
		Player *p = Player::getPlayer();
		// Leave player animation to idle
		p->setRelationToState( CYCLE, "idle", true, true );
		p->setRelationToState(CYCLE, "thrown", false, true);
		p->setCurrentSpeed(3.0f);

		return 0;
	}

	static int deep_player_into_ass(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 1)
			Global::playerDeepingInAss = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in deep_player_into_ass in LuaIuranusRoutines" );
		}

		return 0;
	}
	
	static int expel_enemies_around_ass(lua_State *ls)
	{		
		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->checkForEnemiesInAssExpelRadius();

		return 0;
	}

	static int anim_iuranus_enemies_victory(lua_State *ls)
	{
		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->changeEnemiesStateToVictory();
		
		return 0;
	}

	static int reset_iuranus_enemies_states(lua_State *ls)
	{
		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->resetEnemiesStates();

		return 0;
	}

	static int anim_mouth(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 2) {
			int actionType = (int) lua_tonumber(ls, 1);
			std::string mouthAnimName = lua_tostring(ls,2);
			IuranusPlanetElementsManager *p = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
			p->animateMouth((ActionType) actionType, mouthAnimName);
		}
		else {
			assert( !"Number of parameters incorrect in anim_mouth in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int anim_ass(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 2 ) {
			int actionType = (int) lua_tonumber(ls, 1);
			std::string assAnimName = lua_tostring(ls, 2);
			IuranusPlanetElementsManager *p = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
			p->animateAss((ActionType) actionType, assAnimName);
		}
		else {
			assert( !"Number of parameters incorrect in anim_ass in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int move_object(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) {
			std::string bush_name = lua_tostring(ls,1);

			IuranusAnimatedObjectManager *aom = IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager();
			
			aom->moveObject( bush_name );
		}
		else {
			assert( !"Number of parameters incorrect in move_object in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int kill_enemies_in_mouth(lua_State *ls)
	{
		IuranusEnemyManager::getIuranusEnemyManager()->killEatenEnemies();
		return 0;
	}

	static int add_particle(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 2 ) {
			std::string bush_name = lua_tostring(ls,1);
			std::string particle = lua_tostring(ls, 2);

			IuranusAnimatedObjectManager *aom = IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager();
			
			AnimatedObject *ao = aom->getAnimatedObject(bush_name);

			ParticlesManager *pm = ParticlesManager::getParticlesManager();
			pm->addParticle(particle, ao, false);
		}
		else {
			assert( !"Number of parameters incorrect in add_particle in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int add_planet_particle(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 2 ) {
			std::string element = lua_tostring(ls,1);
			std::string particle = lua_tostring(ls, 2);

			IuranusPlanetElementsManager *ipem = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
			
			AnimatedObject *ao = ipem->getElement(element);

			ParticlesManager *pm = ParticlesManager::getParticlesManager();
			pm->addParticle(particle, ao, false);
		}
		else {
			assert( !"Number of parameters incorrect in add_particle in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int reset_iuranus_logic_counters(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->resetCounters();

		return 0;
	}
	
	static int reset_iuranus_enemies_in_screen(lua_State *ls)
	{		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		iuranusPlanetLogic->resetNumEnemiesInTheScreen();

		return 0;
	}

	static int play_player_expel_sample_rand(lua_State *ls)
	{
		if( Global::samplesEnabled )
		{
			SoundManager *soundManager = SoundManager::getSoundManager();

			int randNum = floor(Global::obtainRandom( 1.0f, 3.5f ));
			randNum = Global::clamp( randNum, 1, 3 );

			char sound[ 128 ];
			sprintf_s(sound, sizeof(sound), "player_expeled_by_ass_%d", randNum);

			soundManager->playSample(sound);
		}
		return 0;
	}

	// LUA REGISTER FUNCTION
	void registerLuaIuranusRoutines(lua_State *ls) 
	{
		lua_register(ls, "create_enemy", &create_enemy);
		lua_register(ls, "increment_eaten_enemy", &increment_eaten_enemy);
		lua_register(ls, "check_fart_to_do", &check_fart_to_do);
		lua_register(ls, "player_win", &player_win);
		lua_register(ls, "player_lose", &player_lose);
		lua_register(ls, "player_lose_timeout", &player_lose_timeout);
		lua_register(ls, "take_player_control_out", &take_player_control_out);
		lua_register(ls, "check_iuranus_victory_achieved", &check_iuranus_victory_achieved);
		lua_register(ls, "player_inside_ass", &player_inside_ass);
		lua_register(ls, "reset_counter_to_fail_point", &reset_counter_to_fail_point);
		lua_register(ls, "decrement_enemies_in_screen", &decrement_enemies_in_screen);
		lua_register(ls, "create_initial_enemies", &create_initial_enemies);
		lua_register(ls, "expel_enemies_around_mouth", &expel_enemies_around_mouth);
		lua_register(ls, "expel_enemies_around_ass", &expel_enemies_around_ass);
		lua_register(ls, "expel_player_from_ass", &expel_player_from_ass);
		lua_register(ls, "deep_player_into_ass", &deep_player_into_ass);
		lua_register(ls, "anim_iuranus_enemies_victory", &anim_iuranus_enemies_victory);
		lua_register(ls, "reset_iuranus_enemies_states", &reset_iuranus_enemies_states);
		lua_register(ls, "anim_mouth", &anim_mouth);
		lua_register(ls, "anim_ass", &anim_ass);
		lua_register(ls, "move_object", &move_object);
		lua_register(ls, "kill_enemies_in_mouth", &kill_enemies_in_mouth);
		lua_register(ls, "add_particle", &add_particle);
		lua_register(ls, "add_planet_particle", &add_planet_particle);
		lua_register(ls, "reset_iuranus_logic_counters", &reset_iuranus_logic_counters);
		lua_register(ls, "reset_iuranus_enemies_in_screen", &reset_iuranus_enemies_in_screen);
		lua_register(ls, "take_player_control_out_but_anim", &take_player_control_out_but_anim);
		lua_register(ls, "play_player_expel_sample_rand", &play_player_expel_sample_rand);
	}

}
