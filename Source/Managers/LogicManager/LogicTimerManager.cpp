#include "LogicTimerManager.h"

LogicTimerManager * LogicTimerManager::logicTimerManager = NULL;

LogicTimerManager * LogicTimerManager::getLogicTimerManager() {
	if(logicTimerManager == NULL)
		logicTimerManager = new LogicTimerManager();

	return logicTimerManager;
}

void LogicTimerManager::createTimer(const std::string &timerName, float timeout)
{
	LogicTimer *logicTimer = new LogicTimer(timerName, timeout);
	push_back(logicTimer);
}

void LogicTimerManager::deleteTimer(const std::string &timerName)
{
	iterator it = begin();
	iterator it_end = end();
	while(it != it_end)
	{
		LogicTimer *logicTimer = (*it);

		if ( logicTimer->getScriptName() == timerName ) {
			if( logicTimer )
				delete logicTimer, logicTimer = NULL;

			it = erase( it );
			it_end = end();
		}
		else
			++it;
	}
}

void LogicTimerManager::update( float deltaTime )
{
	/*
	iterator it = begin();
	while(it != end())
	{
		// TODO break?
		if( (*it)->isActive() )
			if ( (*it)->recalc( deltaTime ) ) break;
		++it;
	}

	// TODO Borrar los timers que han terminado
	*/

	iterator it = begin();
	iterator it_end = end();
	while(it != it_end)
	{
		LogicTimer *logicTimer = (*it);

		if ( logicTimer->recalc( deltaTime ) ) {
			if( logicTimer )
				delete logicTimer, logicTimer = NULL;

			it = erase( it );
			it_end = end();
		}
		else
			++it;
	}
}

const LogicTimer * LogicTimerManager::getLogicTimer( const std::string &timerName ) const {
	const_iterator it = begin();
	while( it != end() )
	{
		const std::string &name = (*it)->getScriptName( );
		if( name == timerName )
			return (*it);

		++it;
	}

	return NULL;
}

void LogicTimerManager::cleanUp() {
	while(!empty()) {
		LogicTimer * logicTimer = back();
		if(logicTimer)
			delete logicTimer, logicTimer = NULL;
		pop_back();
	}
}
