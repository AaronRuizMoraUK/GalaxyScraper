#include "LogicTimer.h"
#include "Logic/LuaVM/LuaVM.h"

LogicTimer::LogicTimer(const std::string &pScriptName, float pTimeout) 
: timeout(pTimeout)
, currentTime( 0.0f )
, scriptName(pScriptName)
//, active(true)
//, birth(timeGetTime())
{
}

bool LogicTimer::recalc( float deltaTime )
{
	//float currentTime = (timeGetTime() - birth) / 1000.0f;
	currentTime += deltaTime;
	
	if( /*active &&*/ currentTime >= timeout )
	{
		currentTime = timeout;	// This make currentTime not be greater than timeout
		//active = false;

		LuaVM *luaVM = LuaVM::getLuaVM();
		luaVM->execute("timeouts/on_timeout_"+scriptName+".lua");

		return true;
	}

	return false;
}
