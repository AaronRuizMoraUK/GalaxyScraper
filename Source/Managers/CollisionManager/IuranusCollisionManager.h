#ifndef IURANUS_COLLISION_MANAGER_H
#define IURANUS_COLLISION_MANAGER_H

#include "CollisionManager.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include <vector>

typedef struct PlayerEnemyCollisionStruct {
	IntelligentObject *enemy;
	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;

	PlayerEnemyCollisionStruct(IntelligentObject *pEnemy, const D3DXVECTOR3 &pPoint1, const D3DXVECTOR3 &pPoint2)
	: enemy(pEnemy)
	, point1(pPoint1)
	, point2(pPoint2)
	{
	};
} PlayerEnemyCollision;

typedef struct BetweenEnemiesCollisionStruct {
	IntelligentObject *enemy1;
	IntelligentObject *enemy2;
	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;

	BetweenEnemiesCollisionStruct(IntelligentObject *pEnemy1, IntelligentObject *pEnemy2, const D3DXVECTOR3 &pPoint1, const D3DXVECTOR3 &pPoint2)
	: enemy1(pEnemy1)
	, enemy2(pEnemy2)
	, point1(pPoint1)
	, point2(pPoint2)
	{
	};
} BetweenEnemiesCollision;

/**
* Manager of collision events for Iuranus planet.
*
* It is a singleton.
*/

class IuranusCollisionManager : public CollisionManager
{
protected:
	std::vector<PlayerEnemyCollision> playerEnemyCollisionVector;
	std::vector<BetweenEnemiesCollision> betweenEnemiesCollisionVector;

public:
	~IuranusCollisionManager() {
		OutputDebugString("Destroying IuranusCollisionManager...\n");

		clean();
	};
	static IuranusCollisionManager * getIuranusCollisionManager();

	void detectCollisions();
	void treatCollisions();

	void clean();

private:
	static IuranusCollisionManager * iuranusCollisionManager;
	IuranusCollisionManager();

	void detectCollisionsPlayerEnemies();
	void detectCollisionsBetweenEnemies();
};

#endif //IURANUS_COLLISION_MANAGER_H
