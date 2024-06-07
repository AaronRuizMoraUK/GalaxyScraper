#ifndef LOGIC_TIMERMANAGER_H
#define LOGIC_TIMERMANAGER_H

#include <list>
#include <string>
#include "Logic/LogicTimer/LogicTimer.h"

/**
* Manager of logic timers.
*
* It is a singleton.
*/

class LogicTimerManager : public std::list<LogicTimer *>
{
public:
	~LogicTimerManager() {
		OutputDebugString("Destroying LogicTimerManager...\n");
		cleanUp();
	};
	void cleanUp();

	static LogicTimerManager * getLogicTimerManager();

	void createTimer(const std::string &timerName, float timeout);
	void deleteTimer(const std::string &timerName);
	void update( float deltaTime=(1.0f/60.0f) );

	const LogicTimer * getLogicTimer( const std::string &timerName ) const;

private:
	static LogicTimerManager * logicTimerManager;
	LogicTimerManager() {};
};

#endif //LOGIC_TIMERMANAGER_H
