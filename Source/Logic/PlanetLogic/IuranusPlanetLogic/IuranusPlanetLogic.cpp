#include "IuranusPlanetLogic.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/DataLocatorManager.h"
#include "Logic/LogicVolume/LogicSphere.h"
#include "Source/Game.h"

IuranusPlanetLogic * IuranusPlanetLogic::iuranusPlanetLogic = NULL;

IuranusPlanetLogic * IuranusPlanetLogic::getIuranusPlanetLogic() 
{
	if(iuranusPlanetLogic == NULL)
		iuranusPlanetLogic = new IuranusPlanetLogic();

	return iuranusPlanetLogic;
}

IuranusPlanetLogic::IuranusPlanetLogic() 
: IPlanetLogic()
, numBirthPoints(0)
, numEnemiesEatenForVictory(0)
, totalEnemiesInScreen(0)
, initialTotalEnemiesInScreen(0)
, initialEnemiesInScreen(0)
, intervalEnemiesCreation(0.0f)
, checkPointPercent1(0)
, checkPointPercent2(0)
, checkPointPercent3(0)
, failVictoryCheckPointRecovery(0.0f)
, enemiesInTheScreen(0)
, numEnemiesEaten(0)
, checkPointActived(false)
, victoryActived(false)
, alreadyWon(false)
{
	init();
};

void IuranusPlanetLogic::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/iuranus/iuranus_logic_parameters.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void IuranusPlanetLogic::resetCounters()
{
	numEnemiesEaten = 0;
	victoryActived = false;
	checkPointActived = false;
	alreadyWon = false;
}

void IuranusPlanetLogic::setTotalEnemiesInScreen( ) {
	int divisor = 1;

	if( Game::configOptions.quality == "high" )
		divisor = 1;
	else if( Game::configOptions.quality == "medium" )
		divisor = 2;
	else if( Game::configOptions.quality == "low" )
		divisor = 3;
	else if( Game::configOptions.quality == "minimum" )
		divisor = 4;
	else {
		assert( !"Type of quality unknown in IuranusPlanetLogic" );
		return;
	}

	totalEnemiesInScreen = initialTotalEnemiesInScreen / divisor;
}

void IuranusPlanetLogic::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if (elem == "logic") 
	{
		numBirthPoints = atts.getInt( "num_birth_points", 0 );
		numEnemiesEatenForVictory = atts.getInt( "num_enemies_eaten_for_victory", 0 );
		initialTotalEnemiesInScreen = atts.getInt( "total_enemies_in_screen", 0 );
		setTotalEnemiesInScreen( );
		initialEnemiesInScreen = atts.getInt( "initial_enemies_in_screen", 0 );
		intervalEnemiesCreation = atts.getFloat( "interval_enemies_creation", 0.0f );


		float checkPointPercent = atts.getFloat( "check_point_percent_1", 0.0f );
		checkPointPercent1 = floor(numEnemiesEatenForVictory * checkPointPercent);

		checkPointPercent = atts.getFloat( "check_point_percent_2", 0.0f );
		checkPointPercent2 = floor(numEnemiesEatenForVictory * checkPointPercent);

		checkPointPercent = atts.getFloat( "check_point_percent_3", 0.0f );
		checkPointPercent3 = floor(numEnemiesEatenForVictory * checkPointPercent);

		failVictoryCheckPointRecovery = atts.getFloat( "fail_victory_check_point_recovery", 0.0f );
	}
}

void IuranusPlanetLogic::createEnemy()
{
	// check if enemy has to be created
	// if so
	//	get random birth point number
	//	get birth point position
	//	create lua timer for creating next enemy

	if( enemiesInTheScreen < totalEnemiesInScreen )
	{
		int randomBirthPoint = floor(Global::obtainRandom( 1.0f, numBirthPoints + 1.0f ));
		randomBirthPoint = Global::clamp( randomBirthPoint, 1, numBirthPoints );

		char birthPointNameTmp[ 128 ];
		sprintf_s(birthPointNameTmp, sizeof(birthPointNameTmp), "birth_point_%d", randomBirthPoint);
		std::string birthPointName = birthPointNameTmp;	

		const LogicVolumeManager &iuranusLogicVolumeManager = LogicManager::getLogicManager()->getIuranusLogicVolumeManager();
		const LogicSphere *logicSphere = (const LogicSphere *) iuranusLogicVolumeManager.getElement( birthPointName );
		const D3DXVECTOR3 &birthPoint = logicSphere->getCenter();

		std::string enemyType;
		float enemyTypeRandom = Global::obtainRandom( 0.0f, 3.0f );
		if( enemyTypeRandom >= 0.0f && enemyTypeRandom < 1.0f )
			enemyType = "malo";
		else if( enemyTypeRandom >= 1.0f && enemyTypeRandom < 2.0f )
			enemyType = "bueno";
		else
			enemyType = "feo";

		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->createEnemy(birthPoint, enemyType);

		++enemiesInTheScreen;

		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createTimer("create_iuranus_enemy", intervalEnemiesCreation);
	}
}

