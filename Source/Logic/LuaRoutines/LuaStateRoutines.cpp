#include "LuaStateRoutines.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Global/GlobalVariables.h"

namespace LuaStateRoutines {

	// LUA ROUTINES

	static int load_menu_camera(lua_State *ls)
	{		
		// Set the camera in init menu position
		//D3DXVECTOR3 cameraInitialPosition(419.05f, 20.64f, 394.65f);
		//D3DXVECTOR3 cameraLookAt( menuPlanet.getPosition()+D3DXVECTOR3(0.0f,5.0f,4.0f) );
		//Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
		//Global::camera->lookAt( cameraInitialPosition, cameraLookAt );

		return 0;
	}

	static int reset_continues(lua_State *ls)
	{
		return 0;
	}

	static int place_player_initial_position(lua_State *ls)
	{
		Player *player = Player::getPlayer();
		player->setPlayerToInitPosition();

		return 0;
	}

	static int place_camera_initial_position(lua_State *ls)
	{
		Global::camera->setCameraToInitialPosition();

		return 0;
	}

	static int enable_camera_vibration(lua_State *ls)
	{
		int num = lua_gettop(ls);
		if( num == 1 )
		{
			float timeout = (float) lua_tonumber(ls, 1);
			Global::cameraVibrationEnabled = true;
			LogicManager::getLogicManager()->createTimer("disable_camera_vibration", timeout);
		}
		else if( num == 0 ) {
			float timeout = (float) lua_tonumber(ls, 1);
			Global::cameraVibrationEnabled = true;
		}
		else {
			assert( !"Number of parameters incorrect in enable_camera_vibration in LuaStateRoutines" );
		}

		return 0;
	}

	static int disable_camera_vibration(lua_State *ls)
	{
		Global::cameraVibrationEnabled = false;

		return 0;
	}

	static int reset_iuranus_logic(lua_State *ls)
	{
		return 0;
	}

	static int reset_iuranus_collision_manager(lua_State *ls)
	{
		return 0;
	}

	static int reset_iuranus_enemy_manager(lua_State *ls)
	{
		return 0;
	}

	static int reset_boiler_logic(lua_State *ls)
	{
		return 0;
	}

	static int reset_boiler_collision_manager(lua_State *ls)
	{
		return 0;
	}

	static int reset_boiler_enemy_manager(lua_State *ls)
	{
		return 0;
	}

	static int set_camera_params(lua_State *ls)
	{
		int num = lua_gettop(ls);
		if( num == 3 )
		{
			float cameraDistance = (float) lua_tonumber(ls, 1);
			float cameraPositionSpeed = (float) lua_tonumber(ls, 2);
			float cameraTargetSpeed = (float) lua_tonumber(ls, 3);

			ICameraController *cameraController = Global::camera->getCameraController();
			if( cameraController )
			{
				if( cameraController->getControllerType() != "ss_player_camera_controller" ) {
					assert( !"Camera controller is not a ss_player_camera_controller" );
					return 0;
				}

				SSPlayerCameraController * ssCameraController = (SSPlayerCameraController *) cameraController;
				ssCameraController->setDistance( cameraDistance );
				ssCameraController->setPositionSpeed( cameraPositionSpeed );
				ssCameraController->setTargetSpeed( cameraTargetSpeed );
			}
		}
		else {
			assert( !"Number of parameters incorrect in set_camera_params in LuaStateRoutines" );
		}
		return 0;
	}

	static bool already_in_save = false;

	static D3DXVECTOR3 last_saved_camera_up(0,1,0);
	static D3DXVECTOR3 last_saved_camera_position(0,0,0);
	static D3DXVECTOR3 last_saved_camera_target(1,0,0);

	// Things to save for SSPlayerCameraController
	static bool restore_ssplayer_camera_controller = false;
	static float targetSpeed = 0.0f;
	static float positionSpeed = 0.0f;
	static float distance = 0.0f;

	static int save_camera(lua_State *ls)
	{
		if( already_in_save ) {
			assert( !"This must not happend" );
			return 0;
		}

		last_saved_camera_up = Global::camera->getUpVector();
		last_saved_camera_position = Global::camera->getPosition();
		last_saved_camera_target = Global::camera->getTarget();

		// Save also the ssplayer_camera_controller
		ICameraController *cameraController = Global::camera->getCameraController();
		if( cameraController && cameraController->getControllerType() == "ss_player_camera_controller" ) {
			restore_ssplayer_camera_controller = true;

			targetSpeed = ((SSPlayerCameraController *) cameraController)->getTargetSpeed();
			positionSpeed = ((SSPlayerCameraController *) cameraController)->getPositionSpeed();
			distance = ((SSPlayerCameraController *) cameraController)->getDistance();
		}

		already_in_save = true;

		return 0;
	}

	static int restore_camera(lua_State *ls)
	{
		if( !already_in_save ) {
			assert( !"This must not happend" );
			return 0;
		}

		// Restore also the ssplayer_camera_controller
		if( restore_ssplayer_camera_controller ) {
			restore_ssplayer_camera_controller = false;
			Global::camera->loadSSPlayerCameraController(targetSpeed, positionSpeed, distance);
		}

		Global::camera->setUpVector( last_saved_camera_up );
		Global::camera->lookAt( last_saved_camera_position, last_saved_camera_target );

		already_in_save = false;
		return 0;
	}

	static int load_camera_dummy(lua_State *ls) {
		Global::camera->loadDummyCameraController();
		return 0;
	}

	// LUA REGISTER FUNCTION
	void registerLuaStateRoutines(lua_State *ls) 
	{
		lua_register(ls, "load_menu_camera", &load_menu_camera);
		lua_register(ls, "reset_continues", &reset_continues);
		lua_register(ls, "place_player_initial_position", &place_player_initial_position);
		lua_register(ls, "place_camera_initial_position", &place_camera_initial_position);
		lua_register(ls, "enable_camera_vibration", &enable_camera_vibration);
		lua_register(ls, "disable_camera_vibration", &disable_camera_vibration);
		lua_register(ls, "reset_iuranus_logic", &reset_iuranus_logic);
		lua_register(ls, "reset_iuranus_collision_manager", &reset_iuranus_collision_manager);
		lua_register(ls, "reset_iuranus_enemy_manager", &reset_iuranus_enemy_manager);
		lua_register(ls, "reset_boiler_logic", &reset_boiler_logic);
		lua_register(ls, "reset_boiler_collision_manager", &reset_boiler_collision_manager);
		lua_register(ls, "reset_boiler_enemy_manager", &reset_boiler_enemy_manager);
		lua_register(ls, "set_camera_params", &set_camera_params);
		lua_register(ls, "save_camera", &save_camera);
		lua_register(ls, "restore_camera", &restore_camera);
		lua_register(ls, "load_camera_dummy", &load_camera_dummy);
	}
}
