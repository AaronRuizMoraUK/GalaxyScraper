#include "LuaVM.h"
#include "Logic/LuaRoutines/LuaRoutines.h"
#include "Logic/LuaRoutines/LuaSoundRoutines.h"
#include "Logic/LuaRoutines/LuaIuranusRoutines.h"
#include "Logic/LuaRoutines/LuaBoilerRoutines.h"
#include "Logic/LuaRoutines/LuaLogicRoutines.h"
#include "Logic/LuaRoutines/LuaStateRoutines.h"
#include "Managers/DataLocatorManager.h"
#include <cassert>

LuaVM * LuaVM::luaVM = NULL;

LuaVM * LuaVM::getLuaVM() {
	if(luaVM == NULL) {
		luaVM = new LuaVM();
		luaVM->create();
	}

	return luaVM;
}

LuaVM::LuaVM()
: ls(NULL)
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	pathToScripts = dataLocatorManager->getPath("script");
}

void LuaVM::create() {
	// LUA
	ls=lua_open();

	// Register LUA Routines
	LuaRoutines::registerLuaRoutines(ls);
	LuaSoundRoutines::registerLuaSoundRoutines(ls);
	LuaIuranusRoutines::registerLuaIuranusRoutines(ls);
	LuaBoilerRoutines::registerLuaBoilerRoutines(ls);
	LuaLogicRoutines::registerLuaLogicRoutines(ls);
	LuaStateRoutines::registerLuaStateRoutines(ls);
}

void LuaVM::destroy() {
	lua_close(ls);
}

void LuaVM::execute(const std::string &pScript, bool notifyNoScript) 
{
	std::string script = pathToScripts + pScript;

	int result=luaL_dofile(ls, script.c_str());

	// TODO Quitar esto al final
	if( script.find("on_exit_") != -1  )
		return;

	if(notifyNoScript) {
		if(result!=0) {
			std::string errorMsg = "Lua File " + script + " not found\n";
			OutputDebugString(errorMsg.c_str());
			MessageBox( NULL, errorMsg.c_str(), "LUA", MB_ICONERROR | MB_OK );
			assert( !"Lua File not found in LuaVM");
		}
	}
}
