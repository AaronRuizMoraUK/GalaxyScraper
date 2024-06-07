#include "LogicManager.h"
#include "GameState/IGameState.h"

LogicManager * LogicManager::logicManager = NULL;

LogicManager * LogicManager::getLogicManager() 
{
	if(logicManager == NULL)
		logicManager = new LogicManager();

	return logicManager;
}

LogicManager::LogicManager()
: logicTimerManager(NULL)
, userEventsManager(NULL)
{
	iuranusLogicVolumeManager.load( "iuranus" );
	boilerLogicVolumeManager.load( "boiler" );

	logicTimerManager = LogicTimerManager::getLogicTimerManager();
	userEventsManager = UserEventsManager::getUserEventsManager();
}

void LogicManager::update(float deltaTime)
{
	if( IGameState::currentState->getName() == "iuranus_state" )
		iuranusLogicVolumeManager.update( );
	else if( IGameState::currentState->getName() == "boiler_state" )
		boilerLogicVolumeManager.update( );
	else if( IGameState::currentState->getName() == "test_boiler_planet_state" )
		boilerLogicVolumeManager.update( );
	// TODO: eliminar esto último, solo para pruebas

	logicTimerManager->update( deltaTime );
}

void LogicManager::createTimer(const std::string &timerName, float timeout)
{
	logicTimerManager->createTimer(timerName, timeout);
}

void LogicManager::deleteTimer(const std::string &timerName) {
	logicTimerManager->deleteTimer(timerName);
}

void LogicManager::createUserEvent(const std::string &eventName)
{
	userEventsManager->execute(eventName);
}

void LogicManager::cleanUp()
{
	if(logicTimerManager != NULL)
		delete logicTimerManager, logicTimerManager = NULL;
	if(userEventsManager != NULL)
		delete userEventsManager, userEventsManager = NULL;
}
