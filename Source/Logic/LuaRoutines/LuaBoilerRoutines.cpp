#include "LuaBoilerRoutines.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"
#include "Managers/BoilerManagers/CheckpointsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/SoundManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerPlanetLogic.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/ParticlesManager.h"
#include "GameState/Boiler/BoilerState.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Managers/PlanetManager.h"
#include "GameState/IGameState.h"
#include "GameState/Boiler/BoilerState.h"
#include <Windows.h>

namespace LuaBoilerRoutines {

	// LUA ROUTINES
	static int start_platform_sequence(lua_State *ls)
	{		
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int platformId = (int) lua_tonumber(ls, 1);
			FirePlatformsManager::getFirePlatformsManager()->startSequence( platformId );
		}
		else {
			assert( !"Number of parameters incorrect in start_platform_sequence of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int init_zone(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int zoneId = (int) lua_tonumber(ls, 1);
			BoilerPlanetLogic::getBoilerPlanetLogic()->initZone( zoneId );
		}
		else {
			assert( !"Number of parameters incorrect in init_zone of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int zone_entered(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int zoneId = (int) lua_tonumber(ls, 1);
			BoilerPlanetLogic::getBoilerPlanetLogic()->zoneEntered( zoneId );
		}
		else {
			assert( !"Number of parameters incorrect in zone_entered of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int mini_boiler_killed_at_zone(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int zoneId = (int) lua_tonumber(ls, 1);
			BoilerPlanetLogic::getBoilerPlanetLogic()->miniBoilerKilledAtZone( zoneId );
		}
		else {
			assert( !"Number of parameters incorrect in mini_boiler_killed_at_zone of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int open_next_zone_to(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int zoneId = (int) lua_tonumber(ls, 1);
			BoilerPlanetLogic::getBoilerPlanetLogic()->openNextZoneTo( zoneId );
		}
		else {
			assert( !"Number of parameters incorrect in open_next_zone_to of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int check_zone_platform_firing(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 2 ) 
		{
			int zoneId = (int) lua_tonumber(ls, 1);
			int zonePlatformId = (int) lua_tonumber(ls, 2);
			BoilerPlanetLogic::getBoilerPlanetLogic()->checkPlatformFiring( zoneId, zonePlatformId );
		}
		else {
			assert( !"Number of parameters incorrect in check_zone_platform_firing of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int check_platform_firing(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int platformId = (int) lua_tonumber(ls, 1);
			FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();
			bool platformFiring = fpm->isPlatformFiring( platformId );
			if( platformFiring )
			{
				LogicManager::getLogicManager()->createUserEvent("on_player_fired");
			}
		}
		else {
			assert( !"Number of parameters incorrect in check_platform_firing of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int check_lever_activated(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int leverId = (int) lua_tonumber(ls, 1);
			LeversManager *lm = LeversManager::getLeversManager();
			lm->activateLever(leverId);
		}
		else {
			assert( !"Number of parameters incorrect in check_lever_activated of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int check_canon_activated(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int canonId = (int) lua_tonumber(ls, 1);
			CanonsManager *cm = CanonsManager::getCanonsManager();
			cm->activateCanon(canonId);
		}
		else {
			assert( !"Number of parameters incorrect in check_canon_activated of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int change_canon_animation_controller(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int canonId = (int) lua_tonumber(ls, 1);
			CanonsManager *cm = CanonsManager::getCanonsManager();
			cm->changeAnimationController(canonId);
		}
		else {
			assert( !"Number of parameters incorrect in check_canon_activated of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int shoot_ball(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int number = (int) lua_tonumber(ls, 1);
			std::string name;

			switch (number)
			{
			case 1:
				name = "canonball_1";
				break;
			case 2:
				name = "canonball_2";
				break;
			case 3:
				name = "canonball_3";
				break;
			case 4:
				name = "canonball_4";
				break;
			}
			BoilerFireballManager *bfm = BoilerFireballManager::getBoilerFireballManager();
			bfm->shootBall(name);
		}
		else {
			assert( !"Number of parameters incorrect in shoot_ball of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int player_checkpoint_achieved(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) 
		{
			int checkpointId = (int) lua_tonumber(ls, 1);
			
			CheckpointsManager *cm = CheckpointsManager::getCheckpointsManager();
			int lastCheckPointId = cm->getLastCheckpointAchievedId();
			if( checkpointId != lastCheckPointId )
				cm->playerCheckpointAchieved(checkpointId);
		}
		else {
			assert( !"Number of parameters incorrect in player_checkpoint_achieved of LuaBoilerRoutines" );
		}

		return 0;
	}

	static int update_player_fired_retries_count(lua_State *ls)
	{
		BoilerPlanetLogic::getBoilerPlanetLogic()->updatePlayerFiredRetries();
		return 0;
	}

	static int reset_fired_death_count(lua_State *ls)
	{
		BoilerPlanetLogic::getBoilerPlanetLogic()->resetFiredDeathsCount();
		return 0;
	}

	static int boiler_door_next_animation(lua_State *ls)
	{
		BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->doNextDoorAnimation( );
		return 0;
	}

	static int enable_player_flicker(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			Global::playerFlickering = lua_toboolean(ls, 1) ? true : false;
			if( Global::playerFlickering && Global::samplesEnabled )
			{	
				SoundManager *soundManager = SoundManager::getSoundManager();

				int randNum = floor(Global::obtainRandom( 1.0f, 2.5f ));
				randNum = Global::clamp( randNum, 1, 2 );

				char sound[ 128 ];
				sprintf_s(sound, sizeof(sound), "after_player_burned_%d", randNum);

				soundManager->playSample(sound);
				soundManager->playSample("player_clothes_burned");
			}
		}
		else {
			assert( !"Number of parameters incorrect in enable_player_flicker in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int set_fallen_condition(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
			Global::alreadyFallen = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in set_fallen_condition in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int reset_boiler_camera_position(lua_State *ls)
	{
		ICameraController *cameraController = Global::camera->getCameraController();
		if( cameraController->getControllerType() != "ss_player_camera_controller" ) {
			assert( !"Camera controller is not a ss_player_camera_controller" );
			return 0;
		}

		SSPlayerCameraController * ssCameraController = (SSPlayerCameraController *) cameraController;
		ssCameraController->setDistance( BoilerState::boilerCameraDistance );
		return 0;
	}

	static int set_camera_distance(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			float cameraDistance = (float) lua_tonumber(ls,1);
			ICameraController *cameraController = Global::camera->getCameraController();
			if( cameraController->getControllerType() != "ss_player_camera_controller" ) {
				assert( !"Camera controller is not a ss_player_camera_controller" );
				return 0;
			}

			SSPlayerCameraController * ssCameraController = (SSPlayerCameraController *) cameraController;
			ssCameraController->setDistance( cameraDistance );
		}
		else {
			assert( !"Number of parameters incorrect in set_camera_position in LuaIuranusRoutines" );
		}

		return 0;
	}
	
	static int position_camera_to_look_boiler_door(lua_State *ls) {
		static const D3DXVECTOR3 local_camera_position(35.3143f, 11.6558f, 11.8037f);
		static const D3DXVECTOR3 local_camera_target(30.8736f, 9.96323f, -2.62102f);

		const Planet * planet = PlanetManager::getPlanetManager()->getPlanet( "boiler" );
		const D3DXVECTOR3 &center = planet->getCenter();

		D3DXVECTOR3 position(local_camera_position + center );
		D3DXVECTOR3 target(local_camera_target + center );
		D3DXVECTOR3 up = position - center;
		D3DXVec3Normalize( &up, &up );

		Global::camera->setUpVector( up );
		Global::camera->lookAt( position, target );

		return 0;
	}

	static int position_camera_to_look_boiler_up(lua_State *ls) {
		static const D3DXVECTOR3 local_camera_position(21.203f, 0.034f, -95.747f);
		static const D3DXVECTOR3 local_camera_target(5.50372f, -0.493218f, -29.4485f);

		const Planet * planet = PlanetManager::getPlanetManager()->getPlanet( "boiler" );
		const D3DXVECTOR3 &center = planet->getCenter();

		D3DXVECTOR3 position(local_camera_position + center );
		D3DXVECTOR3 target(local_camera_target + center );
		D3DXVECTOR3 up = position - center;
		D3DXVec3Normalize( &up, &up );

		Global::camera->setUpVector( up );
		//Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
		Global::camera->lookAt( position, target );

		return 0;
	}

	static int add_burned_particles_to_player(lua_State *ls)
	{
		Player *player = Player::getPlayer();
		ParticlesManager *pm = ParticlesManager::getParticlesManager();
		pm->addParticle("player_burned", player, false);

		return 0;
	}

	static int reset_logic(lua_State *ls)
	{
		BoilerPlanetLogic::getBoilerPlanetLogic()->resetLogic();
		return 0;
	}

	static int reset_checkpoints(lua_State *ls)
	{
		CheckpointsManager::getCheckpointsManager()->reset();
		return 0;
	}

	static int place_player_initial_checkpoint(lua_State *ls)
	{
		CheckpointsManager::getCheckpointsManager()->placePlayerInitialCheckpoint();
		return 0;
	}

	static int init_boss_enemy_flame_zone(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 2)
		{
			std::string enemyName = lua_tostring(ls, 1);
			int flameZoneId = (int) lua_tonumber(ls, 2);
			BoilerEnemyManager::getBoilerEnemyManager()->initEnemyFlameZone(enemyName, flameZoneId);
		}
		else {
			assert( !"Number of parameters incorrect in init_boss_enemy_flame_zone in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int enable_atmospheres_in_boiler(lua_State *ls) {
		BoilerState *boilerState = (BoilerState *)IGameState::getState( "boiler_state" );
		boilerState->drawAtmospheres = true;
		return 0;
	}

	static int disable_atmospheres_in_boiler(lua_State *ls) {
		BoilerState *boilerState = (BoilerState *)IGameState::getState( "boiler_state" );
		boilerState->drawAtmospheres = false;
		return 0;
	}

	static int init_boss_enemy_sequence(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 1)
		{
			std::string enemyName = lua_tostring(ls, 1);
			BoilerEnemyManager::getBoilerEnemyManager()->initEnemyFlamesSequence(enemyName);
		}
		else {
			assert( !"Number of parameters incorrect in init_boss_enemy_sequence in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int kill_boss_zone(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 2)
		{
			std::string enemyName = lua_tostring(ls, 1);
			int flameZoneId = (int) lua_tonumber(ls, 2);
			BoilerEnemyManager::getBoilerEnemyManager()->killEnemyFlameZone(enemyName, flameZoneId);
		}
		else {
			assert( !"Number of parameters incorrect in kill_boss_zone in LuaIuranusRoutines" );
		}

		return 0;
	}

	static int active_godray(lua_State *ls)
	{
		int num =lua_gettop(ls);

		if(num == 2)
		{
			int godrayId = (int) lua_tonumber(ls, 1);
			bool activateGodray = lua_toboolean(ls, 2) ? true : false;
			Static2DObjectManager::getStatic2DObjectManager()->activeGodray( godrayId, activateGodray );
		}
		else {
			assert( !"Number of parameters incorrect in active_godray in LuaIuranusRoutines" );
		}

		return 0;		
	}

	static int show_all_boss_zone(lua_State *ls)
	{
		if( !BoilerPlanetLogic::getBoilerPlanetLogic()->alreadyEnteredBossZone )
		{
			BoilerPlanetLogic::getBoilerPlanetLogic()->alreadyEnteredBossZone = true;
			LogicManager::getLogicManager()->createUserEvent("on_enter_boss_zone");
		}
		return 0;
	}

	// LUA REGISTER FUNCTION
	void registerLuaBoilerRoutines(lua_State *ls) 
	{
		lua_register(ls, "start_platform_sequence", &start_platform_sequence);
		lua_register(ls, "init_zone", &init_zone);
		lua_register(ls, "zone_entered", &zone_entered);
		lua_register(ls, "mini_boiler_killed_at_zone", &mini_boiler_killed_at_zone);
		lua_register(ls, "open_next_zone_to", &open_next_zone_to);
		lua_register(ls, "check_zone_platform_firing", &check_zone_platform_firing);
		lua_register(ls, "check_platform_firing", &check_platform_firing);
		lua_register(ls, "check_lever_activated", &check_lever_activated);
		lua_register(ls, "check_canon_activated", &check_canon_activated);
		lua_register(ls, "shoot_ball", &shoot_ball);
		lua_register(ls, "change_canon_animation_controller", &change_canon_animation_controller);
		lua_register(ls, "player_checkpoint_achieved", &player_checkpoint_achieved);
		lua_register(ls, "update_player_fired_retries_count", &update_player_fired_retries_count);
		lua_register(ls, "reset_fired_death_count", &reset_fired_death_count);
		lua_register(ls, "boiler_door_next_animation", &boiler_door_next_animation);
		lua_register(ls, "enable_player_flicker", &enable_player_flicker);
		lua_register(ls, "set_fallen_condition", &set_fallen_condition);
		lua_register(ls, "reset_boiler_camera_position", &reset_boiler_camera_position);
		lua_register(ls, "set_camera_distance", &set_camera_distance);
		lua_register(ls, "position_camera_to_look_boiler_door", &position_camera_to_look_boiler_door);
		lua_register(ls, "position_camera_to_look_boiler_up", &position_camera_to_look_boiler_up);
		lua_register(ls, "add_burned_particles_to_player", &add_burned_particles_to_player);
		lua_register(ls, "reset_logic", &reset_logic);
		lua_register(ls, "reset_checkpoints", &reset_checkpoints);
		lua_register(ls, "place_player_initial_checkpoint", &place_player_initial_checkpoint);
		lua_register(ls, "init_boss_enemy_flame_zone", &init_boss_enemy_flame_zone);
		lua_register(ls, "enable_atmospheres_in_boiler", &enable_atmospheres_in_boiler);
		lua_register(ls, "disable_atmospheres_in_boiler", &disable_atmospheres_in_boiler);
		lua_register(ls, "init_boss_enemy_sequence", &init_boss_enemy_sequence);
		lua_register(ls, "kill_boss_zone", &kill_boss_zone);
		lua_register(ls, "active_godray", &active_godray);
		lua_register(ls, "show_all_boss_zone", &show_all_boss_zone);
	}

}
