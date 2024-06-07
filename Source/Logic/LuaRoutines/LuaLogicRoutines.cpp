#include "LuaLogicRoutines.h"
#include <Windows.h>
#include "Managers/LogicManager/LogicManager.h"
#include "Global/GlobalVariables.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include <string>

namespace LuaLogicRoutines {

	// LUA ROUTINES

	// create_timer
	// input:	std::string timerName
	//			float timeout
	static int create_timer(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 2 ) {
			std::string timerName = lua_tostring(ls,1);
			float timeout = (float) lua_tonumber(ls,2);

			LogicManager *logicManager = LogicManager::getLogicManager();
			logicManager->createTimer(timerName, timeout);
		}
		else {
			assert( !"Number of parameters incorrect in create_timer in LuaLogicRoutines" );
		}

		return 0;
	}

	static int delete_timer(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 )  {
			std::string timerName = lua_tostring(ls,1);

			LogicManager *logicManager = LogicManager::getLogicManager();
			logicManager->deleteTimer(timerName);
		}
		else {
			assert( !"Number of parameters incorrect in delete_timer in LuaLogicRoutines" );
		}

		return 0;
	}

	// LUA REGISTER FUNCTION
	void registerLuaLogicRoutines(lua_State *ls) 
	{
		lua_register(ls, "create_timer", &create_timer);
		lua_register(ls, "delete_timer", &delete_timer);
	}
}