void IuranusPlanetLogic::createInitialEnemies()
{ 

	if( initialEnemiesInScreen > totalEnemiesInScreen )
		initialEnemiesInScreen = totalEnemiesInScreen;

	for(int i=0;(i<initialEnemiesInScreen)&&(enemiesInTheScreen<totalEnemiesInScreen);++i)
	{
		// SAME CODE THAN CREATE ENEMY FUNCTION //
		int randomBirthPoint = floor(Global::obtainRandom( 1.0f, numBirthPoints + 1.0f ));

		char birthPointNameTmp[ 128 ];
		sprintf_s(birthPointNameTmp, sizeof(birthPointNameTmp), "birth_point_%d", randomBirthPoint);
		std::string birthPointName = birthPointNameTmp;	

		const LogicVolumeManager &iuranusLogicVolumeManager = LogicManager::getLogicManager()->getIuranusLogicVolumeManager();
		const LogicSphere *logicSphere = (const LogicSphere *) iuranusLogicVolumeManager.getElement( birthPointName );
		const D3DXVECTOR3 &birthPoint = logicSphere->getCenter();

		std::string enemyType;
		float enemyTypeRandom = Global::obtainRandom( 0.0f, 3.0f );
		if( enemyTypeRandom >= 0.0f && enemyTypeRandom < 1.0f )
			enemyType = "malo";
		else if( enemyTypeRandom >= 1.0f && enemyTypeRandom < 2.0f )
			enemyType = "bueno";
		else
			enemyType = "feo";

		IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
		iuranusEnemyManager->createEnemy(birthPoint, enemyType);
		// ************* //

		++enemiesInTheScreen;
	}

}

void IuranusPlanetLogic::incrementEatenEnemy()
{
	if( numEnemiesEaten < numEnemiesEatenForVictory )
		numEnemiesEaten++;

	bool victoryConditionAchieved = checkVictoryConditionAchieved();

	if( !victoryActived && victoryConditionAchieved )
	{
		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createUserEvent("on_iuranus_victory_condition_achieved");
		victoryActived = true;
	} 
	else if( !victoryConditionAchieved )
	{
		victoryActived = false;
		checkPointActived = checkCheckPointAchieved();
		/*
		if( !checkPointActived && checkCheckPointAchieved() )
			checkPointActived = true;
		else
			checkPointActived = false;
		*/
	}

	createEnemy();
}

void IuranusPlanetLogic::decrementEnemiesInScreen()
{
	if( enemiesInTheScreen>0 )
		--enemiesInTheScreen;
}

void IuranusPlanetLogic::resetCountersToFailPoint()
{
	victoryActived = false;
	alreadyWon = false;
	numEnemiesEaten = floor(numEnemiesEatenForVictory * failVictoryCheckPointRecovery);
}

bool IuranusPlanetLogic::checkVictoryConditionAchieved() const
{
	return ( numEnemiesEaten >= numEnemiesEatenForVictory && !Global::youLose );
}

bool IuranusPlanetLogic::checkCheckPointAchieved()
{
	if( numEnemiesEaten == checkPointPercent1 )
	{
		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createUserEvent("on_iuranus_checkpoint_1_achieved");
		return true;
	}
	else if( numEnemiesEaten == checkPointPercent2 )
	{
		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createUserEvent("on_iuranus_checkpoint_2_achieved");
		return true;
	}
	else if( numEnemiesEaten == checkPointPercent3 )
	{
		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createUserEvent("on_iuranus_checkpoint_3_achieved");
		return true;
	}

	return false;
}

void IuranusPlanetLogic::checkFartToDo()
{
	if( !victoryActived ) //&& !checkPointActived )
	{
		LogicManager *logicManager = LogicManager::getLogicManager();
		logicManager->createUserEvent("on_player_in_ass");
	} 
	else if( victoryActived )
	{
		checkVictoryAchieved();
	}
}

void IuranusPlanetLogic::checkVictoryAchieved()
{
	LogicManager *logicManager = LogicManager::getLogicManager();

	if( Global::playerInsideAssVolume && !alreadyWon)
	{
		logicManager->createUserEvent("on_iuranus_victory_achieved");
		alreadyWon = true;
	}
	else if( !Global::playerInsideAssVolume && !alreadyWon )
	{
		logicManager->createUserEvent("on_iuranus_victory_not_achieved");
	}

}

void IuranusPlanetLogic::resetNumEnemiesInTheScreen() {
	enemiesInTheScreen=0;
}
