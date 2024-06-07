#ifndef BOILER_ENEMY_MANAGER_H
#define BOILER_ENEMY_MANAGER_H

#include <Windows.h>
#include <map>
#include "XMLParser/XMLParser.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/BoilerBehaviour/BoilerBehaviour.h"

/**
* Manager of Boiler Enemies
*
* It is a singleton.
*/

class BoilerEnemyManager : public std::map<std::string, IntelligentObject *>, public CXMLParser
{
protected:
	// Just for XML
	IntelligentObject *currentParsed;
	std::string currentParsedElementName;
	
	FlameZone *currentFlameZone;
	std::map<int, FlameZone *> flameZones;
	int currentFlameZoneId;
	float currentFlameTimeSequence;
	int currentNumFlamesZones;

public:
	~BoilerEnemyManager() {	
		OutputDebugString("Destroying BoilerEnemyManager...\n");
		cleanUp();
	};
	void cleanUp();

	static BoilerEnemyManager * getBoilerEnemyManager();

	void updateAI(float deltaTime=(1.0f/60.0f));
	void updateAnimation(float deltaTime=(1.0f/60.0f));
	void render() const;

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void reloadShader();

	void initEnemyFlameZone(const std::string &enemyName, int flameZoneId);
	void initEnemyFlamesSequence(const std::string &enemyName);
	void killEnemyFlameZone(const std::string &enemyName, int flameZoneId);

	void restart();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);
	
	static BoilerEnemyManager * boilerEnemyManager;
	BoilerEnemyManager();
};

#endif //BOILER_ENEMY_MANAGER_H
