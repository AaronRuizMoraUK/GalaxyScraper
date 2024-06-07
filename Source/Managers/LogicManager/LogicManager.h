#ifndef LOGIC_MANAGER_H
#define LOGIC_MANAGER_H

#include "Managers/LogicManager/LogicVolumeManager.h"
#include "Managers/LogicManager/LogicTimerManager.h"
#include "Managers/LogicManager/UserEventsManager.h"
#include <string>

/**
* Manager of logic events.
*
* It is a singleton.
*/

class LogicManager
{
protected:
	LogicVolumeManager iuranusLogicVolumeManager;
	LogicVolumeManager boilerLogicVolumeManager;
	LogicTimerManager *logicTimerManager;
	UserEventsManager *userEventsManager;

public:
	~LogicManager() {
		OutputDebugString("Destroying LogicManager...\n");
		cleanUp();
	};
	static LogicManager * getLogicManager();

	void update(float deltaTime=(1.0f/60.0f));
	void cleanUp();

	void createTimer(const std::string &timerName, float timeout);
	void deleteTimer(const std::string &timerName);
	void createUserEvent(const std::string &eventName);

	const LogicVolumeManager & getIuranusLogicVolumeManager( ) const { return iuranusLogicVolumeManager; };
	const LogicVolumeManager & getBoilerLogicVolumeManager( ) const { return boilerLogicVolumeManager; };

	void cleanLogicTimerManager() {
		if(logicTimerManager)
			logicTimerManager->cleanUp();
	};

private:
	static LogicManager * logicManager;
	LogicManager();
	
};

#endif //LOGIC_MANAGER_H
