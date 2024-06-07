#include "UserEventsManager.h"
#include "Logic/LuaVM/LuaVM.h"

UserEventsManager * UserEventsManager::userEventsManager = NULL;

UserEventsManager * UserEventsManager::getUserEventsManager() {
	if(userEventsManager == NULL)
		userEventsManager = new UserEventsManager();

	return userEventsManager;
}

void UserEventsManager::execute(const std::string &scriptName)
{
	LuaVM *luaVM = LuaVM::getLuaVM();
	luaVM->execute("user_events/" + scriptName + ".lua");
}
