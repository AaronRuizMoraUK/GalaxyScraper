#ifndef IURANUS_ENEMY_MANAGER_H
#define IURANUS_ENEMY_MANAGER_H

#include <list>
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AnimationControllers/EnemyAnimation/EnemyAnimation.h"
#include "Util/MapComparers/MapComparers.h"
#include "XMLParser/XMLParser.h"

typedef std::list<AnimatedObject *> Foolisheds;
typedef std::list<IntelligentObject *> EnemiesEaten;
typedef std::map<std::string, IntelligentObject *, LessString> EnemyTypes;

/**
* Iuranus Manager of enemies.
*
* It is a singleton.
*/

class IuranusEnemyManager : public std::list<IntelligentObject *>, public CXMLParser
{
protected:
	Foolisheds foolisheds;
	EnemyTypes enemyTypes;
	EnemiesEaten enemiesEaten;

	// variable for reading xml file //
	std::string currentEnemyType;

public:
	~IuranusEnemyManager() {
		OutputDebugString("Destroying IuranusEnemyManager...\n");
		cleanAll();
	};
	void cleanAll();
	void clean();

	static IuranusEnemyManager * getIuranusEnemyManager();

	//void read(const char *fileName);

	void onStartElement (const std::string &elem, MKeyValue &atts);
//	void onEndElement (const std::string &elem);

	void updateAI(float deltaTime=(1.0f/60.0f));
	void updateAnimation(float deltaTime=(1.0f/60.0f));
	void render() const;

	IntelligentObject * getGroupablePartner(const IntelligentObject &partnerSearcher) const;
	void createEnemy(const D3DXVECTOR3 &createWhere, const std::string &enemyType);
	IuranusEnemyManager::iterator killEnemy(IuranusEnemyManager::iterator &enemyIterator);
	void addEnemyEaten(IntelligentObject *eaten);
	void addFoolished(AnimatedObject *foolished);
	void removeFoolished(AnimatedObject *foolished);
	bool someNearFoolished(const AnimatedObject &nonFoolished, float visionDistance) const;

	void killEatenEnemies();
	bool enemyAlreadyEaten( const IntelligentObject *object ) const;
	
	void checkForEnemiesInMouthExpelRadius() const;
	void checkForEnemiesInAssExpelRadius() const;
	void changeEnemiesStateToVictory() const;
	void resetEnemiesStates();

	void reloadShader();

//	IAnimationController *animationControllerFactory(const std::string &animController);

private:
	static IuranusEnemyManager * iuranusEnemyManager;
	IuranusEnemyManager();

	void init();
	void changeStatusEnemiesKicked();
};

#endif //IURANUS_ENEMY_MANAGER_H
