#ifndef BOILER_COLLISION_MANAGER_H
#define BOILER_COLLISION_MANAGER_H

#include "CollisionManager.h"
#include "Object/CollisionableObject/CollisionableObject.h"
#include <vector>

/**
* Manager of collision events for Boiler planet.
*
* It is a singleton.
*/

typedef struct PlayerSolidCollisionStruct {
	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;

	PlayerSolidCollisionStruct(const D3DXVECTOR3 &pPoint1, const D3DXVECTOR3 &pPoint2)
	: point1(pPoint1)
	, point2(pPoint2)
	{
	};
} PlayerSolidCollision;

class BoilerCollisionManager : public CollisionManager
{
protected:
	bool fireballCollisionDetected;
	std::vector<PlayerSolidCollision> playerSolidCollisionVector;

public:
	~BoilerCollisionManager() {
		OutputDebugString("Destroying BoilerCollisionManager...\n");

		clean();
	};
	static BoilerCollisionManager * getBoilerCollisionManager();

	void detectCollisions();
	void treatCollisions();

	void clean();

private:
	static BoilerCollisionManager * boilerCollisionManager;
	BoilerCollisionManager();

protected:
	void detectCollisionsPlayerFireballs();
	void detectCollisionsPlayerSolids();
};

#endif //BOILER_COLLISION_MANAGER_H
