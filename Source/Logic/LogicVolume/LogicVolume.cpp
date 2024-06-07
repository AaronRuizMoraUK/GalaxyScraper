#include "LogicVolume.h"
#include "Logic/LuaVM/LuaVM.h"

LogicVolume::LogicVolume(const std::string &pVolumeName, bool pCheckForPlayer, bool pCheckIsInside) 
: volumeName(pVolumeName)
, inside(false)
, checkForPlayer(pCheckForPlayer)
, checkIsInside(pCheckIsInside)
{
}


void LogicVolume::recalc(const std::string &planetName) 
{
	std::string scriptName;
	const bool newInside = checkInside();
	bool executeScript = false;
	
	if(newInside && !inside)		// ON ENTER
	{
		scriptName = "on_enter_"+volumeName+".lua";
		executeScript = true;
	}
	else if(!newInside && inside)	// ON EXIT
	{
		scriptName = "on_exit_"+volumeName+".lua";
		executeScript = true;
	}
	else if( inside && checkIsInside ) // ON STAY
	{
		scriptName = "on_stay_"+volumeName+".lua";
		executeScript = true;
	}

	inside = newInside;

	if(executeScript)
	{
		LuaVM *luaVM = LuaVM::getLuaVM();
		luaVM->execute("logic_volumes/"+planetName+"/"+scriptName);
	}
}
