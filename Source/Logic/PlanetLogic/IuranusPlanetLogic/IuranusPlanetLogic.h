#ifndef IURANUS_PLANET_LOGIC_H
#define IURANUS_PLANET_LOGIC_H

#include "Logic/PlanetLogic/IPlanetLogic.h"
#include "XMLParser/XMLParser.h"

/**
* Iuranus planet logic. This makes the gameplay of
* Iuranus planet.
*
* It is a singleton.
*/

class IuranusPlanetLogic : public IPlanetLogic, public CXMLParser
{
public:
	
	// loaded from xml
	int numBirthPoints;
	int numEnemiesEatenForVictory;
	int totalEnemiesInScreen;
	int initialTotalEnemiesInScreen;
	int initialEnemiesInScreen;
	float intervalEnemiesCreation;
	int checkPointPercent1;
	int checkPointPercent2;
	int checkPointPercent3;
	float failVictoryCheckPointRecovery;

	int enemiesInTheScreen;
	int numEnemiesEaten;
	bool checkPointActived;
	bool victoryActived;
	bool alreadyWon;
	
public:
	~IuranusPlanetLogic() {
		OutputDebugString("Destroying IuranusPlanetLogic...\n");
	};

	static IuranusPlanetLogic * getIuranusPlanetLogic();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

	void init();
	void resetCounters();
	void checkVictoryAchieved();
	void createEnemy();
	void createInitialEnemies();
	void incrementEatenEnemy();
	void decrementEnemiesInScreen();
	bool checkVictoryConditionAchieved() const;
	bool checkCheckPointAchieved();
	void checkFartToDo();
	void resetCountersToFailPoint();

	void resetNumEnemiesInTheScreen();

	int getNumEnemiesLeft( ) const { return (numEnemiesEatenForVictory - numEnemiesEaten); };
	bool isVictoryActived( ) const { return victoryActived; };

	void setTotalEnemiesInScreen( );

private:
	static IuranusPlanetLogic * iuranusPlanetLogic;
	IuranusPlanetLogic();

};

#endif //IURANUS_PLANET_LOGIC_H
