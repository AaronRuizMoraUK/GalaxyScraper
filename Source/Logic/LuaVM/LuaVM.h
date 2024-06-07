#ifndef LUA_VM_H
#define LUA_VM_H

#include <Windows.h>
#include <string>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

/**
* Lua Virtual Machine to execute script code.
*
* It is a singleton.
*/

class LuaVM
{
protected:
	std::string pathToScripts;

public:
	virtual ~LuaVM(void) {
		OutputDebugString("Destroying LuaVM...\n");
		destroy();
	};

	static LuaVM * getLuaVM();

	void execute(const std::string &pScript, bool notifyNoScript = true);

private:
	static LuaVM * luaVM;
	LuaVM();

	lua_State *ls;

	void create();
	void destroy();
};

#endif //LUA_VM_H